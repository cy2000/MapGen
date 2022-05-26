#pragma once
#include "Vector2.h"
#include "EventListener.h"
#include "Component.h"

#include <vector>
#include <typeinfo>
#include <string>

namespace E2
{
    class TransformComponent;
    class KinematicComponent;

    class GameObject : public EventListener
    {
    private:
        std::vector<Component*> m_components;
        TransformComponent* m_pTransform;
    public:
        GameObject();
        ~GameObject();

        size_t GetId() const;

        void Update(float deltaTime);
        void Draw();

        void SetPosition(const Vector2f& pos);
        void SetRotation(float rot);

        void AddComponent(Component* pComponent);
        TransformComponent* GetTransform() { return m_pTransform; }
        KinematicComponent* GetKinematic() { return nullptr; }

        E2::Vector2f GetPosition() const;
        Component* GetComponent(ComponentType type);

    };
}