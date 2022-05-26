#include "WorldMapUIRect.h"
#include "FreeFunctions.h"
#include "SimpleMath.h"

#include <CallBack.h>

WorldMapUIRect::WorldMapUIRect(const E2::Color& color)
    : UIRect(color,false,true)
    , m_maxXLimit{ GetEngine().GetWindowSize().x }
    , m_maxYLimit{ GetEngine().GetWindowSize().y }
{
}

WorldMapUIRect::~WorldMapUIRect()
{
    delete m_pCallBack;
    m_pCallBack = nullptr;
}

void WorldMapUIRect::Update()
{
    if (!m_isVisable)
    {
        return;
    }
    auto mousePos = GetEngine().GetMousePos();
    m_realRegion.x = mousePos.x - m_realRegion.w / 2;
    m_realRegion.y = mousePos.y - m_realRegion.h / 2;

    m_realRegion.x = E2::Clamp<int>(0, m_maxXLimit - m_realRegion.w, m_realRegion.x);
    m_realRegion.y = E2::Clamp<int>(0, m_maxYLimit - m_realRegion.h, m_realRegion.y);
}

void WorldMapUIRect::OnNotify(E2::Event evt)
{
    if (!m_isVisable)
    {
        return;
    }
    else
    {
        if (this == HitTest({ evt.m_mouseEvent.x, evt.m_mouseEvent.y }))
        {
            m_pCallBack->Call();
        }
    }
}

void WorldMapUIRect::SetCallBack(E2::CallBack* pCallBack)
{
    m_pCallBack = std::move(pCallBack);
}
