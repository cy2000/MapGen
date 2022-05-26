#pragma once
#include <Texture.h>
#include <FreeFunctions.h>
enum class TileSetType
{
    GrassTerrain,
    SnowTerrain,
    SandTerrain,
    Beach,
    CoastLine,
    ShallowWater,
    DeepWater,
    ThinIce,
    IceCap,
};

class TileSet
{
private:
    int m_tileSize = 16;
    E2::Texture m_texture; 
    TileSetType m_type;

public:
    void Draw(int index,E2::Rect& destRect)
    {
        int x = index % 3;
        int y = index / 3;

        E2::Rect srcRect = {x* m_tileSize, y* m_tileSize ,m_tileSize ,m_tileSize };

        DrawTexture(m_texture, &srcRect, &destRect);
    }
};