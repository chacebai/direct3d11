#include "App.h"
#include <sstream>
#include <iomanip>

App::App()
{
    wnd.Initialize(1024, 768, "DirectX");
}

int App::Go()
{
    BOOL ecode;
    while (true)
    {
        if (ecode = Window::ProcessGetMessages())
        {
            break;
        }
        DoFrame();
    }
    return ecode;
}

void App::DoFrame()
{
    /*
    const float t = timer.Peek();
    std::ostringstream oss;
    oss << "Time elapsed: " << std::fixed << t << "s";
    wnd.SetTitle(oss.str());
    */

    //²âÊÔ¿Õ¸ñ¼ü
    if (wnd.m_kbd.KeyIsPressed(VK_SPACE))
    {
        MessageBox(NULL, "Something Happon", "The key is press", MB_OK);
    }
    //²âÊÔÊó±ê
    static int i = 0;
    while (!wnd.m_mouse.IsEmpty())
    {
        const auto e = wnd.m_mouse.Read();
        switch (e.GetType())
        {
            //²âÊÔÊó±ê¼ü
        case Mouse::Event::Type::Leave:
            wnd.SetTitle("Gone!");
            break;
        case Mouse::Event::Type::Move:
        {
                                         std::ostringstream oss;
                                         oss << "Mouse moved to (" << e.GetPosX() << "," << e.GetPosY() << ")";
                                         wnd.SetTitle(oss.str());
        }
            break;
            //²âÊÔÊó±ê¹öÂÖ
        case Mouse::Event::Type::WheelUp:
        {
                                            i++;
                                            std::ostringstream oss;
                                            oss << "Up: " << i;
                                            wnd.SetTitle(oss.str());
        }
            break;
        case Mouse::Event::Type::WheelDown:
        {
                                              i--;
                                              std::ostringstream oss;
                                              oss << "Down: " << i;
                                              wnd.SetTitle(oss.str());
        }
            break;
        }
    }
}