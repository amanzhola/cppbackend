#include "audio.h"

#include <boost/asio.hpp>

#include <chrono>
#include <cstdint>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

using boost::asio::ip::udp;
using namespace std::literals;

namespace {

constexpr size_t MAX_FRAMES = 65000;
constexpr auto AUDIO_DURATION = 1500ms;

void StartServer(uint16_t port) {
    boost::asio::io_context io_context;
    udp::socket socket(io_context, udp::endpoint(udp::v4(), port));

    Player player(ma_format_u8, 1);
    const size_t frame_size = player.GetFrameSize();

    std::vector<char> buffer(MAX_FRAMES * frame_size);

    std::cout << "Server started on UDP port " << port << std::endl;

    while (true) {
        udp::endpoint remote_endpoint;

        size_t bytes_received =
            socket.receive_from(boost::asio::buffer(buffer), remote_endpoint);

        if (bytes_received == 0) {
            continue;
        }

        if (bytes_received % frame_size != 0) {
            std::cerr << "Invalid packet size: " << bytes_received
                      << " bytes is not divisible by frame size "
                      << frame_size << std::endl;
            continue;
        }

        size_t frames = bytes_received / frame_size;

        std::cout << "Received " << bytes_received
                  << " bytes from "
                  << remote_endpoint.address().to_string()
                  << ":" << remote_endpoint.port() << std::endl;

        std::cout << "Playing received audio..." << std::endl;
        player.PlayBuffer(buffer.data(), frames, AUDIO_DURATION);
        std::cout << "Playing done" << std::endl;
    }
}

void StartClient(uint16_t port) {
    boost::asio::io_context io_context;
    udp::socket socket(io_context);
    socket.open(udp::v4());

    Recorder recorder(ma_format_u8, 1);

    std::cout << "Client started. Destination port: " << port << std::endl;

    while (true) {
        std::string ip;
        std::cout << "Enter server IP: ";
        std::getline(std::cin, ip);

        if (ip.empty()) {
            break;
        }

        udp::endpoint endpoint(boost::asio::ip::make_address(ip), port);

        std::cout << "Recording... Speak now!" << std::endl;
        auto rec = recorder.Record(MAX_FRAMES, AUDIO_DURATION);
        std::cout << "Recording done" << std::endl;

        size_t bytes = rec.frames * recorder.GetFrameSize();

        socket.send_to(boost::asio::buffer(rec.data.data(), bytes), endpoint);

        std::cout << "Sent " << bytes
                  << " bytes to " << ip
                  << ":" << port << std::endl;
    }
}

}  // namespace

int main(int argc, char* argv[]) {
    try {
        if (argc != 3) {
            std::cerr << "Usage:\n"
                      << argv[0] << " server <port>\n"
                      << argv[0] << " client <port>\n";
            return 1;
        }

        std::string mode = argv[1];
        int port_value = std::stoi(argv[2]);

        if (port_value < 0 || port_value > 65535) {
            std::cerr << "Invalid port" << std::endl;
            return 1;
        }

        uint16_t port = static_cast<uint16_t>(port_value);

        if (mode == "server") {
            StartServer(port);
        } else if (mode == "client") {
            StartClient(port);
        } else {
            std::cerr << "Mode must be client or server" << std::endl;
            return 1;
        }

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
