#ifndef APP_H
#define APP_H

#include "Window.h"
#include "CTimer.hpp"

class App
{
public:
    App();
    // master frame / message loop
    int Go();
private:
    void DoFrame();
private:
    Window wnd;
    CTimer timer;
};

#endif