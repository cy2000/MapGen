#pragma once
#include "Engine.h"

inline E2::Engine& GetEngine() { return E2::Engine::Get(); }

inline void DrawRect(const E2::Rect& rect, const E2::Color& color)
{
    E2::Engine::Get().DrawRect(rect, color);
}

inline void DrawRectOutline(const E2::Rect& rect, const E2::Color& color)
{
    E2::Engine::Get().DrawRectOutline(rect, color);
}

