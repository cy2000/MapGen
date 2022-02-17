#include "WorldMapUIRect.h"
#include "MapGenerator.h"
#include "FreeFunctions.h"
#include "SimpleMath.h"

WorldMapUIRect::WorldMapUIRect(const E2::Color& color)
    : UIRect(color,false,true)
    , m_maxXLimit{ GetEngine().GetWindowSize().x }
    , m_maxYLimit{ GetEngine().GetWindowSize().y }
{
}

void WorldMapUIRect::OnNotify(E2::Event* pEvent)
{
    if (!m_isVisable)
    {
        return;
    }
    if (pEvent->m_type == E2::EventType::MouseMovementEvent)
    {
        auto* pEventData = static_cast<E2::MouseMoveEvent*>(pEvent);
        m_realRegion.x = pEventData->x - m_realRegion.w /2;
        m_realRegion.y = pEventData->y - m_realRegion.h /2;

        m_realRegion.x = E2::Clamp<int>(0, m_maxXLimit - m_realRegion.w, m_realRegion.x);
        m_realRegion.y = E2::Clamp<int>(0, m_maxYLimit - m_realRegion.h, m_realRegion.y);

    }
    else if (pEvent->m_type == E2::EventType::MouseButtonUpEvent)
    {
        auto* pEventData = static_cast<E2::MouseClickEvent*>(pEvent);
        if (this == HitTest({ pEventData->x, pEventData->y }))
        {
            MapGenerator::Get().LoadLocalMap();
            MapGenerator::Get().SetDisplay(MapGenerator::DisplayType::LocalBiome);
            m_isVisable = false;
        }
    }
}
