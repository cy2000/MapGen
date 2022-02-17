#pragma once
#include <Rect.h>
#include <Color.h>
#include <vector>

struct Tile
{
    enum class Type
    {
        Empty,
        Filled,
        Road,
        City,
        CityRoad,
        Building,
    };

    int id;
    int x;
    int y;
    E2::Rect rect;
    E2::Color color;
    float rawNoise;
    float heightNoise;
    Type type;
};


class Grid
{
public:
    enum class Direction
    {
        North,
        South,
        East,
        West,
    };
private:
    int m_maxX;
    int m_maxY;
    std::vector<Tile> m_tiles;

public:
    Grid();
    ~Grid();

    void Init(int maxX, int maxY, int tileSize);
    Tile* GetTile(unsigned int index);
    Tile* GetLastTile() { return &m_tiles.back(); }
    Tile* GetTile(unsigned int x, unsigned int y);
    size_t Size() const { return m_tiles.size(); }
    int GetColumn() const { return m_maxX; }
    int GetRow() const { return m_maxY; }
    Tile* GetNeighborTile(Tile* centerTile,Direction dir);
    void ResetNoise();
    static const int Distance2 (Tile* pA, Tile* pB);
    void ClearObjects();
};
