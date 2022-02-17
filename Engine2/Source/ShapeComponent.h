#pragma once
#include "Component.h"
#include "Vector2.h"
#include <vector>

namespace E2
{
    class GameObject;
    class ShapeComponent : public Component
    {
    private:
        GameObject* m_pOwner;

        std::vector<Vector2> m_vertices;
        std::vector<Vector2> m_vertexPositions;
        std::vector<Vector2> m_edges;

    public:
        ShapeComponent(GameObject* pOwner);
        virtual ~ShapeComponent();
        virtual void Update(float deltaTime) final;
        virtual void Draw() final;

        void AddVertex(int x, int y);
        void BuildEdge(int firstVertexId, int secondVertexId);
    };
}