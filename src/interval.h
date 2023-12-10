#pragma once

#include <limits>
#include <cmath>

class interval {
  public:
    double min;
    double max;

    static constexpr double infinity = std::numeric_limits<double>::infinity();

    constexpr interval() : min(-infinity), max(infinity) {}
    constexpr interval(double min_, double max_) : min(min_), max(max_) {}
    constexpr interval(const interval& a, const interval& b) : min(fmin(a.min, b.min)), max(fmax(a.max, b.max)) {}

    [[nodiscard]] inline bool contains(double x) const { return min <= x && x <= max; }

    [[nodiscard]] inline bool surrounds(double x) const { return min < x && x < max; }

    [[nodiscard]] interval expand(double delta) {
        const auto padding = delta / 2.0;
        return {min - padding, max + padding};
    }

    [[nodiscard]] inline double clamp(double x) const {
        if (x < min)
            return min;
        if (x > max)
            return max;
        return x;
    }
};
