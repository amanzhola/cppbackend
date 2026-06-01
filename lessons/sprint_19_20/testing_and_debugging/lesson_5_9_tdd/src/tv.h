#pragma once

class TV {
public:
    bool IsTurnedOn() const noexcept {
        return is_turned_on_;
    }

private:
    bool is_turned_on_ = false;
};
