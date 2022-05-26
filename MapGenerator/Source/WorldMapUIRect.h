#pragma once

#include <UIRect.h>
#include <Color.h>

namespace E2
{
    struct CallBack;
}

class WorldMapUIRect : public E2::UIRect
{
private:
    int m_maxXLimit;
    int m_maxYLimit;
    E2::CallBack* m_pCallBack = nullptr;
public:
    WorldMapUIRect(const E2::Color& color);
    virtual ~WorldMapUIRect();
    virtual void OnRollOver() final {}
    virtual void Update() final;
    virtual void OnNotify(E2::Event evt);
    void SetCallBack(E2::CallBack* pCallBack);
};