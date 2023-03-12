#include "Window.h"
#include "resource.h"
#include "WindowsMessageMap.h"

// Window Class Stuff
Window::WindowClass Window::WindowClass::wndClass;
int Window::m_hWndCount = 0;
const char *Window::WindowClass::wndClassName = "WinMain";

Window::WindowClass::WindowClass() :hInst(GetModuleHandle(nullptr))
{
    WNDCLASSEX wcex = { 0 };
    wcex.cbSize = sizeof(wcex);
    wcex.style = CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = HandleMsgSetup;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = GetInstance();
    wcex.hIcon = static_cast<HICON>(LoadImage(GetInstance(), MAKEINTRESOURCE(IDI_ICON1), IMAGE_ICON, 32, 32, 0));
    wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
    wcex.lpszMenuName = nullptr;
    wcex.lpszClassName = GetName();
    wcex.hIconSm = static_cast<HICON>(LoadImage(GetInstance(), MAKEINTRESOURCE(IDI_ICON1), IMAGE_ICON, 16, 16, 0));
    RegisterClassEx(&wcex);
}

Window::WindowClass::~WindowClass()
{
    UnregisterClass(wndClassName, GetInstance());
}

const char* Window::WindowClass::GetName()
{
    return wndClassName;
}

HINSTANCE Window::WindowClass::GetInstance()
{
    return wndClass.hInst;
}

Window::~Window()
{
    if (IsWindow(m_hWnd))
    {
        DestroyWindow(m_hWnd);
    }
}

// Window Exception Stuff
Window::Exception::Exception(int line, const char * file, HRESULT hr) :BaseException(line, file), hr(hr)
{}

const char * Window::Exception::what() const
{
    std::ostringstream oss;
    oss << GetType() << std::endl
        << "[Error Code] " << GetErrorCode() << std::endl
        << "[Description] " << GetErrorString() << std::endl
        << GetOriginString();
    whatBuffer = oss.str();
    return whatBuffer.c_str();
}

const char* Window::Exception::GetType() const
{
    return "Window Exception";
}

std::string Window::Exception::TranslateErrorCode(HRESULT hr)
{
    char* pMsgBuf = nullptr;
    DWORD nMsgLen = FormatMessage(
        FORMAT_MESSAGE_ALLOCATE_BUFFER |
        FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
        nullptr, hr, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        reinterpret_cast<LPSTR>(&pMsgBuf), 0, nullptr
        );
    if (nMsgLen == 0)
    {
        return "Unidentified error code";
    }
    std::string errorString = pMsgBuf;
    LocalFree(pMsgBuf);
    return errorString;
}

HRESULT Window::Exception::GetErrorCode() const
{
    return hr;
}

std::string Window::Exception::GetErrorString() const
{
    return TranslateErrorCode(hr);
}

void Window::SetTitle(const std::string& title)
{
    if (SetWindowText(m_hWnd, title.c_str()) == 0)
    {
        throw CHWND_LAST_EXCEPT();
    }
}

int Window::Initialize(int width, int height, const char* name, int transparent)
{
    // calculate window size based on desired client region size
    m_width = width;
    m_height = height;
    RECT wr;
    wr.left = 100;
    wr.right = width + wr.left;
    wr.top = 100;
    wr.bottom = height + wr.top;
    if (AdjustWindowRect(&wr, WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU, FALSE) == 0)
    {
        throw CHWND_LAST_EXCEPT();
    }
    // create window & get hWnd
    if (0 < transparent && transparent < 255)
    {
        m_hWnd = CreateWindowEx(NULL, WindowClass::GetName(), name,
            WS_POPUP | WS_VISIBLE, wr.left, wr.top,
            wr.right - wr.left, wr.bottom - wr.top, nullptr, nullptr, WindowClass::GetInstance(), this);
        if (m_hWnd == nullptr)
        {
            throw CHWND_LAST_EXCEPT();
        }
        //show transparent window
        SetWindowLong(m_hWnd, GWL_EXSTYLE, GetWindowLong(m_hWnd, GWL_EXSTYLE) | WS_EX_LAYERED);
        SetLayeredWindowAttributes(m_hWnd, NULL, transparent, LWA_ALPHA);
    }
    else
    {
        m_hWnd = CreateWindowEx(NULL, WindowClass::GetName(), name,
            WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT,
            wr.right - wr.left, wr.bottom - wr.top, nullptr, nullptr, WindowClass::GetInstance(), this);
        if (m_hWnd == nullptr)
        {
            throw CHWND_LAST_EXCEPT();
        }
        // show window
        ShowWindow(m_hWnd, SW_SHOWDEFAULT);
    }
    //window numbers
    Window::m_hWndCount++;
    return 0;
}

BOOL Window::IsInWindow(const POINTS pt) const
{
    if (pt.x >= 0 && pt.x < m_width && pt.y >= 0 && pt.y < m_height)
        return TRUE;
    else
        return FALSE;
}

BOOL Window::ProcessGetMessages()
{
    MSG msg;
    BOOL gResult;
    while ((gResult = GetMessage(&msg, nullptr, 0, 0)) > 0)
    {
        // TranslateMessage will post auxilliary WM_CHAR messages from key msgs
        TranslateMessage(&msg);
        DispatchMessage(&msg);
        return FALSE;
    }

    // check if GetMessage call itself borked
    if (gResult == -1)
    {
        throw CHWND_LAST_EXCEPT();
    }

    // wParam here is the value passed to PostQuitMessage
    return TRUE;
}

