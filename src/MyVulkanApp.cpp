// MyVulkanApp.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "Window.h"

const uint32_t WIDTH = 1440;
const uint32_t HEIGHT = 900;


class HelloTriangleApplication
{
    public:

        void Run()
        {
            Window mainWindow = Window(WIDTH, HEIGHT);
            
            mainWindow.InitWindow();
            mainWindow.InitVulkan();
            mainWindow.MainLoop();
            mainWindow.CleanUp();
        }

    private:
        // Window mainWindow;
};

int main() {

    HelloTriangleApplication app;

    try {
        app.Run();
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << std::endl;

        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
