#pragma once

#include <optional>

class TV {
public:
    bool IsTurnedOn() const noexcept {
        return is_turned_on_;
    }

    std::optional<int> GetChannel() const noexcept {
        if (is_turned_on_) {
            return channel_;
        }

        return std::nullopt;
    }

    void TurnOn() noexcept {
        is_turned_on_ = true;
    }

    void TurnOff() noexcept {
        is_turned_on_ = false;
    }

private:
    bool is_turned_on_ = false;
    int channel_ = 1;
};
