#include "MenuScene.h"
#include "MapGenerator.h"

#include <FreeFunctions.h>
#include <HashString.h>
#include <UIElement.h>
#include <UIButton.h>
#include <UILabel.h>
#include <UITextInput.h>
#include <EventType.h>
#include <CallBack.h>

#include <SquirrelNoise.h>
#include <PerlinNoise.h>

constexpr const char* kTitleScreen = "Script/UI/TitleScreen.lua";
constexpr const char* kFont = "Assets/UI/joystix_monospace.ttf";
constexpr const char* kBackGround = "Assets/UI/UIBackGround.png";


MenuScene::MenuScene()
{
}

MenuScene::~MenuScene()
{
}

void MenuScene::Update(float deltaTime)
{
}

bool MenuScene::Init()
{
    GetEngine().LoadFile(kFont);
    GetEngine().LoadFile(kBackGround);

    auto windowW = E2::Engine::Get().GetWindowSize().x;
    auto windowH = E2::Engine::Get().GetWindowSize().y;

    bool isUILoad = GetEngine().LoadUI(kTitleScreen);
    assert(isUILoad && "UI is not loaded.");

    m_pInputBox = GetEngine().GetUIElement("UITextInput");

    //RandomSeed Button
    auto pRandomSeedButton = new E2::UIButton(E2::Blue::kSkyBlue, E2::Yellow::kGoldYellow, E2::Blue::kCobalt);
    pRandomSeedButton->SetPosition({ E2::UICoordType::Percentage,0.5f }, { E2::UICoordType::Percentage,0.57f });
    pRandomSeedButton->SetOrigin({ E2::UICoordType::Percentage,0.5f }, { E2::UICoordType::Percentage,0.5f });
    pRandomSeedButton->SetDimension({ E2::UICoordType::Pixels, 100 }, { E2::UICoordType::Pixels, 50 });

    std::string SeedButtonString{ "Random" };
    auto pSeedButtonText = new E2::UILabel(GetEngine().CreateFont(kFont, 15), SeedButtonString, E2::Mono::kDark);
    pSeedButtonText->SetPosition({ E2::UICoordType::Percentage, 0.4f }, { E2::UICoordType::Percentage,0.5f });
    pSeedButtonText->SetOrigin({ E2::UICoordType::Percentage, 0.5f }, { E2::UICoordType::Percentage,0.5f });
    pSeedButtonText->SetDimension({ E2::UICoordType::Percentage, 0.5f }, { E2::UICoordType::Percentage,0.5f });
    pRandomSeedButton->AddChild(pSeedButtonText);

    pRandomSeedButton->Reposition(0, 0, windowW, windowH);
    auto randomSeedButtonCallback = [](std::uintptr_t address)
    {
        auto seed = std::to_string(E2::Random());
        auto* pInputBox = reinterpret_cast<MenuScene*>(address)->m_pInputBox;
        if (pInputBox)
        {
            auto* pTextInput = dynamic_cast<E2::UITextInput*>(pInputBox);
            pTextInput->ReplaceText(seed);
        }
    };

    pRandomSeedButton->SetCallBack(new E2::CallBack(reinterpret_cast<std::uintptr_t>(this), randomSeedButtonCallback));

    m_pRandomSeedButton = pRandomSeedButton;
    GetEngine().AddUIElement(m_pRandomSeedButton);
    GetEngine().RegisterListener(m_pRandomSeedButton, kMouseEvent);

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

    auto generateButtonCallback = [](std::uintptr_t address)
    {
        auto* pInputBox = reinterpret_cast<MenuScene*>(address)->m_pInputBox;
        if (pInputBox)
        {
            auto* pTextInput = dynamic_cast<E2::UITextInput*>(pInputBox);
            MapGenerator::Get().SetMapSeed(E2::HashString(pTextInput->GetText()));
        }
        MapGenerator::Get().GenerateWorld();
    };
    pGenerateMapButton->SetCallBack(new E2::CallBack(reinterpret_cast<std::uintptr_t>(this), generateButtonCallback));
    
    m_pGenerateMapButton = pGenerateMapButton;
    GetEngine().AddUIElement(m_pGenerateMapButton);
    GetEngine().RegisterListener(m_pGenerateMapButton, kMouseEvent);

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
    GetEngine().RegisterListener(m_pQuitButton, kMouseEvent);

    return true;
}

void MenuScene::End()
{
    m_pInputBox = nullptr;
    m_pRandomSeedButton = nullptr;
    m_pGenerateMapButton = nullptr;
    m_pQuitButton = nullptr;
    GetEngine().ClearListener();
    GetEngine().ClearUI();
}
