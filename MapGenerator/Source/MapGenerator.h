#pragma once
#include <IGame.h>
#include <vector>

namespace E2
{
    struct Color;
    class UIElement;
}

class WorldMap;
class LocalMap;
class WorldMapUIRect;
struct Tile;
class MapGenerator : public E2::IGame
{
public:
    enum class DisplayType
    {
        Color,
        Noise,
        Biome,
        LocalBiome,
        LocalHeight,
        LocalTerrace,
    };
private:
    bool m_onMenu;
    bool m_hasLocalMap;
    bool m_drawSplitLines;
    size_t m_mapSeed;

    WorldMap* m_pWorldMap;
    LocalMap* m_pLocalMap;

    DisplayType m_display;
    //UI
    //Menu
    E2::UIElement* m_pMenuBackGround;
    E2::UIElement* m_pTitle;
    E2::UIElement* m_pVersion;
    E2::UIElement* m_pInputBox;
    E2::UIElement* m_pRandomSeedButton;
    E2::UIElement* m_pGenerateMapButton;
    E2::UIElement* m_pQuitButton;

    //on World Map

    E2::UIElement* m_selectRect;

public:
    static MapGenerator& Get();
    virtual ~MapGenerator();
    virtual bool Init() final;
    virtual void Update() final;
    virtual void ShutDown() final;
    virtual const E2::GameInfo& GetInfo() final;

    void InitMenu();
    void CloseMenu();
    void InitWorldMap();
    void InitWorldMapSelectionRect();
    void LoadLocalMap();

    void Reset();
    void SetMapSeed(std::string& seed);
    void SetDisplay(DisplayType type) { m_display = type; }

    void InputCheck();
    void AutoGenerateMap();
private:
    MapGenerator();
    void GenerateMap();
};