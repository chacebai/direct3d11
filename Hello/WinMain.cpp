#include <windows.h>
#include <strsafe.h>
#include <commctrl.h>

#include "Window.h"
#include "App.h"

// WINAPI代表__stdcall 参数的传递顺序 从右到左 依次入栈 并且在函数返回前 依次出栈
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    try
    {
        return App{}.Go();
    }
    catch (const BaseException& e)
    {
        MessageBox(nullptr, e.what(), e.GetType(), MB_OK | MB_ICONEXCLAMATION);
    }
    catch (const std::exception& e)
    {
        MessageBox(nullptr, e.what(), "Standard Exception", MB_OK | MB_ICONEXCLAMATION);
    }
    catch (...)
    {
        MessageBox(nullptr, "No details available", "Unknown Exception", MB_OK | MB_ICONEXCLAMATION);
    }
    return -1;
}