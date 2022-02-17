#include "MapGenerator.h"
#include "Grid.h"
#include "WorldMap.h"
#include "LocalMap.h"
#include "HeightBiomes.h"
#include "WorldMapUIRect.h"

#include <PerlinNoise.h>
#include <Random.h>
#include <FreeFunctions.h>
#include <Rect.h>
#include <Color.h>
#include <SimpleMath.h>
#include <Keyboard.h>
#include <HashString.h>

#include <UITextInput.h>
#include <UILabel.h>
#include <UIImage.h>
#include <UIRect.h>
#include <UIButton.h>

#include <iostream>

const E2::GameInfo kInfo{"Map Generator",1080,720,4};
constexpr int kUIRectSize = 10;
constexpr int kLocalHeightLevel = 12;
constexpr int kLocalZoomInLevel = 4;
constexpr int kAutoSlitTimes = 5;

constexpr const char* kFont = "Assets/UI/joystix_monospace.ttf";
constexpr const char* kBackGround = "Assets/UI/UIBackGround.png";

MapGenerator& MapGenerator::Get()
{
    static MapGenerator instance;
    return instance;
}

MapGenerator::MapGenerator()
    : m_onMenu{true}
    , m_hasLocalMap{false}
    , m_drawSplitLines{false}
    , m_pWorldMap{nullptr}
    , m_pLocalMap{nullptr}
    , m_mapSeed{0}
    , m_display{DisplayType::Biome}
    , m_pMenuBackGround{nullptr}
    , m_pTitle{nullptr}
    , m_pVersion{nullptr}
    , m_pInputBox{nullptr}
    , m_pRandomSeedButton{nullptr}
    , m_pGenerateMapButton{nullptr}
    , m_pQuitButton{nullptr}
    , m_selectRect{nullptr}
{
    //
}



MapGenerator::~MapGenerator()
{
    delete m_pWorldMap;
    m_pWorldMap = nullptr;
    delete m_pLocalMap;
    m_pLocalMap = nullptr;
}

bool MapGenerator::Init()
{
    //GetEngine().ToggleManuallyUpdate();
    //GetEngine().DrawUIFrame(true);
    E2::Seed(std::time(nullptr));
    InitMenu();

    return true;
}

void MapGenerator::Update()
{
    if (m_onMenu)
    {
        return;
    }
    InputCheck();

    switch (m_display)
    {
    case DisplayType::Noise: m_pWorldMap->DrawHeightNoise(); break;
    case DisplayType::Biome:
        m_selectRect->SetVisable(true);
        m_pWorldMap->DrawHeightBiome();
        break;
    case DisplayType::LocalBiome: m_pLocalMap->Draw();  break;
    case DisplayType::LocalHeight: m_pLocalMap->DrawHeight(); break;
    case DisplayType::LocalTerrace: m_pLocalMap->DrawHeightTerrace(kLocalHeightLevel); break;
    }

    if (m_drawSplitLines)
    {
        if (m_display == DisplayType::Noise || m_display == DisplayType::Biome)
        {
            m_pWorldMap->DrawSplitLines();
        }
        if (m_display == DisplayType::LocalBiome)
        {
            //m_pLocalMap->DrawSplitLines();
        }
    }
}

void MapGenerator::ShutDown()
{
}

const E2::GameInfo& MapGenerator::GetInfo()
{
    return kInfo;
}

