#ifndef WINDOW_H
#define WINDOW_H

#include <sstream>
#include <Windows.h>
#include "BaseException.h"
#include "Keyboard.h"
#include "Mouse.h"

class Window
{
public:
    class Exception : public BaseException
    {
    public:
        Exception(int line, const char* file, HRESULT hr);
        const char* what() const override;
        virtual const char* GetType() const;
        static std::string TranslateErrorCode(HRESULT hr);
        HRESULT GetErrorCode() const;
        std::string GetErrorString() const;
    private:
        HRESULT hr;
    };
private:
    // singleton manages registration/cleanup of window class
    class WindowClass
    {
    public:
        static const char* GetName();
        static HINSTANCE GetInstance();
    private:
        WindowClass();
        ~WindowClass();
        WindowClass(const WindowClass&) = delete;
        WindowClass& operator=(const WindowClass&) = delete;
        static const char *wndClassName;
        static WindowClass wndClass;
        HINSTANCE hInst;
    };
public:
    ~Window();
    Window(){};
    Window(const Window&) = delete;
    Window& operator=(const Window&) = delete;
    int Initialize(int width, int height, const char* name, int transparent = 255);
    BOOL Window::IsInWindow(const POINTS pt) const;
    void Window::SetTitle(const std::string& title);
    static BOOL ProcessGetMessages();
    static BOOL ProcessPeekMessages();
private:
    static LRESULT CALLBACK HandleMsgSetup(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
    static LRESULT CALLBACK HandleMsgThunk(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
    LRESULT HandleMsg(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
public:
    Keyboard m_kbd;
    Mouse m_mouse;
private:
    int m_width;
    int m_height;
    HWND m_hWnd;
    static int m_hWndCount;
};

// error exception helper macro
#define CHWND_EXCEPT( hr ) Window::Exception( __LINE__,__FILE__,hr )
#define CHWND_LAST_EXCEPT() Window::Exception( __LINE__,__FILE__,GetLastError() )
#endif