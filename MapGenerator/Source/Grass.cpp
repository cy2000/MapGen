#include "Grass.h"
#include <FreeFunctions.h>

Grass::Grass(E2::Texture texture, E2::Vector2 pos, E2::Vector2 dimension)
    : m_texture{ texture }
    , m_position{ pos }
    , m_dimension{ dimension }
{
    m_rect = { m_position.x,m_position.y,m_dimension.x,m_dimension.y};
}

void Grass::Draw()
{
    DrawTexture(m_texture, nullptr, &m_rect);
}
