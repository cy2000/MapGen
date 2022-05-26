#pragma once
#include "Scene.h"

#include <IGame.h>
#include <Vector2.h>
#include <Rect.h>
#include <vector>


namespace E2
{
    struct Color;
    class UIElement;
    class GameObject;
}

class Scene;
class MapGenerator : public E2::IGame
{
private:
    size_t m_mapSeed;

    //Scene
    Scene* m_pMenuScene = nullptr;
    Scene* m_pWorldScene = nullptr;
    Scene* m_pLocalScene = nullptr;

    Scene* m_pCurrentScene = nullptr;
    Scene* m_pPreviousScene = nullptr;
    Scene* m_pNextScene = nullptr;

    bool m_preserveCurrentScene = false;

public:
    static MapGenerator& Get();
    virtual ~MapGenerator();
    virtual bool Init() final;
    virtual void Update(float deltaTime) final;
    virtual const char* Config() final;

    void SetMapSeed(std::string& seed);
    void SetMapSeed(size_t seed);
    size_t GetMapSeed()const { return m_mapSeed; }

    void GenerateWorld();

    void ChangeScene();
    Scene* GetScene(Scene::SceneId id);

    void SetPreserveCurrentScene(bool b) { m_preserveCurrentScene = b; }
    void Restart();

    void GenerateLocalMap();
    void DataTransfer();

    void BackToWorld();

private:
    MapGenerator();
};