void MapGenerator::InitMenu()
{
    GetEngine().LoadFile(kFont);
    GetEngine().LoadFile(kBackGround);

    auto windowW = E2::Engine::Get().GetWindowSize().x;
    auto windowH = E2::Engine::Get().GetWindowSize().y;
    E2::Color textColor{ E2::Red::kCoral };
    E2::Color inputColor{ E2::Yellow::kLemon };
    E2::Color boxColor{ E2::Blue::kTiffanyBlue };

    //background image
    auto pImage = new E2::UIImage(GetEngine().CreateTexture(kBackGround), {}, { 0,0,windowW,windowH }
        , { E2::UICoordType::Percentage,0.5f }
        , { E2::UICoordType::Percentage,0.5f }
        , { E2::UICoordType::Percentage,0.9f }
        , { E2::UICoordType::Percentage,0.9f }
        , { E2::UICoordType::Percentage,0.5f }
        , { E2::UICoordType::Percentage,0.5f });

    pImage->SetSlice({ E2::UICoordType::Percentage,0.25f }
        , { E2::UICoordType::Percentage,0.75f }
        , { E2::UICoordType::Percentage,0.25f }
        , { E2::UICoordType::Percentage,0.75f });
    m_pMenuBackGround = pImage;
    GetEngine().AddUIElement(m_pMenuBackGround);

    //Title
    std::string title{ "MapGen" };
    m_pTitle = new E2::UILabel(GetEngine().CreateFont(kFont, 80), title, E2::Red::kCoral);
    m_pTitle->SetPosition({ E2::UICoordType::Percentage,0.5f }, { E2::UICoordType::Percentage,0.3f });
    m_pTitle->SetOrigin({ E2::UICoordType::Percentage,0.5f }, { E2::UICoordType::Percentage,0.5f });
    m_pTitle->SetDimension({ E2::UICoordType::Percentage,0.37f }, { E2::UICoordType::Percentage,0.15f });
    m_pTitle->Reposition(0, 0, windowW, windowH);
    GetEngine().AddUIElement(m_pTitle);

    //Version
    std::string version{ "ver 0.1.1" };
    m_pVersion = new E2::UILabel(GetEngine().CreateFont(kFont, 10), version, E2::Red::kCoral);
    m_pVersion->SetPosition({ E2::UICoordType::Percentage,0.7f }, { E2::UICoordType::Percentage,0.35f });
    m_pVersion->SetOrigin({ E2::UICoordType::Percentage,0.5f }, { E2::UICoordType::Percentage,0.5f });
    m_pVersion->SetDimension({ E2::UICoordType::Percentage,0.07f }, { E2::UICoordType::Percentage,0.02f });
    m_pVersion->Reposition(0, 0, windowW, windowH);
    GetEngine().AddUIElement(m_pVersion);

    //Seed
    std::string seedString{ "Seed:" };
    auto* pSeed = new E2::UILabel(GetEngine().CreateFont(kFont, 20), seedString, E2::Red::kCoral);
    pSeed->SetPosition({ E2::UICoordType::Percentage,0.2f }, { E2::UICoordType::Percentage,0.5f });
    pSeed->SetOrigin({ E2::UICoordType::Percentage,0.5f }, { E2::UICoordType::Percentage,0.5f });
    pSeed->SetDimension({ E2::UICoordType::Percentage,0.08f }, { E2::UICoordType::Percentage,0.05f });
    pSeed->Reposition(0, 0, windowW, windowH);
    GetEngine().AddUIElement(pSeed);

    //Input box
    m_pInputBox = new E2::UITextInput(GetEngine().CreateFont(kFont, 20), inputColor, boxColor);
    m_pInputBox->SetPosition({ E2::UICoordType::Percentage,0.5f }, { E2::UICoordType::Percentage,0.5f });
    m_pInputBox->SetOrigin({ E2::UICoordType::Percentage,0.5f }, { E2::UICoordType::Percentage,0.5f });
    m_pInputBox->SetDimension({ E2::UICoordType::Percentage,0.5f }, { E2::UICoordType::Percentage,0.05f });
    m_pInputBox->Reposition(0, 0, windowW, windowH);
    GetEngine().AddUIElement(m_pInputBox);
    GetEngine().RegisterListener(m_pInputBox);

    //RandomSeed Button
    auto pRandomSeedButton = new E2::UIButton(E2::Blue::kSkyBlue,E2::Yellow::kGoldYellow,E2::Blue::kCobalt);
    pRandomSeedButton->SetPosition({ E2::UICoordType::Percentage,0.5f }, { E2::UICoordType::Percentage,0.57f });
    pRandomSeedButton->SetOrigin({ E2::UICoordType::Percentage,0.5f }, { E2::UICoordType::Percentage,0.5f });
    pRandomSeedButton->SetDimension({ E2::UICoordType::Pixels, 100 }, { E2::UICoordType::Pixels, 50 });

    std::string SeedButtonString{ "Random" };
    auto pSeedButtonText = new E2::UILabel(GetEngine().CreateFont(kFont, 15), SeedButtonString, E2::Mono::kDark);
    pSeedButtonText->SetPosition({E2::UICoordType::Percentage, 0.4f}, { E2::UICoordType::Percentage,0.5f });
    pSeedButtonText->SetOrigin({E2::UICoordType::Percentage, 0.5f}, { E2::UICoordType::Percentage,0.5f });
    pSeedButtonText->SetDimension({ E2::UICoordType::Percentage, 0.5f }, { E2::UICoordType::Percentage,0.5f });
    pRandomSeedButton->AddChild(pSeedButtonText);

    pRandomSeedButton->Reposition(0, 0, windowW, windowH);
    pRandomSeedButton->SetCallBack([]()
        {
            auto seed = std::to_string(E2::Random());
            MapGenerator::Get().SetMapSeed(seed);
        });
    m_pRandomSeedButton = pRandomSeedButton;
    GetEngine().AddUIElement(m_pRandomSeedButton);
    GetEngine().RegisterListener(m_pRandomSeedButton);

    //GenerateMap Button
    auto pGenerateMapButton = new E2::UIButton(E2::Blue::kSkyBlue, E2::Yellow::kGoldYellow, E2::Blue::kCobalt);
    pGenerateMapButton->SetPosition({ E2::UICoordType::Percentage,0.5f }, { E2::UICoordType::Percentage,0.75f });
    pGenerateMapButton->SetOrigin({ E2::UICoordType::Percentage,0.5f }, { E2::UICoordType::Percentage,0.5f });
    pGenerateMapButton->SetDimension({ E2::UICoordType::Pixels, 100 }, { E2::UICoordType::Pixels, 50 });

    std::string mapButtonString{ "Generate" };
    auto pMapButtonText = new E2::UILabel(GetEngine().CreateFont(kFont, 15), mapButtonString, E2::Mono::kDark);
    pMapButtonText->SetPosition({ E2::UICoordType::Percentage, 0.27f }, { E2::UICoordType::Percentage,0.5f });
    pMapButtonText->SetOrigin({ E2::UICoordType::Percentage, 0.5f }, { E2::UICoordType::Percentage,0.5f });
    pMapButtonText->SetDimension({ E2::UICoordType::Percentage, 0.5f }, { E2::UICoordType::Percentage,0.5f });
    pGenerateMapButton->AddChild(pMapButtonText);

    pGenerateMapButton->Reposition(0, 0, windowW, windowH);
    pGenerateMapButton->SetCallBack([]()
        {
            MapGenerator::Get().CloseMenu();
            MapGenerator::Get().InitWorldMap();
            MapGenerator::Get().InitWorldMapSelectionRect();
#ifdef NDEBUG
            MapGenerator::Get().AutoGenerateMap();
#endif // NDEBUG
        });
    m_pGenerateMapButton = pGenerateMapButton;
    GetEngine().AddUIElement(m_pGenerateMapButton);
    GetEngine().RegisterListener(m_pGenerateMapButton);

    //Quit Button
    auto pQuitButton = new E2::UIButton(E2::Blue::kSkyBlue, E2::Yellow::kGoldYellow, E2::Blue::kCobalt);
    pQuitButton->SetPosition({ E2::UICoordType::Percentage,0.83f }, { E2::UICoordType::Percentage,0.85f });
    pQuitButton->SetOrigin({ E2::UICoordType::Percentage,0.f }, { E2::UICoordType::Percentage,0.f });
    pQuitButton->SetDimension({ E2::UICoordType::Pixels, 100 }, { E2::UICoordType::Pixels, 50 });

    std::string quitButtonString{ "Quit" };
    auto pQuitButtonText = new E2::UILabel(GetEngine().CreateFont(kFont, 15), quitButtonString, E2::Mono::kDark);
    pQuitButtonText->SetPosition({ E2::UICoordType::Percentage, 0.5f }, { E2::UICoordType::Percentage,0.5f });
    pQuitButtonText->SetOrigin({ E2::UICoordType::Percentage, 0.5f }, { E2::UICoordType::Percentage,0.5f });
    pQuitButtonText->SetDimension({ E2::UICoordType::Percentage, 0.5f }, { E2::UICoordType::Percentage,0.5f });
    pQuitButton->AddChild(pQuitButtonText);

    pQuitButton->Reposition(0, 0, windowW, windowH);
    pQuitButton->SetCallBack([]() {GetEngine().Quit(); });
    m_pQuitButton = pQuitButton;
    GetEngine().AddUIElement(m_pQuitButton);
    GetEngine().RegisterListener(m_pQuitButton);
}

