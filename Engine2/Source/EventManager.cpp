#include "pch.h"
#include "EventManager.h"
#include "UIElement.h"

E2::EventManager::EventManager()
    : m_breakNow{false}
{
    //
}

void E2::EventManager::AddUIListener(UIElement* pElement)
{
    m_UIEventListeners.push_back(pElement);
}

void E2::EventManager::NotifyUI(Event* pEvent)
{
    for (auto* pElement : m_ExecuteListeners)
    {
        pElement->OnNotify(pEvent);
        if (m_breakNow)
        {
            break;
        }
    }
}

void E2::EventManager::ClearListener()
{
    m_ExecuteListeners.clear();
    m_breakNow = true;
}

void E2::EventManager::Update()
{
    if (!m_UIEventListeners.empty())
    {
        for (auto* p : m_UIEventListeners)
        {
            m_ExecuteListeners.push_back(p);
        }
        m_UIEventListeners.clear();
    }
    m_breakNow = false;
}
