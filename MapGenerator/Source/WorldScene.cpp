#include "MapGenerator.h"
#include "WorldScene.h"
#include "WorldMap.h"
#include "WorldMapUIRect.h"

#include <FreeFunctions.h>
#include <EventType.h>
#include <CallBack.h>

#include <iostream>

constexpr int kEquator = 480;
constexpr int kTemperatureComplexity = 5;
constexpr int kMaxWorldSplits = 6;
constexpr const char* kBiome = "Script/Biome.lua";

constexpr float kMinCityHeight = 0.5f;
constexpr float kMaxCityHeight = 0.7f;

WorldScene::~WorldScene()
{
    End();
}

bool WorldScene::Init()
{
    SetWorldSeed();
    
    GenerateWorld();

    InitUI();

    std::cout << "World Scene Init.\n";
    return true;
}

void WorldScene::Update(float deltaTime)
{
    InputCheck();

    DrawTexture(m_worldView, nullptr, nullptr);
}

void WorldScene::End()
{
    delete m_pWorldMap;
    m_pWorldMap = nullptr;
    GetEngine().ClearUI();
    GetEngine().ClearListener();
    GetEngine().DestroyTexture(m_worldView);
    std::cout << "World Scene Destroyed.\n";
}

void WorldScene::Reset()
{
    m_selectionRect->SetVisable(true);
}

void WorldScene::SetWorldSeed()
{
    auto mapSeed = MapGenerator::Get().GetMapSeed();
    if (mapSeed == 0)
    {
        mapSeed = std::time(nullptr);
    }
    E2::Seed(mapSeed);
}

void WorldScene::GenerateWorld()
{
    auto& gameSetting = GetEngine().GetGameSetting();
    m_pWorldMap = new WorldMap(gameSetting.m_windowWidth, gameSetting.m_windowHeight, gameSetting.m_tileSize);

    m_pWorldMap->Reset();
    m_pWorldMap->GenerateBasicHeightNoise();
    m_pWorldMap->ReShapeHeightNoise();
    m_pWorldMap->SplitRegion(kMaxWorldSplits);
    m_pWorldMap->AdjustRegionHeightNoise();
    m_pWorldMap->SmoothNoiseAroundSplitLines();
    m_pWorldMap->ReShapeHeightNoise();
    m_pWorldMap->GenerateTemperatureNoise(kEquator, kTemperatureComplexity);
    m_pWorldMap->GenerateBiome(kBiome);

    m_pWorldMap->FoundCity(kMinCityHeight, kMaxCityHeight);
    m_pWorldMap->LinkCities();
    m_pWorldMap->BuildRoads();

    GetEngine().CleanRenderer();
    m_pWorldMap->DrawWorld();
    SetWorldView(GetEngine().CombineCurrentView());
}

void WorldScene::InputCheck()
{
    if (GetEngine().IsKeyPressed(E2::Keyboard::Key::Escape))
    {
        MapGenerator::Get().Restart();
    }
}

void WorldScene::InitUI()
{
    auto windowW = E2::Engine::Get().GetWindowSize().x;
    auto windowH = E2::Engine::Get().GetWindowSize().y;

    auto* pSelectionRect = new WorldMapUIRect(E2::Red::kRed);
    pSelectionRect->SetDimension({ E2::UICoordType::Percentage,0.1f }, { E2::UICoordType::Percentage,0.1f });
    pSelectionRect->Reposition(0, 0, windowW, windowH);
    auto selectionRectCallBack = [](std::uintptr_t address)
    {
        MapGenerator::Get().GenerateLocalMap();
        reinterpret_cast<WorldScene*>(address)->m_selectionRect->SetVisable(false);
    };

    pSelectionRect->SetCallBack(new E2::CallBack(reinterpret_cast<std::uintptr_t>(this), selectionRectCallBack));

    m_selectionRect = pSelectionRect;
    GetEngine().AddUIElement(m_selectionRect);
    GetEngine().RegisterListener(m_selectionRect, kMouseEvent);
}

E2::Vector2 WorldScene::GetSelectionStart()
{
    return E2::Vector2(m_selectionRect->GetActualX(), (m_selectionRect->GetActualY()));
}

E2::Vector2 WorldScene::GetSelectionEnd()
{
    return E2::Vector2(m_selectionRect->GetActualX()+ m_selectionRect->GetActualW(), (m_selectionRect->GetActualY() + m_selectionRect->GetActualH()));
}
