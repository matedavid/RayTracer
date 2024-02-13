#include <iostream>

#include "image_dumper.h"

int main(int32_t argc, const char* argv[]) {
    if (argc < 2) {
        std::cout << "Usage: ./RayTracerRenderer <scene_file>.json\n";
        return 1;
    }

    const std::string scene_file = argv[1];

    // TODO:

    return 0;
}