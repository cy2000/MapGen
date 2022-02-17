#pragma once

#include <UIRect.h>
#include <Color.h>

class WorldMapUIRect : public E2::UIRect
{
private:
    int m_maxXLimit;
    int m_maxYLimit;
public:
    WorldMapUIRect(const E2::Color& color);
    virtual ~WorldMapUIRect() = default;
    virtual void OnRollOver() final {}
    virtual void OnNotify(E2::Event* pEvent)final;
};