BOOL Window::ProcessPeekMessages()
{
    MSG msg;
    // while queue has messages, remove and dispatch them (but do not block on empty queue)
    while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
    {
        // check for quit because peekmessage does not signal this via return val
        if (msg.message == WM_QUIT)
        {
            // return optional wrapping int (arg to PostQuitMessage is in wparam) signals quit
            return TRUE;
        }

        // TranslateMessage will post auxilliary WM_CHAR messages from key msgs
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    // return empty optional when not quitting app
    return FALSE;
}

LRESULT CALLBACK Window::HandleMsgSetup(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    // use create parameter passed in from CreateWindow() to store window class pointer at WinAPI side
    if (msg == WM_NCCREATE)
    {
        // extract ptr to window class from creation data
        const CREATESTRUCTW* const pCreate = reinterpret_cast<CREATESTRUCTW*>(lParam);
        Window* const pWnd = static_cast<Window*>(pCreate->lpCreateParams);
        // set WinAPI-managed user data to store ptr to window class
        SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pWnd));
        // set message proc to normal (non-setup) handler now that setup is finished
        SetWindowLongPtr(hWnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(&Window::HandleMsgThunk));
        // forward message to window class handler
        return pWnd->HandleMsg(hWnd, msg, wParam, lParam);
    }
    // if we get a message before the WM_NCCREATE message, handle with default handler
    return DefWindowProc(hWnd, msg, wParam, lParam);
}

LRESULT CALLBACK Window::HandleMsgThunk(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    // retrieve ptr to window class
    Window* const pWnd = reinterpret_cast<Window*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
    // forward message to window class handler
    return pWnd->HandleMsg(hWnd, msg, wParam, lParam);
}

LRESULT Window::HandleMsg(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    static WindowsMessageMap mm;
    OutputDebugString(mm(uMsg, lParam, wParam).c_str());
    switch (uMsg)
    {
    case WM_ERASEBKGND:
        break;
        // clear keystate when window loses focus to prevent input getting "stuck"
    case WM_KILLFOCUS:
        m_kbd.ClearState();
        break;
        /*********** KEYBOARD MESSAGES ***********/
    case WM_KEYDOWN:
    case WM_SYSKEYDOWN:
        if (!(lParam & 0x40000000) || m_kbd.AutorepeatIsEnabled())
        {
            m_kbd.OnKeyPressed(static_cast<unsigned char>(wParam));
        }
        break;
    case WM_KEYUP:
    case WM_SYSKEYUP:
        m_kbd.OnKeyReleased(static_cast<unsigned char>(wParam));
        break;
    case WM_CHAR:
        m_kbd.OnChar(static_cast<unsigned char>(wParam));
        break;
        /*********** END KEYBOARD MESSAGES ***********/

        /************* MOUSE MESSAGES ****************/
    case WM_MOUSEMOVE:
    {
                         const POINTS pt = MAKEPOINTS(lParam);
                         // in client region -> log move, and log enter + capture mouse (if not previously in window)
                         if (pt.x >= 0 && pt.x < m_width && pt.y >= 0 && pt.y < m_height)
                         {
                             m_mouse.OnMouseMove(pt.x, pt.y);
                             if (!m_mouse.IsInWindow())
                             {
                                 SetCapture(hWnd);
                                 m_mouse.OnMouseEnter();
                             }
                         }
                         // not in client -> log move / maintain capture if button down
                         else
                         {
                             if (wParam & (MK_RBUTTON))
                             {
                                 m_mouse.OnMouseMove(pt.x, pt.y);
                             }
                             // button up -> release capture / log event for leaving
                             else
                             {
                                 ReleaseCapture();
                                 m_mouse.OnMouseLeave();
                             }
                         }
                         break;
    }
    case WM_LBUTTONDOWN:
    {
                           const POINTS pt = MAKEPOINTS(lParam);
                           m_mouse.OnLeftPressed(pt.x, pt.y);
                           //drag window move
                           ReleaseCapture();
                           SendMessage(hWnd, WM_NCLBUTTONDOWN, HTCAPTION, 0);
                           SendMessage(hWnd, WM_LBUTTONUP, NULL, NULL);
                           SetCapture(hWnd);
                           break;
    }
    case WM_RBUTTONDOWN:
    {
                           const POINTS pt = MAKEPOINTS(lParam);
                           m_mouse.OnRightPressed(pt.x, pt.y);
                           //active window
                           SetForegroundWindow(hWnd);
                           break;
    }
    case WM_LBUTTONUP:
    {
                         const POINTS pt = MAKEPOINTS(lParam);
                         m_mouse.OnLeftReleased(pt.x, pt.y);
                         break;
    }
    case WM_RBUTTONUP:
    {
                         const POINTS pt = MAKEPOINTS(lParam);
                         m_mouse.OnRightReleased(pt.x, pt.y);
                         if (!IsInWindow(pt))
                         {
                             ReleaseCapture();
                             m_mouse.OnMouseLeave();
                         }
                         break;
    }
    case WM_MOUSEWHEEL:
    {
                          const POINTS pt = MAKEPOINTS(lParam);
                          const int delta = GET_WHEEL_DELTA_WPARAM(wParam);
                          m_mouse.OnWheelDelta(pt.x, pt.y, delta);
                          break;
    }
        /************** END MOUSE MESSAGES **************/
    case WM_CLOSE:
        //发送PostQuitMessage消息但不销毁窗口 交给析构函数销毁窗口
        Window::m_hWndCount--;
        if (Window::m_hWndCount < 1)
        {
            PostQuitMessage(0);
        }
        break;
    default:
    {
    }
    }
    return DefWindowProc(hWnd, uMsg, wParam, lParam);
}