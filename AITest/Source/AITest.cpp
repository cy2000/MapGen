#include "AITest.h"
#include <FreeFunctions.h>
#include <GameObject.h>
#include <TransformComponent.h>
#include <ShapeComponent.h>

#include <cmath>
#include <iostream>

const E2::GameInfo kInfo{ "AITest",1920,720,10 };
constexpr float kMaxMoveSpeed = 3.f;
constexpr float kMaxSpinSpeed = 5.f;

//TODO: Use real timer
constexpr float kFakeTime = 0.01f;
constexpr float kSmallNumber = 0.001f;
constexpr float kVerySmallNumber = 0.0001f;

AITest& AITest::Get()
{
    static AITest instance;
    return instance;
}

AITest::~AITest()
{
    for (auto* pObjects : m_objects)
    {
        delete pObjects;
        pObjects = nullptr;
    }
    m_objects.clear();
}

bool AITest::Init()
{
    //create 2 triangle objects
    {
        auto* pObject = new E2::GameObject();
        pObject->GetTransform()->SetPosition(0, 240);
        pObject->GetTransform()->SetDimension(50, 50);
        auto* pShape = new E2::ShapeComponent(pObject);
        pObject->AddComponent(pShape);

        pShape->AddVertex(0,0);
        pShape->AddVertex(65,25);
        pShape->AddVertex(0,50);
        pShape->BuildEdge(0,1);
        pShape->BuildEdge(1,2);
        pShape->BuildEdge(2,0);

        m_objects.push_back(pObject);
    }
    {
        auto* pObject = new E2::GameObject();
        pObject->GetTransform()->SetPosition(1920, 480);
        pObject->GetTransform()->SetDimension(50, 50);
        auto* pShape = new E2::ShapeComponent(pObject);
        pObject->AddComponent(pShape);

        pShape->AddVertex(50, 0);
        pShape->AddVertex(-15, 25);
        pShape->AddVertex(50, 50);
        pShape->BuildEdge(0, 1);
        pShape->BuildEdge(1, 2);
        pShape->BuildEdge(2, 0);

        m_objects.push_back(pObject);
    }

    return true;
}

void AITest::Update()
{
    //speed up to max speed, and then slow down to 0
    if (m_speedUp)
    {
        m_currentMoveSpeed += kVerySmallNumber;
        m_currentSpinSpeed += kSmallNumber;
        if (m_currentMoveSpeed > kMaxMoveSpeed)
        {
            m_speedUp = false;
        }
    }
    else
    {
        m_currentMoveSpeed -= kVerySmallNumber;
        m_currentSpinSpeed -= kSmallNumber;
        if (m_currentMoveSpeed < 0)
        {
            m_speedUp = true;
        }
    }

    // move the objects
    MoveObject(m_objects[0], E2::Vector2{1,0}, m_currentMoveSpeed, kFakeTime);
    MoveObject(m_objects[1], E2::Vector2{-1,0}, m_currentMoveSpeed, kFakeTime);
    // spin the objects
    RotateObject(m_objects[0], m_currentSpinSpeed, kFakeTime);
    RotateObject(m_objects[1], -m_currentSpinSpeed, kFakeTime);

    for (auto* pObject : m_objects)
    {
        pObject->Update(0);
    }
    for (auto* pObject : m_objects)
    {
        pObject->Draw();
    }

}

void AITest::ShutDown()
{
}

const E2::GameInfo& AITest::GetInfo()
{
    return kInfo;
}

void AITest::MoveObject(E2::GameObject* pObject, E2::Vector2 direction, float speed, float time)
{
    //current position + delta movement
    auto currentPos = pObject->GetTransform()->GetPosition();
    float newX = currentPos.x + ((float)direction.x * (float)speed* time);
    float newY = currentPos.y + ((float)direction.y * (float)speed* time);
    pObject->GetTransform()->SetPosition(newX,newY);
}

void AITest::RotateObject(E2::GameObject* pObject, float angle, float time)
{
    //current rotation + delta angle
    auto currentRot = pObject->GetTransform()->GetRotation();
    pObject->GetTransform()->SetRotation(currentRot + angle * time);
}

AITest::AITest()
    : m_currentMoveSpeed{0}
    , m_currentSpinSpeed{0}
    , m_speedUp{true}
{
}
