#pragma once

#include "UIElement.h"

#include <unordered_map>

namespace E2
{
    class UIButton : public UIElement
    {
    public:
        enum class State
        {
            Up,
            Over,
            Down,
            Selected,
        };
    private:
        bool m_isSelected;
        State m_currentState;
        std::unordered_map<State, UIElement*> m_stateElements;
        void(*m_pOnClickCallBack)();
    public:
        UIButton(const Color& upColor, const Color& downColor, const Color& overColor);
        virtual ~UIButton() = default;

        virtual void OnRollOver() override;
        virtual void OnRollOut() override;
        virtual void OnPress() override;
        virtual void OnClick() override;
        virtual void OnReleaseOutside() override;
        virtual void OnNotify(Event* event) override;

        void AddStateElement(State state, UIElement* pElement);
        void SetSelected(bool select);
        void ChangeState(State state);
        void SetCallBack(void(*pCallBack)());
    };
}