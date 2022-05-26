#pragma once
#include <unordered_map>


namespace E2
{
    class GameObject;
    class GameObjectManager
    {
    public:
        using GameObjectId = size_t;
    private:
        std::unordered_map<GameObjectId,GameObject*> m_gameObjects;

    public:
        GameObjectManager() = default;
        ~GameObjectManager();
        void Update(float deltaTime);
        void RenderGameObjects();

        void AddGameObject(GameObject* pObject);
        void RemoveGameObject(GameObjectId id);
        void DestroyAll();

    };
}
