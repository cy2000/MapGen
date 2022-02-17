#pragma once
#include <IGame.h>
#include <Vector2.h>

#include <vector>
#include <chrono>
namespace E2
{
    class GameObject;
}

class AITest : public E2::IGame
{
private:
    std::vector <E2::GameObject*> m_objects;
     
    float m_currentMoveSpeed;
    float m_currentSpinSpeed;
    bool m_speedUp;

public:
    static AITest& Get();
    virtual ~AITest();
    virtual bool Init() final;
    virtual void Update() final;
    virtual void ShutDown() final;
    virtual const E2::GameInfo& GetInfo() final;

    void MoveObject(E2::GameObject* pObject,E2::Vector2 direction, float speed, float time);
    void RotateObject(E2::GameObject* pObject, float angel, float time);
private:
    AITest();
};
