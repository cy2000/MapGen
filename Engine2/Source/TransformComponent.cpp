#include "pch.h"
#include "TransformComponent.h"
#include "GameObject.h"
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
}

E2::TransformComponent::~TransformComponent()
{
}

void E2::TransformComponent::Update(float deltaTime)
{
}

void E2::TransformComponent::Draw()
{
}

void E2::TransformComponent::SetRotation(float degree)
{
    //truncate the angle to make it fit 0~360
    int wholeDegree = (int)degree;
    float desDgree = degree -(float)wholeDegree;
    wholeDegree = wholeDegree % 360;
    m_rotation = (float)wholeDegree + desDgree;
}
