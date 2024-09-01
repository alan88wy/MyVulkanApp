#pragma once

#include "Window.h"

namespace mge {
    class FirstApp
    {
    public:
        static constexpr int WIDTH = 1440;
        static constexpr int HEIGHT = 900;


        std::string windowName = { "My First App" };

        void run();

    private:
        MgeWindow mainWindow{ WIDTH, HEIGHT, windowName };

    };
}
