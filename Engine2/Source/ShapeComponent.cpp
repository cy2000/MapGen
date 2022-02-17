#include "pch.h"
#include "ShapeComponent.h"
#include "TransformComponent.h"
#include "GameObject.h"
#include "Engine.h"
#include "Color.h"

#include <cmath>

constexpr float kToRadian = 0.017453f; // Pi / 180

E2::ShapeComponent::ShapeComponent(GameObject* pOwner)
    : m_pOwner{pOwner}
{
}

E2::ShapeComponent::~ShapeComponent()
{

}

void E2::ShapeComponent::Update(float deltaTime)
{
    //apply rotation first
    auto dimension = m_pOwner->GetTransform()->GetDimension();
    //find the center of owner
    E2::Vector2f center = dimension / 2;
    //convert angle to ragians
    auto rot = m_pOwner->GetTransform()->GetRotation() * kToRadian;
    //rotate each vertex from the center
    for (int i = 0; i < m_vertices.size(); ++i)
    {
        auto newX = m_vertices[i].x - center.x;
        auto newY = m_vertices[i].y - center.y;
        auto rotX = newX * std::cos(rot) - newY * std::sin(rot);
        auto rotY = newX * std::sin(rot) + newY * std::cos(rot);
        m_vertexPositions[i].x = (int)rotX;
        m_vertexPositions[i].y = (int)rotY;
    }

    //translate all the vertices to the position of owner 
    auto pos = m_pOwner->GetTransform()->GetPosition();
    for (int i = 0; i < m_vertices.size(); ++i)
    {
        m_vertexPositions[i].x = (int)pos.x + m_vertexPositions[i].x;
        m_vertexPositions[i].y = (int)pos.y + m_vertexPositions[i].y;
    }
    
}

void E2::ShapeComponent::Draw()
{
    for (auto& edge : m_edges)
    {
        E2::Engine::Get().DrawLine(m_vertexPositions[edge.x], m_vertexPositions[edge.y],E2::Mono::kWhite);
    }
}

void E2::ShapeComponent::AddVertex(int x, int y)
{
    m_vertices.emplace_back( x,y );
    m_vertexPositions.emplace_back( x,y );
}

void E2::ShapeComponent::BuildEdge(int firstVertexId, int secondVertexId)
{
    m_edges.emplace_back(firstVertexId, secondVertexId);
}
