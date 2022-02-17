#pragma once
#include "Component.h"
#include "Vector2.h"
namespace E2
{
    class GameObject;
    class TransformComponent : public Component
    {
    private:
        GameObject* m_pOwner;
        Vector2f m_position;
        Vector2f m_dimension;
        float m_rotation;
    public:
        TransformComponent(GameObject* pOwner);
        TransformComponent(GameObject* pOwner, float x, float y, float w, float h, float rotation);
        virtual ~TransformComponent();

        virtual void Update(float deltaTime);
        virtual void Draw();

        void SetPosition(float x, float y) { m_position.x = x; m_position.y = y; }
        void SetDimension(float w, float h) { m_dimension.x = w; m_dimension.y = h; }
        void SetRotation(float degree);

        Vector2f GetPosition() const { return m_position; }
        Vector2f GetDimension() const { return m_dimension; }
        float GetRotation() const { return m_rotation; }
    };
}