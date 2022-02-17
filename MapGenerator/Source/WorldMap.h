#pragma once

#include <Color.h>

#include <vector>
#include <utility>
#include <unordered_map>
struct Tile;
class Grid;
struct Region;

class WorldMap
{
public:
    using SplitLine = std::pair<Tile*, Tile*>;
    struct PathNode
    {
        Tile* m_pTile;
        float m_gScore;
        float m_fScore;
        PathNode* m_cameFrom;
    };
private:
    // W and H as in tile count
    int m_mapW;
    int m_mapH;
    int m_tileSize;
    int m_currentInputRange;
    int m_currentOctaves;
    float m_currentPersistence;
    Grid* m_pGrid;

    Region* m_pRootRegion;

    std::vector<Tile*> m_cities;
    std::vector<std::vector<int>> m_cityAdjacencyList;

    std::vector<Tile*> m_roads;

    std::vector<int> m_survey;
public:
    WorldMap(int width,int height, int tileSize);
    ~WorldMap();

    //Height Noise
    void GenerateBasicHeightNoise();
    void AdjustRegionHeightNoise();
    void ReShapeHeightNoise();
    void SmoothNoiseAroundSplitLines();
    void DrawHeightNoise();
    void DrawHeightBiome();

    //Adjust Regions
    void SplitRegion();
    void DrawSplitLines();
    
    //Cities and Roads
    void FoundCity(float minHeight,float maxHeight);
    int GetCityCount(Region* pNode);
    void LinkCities();
    void BuildRoads();
    void FindPath(Tile* pStart, Tile* pEnd);
    void ExpandCityTiles(Tile* pCenter, int range);
    void DrawCities();
    void DrawRoads();

    //Misc
    void ChangeInputRange(int delta);
    void ChangePersistence(float delta);
    void ChangeOctave(int delta);
    void Reset();
    Tile* GetTile(int x, int y); 
    void DoSurvey();
    void PrintInfo();
private:
    float RegionEmptiness(int regionTileCount, int slope);
    const E2::Color& PickColor(float noise);
};
