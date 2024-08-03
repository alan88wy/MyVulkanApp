#pragma once

#include "Window.h"

class FirstApp
{
public:
    static constexpr int WIDTH = 1440;
    static constexpr int HEIGHT = 900;

    void Run();

private:
    Window mainWindow{ WIDTH, HEIGHT };

};
