#include "rand.h"

#include <random>

static std::uniform_real_distribution<double> s_distribution{0.0, 1.0};
static std::mt19937 s_generator{std::random_device()()};

double random_double() {
    return s_distribution(s_generator);
}

double random_double(double min, double max) {
    return (max - min) * s_distribution(s_generator) + min;
}

int32_t random_int(int32_t min, int32_t max) {
    return static_cast<int32_t>(random_double(min, max + 1));
}