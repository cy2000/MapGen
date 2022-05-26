#pragma once

namespace E2
{
    enum class ComponentType
    {
        Base,
        Transform,
        Kinematic,
        Shape,
        Image,
        StateMachine,
    };

    class Component
    {
    protected:
        ComponentType m_type = ComponentType::Base;
    public:
        Component() = default;
        virtual ~Component() = default;
        virtual void Update(float deltaTime) {};
        virtual void Draw(){}
        ComponentType Type() const { return m_type; }
    };
}