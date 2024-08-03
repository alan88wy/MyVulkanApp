// MyVulkanApp.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <stdexcept>
#include "FirstApp.h"

const uint32_t WIDTH = 1440;
const uint32_t HEIGHT = 900;

int main() {

    mge::FirstApp app{};

    try {
        app.run();
    }
    catch (const std::exception& e) {
        std::cerr << e.what() << "\n";
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
