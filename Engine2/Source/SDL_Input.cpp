#include "pch.h"
#include "Engine.h"
#include "Event.h"

static E2::Keyboard::Key TranslateScancode(SDL_Scancode code)
{
    if (code >= SDL_SCANCODE_A && code <= SDL_SCANCODE_0)
    {
        return static_cast<E2::Keyboard::Key>(code - SDL_SCANCODE_A);
    }
    else
    {
        switch (code)
        {
        case SDL_SCANCODE_SPACE: return E2::Keyboard::Key::Space; break;
        case SDL_SCANCODE_RETURN: return E2::Keyboard::Key::Return; break;
        case SDL_SCANCODE_BACKSPACE: return E2::Keyboard::Key::BackSpace; break;
        case SDL_SCANCODE_ESCAPE: return E2::Keyboard::Key::Escape; break;

        case SDL_SCANCODE_LEFT: return E2::Keyboard::Key::Left; break;
        case SDL_SCANCODE_RIGHT: return E2::Keyboard::Key::Right; break;
        case SDL_SCANCODE_UP: return E2::Keyboard::Key::Up; break;
        case SDL_SCANCODE_DOWN: return E2::Keyboard::Key::Down; break;

        case SDL_SCANCODE_LALT: return E2::Keyboard::Key::LeftAlt; break;
        case SDL_SCANCODE_LCTRL: return E2::Keyboard::Key::LeftCtrl; break;
        case SDL_SCANCODE_LSHIFT: return E2::Keyboard::Key::LeftShift; break;

        default: assert("Key missing!\n"); return E2::Keyboard::Key::KeyCodeMax; break;
        }
    }
}

static E2::Mouse::Button TranslateMouseButton(uint8_t button)
{
    switch (button)
    {
    case SDL_BUTTON_LEFT: return E2::Mouse::Button::Left; break;  //left
    case SDL_BUTTON_MIDDLE: return E2::Mouse::Button::Middle; break;  //middle
    case SDL_BUTTON_RIGHT: return E2::Mouse::Button::Right; break;  //right
    default: assert("Button missing!\n"); return E2::Mouse::Button::ButtonMax; break;
    }
}

void E2::Device::ProcessInput()
{
    SDL_Event event;
    if (SDL_PollEvent(&event))
    {
        switch (event.type)
        {
        case SDL_QUIT: E2::Engine::Get().Quit(); break;

        case SDL_KEYDOWN:
            E2::Engine::Get().SetKeyState(TranslateScancode(event.key.keysym.scancode), true);
            break;
        case SDL_KEYUP:
            E2::Engine::Get().SetKeyState(TranslateScancode(event.key.keysym.scancode), false);
            break;

        case SDL_TEXTINPUT:
        {
            KeyPressEvent evt{ EventType::KeyBoardEvent,event.text.text[0] };
            E2::Engine::Get().Notify(&evt);
            break;
        }
        case SDL_MOUSEMOTION:
        {
            Engine::Get().SetMousePos(event.motion.x, event.motion.y);
            MouseMoveEvent evt{ EventType::MouseMovementEvent,event.motion.x, event.motion.y };
            E2::Engine::Get().Notify(&evt);
            break;
        }
        case SDL_MOUSEBUTTONDOWN:
            E2::Engine::Get().SetMouseState(TranslateMouseButton(event.button.button), true);
            break;

        case SDL_MOUSEBUTTONUP:
        {
            E2::Engine::Get().SetMouseState(TranslateMouseButton(event.button.button), false);
            MouseClickEvent evt{ EventType::MouseButtonUpEvent,event.motion.x, event.motion.y, event.button.button };
            E2::Engine::Get().Notify(&evt);
            break;
        }
        default: break;
        }
    }
}

bool E2::Device::PointInRect(int pX, int pY, int x, int y, int w, int h)
{
    SDL_Point point{ pX , pY};
    SDL_Rect rect{x,y,w,h};
    return SDL_PointInRect(&point,&rect);
}