#pragma once
#include <vector>

namespace E2
{
    class Component;
    class TransformComponent;
    class GameObject
    {
    private:
        std::vector<Component*> m_components;
        TransformComponent* m_pTransform;
    public:
        GameObject();
        ~GameObject();
        void Update(float deltaTime);
        void Draw();

        void AddComponent(Component* pComponent);
        TransformComponent* GetTransform() { return m_pTransform; }

    };
}