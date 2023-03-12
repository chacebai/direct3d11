#include <windows.h>
#include <strsafe.h>
#include <commctrl.h>

#include "Window.h"
#include "App.h"

// WINAPI����__stdcall �����Ĵ���˳�� ���ҵ��� ������ջ �����ں�������ǰ ���γ�ջ
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