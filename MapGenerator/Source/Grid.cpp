#include "Grid.h"
#include <assert.h>

Grid::Grid()
    :m_maxX{0}
    ,m_maxY{0}
{
}

Grid::~Grid()
{
}

void Grid::Init(int maxX, int maxY, int tileSize)
{
    m_maxX = maxX;
    m_maxY = maxY;
    int id = 0;
    for (int y = 0; y < m_maxY; ++y)
    {
        for (int x = 0; x < m_maxX; ++x)
        {
            m_tiles.emplace_back(Tile{ id,x,y,{x * tileSize, y * tileSize, tileSize, tileSize},{},0,0, Tile::Type::Empty });
            ++id;
        }
    }
}

Tile* Grid::GetTile(unsigned int index)
{
    if (index < m_tiles.size())
    {
        return &m_tiles[index];
    }
    else
    {
        return nullptr;
    }
}

Tile* Grid::GetTile(unsigned int x, unsigned int y)
{
    return GetTile(x + y * m_maxX);
}

Tile* Grid::GetNeighborTile(Tile* centerTile, Direction dir)
{
    switch (dir)
    {
    case Direction::North: return GetTile(centerTile->x, centerTile->y - 1);
    case Direction::South: return GetTile(centerTile->x, centerTile->y + 1);
    case Direction::East: return GetTile(centerTile->x + 1, centerTile->y);
    case Direction::West: return GetTile(centerTile->x - 1, centerTile->y);;
    }

    return nullptr;
}

void Grid::ResetNoise()
{
    for (auto& tile : m_tiles)
    {
        tile.rawNoise = 0;
    }
}

const int Grid::Distance2(Tile* pA, Tile* pB)
{
    return (pA->x - pB->x)* (pA->x - pB->x) + (pA->y - pB->y)* (pA->y - pB->y);
}

void Grid::ClearObjects()
{
    for (auto& tile : m_tiles)
    {
        tile.type = Tile::Type::Empty;
    }
}