void MapGenerator::CloseMenu()
{
    auto* pInput = static_cast<E2::UITextInput*>(m_pInputBox);
    m_mapSeed = E2::HashString(pInput->GetText());

    m_onMenu = false;
    m_pMenuBackGround = nullptr;
    m_pTitle = nullptr;
    m_pVersion = nullptr;
    m_pInputBox = nullptr;
    m_pRandomSeedButton = nullptr;
    m_pGenerateMapButton = nullptr;
    m_pQuitButton = nullptr;
    GetEngine().ClearListener();
    GetEngine().ClearUI();
}

void MapGenerator::InitWorldMap()
{
    if (m_mapSeed == 0)
    {
        m_mapSeed = std::time(nullptr);
    }
    E2::Seed(m_mapSeed);

    m_pWorldMap = new WorldMap(kInfo.m_windowWidth, kInfo.m_windowHeight, kInfo.m_tileSize);
    m_pWorldMap->GenerateBasicHeightNoise();
    m_pWorldMap->ReShapeHeightNoise();
    m_pLocalMap = new LocalMap;
}

void MapGenerator::InitWorldMapSelectionRect()
{
    auto windowW = E2::Engine::Get().GetWindowSize().x;
    auto windowH = E2::Engine::Get().GetWindowSize().y;

    m_selectRect = new WorldMapUIRect(E2::Red::kRed);
    m_selectRect->SetDimension({E2::UICoordType::Percentage,0.1f}, { E2::UICoordType::Percentage,0.1f });
    m_selectRect->Reposition(0, 0, windowW, windowH);
    GetEngine().AddUIElement(m_selectRect);
    GetEngine().RegisterListener(m_selectRect);
}

