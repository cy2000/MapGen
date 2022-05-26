#include "pch.h"
#include "GameObject.h"
#include "Component.h"
#include "TransformComponent.h"
#include "KinematicComponent.h"

static size_t s_id = 0;

E2::GameObject::GameObject()
    :m_pTransform{nullptr}
{
    ++s_id;

    m_pTransform = new TransformComponent(this);
    AddComponent(m_pTransform);
}

E2::GameObject::~GameObject()
{
    for (auto* p : m_components)
    {
        delete p;
    }
    m_components.clear();
}

size_t E2::GameObject::GetId() const
{
    return s_id;
}

void E2::GameObject::Update(float deltaTime)
{
    for (auto* p : m_components)
    {
        p->Update(deltaTime);
    }
}

void E2::GameObject::Draw()
{
    for (auto* p : m_components)
    {
        p->Draw();
    }
}

void E2::GameObject::AddComponent(Component* pComponent)
{
    m_components.push_back(pComponent);
}

void E2::GameObject::SetPosition(const Vector2f& pos)
{
    m_pTransform->SetPosition(pos);
}

void E2::GameObject::SetRotation(float rot)
{
    m_pTransform->SetRotation(rot);
}

E2::Vector2f E2::GameObject::GetPosition() const
{
    return m_pTransform->GetPosition();
}

E2::Component* E2::GameObject::GetComponent(ComponentType type)
{
    for (auto* pComp : m_components)
    {
        if (pComp->Type() == type)
        {
            return pComp;
        }
    }
    return nullptr;
}