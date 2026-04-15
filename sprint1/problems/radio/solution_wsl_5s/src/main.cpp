#include <boost/asio.hpp>

#include <arpa/inet.h>
#include <chrono>
#include <cstdint>
#include <cstring>
#include <fstream>
#include <iostream>
#include <string>
#include <thread>
#include <unordered_map>
#include <vector>
#include <algorithm>

using boost::asio::ip::udp;
using namespace std::literals;

constexpr uint32_t MAGIC = 0x52414431;          // "RAD1"
constexpr uint16_t CHUNK_DATA_SIZE = 1400;
constexpr std::chrono::seconds MESSAGE_TTL(30);

#pragma pack(push, 1)
struct PacketHeader {
    uint32_t magic;
    uint32_t message_id;
    uint32_t total_size;
    uint16_t chunk_index;
    uint16_t chunk_count;
    uint16_t payload_size;
};
#pragma pack(pop)

static PacketHeader ToNetwork(PacketHeader h) {
    h.magic = htonl(h.magic);
    h.message_id = htonl(h.message_id);
    h.total_size = htonl(h.total_size);
    h.chunk_index = htons(h.chunk_index);
    h.chunk_count = htons(h.chunk_count);
    h.payload_size = htons(h.payload_size);
    return h;
}

static PacketHeader FromNetwork(PacketHeader h) {
    h.magic = ntohl(h.magic);
    h.message_id = ntohl(h.message_id);
    h.total_size = ntohl(h.total_size);
    h.chunk_index = ntohs(h.chunk_index);
    h.chunk_count = ntohs(h.chunk_count);
    h.payload_size = ntohs(h.payload_size);
    return h;
}

static std::vector<char> RecordAudio() {
    std::cout << "Recording 5 seconds..." << std::endl;

    int rc = std::system(
        "timeout 5s parecord --device=RDPSource --raw --format=u8 --channels=1 --rate=44100 /tmp/rec.raw"
    );

    if (rc != 0) {
        std::cerr << "parecord failed, exit code=" << rc << std::endl;
        return {};
    }

    std::ifstream file("/tmp/rec.raw", std::ios::binary);
    if (!file) {
        std::cerr << "Cannot open /tmp/rec.raw" << std::endl;
        return {};
    }

    std::vector<char> data((std::istreambuf_iterator<char>(file)),
                           std::istreambuf_iterator<char>());

    std::cout << "Recording done. Bytes: " << data.size() << std::endl;
    return data;
}

static void PlayAudio(const std::vector<char>& data) {
    std::ofstream file("/tmp/audio.raw", std::ios::binary);
    if (!file) {
        std::cerr << "Cannot open /tmp/audio.raw for writing" << std::endl;
        return;
    }

    file.write(data.data(), static_cast<std::streamsize>(data.size()));
    file.close();

    std::cout << "Playing..." << std::endl;
    int rc = std::system("paplay --raw --format=u8 --channels=1 --rate=44100 /tmp/audio.raw");
    if (rc != 0) {
        std::cerr << "paplay failed, exit code=" << rc << std::endl;
    } else {
        std::cout << "Done" << std::endl;
    }
}

struct IncomingMessage {
    uint32_t message_id = 0;
    uint32_t total_size = 0;
    uint16_t chunk_count = 0;
    std::vector<char> data;
    std::vector<bool> received;
    size_t received_chunks = 0;
    std::chrono::steady_clock::time_point last_update = std::chrono::steady_clock::now();
};

static std::string MakeMessageKey(const udp::endpoint& sender, uint32_t message_id) {
    return sender.address().to_string() + ":" + std::to_string(sender.port()) + ":" + std::to_string(message_id);
}

static void CleanupExpired(std::unordered_map<std::string, IncomingMessage>& messages) {
    auto now = std::chrono::steady_clock::now();

    for (auto it = messages.begin(); it != messages.end();) {
        if (now - it->second.last_update > MESSAGE_TTL) {
            std::cout << "Dropping stale message: " << it->first << std::endl;
            it = messages.erase(it);
        } else {
            ++it;
        }
    }
}