void MapGenerator::LoadLocalMap()
{
    int startX = m_selectRect->GetActualX() / kInfo.m_tileSize;
    int startY = m_selectRect->GetActualY() / kInfo.m_tileSize;
    int endX = (m_selectRect->GetActualX()+ m_selectRect->GetActualW()) / kInfo.m_tileSize;
    int endY = (m_selectRect->GetActualY()+ m_selectRect->GetActualH()) / kInfo.m_tileSize;
    auto localTileSize = kInfo.m_windowWidth / (endX - startX) / kLocalZoomInLevel;
    m_pLocalMap->Reset((endX - startX)* kLocalZoomInLevel,(endY - startY)* kLocalZoomInLevel, localTileSize);
    
    int worldX = 0;
    int worldY = 0;
    for (int y = 0; y < (endY - startY) * kLocalZoomInLevel; ++y)
    {
        for (int x = 0; x < (endX - startX) * kLocalZoomInLevel; ++x)
        {
            worldX = x / kLocalZoomInLevel + startX;
            worldY = y / kLocalZoomInLevel + startY;
            m_pLocalMap->CopyTileInfo(x,y,m_pWorldMap->GetTile(worldX, worldY));
        }
    }
    
    m_hasLocalMap = true;
    m_pLocalMap->GenerateBuildings();
    m_pLocalMap->ConnectBuildings();
}


void MapGenerator::Reset()
{
    m_pWorldMap->Reset();
    m_pWorldMap->GenerateBasicHeightNoise();
    m_pWorldMap->ReShapeHeightNoise();
}

void MapGenerator::SetMapSeed(std::string& seed)
{
    m_mapSeed = E2::HashString(seed);
    if (m_pInputBox)
    {
        auto* pTextInput = static_cast<E2::UITextInput*>(m_pInputBox);
        pTextInput->ReplaceText(seed);
    }
}

