#pragma once
#include "Scene.h"
#include <Vector2.h>
#include <Texture.h>
class LocalMap;
class WorldMap;
class LocalScene : public Scene
{
    enum class Display
    {
        TileSet,
        Data,
    };

    enum class CreatureType
    {
        Unknown,
        Green,
        Snow,
        Worm,
        Pink,
        Bug,
        Dry,
    };
    
private:
    Display m_display = Display::TileSet;
    LocalMap* m_pLocalMap = nullptr;

    E2::Texture m_localView;


    // TODO: 
    WorldMap* m_pWorldMap = nullptr;
    E2::Vector2 m_startPos;
    E2::Vector2 m_endPos;

    int m_currentCreatureCount = 0;
    int m_maxCreatureCount = 0;
    
public:
    ~LocalScene();
    virtual bool Init() override;
    virtual void Update(float deltaTime) override;
    virtual SceneId GetId() override { return SceneId::LocalScene; }
    virtual void End() override;

    void InputCheck();
    void InitLocalMap();

    LocalMap* GetLocalMap() { return m_pLocalMap; }
    void SetLocalView(E2::Texture texture) { m_localView = texture; }

    void SetWorldMap(WorldMap* pWorld) { m_pWorldMap = pWorld; }
    void SetStartAndEnd(int startX, int startY, int endX, int endY);
    void SetStartAndEnd(E2::Vector2 start, E2::Vector2 end);

    void SpawnCreatures();

    //nav
    E2::Vector2f GetRandomNearPositionInNav(E2::Vector2f origin, float radius);
    std::vector<E2::Vector2f> BuildPath(E2::Vector2f from, E2::Vector2f to);
};