static bool ValidateHeader(const PacketHeader& header, size_t len) {
    if (header.magic != MAGIC) {
        std::cout << "Drop: bad magic=0x" << std::hex << header.magic << std::dec << std::endl;
        return false;
    }

    if (header.chunk_count == 0) {
        std::cout << "Drop: chunk_count=0" << std::endl;
        return false;
    }

    if (header.chunk_index >= header.chunk_count) {
        std::cout << "Drop: chunk_index=" << header.chunk_index
                  << " >= chunk_count=" << header.chunk_count << std::endl;
        return false;
    }

    if (header.payload_size > CHUNK_DATA_SIZE) {
        std::cout << "Drop: payload_size too big: " << header.payload_size << std::endl;
        return false;
    }

    if (len < sizeof(PacketHeader) + header.payload_size) {
        std::cout << "Drop: short packet. len=" << len
                  << " expected_at_least=" << (sizeof(PacketHeader) + header.payload_size) << std::endl;
        return false;
    }

    size_t offset = static_cast<size_t>(header.chunk_index) * CHUNK_DATA_SIZE;
    if (offset + header.payload_size > header.total_size) {
        std::cout << "Drop: payload goes past total_size. offset=" << offset
                  << " payload=" << header.payload_size
                  << " total=" << header.total_size << std::endl;
        return false;
    }

    return true;
}

static void StartServer(int port) {
    boost::asio::io_context io;
    udp::endpoint endpoint(boost::asio::ip::address_v4::any(), static_cast<unsigned short>(port));
    udp::socket socket(io);

    socket.open(udp::v4());
    socket.bind(endpoint);

    std::cout << "Server started on port " << port << std::endl;

    std::vector<char> buffer(65536);
    udp::endpoint sender;

    std::unordered_map<std::string, IncomingMessage> messages;

    while (true) {
        size_t len = socket.receive_from(boost::asio::buffer(buffer), sender);

        std::cout << "RAW packet: " << len
                  << " bytes from " << sender.address().to_string()
                  << ":" << sender.port() << std::endl;

        if (len < sizeof(PacketHeader)) {
            std::cout << "Drop: packet smaller than header (" << len << " bytes)" << std::endl;
            CleanupExpired(messages);
            continue;
        }

        PacketHeader header{};
        std::memcpy(&header, buffer.data(), sizeof(header));
        header = FromNetwork(header);

        std::cout << "Header: magic=0x" << std::hex << header.magic << std::dec
                  << " msg_id=" << header.message_id
                  << " total_size=" << header.total_size
                  << " chunk_index=" << header.chunk_index
                  << " chunk_count=" << header.chunk_count
                  << " payload_size=" << header.payload_size
                  << std::endl;

        if (!ValidateHeader(header, len)) {
            CleanupExpired(messages);
            continue;
        }

        std::string key = MakeMessageKey(sender, header.message_id);
        auto it = messages.find(key);

        if (it == messages.end()) {
            IncomingMessage msg;
            msg.message_id = header.message_id;
            msg.total_size = header.total_size;
            msg.chunk_count = header.chunk_count;
            msg.data.assign(header.total_size, 0);
            msg.received.assign(header.chunk_count, false);
            msg.last_update = std::chrono::steady_clock::now();

            auto [new_it, inserted] = messages.emplace(key, std::move(msg));
            it = new_it;

            std::cout << "New message: key=" << key
                      << " total_size=" << header.total_size
                      << " chunks=" << header.chunk_count << std::endl;
        } else {
            if (it->second.total_size != header.total_size ||
                it->second.chunk_count != header.chunk_count) {
                std::cout << "Drop: inconsistent header for existing message key=" << key << std::endl;
                CleanupExpired(messages);
                continue;
            }
        }

        IncomingMessage& msg = it->second;
        msg.last_update = std::chrono::steady_clock::now();

        size_t offset = static_cast<size_t>(header.chunk_index) * CHUNK_DATA_SIZE;

        if (!msg.received[header.chunk_index]) {
            std::memcpy(msg.data.data() + offset,
                        buffer.data() + sizeof(PacketHeader),
                        header.payload_size);

            msg.received[header.chunk_index] = true;
            msg.received_chunks++;

            std::cout << "Accepted chunk "
                      << (header.chunk_index + 1) << "/" << msg.chunk_count
                      << " for msg_id=" << msg.message_id
                      << " received_chunks=" << msg.received_chunks
                      << "/" << msg.chunk_count << std::endl;
        } else {
            std::cout << "Duplicate chunk "
                      << (header.chunk_index + 1) << "/" << msg.chunk_count
                      << " for msg_id=" << msg.message_id << std::endl;
        }

        if (msg.received_chunks == msg.chunk_count) {
            std::cout << "Message complete: msg_id=" << msg.message_id
                      << " bytes=" << msg.total_size << std::endl;
            PlayAudio(msg.data);
            messages.erase(it);
        }

        CleanupExpired(messages);
    }
}