void MapGenerator::InputCheck()
{
#ifdef _DEBUG
    if (GetEngine().IsKeyPressed(E2::Keyboard::Key::Q))
    {
        m_pWorldMap->ChangeInputRange(-1);
        m_pWorldMap->GenerateBasicHeightNoise();
        m_pWorldMap->ReShapeHeightNoise();
    }
    if (GetEngine().IsKeyPressed(E2::Keyboard::Key::W))
    {
        m_pWorldMap->ChangeInputRange(+1);
        m_pWorldMap->GenerateBasicHeightNoise();
        m_pWorldMap->ReShapeHeightNoise();
    }
    if (GetEngine().IsKeyPressed(E2::Keyboard::Key::A))
    {
        m_pWorldMap->ChangePersistence(-0.1f);
        m_pWorldMap->GenerateBasicHeightNoise();
        m_pWorldMap->ReShapeHeightNoise();
    }
    if (GetEngine().IsKeyPressed(E2::Keyboard::Key::S))
    {
        m_pWorldMap->ChangePersistence(+0.1f);
        m_pWorldMap->GenerateBasicHeightNoise();
        m_pWorldMap->ReShapeHeightNoise();
    }
    if (GetEngine().IsKeyPressed(E2::Keyboard::Key::Z))
    {
        m_pWorldMap->ChangeOctave(-1);
        m_pWorldMap->GenerateBasicHeightNoise();
        m_pWorldMap->ReShapeHeightNoise();
    }
    if (GetEngine().IsKeyPressed(E2::Keyboard::Key::X))
    {
        m_pWorldMap->ChangeOctave(+1);
        m_pWorldMap->GenerateBasicHeightNoise();
        m_pWorldMap->ReShapeHeightNoise();
    }
    if (GetEngine().IsKeyPressed(E2::Keyboard::Key::Space))
    {
        m_pWorldMap->SplitRegion();
    }
    if (GetEngine().IsKeyPressed(E2::Keyboard::Key::Num1))
    {
        m_drawSplitLines = !m_drawSplitLines;
    }
    if (GetEngine().IsKeyPressed(E2::Keyboard::Key::Num2))
    {
        m_display = DisplayType::Noise;
    }
    if (GetEngine().IsKeyPressed(E2::Keyboard::Key::Num3))
    {
        m_display = DisplayType::Biome;
    }
    if (GetEngine().IsKeyPressed(E2::Keyboard::Key::Return))
    {
        GenerateMap();
        //m_pWorldMap->DoSurvey();
        //m_pWorldMap->PrintInfo();
    }

    if (m_hasLocalMap)
    {
        if (GetEngine().IsKeyPressed(E2::Keyboard::Key::Num4))
        {
            m_display = DisplayType::LocalBiome;
        }
        if (GetEngine().IsKeyPressed(E2::Keyboard::Key::Num5))
        {
            m_display = DisplayType::LocalHeight;
        }
        if (GetEngine().IsKeyPressed(E2::Keyboard::Key::Num6))
        {
            m_display = DisplayType::LocalTerrace;
        }
    }

#endif // DEBUG
    if (GetEngine().IsKeyPressed(E2::Keyboard::Key::R))
    {
        if (m_display != DisplayType::Biome)
        {
            return;
        }
        Reset();
#ifdef NDEBUG
        AutoGenerateMap();
#endif // NDEBUG
    }
    if (GetEngine().IsKeyPressed(E2::Keyboard::Key::Escape))
    {
        m_display = DisplayType::Biome;
    }
}

void MapGenerator::AutoGenerateMap()
{
    for (int i = 0; i < kAutoSlitTimes; ++i)
    {
        m_pWorldMap->SplitRegion();
    }
    GenerateMap();
}

void MapGenerator::GenerateMap()
{
    m_pWorldMap->AdjustRegionHeightNoise();
    m_pWorldMap->SmoothNoiseAroundSplitLines();
    m_pWorldMap->ReShapeHeightNoise();
    m_pWorldMap->FoundCity(kGrass, kJungle);
    m_pWorldMap->LinkCities();
    m_pWorldMap->BuildRoads();
}