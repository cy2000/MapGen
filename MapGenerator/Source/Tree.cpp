#include "Tree.h"

#include <Rect.h>
#include <FreeFunctions.h>


Tree::Tree(E2::Texture texture, E2::Vector2 pos, E2::Vector2 dimension)
    : m_texture{texture}
    , m_position{pos}
    , m_dimension{dimension}
{

}

void Tree::Draw()
{
    E2::Rect temp;
    temp.x = m_position.x;
    temp.y = m_position.y;
    temp.w = m_dimension.x;
    temp.h = m_dimension.y;
    DrawTexture(m_texture, nullptr, &temp);
}