static uint32_t GenerateMessageId() {
    auto now = std::chrono::steady_clock::now().time_since_epoch();
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now).count();
    return static_cast<uint32_t>(ms & 0xFFFFFFFFu);
}

static void StartClient(int port) {
    boost::asio::io_context io;
    udp::socket socket(io);
    socket.open(udp::v4());

    while (true) {
        std::string ip;
        std::cout << "Enter server IP: ";
        std::getline(std::cin, ip);

        if (ip.empty()) {
            continue;
        }

        udp::endpoint endpoint(boost::asio::ip::make_address(ip), static_cast<unsigned short>(port));

        auto audio = RecordAudio();
        if (audio.empty()) {
            std::cerr << "Nothing recorded" << std::endl;
            continue;
        }

        uint32_t message_id = GenerateMessageId();
        size_t total_chunks = (audio.size() + CHUNK_DATA_SIZE - 1) / CHUNK_DATA_SIZE;

        std::cout << "Sending message_id=" << message_id
                  << " bytes=" << audio.size()
                  << " chunks=" << total_chunks << std::endl;

        for (size_t i = 0; i < total_chunks; i++) {
            PacketHeader header{};
            header.magic = MAGIC;
            header.message_id = message_id;
            header.total_size = static_cast<uint32_t>(audio.size());
            header.chunk_index = static_cast<uint16_t>(i);
            header.chunk_count = static_cast<uint16_t>(total_chunks);

            size_t offset = i * CHUNK_DATA_SIZE;
            size_t size = std::min(static_cast<size_t>(CHUNK_DATA_SIZE), audio.size() - offset);
            header.payload_size = static_cast<uint16_t>(size);

            PacketHeader net = ToNetwork(header);

            std::vector<char> packet(sizeof(net) + size);
            std::memcpy(packet.data(), &net, sizeof(net));
            std::memcpy(packet.data() + sizeof(net), audio.data() + offset, size);

            size_t sent = socket.send_to(boost::asio::buffer(packet), endpoint);

            std::cout << "Sent chunk " << (i + 1) << "/" << total_chunks
                      << " (" << sent << " bytes)" << std::endl;

            std::this_thread::sleep_for(5ms);
        }

        std::cout << "Message sent." << std::endl;
    }
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cout << "Usage: radio server <port> | radio client <port>" << std::endl;
        return 1;
    }

    std::string mode = argv[1];
    int port = std::stoi(argv[2]);

    if (port <= 0 || port > 65535) {
        std::cerr << "Invalid port: " << port << std::endl;
        return 1;
    }

    try {
        if (mode == "server") {
            StartServer(port);
        } else if (mode == "client") {
            StartClient(port);
        } else {
            std::cout << "Mode must be 'server' or 'client'" << std::endl;
            return 1;
        }
    } catch (const std::exception& e) {
        std::cerr << "Fatal error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
