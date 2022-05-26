#include "pch.h"
#include "TransformComponent.h"
#include "GameObject.h"

#include <cmath>


E2::TransformComponent::TransformComponent(GameObject* pOwner)
    : TransformComponent(pOwner,0,0,0,0,0)
{
}

E2::TransformComponent::TransformComponent(GameObject* pOwner, float x, float y, float w, float h, float rotation)
    : m_pOwner{ pOwner }
    , m_position{ x,y }
    , m_dimension{ w,h }
    , m_rotation{ rotation }
{
    m_type = ComponentType::Transform;
}

void E2::TransformComponent::SetRotation(Vector2f front)
{
    //x axis goes east
    Vector2f xAxis{ 1.f,0 };
    //y axis goes south
    Vector2f trueFront = { -front.x, -front.y };
    m_rotation = std::acosf((Vector2f::Dot(trueFront, xAxis)) / trueFront.Magnitude());
    if (trueFront.y > 0)
    {
        m_rotation += (float)M_PI;
    }
}

