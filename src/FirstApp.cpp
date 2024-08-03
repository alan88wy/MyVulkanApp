
#include "FirstApp.h"

const uint32_t WIDTH = 1440;
const uint32_t HEIGHT = 900;


void FirstApp::Run()
{
    mainWindow.InitWindow();
    mainWindow.InitVulkan();
    mainWindow.MainLoop();
    mainWindow.CleanUp();
}
