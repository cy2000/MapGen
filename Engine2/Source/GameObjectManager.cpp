#include "pch.h"
#include "GameObjectManager.h"
#include "GameObject.h"

E2::GameObjectManager::~GameObjectManager()
{
    DestroyAll();
}

void E2::GameObjectManager::Update(float deltaTime)
{
    for (auto& [objectId, object] : m_gameObjects)
    {
        object->Update(deltaTime);
    }
}

void E2::GameObjectManager::RenderGameObjects()
{
    for (auto& [objectId, object] : m_gameObjects)
    {
        object->Draw();
    }
}

void E2::GameObjectManager::AddGameObject(GameObject* pObject)
{
    if (pObject)
    {
        m_gameObjects[pObject->GetId()] = pObject;
    }
}
void E2::GameObjectManager::RemoveGameObject(GameObjectId id)
{
    if (m_gameObjects.find(id) != m_gameObjects.end())
    {
        delete m_gameObjects[id];
        m_gameObjects[id] = nullptr;
        m_gameObjects.erase(id);
    }
}

void E2::GameObjectManager::DestroyAll()
{
    for (auto& [objectId, object] : m_gameObjects)
    {
        delete object;
        object = nullptr;
    }
    m_gameObjects.clear();
}
