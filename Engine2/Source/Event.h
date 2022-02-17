#pragma once

namespace E2
{
    enum class EventType : uint8_t
    {
        KeyBoardEvent,
        MouseButtonDownEvent,
        MouseButtonUpEvent,
        MouseMovementEvent,
    };

    struct Event
    {
        EventType m_type;
    };

    struct KeyPressEvent : public Event
    {
        char key;
    };

    struct MouseMoveEvent : public Event
    {
        int x;
        int y;
    };

    struct MouseClickEvent : public Event
    {
        int x;
        int y;
        int button;
    };

}