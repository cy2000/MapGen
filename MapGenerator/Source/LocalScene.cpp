#include "MapGenerator.h"
#include "LocalScene.h"
#include "LocalMap.h"
#include "WorldMap.h"
#include "Grid.h"

#include "IdleState.h"
#include "WanderState.h"
#include "TransitionToWander.h"
#include "TransitionToIdle.h"

#include <FreeFunctions.h>
#include <GameObject.h>
#include <ImageComponent.h>
#include <TransformComponent.h>
#include <StateComponent.h>

#include <iostream>

constexpr int kLocalZoomInLevel = 3;
constexpr int kLocalHeightLevel = 10;

constexpr float kCreatureCountPerGrassTile = 0.05f;

constexpr float kMinIdleTime = 1.f;
constexpr float kMaxIdleTime = 5.f;
constexpr float kWanderRadius = 6.f; //unit as in tiles
constexpr float kWanderSpeed = 2.f; //unit as in pixels

LocalScene::~LocalScene()
{
    End();
}

bool LocalScene::Init()
{
    std::cout << "Local Scene Init.\n";
    if (m_pLocalMap)
    {
        delete m_pLocalMap;
    }
    m_pLocalMap = new LocalMap;

    MapGenerator::Get().DataTransfer();
    InitLocalMap();

    SpawnCreatures();

    return true;
}

void LocalScene::Update(float deltaTime)
{
    InputCheck();

    switch (m_display)
    {
    case Display::TileSet: DrawTexture(m_localView, nullptr, nullptr); break;
    case Display::Data: m_pLocalMap->DrawAreaColor(); break;
    }
}

void LocalScene::End()
{
    if (m_pLocalMap)
    {
        delete m_pLocalMap;
        m_pLocalMap = nullptr;
        GetEngine().DestroyTexture(m_localView);
        GetEngine().DestroyAllGameObject();
        std::cout << "Local Scene Destroyed.\n";
    }
}

void LocalScene::InputCheck()
{
    if (GetEngine().IsKeyPressed(E2::Keyboard::Key::Escape))
    {
        MapGenerator::Get().BackToWorld();
    }
    if (GetEngine().IsKeyPressed(E2::Keyboard::Key::Num1))
    {
        m_display = Display::TileSet;
    }
    if (GetEngine().IsKeyPressed(E2::Keyboard::Key::Num2))
    {
        m_display = Display::Data;
    }
}

void LocalScene::InitLocalMap()
{
    auto tileSize = GetEngine().GetGameSetting().m_tileSize;
    int startX = m_startPos.x / tileSize;
    int startY = m_startPos.y / tileSize;
    int endX = m_endPos.x / tileSize;
    int endY = m_endPos.y / tileSize;

    // divide by kLocalZoomInLevel 
    // break down world map tiles : if kLocalZoomInLevel == 4, 1 world tile becomes 16 local tiles
    auto localTileSize = GetEngine().GetWindowSize().x / (endX - startX) / kLocalZoomInLevel;
    m_pLocalMap->Reset((endX - startX) * kLocalZoomInLevel, (endY - startY) * kLocalZoomInLevel, localTileSize);

    int worldX = 0;
    int worldY = 0;
    for (int y = 0; y < (endY - startY) * kLocalZoomInLevel; ++y)
    {
        for (int x = 0; x < (endX - startX) * kLocalZoomInLevel; ++x)
        {
            worldX = x / kLocalZoomInLevel + startX;
            worldY = y / kLocalZoomInLevel + startY;
            m_pLocalMap->CopyTileInfo(x, y, m_pWorldMap->GetTile(worldX, worldY));
        }
    }
    m_pLocalMap->LoadArea(kLocalHeightLevel);
}

void LocalScene::SetStartAndEnd(int startX, int startY, int endX, int endY)
{
    m_startPos.x = startX;
    m_startPos.y = startY;
    m_endPos.x = endX;
    m_endPos.y = endY;
}

