#include "MapGenerator.h"
#include "MenuScene.h"
#include "WorldScene.h"
#include "LocalScene.h"

#include <FreeFunctions.h>
#include <HashString.h>


constexpr const char* kConfig = "Script/Config.lua";
constexpr const char* kTextures = "Script/TextureIndex.lua";

MapGenerator& MapGenerator::Get()
{
    static MapGenerator instance;
    return instance;
}

MapGenerator::MapGenerator()
    : m_mapSeed{0}
{
    //
}

MapGenerator::~MapGenerator()
{
    delete m_pMenuScene;
    delete m_pWorldScene;
    delete m_pLocalScene;
}

bool MapGenerator::Init()
{
    E2::Seed(std::time(nullptr));

    GetEngine().LoadFilesFromScript(kTextures);

    m_pMenuScene = new MenuScene;
    m_pNextScene = m_pMenuScene;

    return true;
}

void MapGenerator::Update(float deltaTime)
{
    if (m_pCurrentScene)
    {
        m_pCurrentScene->Update(deltaTime);
    }
    ChangeScene();
}

const char* MapGenerator::Config()
{
    return kConfig;
}

void MapGenerator::SetMapSeed(std::string& seed)
{
    m_mapSeed = E2::HashString(seed);
}

void MapGenerator::SetMapSeed(size_t seed)
{
    m_mapSeed = seed;
}

void MapGenerator::ChangeScene()
{
    if (m_pNextScene)
    {
        if (m_pCurrentScene)
        {
            if (m_preserveCurrentScene)
            {
                m_pPreviousScene = m_pCurrentScene;
            }
            else
            {
                m_pCurrentScene->End();
            }
        }

        if (m_pPreviousScene == m_pNextScene)
        {
            m_pCurrentScene = m_pNextScene;
            m_pNextScene = nullptr;
            m_pPreviousScene = nullptr;
        }
        else
        {
            bool test = m_pNextScene->Init();
            assert(test && "Scene Init failed.");
            m_pCurrentScene = m_pNextScene;
            m_pNextScene = nullptr;
        }
    }
}

Scene* MapGenerator::GetScene(Scene::SceneId id)
{
    switch (id)
    {
    case Scene::SceneId::MenuScene: return m_pMenuScene;
    case Scene::SceneId::WorldScene: return m_pWorldScene;
    case Scene::SceneId::LocalScene: return m_pLocalScene;
    }

    return nullptr;
}

void MapGenerator::Restart()
{
    if (m_pMenuScene)
    {
        delete m_pMenuScene;
    }
    m_pMenuScene = new MenuScene;
    m_pNextScene = m_pMenuScene;
    m_preserveCurrentScene = false;
}

void MapGenerator::GenerateWorld()
{
    if (m_pWorldScene)
    {
        delete m_pWorldScene;
    }
    m_pWorldScene = new WorldScene;
    m_pNextScene = m_pWorldScene;
    m_preserveCurrentScene = false;
}

void MapGenerator::GenerateLocalMap()
{
    if (m_pLocalScene)
    {
        delete m_pLocalScene;
    }
    m_pLocalScene = new LocalScene;
    m_pNextScene = m_pLocalScene;
    m_preserveCurrentScene = true;
}

// TODO: 
void MapGenerator::DataTransfer()
{
    auto* pLocal = dynamic_cast<LocalScene*>(m_pLocalScene);
    auto* pWorld = dynamic_cast<WorldScene*>(m_pWorldScene);
    pLocal->SetWorldMap(pWorld->GetWorldMap());
    pLocal->SetStartAndEnd(pWorld->GetSelectionStart(),pWorld->GetSelectionEnd());
}

void MapGenerator::BackToWorld()
{
    assert(m_pWorldScene && "World Scene must exist");
    m_pNextScene = m_pWorldScene;
    m_pWorldScene->Reset();
    m_preserveCurrentScene = false;
}
