#include "pch.h"
#include "GameObject.h"
#include "Component.h"
#include "TransformComponent.h"

E2::GameObject::GameObject()
    :m_pTransform{nullptr}
{
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
