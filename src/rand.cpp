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
