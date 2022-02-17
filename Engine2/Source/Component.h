#pragma once

namespace E2
{
    class Component
    {
    public:
        Component() = default;
        virtual ~Component() = default;
        virtual void Update(float deltaTime) = 0;
        virtual void Draw() = 0;
    };
}