#include "pch.h"
#include "EventManager.h"

#include <functional>


E2::EventManager::EventManager()
    : m_reset{false}
{
    //
}

void E2::EventManager::AddListener(EventListener* pListener, const char* pEventType)
{
    if (pListener)
    {
        size_t eventType = std::hash<std::string>{}(pEventType);

        m_eventListeners[eventType].push_back(pListener);
    }
}

void E2::EventManager::ClearListener()
{
    m_eventListeners.clear();

    m_reset = true;
}

void E2::EventManager::Notify(Event evt)
{
    m_reset = false;
    for (auto* pListener : m_eventListeners[evt.m_eventType])
    {
        if (m_reset)
        {
            m_reset = false;
            break;
        }
        pListener->OnNotify(evt);
    }
}

void E2::EventManager::Update()
{
    
}


