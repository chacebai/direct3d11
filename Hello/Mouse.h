#ifndef MOUSE_H
#define MOUSE_H

#include <queue>

class Mouse
{
    friend class Window;
public:
    class Event
    {
    public:
        enum class Type
        {
            LPress,
            LRelease,
            RPress,
            RRelease,
            WheelUp,
            WheelDown,
            Move,
            Invalid,
            Enter,
            Leave,
        };
    private:
        Type type;
        bool leftIsPressed;
        bool rightIsPressed;
        int x;
        int y;
    public:
        Event()
            :
            type(Type::Invalid),
            leftIsPressed(false),
            rightIsPressed(false),
            x(0),
            y(0)
        {}
        Event(Type type, const Mouse& parent)
            :
        type(type),
            leftIsPressed(parent.leftIsPressed),
            rightIsPressed(parent.rightIsPressed),
            x(parent.x),
            y(parent.y)
        {}
        bool IsValid() const
        {
            return type != Type::Invalid;
        }
        Type GetType() const
        {
            return type;
        }
        std::pair<int, int> GetPos() const
        {
            return{ x, y };
        }
        int GetPosX() const
        {
            return x;
        }
        int GetPosY() const
        {
            return y;
        }
        bool LeftIsPressed() const
        {
            return leftIsPressed;
        }
        bool RightIsPressed() const
        {
            return rightIsPressed;
        }
    };
public:
    Mouse() = default;
    Mouse(const Mouse&) = delete;
    Mouse& operator=(const Mouse&) = delete;
    std::pair<int, int> GetPos() const;
    int GetPosX() const;
    int GetPosY() const;
    bool IsInWindow() const;
    bool LeftIsPressed() const;
    bool RightIsPressed() const;
    Mouse::Event Read();
    bool IsEmpty() const
    {
        return buffer.empty();
    }
    void Flush();
private:
    void OnMouseMove(int x, int y);
    void OnMouseLeave();
    void OnMouseEnter();
    void OnLeftPressed(int x, int y);
    void OnLeftReleased(int x, int y);
    void OnRightPressed(int x, int y);
    void OnRightReleased(int x, int y);
    void OnWheelUp(int x, int y);
    void OnWheelDown(int x, int y);
    void TrimBuffer();
    void OnWheelDelta(int x, int y, int delta);
private:
    static const unsigned int bufferSize = 16u;
    int x;
    int y;
    bool leftIsPressed = false;
    bool rightIsPressed = false;
    bool isInWindow = false;
    int wheelDeltaCarry = 0;
    std::queue<Event> buffer;
};

#endif