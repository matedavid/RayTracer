#pragma once

#include <limits>

class interval {
  public:
    double min;
    double max;

    static constexpr double infinity = std::numeric_limits<double>::infinity();

    constexpr interval() : min(-infinity), max(infinity) {}
    constexpr interval(double min_, double max_) : min(min_), max(max_) {}

    [[nodiscard]] inline bool contains(double x) const { return min <= x && x <= max; }

    [[nodiscard]] inline bool surrounds(double x) const { return min < x && x < max; }

    [[nodiscard]] inline double clamp(double x) const {
        if (x < min) return min;
        if (x > max) return max;
        return x;
    }
};
