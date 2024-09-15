// MyVulkanApp.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <stdexcept>
//#include "FirstApp.h"
#include "Window.h"

 const uint32_t WIDTH = 1440;
 const uint32_t HEIGHT = 900;
 std::string windowName = { "My First App" };

int main() {

    //mge::FirstApp app{};
    mge::MgeEngine mainWindow{ WIDTH, HEIGHT, windowName };

    try {
        mainWindow.run();
    }
    catch (const std::exception& e) {
        std::cerr << e.what() << "\n";

        system("pause");

        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
