#pragma once

#include "Event.h"
#include <vector>
#include <utility>
#include <type_traits>

namespace E2
{
    class UIElement;
    class EventManager
    {
    private:
        // TODO:DOESN'T WORK 
        //std::vector<std::pair<UIElement*, void(UIElement::*)(Event*)>> m_UIEventListeners;
        // other type of listeners..
        bool m_breakNow;    //hack
        std::vector<UIElement*> m_UIEventListeners;
        std::vector<UIElement*> m_ExecuteListeners;
    public:
        EventManager();
        ~EventManager() = default;
        void AddUIListener(UIElement* pElement);
        void NotifyUI(Event* pEvent);
        void ClearListener();
        void Update();
    };
}