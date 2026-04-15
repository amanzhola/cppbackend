#pragma once

#define MINIAUDIO_IMPLEMENTATION
#include <miniaudio.h>

#include <algorithm>
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <stdexcept>
#include <thread>
#include <vector>

class Recorder {
    static void Callback(ma_device* pDevice, void* pOutput, const void* pInput,
                         ma_uint32 frameCount) {
        (void)pOutput;

        Recorder* recorder = reinterpret_cast<Recorder*>(pDevice->pUserData);
        if (recorder == nullptr) {
            return;
        }

        recorder->SaveBuffer(pInput, frameCount);
    }

    void SaveBuffer(const void* pInput, ma_uint32 frameCount) {
        if (pInput == nullptr || current_off_ >= buffer_.size()) {
            return;
        }

        size_t bytes_requested = static_cast<size_t>(frameCount) * frame_size_;
        size_t bytes_available = buffer_.size() - current_off_;
        size_t bytes_to_copy = std::min(bytes_requested, bytes_available);

        std::copy_n(reinterpret_cast<const char*>(pInput),
                    bytes_to_copy,
                    buffer_.data() + current_off_);

        current_off_ += bytes_to_copy;
    }

public:
    Recorder(ma_format format, int channels, ma_uint32 sampleRate = 44100) {
        const ma_backend backends[] = {ma_backend_pulseaudio};

        ma_context_config context_config = ma_context_config_init();
        init_context_result_ = ma_context_init(backends, 1, &context_config, &context_);
        if (init_context_result_ != MA_SUCCESS) {
            throw std::runtime_error("Recorder: failed to init PulseAudio context");
        }

        ma_device_config device_config = ma_device_config_init(ma_device_type_capture);
        device_config.capture.pDeviceID = nullptr;
        device_config.capture.format = format;
        device_config.capture.channels = channels;
        device_config.sampleRate = sampleRate;
        device_config.dataCallback = Callback;
        device_config.pUserData = this;

        frame_size_ = ma_get_bytes_per_frame(format, channels);
        if (frame_size_ <= 0) {
            ma_context_uninit(&context_);
            throw std::runtime_error("Recorder: invalid frame size");
        }

        init_device_result_ = ma_device_init(&context_, &device_config, &device_);
        if (init_device_result_ != MA_SUCCESS) {
            ma_context_uninit(&context_);
            throw std::runtime_error("Recorder: failed to init capture device");
        }
    }

    ~Recorder() {
        ma_device_uninit(&device_);
        ma_context_uninit(&context_);
    }

    struct RecordingResult {
        std::vector<char> data;
        size_t frames;
    };

    template <typename Rep, typename Period>
    RecordingResult Record(size_t max_frames, std::chrono::duration<Rep, Period> dur) {
        current_off_ = 0;
        buffer_.assign(max_frames * static_cast<size_t>(frame_size_), 0);

        if (ma_device_start(&device_) != MA_SUCCESS) {
            return {{}, 0};
        }

        std::this_thread::sleep_for(dur);
        ma_device_stop(&device_);

        buffer_.resize(current_off_);
        return {buffer_, current_off_ / static_cast<size_t>(frame_size_)};
    }

    int GetFrameSize() const {
        return frame_size_;
    }

private:
    ma_context context_{};
    ma_device device_{};
    ma_result init_context_result_{MA_SUCCESS};
    ma_result init_device_result_{MA_SUCCESS};
    int frame_size_{1};

    std::vector<char> buffer_;
    size_t current_off_{0};
};

class Player {
    static void Callback(ma_device* pDevice, void* pOutput, const void* pInput,
                         ma_uint32 frameCount) {
        (void)pInput;

        Player* player = reinterpret_cast<Player*>(pDevice->pUserData);
        if (player == nullptr) {
            return;
        }

        player->FillBuffer(pOutput, frameCount);
    }

    void FillBuffer(void* pOutput, ma_uint32 frameCount) {
        size_t wanted = static_cast<size_t>(frameCount) * static_cast<size_t>(frame_size_);
        char* out = reinterpret_cast<char*>(pOutput);

        if (current_buffer_ == nullptr || current_off_ >= max_bytes_) {
            std::fill_n(out, wanted, 0);
            return;
        }

        size_t available = max_bytes_ - current_off_;
        size_t bytes_to_copy = std::min(wanted, available);

        std::copy_n(current_buffer_ + current_off_, bytes_to_copy, out);
        current_off_ += bytes_to_copy;

        if (bytes_to_copy < wanted) {
            std::fill_n(out + bytes_to_copy, wanted - bytes_to_copy, 0);
        }
    }

public:
    Player(ma_format format, int channels, ma_uint32 sampleRate = 44100) {
        const ma_backend backends[] = {ma_backend_pulseaudio};

        ma_context_config context_config = ma_context_config_init();
        init_context_result_ = ma_context_init(backends, 1, &context_config, &context_);
        if (init_context_result_ != MA_SUCCESS) {
            throw std::runtime_error("Player: failed to init PulseAudio context");
        }

        ma_device_config device_config = ma_device_config_init(ma_device_type_playback);
        device_config.playback.pDeviceID = nullptr;
        device_config.playback.format = format;
        device_config.playback.channels = channels;
        device_config.sampleRate = sampleRate;
        device_config.dataCallback = Callback;
        device_config.pUserData = this;

        frame_size_ = ma_get_bytes_per_frame(format, channels);
        if (frame_size_ <= 0) {
            ma_context_uninit(&context_);
            throw std::runtime_error("Player: invalid frame size");
        }

        init_device_result_ = ma_device_init(&context_, &device_config, &device_);
        if (init_device_result_ != MA_SUCCESS) {
            ma_context_uninit(&context_);
            throw std::runtime_error("Player: failed to init playback device");
        }
    }

    ~Player() {
        ma_device_uninit(&device_);
        ma_context_uninit(&context_);
    }

    template <typename Rep, typename Period>
    void PlayBuffer(const char* data, size_t frames, std::chrono::duration<Rep, Period> dur) {
        current_buffer_ = data;
        current_off_ = 0;
        max_bytes_ = frames * static_cast<size_t>(frame_size_);

        if (ma_device_start(&device_) != MA_SUCCESS) {
            throw std::runtime_error("Player: ma_device_start failed");
        }

        std::this_thread::sleep_for(dur);
        ma_device_stop(&device_);
    }

    int GetFrameSize() const {
        return frame_size_;
    }

private:
    ma_context context_{};
    ma_device device_{};
    ma_result init_context_result_{MA_SUCCESS};
    ma_result init_device_result_{MA_SUCCESS};
    int frame_size_{1};

    const char* current_buffer_{nullptr};
    size_t current_off_{0};
    size_t max_bytes_{0};
};
