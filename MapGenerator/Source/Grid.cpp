#include "Grid.h"
#include <assert.h>
#include <cmath>

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
    m_tileSize = tileSize;
    int id = 0;
    for (int y = 0; y < m_maxY; ++y)
    {
        for (int x = 0; x < m_maxX; ++x)
        {
            m_tiles.emplace_back(Tile{ id,x,y,{x * tileSize, y * tileSize, tileSize, tileSize},{},0,0,0, Tile::Type::Empty });
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

Tile* Grid::GetTile(int x, int y)
{
    if (x >= m_maxX || y>= m_maxY || x <0 || y<0)
    {
        return nullptr;
    }
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

//Return the index of the tile in this position, or -1 if none-exist
int Grid::FindTile(E2::Vector2f position)
{
    int xCoord = std::lroundf(position.x);
    int yCoord = std::lroundf(position.y);
    int x = xCoord / m_tileSize;
    int y = yCoord / m_tileSize;


    if (x <0 || x > m_maxX || y <0 || y > m_maxY)
    {
        return -1;
    }
    else
    {
        return GetTile(x,y)->id;
    }
}
