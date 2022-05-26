#pragma once
#include <Rect.h>
#include <Color.h>
#include <Vector2.h>

#include <vector>

struct Tile
{
    enum class Type
    {
        Empty,
        Filled,
        City,
        Building,
        Road,
    };

    int id;
    int x;
    int y;
    E2::Rect rect;
    E2::Color biomeColor;
    float rawNoise;
    float heightNoise;
    float rawTemperature;
    Type type;
    int biomeId;
    int citySize;
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
    // the max number of columns
    int m_maxX;
    // the max number of rows
    int m_maxY;

    int m_tileSize = 0;
    std::vector<Tile> m_tiles;

public:
    Grid();
    ~Grid();

    void Init(int maxX, int maxY, int tileSize);
    Tile* GetTile(unsigned int index);
    Tile* GetTile(int x, int y);
    Tile* GetLastTile() { return &m_tiles.back(); }
    size_t Size() const { return m_tiles.size(); }
    E2::Vector2 GetTileSize() { return { m_tileSize, m_tileSize }; }
    int GetColumn() const { return m_maxX; }
    int GetRow() const { return m_maxY; }
    Tile* GetNeighborTile(Tile* centerTile,Direction dir);

    void ResetNoise();
    static const int Distance2 (Tile* pA, Tile* pB);
    void ClearObjects();

    int FindTile(E2::Vector2f position);
};