void LocalScene::SetStartAndEnd(E2::Vector2 start, E2::Vector2 end)
{
    m_startPos = start;
    m_endPos = end;
}

void LocalScene::SpawnCreatures()
{
    std::unordered_map<CreatureType, E2::Texture> creatureTextureMap;
    creatureTextureMap[CreatureType::Bug] = GetEngine().CreateTexture("Assets/Creature/Bug.png");
    creatureTextureMap[CreatureType::Snow] = GetEngine().CreateTexture("Assets/Creature/Snow.png");
    creatureTextureMap[CreatureType::Green] = GetEngine().CreateTexture("Assets/Creature/Green.png");
    creatureTextureMap[CreatureType::Worm] = GetEngine().CreateTexture("Assets/Creature/Worm.png");
    creatureTextureMap[CreatureType::Pink] = GetEngine().CreateTexture("Assets/Creature/Pink.png");
    creatureTextureMap[CreatureType::Dry] = GetEngine().CreateTexture("Assets/Creature/Dry.png");

    auto goodTiles = m_pLocalMap->GetSpawnAbleTiles();

    if (goodTiles.empty())
    {
        return;
    }

    //calculate how many creature there shall be
    int creatureCount = (int)((float)goodTiles.size() * kCreatureCountPerGrassTile);

    for (int i = 0; i < creatureCount; ++i)
    {
        //find a random spot
        int tileId = -1;
        while (tileId == -1)
        {
            auto choice = E2::Random(0, goodTiles.size() - 1);
            tileId = goodTiles[choice];
            goodTiles[choice] = -1;
        }
        auto* pTile = m_pLocalMap->GetTile(tileId);

        auto creatureType = CreatureType::Unknown;
        switch (pTile->biomeId)
        {
        case 7:  creatureType = CreatureType::Green; break;
        case 8:  creatureType = CreatureType::Bug; break;
        case 9:  creatureType = CreatureType::Green; break;
        case 10: creatureType = CreatureType::Pink; break;
        case 11: creatureType = CreatureType::Worm; break;
        case 12: creatureType = CreatureType::Worm; break;
        case 13: creatureType = CreatureType::Snow; break;
        case 14: creatureType = CreatureType::Pink; break;
        case 15: creatureType = CreatureType::Dry; break;
        case 22: creatureType = CreatureType::Snow; break;
        default: assert(false && "No creature type."); break;
        }
        //new object
        auto* pNewObject = new E2::GameObject;
        auto* pImageComp = new E2::ImageComponent(pNewObject, creatureTextureMap[creatureType]);
        pNewObject->AddComponent(pImageComp);

        auto rect = pTile->rect;
        pNewObject->GetTransform()->SetPosition((float)rect.x, (float)rect.y);
        pNewObject->GetTransform()->SetDimension((float)rect.w, (float)rect.h);
        GetEngine().AddGameObject(pNewObject);

        //Add states
        auto* pIdleState = new IdleState(pNewObject,kMinIdleTime,kMaxIdleTime);
        auto* pWanderState = new WanderState(pNewObject,kWanderRadius,kWanderSpeed);
        auto* pTransitionToWander = new TransitionToWander(pNewObject);
        auto* pTransitionToIdle = new TransitionToIdle(pNewObject);
        auto* pStateMachine = new E2::StateComponent();
        pStateMachine->AddState(pIdleState, pTransitionToWander, pWanderState);
        pStateMachine->AddState(pWanderState, pTransitionToIdle, pIdleState);

        pStateMachine->SetState(pIdleState);
        pNewObject->AddComponent(pStateMachine);
    }
}

E2::Vector2f LocalScene::GetRandomNearPositionInNav(E2::Vector2f origin, float radius)
{
    return m_pLocalMap->GetRandomNearPositionInNav(origin,radius);
}

std::vector<E2::Vector2f> LocalScene::BuildPath(E2::Vector2f from, E2::Vector2f to)
{
    return m_pLocalMap->BuildPath(from, to);
}
