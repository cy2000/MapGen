#pragma once
#include <vector>
namespace E2
{
    struct Color;
}
class Grid;
struct Tile;
struct Region;
class LocalMap
{
    using SplitLine = std::pair<Tile*, Tile*>;
    using City = std::pair<Tile*, Tile*>;
private:
    Grid* m_pGrid;
    std::vector<City> m_cities;

    std::vector<Region*> m_rootRegions;

public:
    LocalMap();
    ~LocalMap();

    Tile* GetTile(int x, int y);

    void Reset(int maxX, int maxY, int tileSize);
    void CopyTileInfo(int index, Tile* pInTile);
    void CopyTileInfo(int x, int y, Tile* pInTile);
    void Terrace(int level);

    //Adjust Regions
    void SplitRegion(Region* pRoot);
    void GenerateBuildings();
    void ConnectBuildings();

    void Draw();
    void DrawHeight();
    void DrawHeightTerrace(int level);

private:
    void FindCities();
    void SpawnBuildings();
    bool IsTileInCity(Tile* pTile);
    static const E2::Color& PickColor(float noise);
};