#pragma once
#include "UIElement.h"
#include "Texture.h"
#include "Font.h"
namespace E2
{
    class UILabel;
    class UIRect;
    class UITextInput : public UIElement
    {
    private:
        bool m_isFocused;
        int m_cursorIndex;
        std::string m_text;
        
        UILabel* m_pContents;
        UIRect* m_pBackground;

        int m_charW;
        int m_charH;
        int m_maxCharPerLine;
        int m_maxLine;
    public:
        UITextInput(Texture texture, Color& backGroundcolor);
        UITextInput(Font font, Color& textColor, Color& backGroundcolor);
        virtual ~UITextInput();

        virtual bool SupportsKeyFocus() const override {return true;}
        virtual void OnFocusGained() override {m_isFocused = true;}
        virtual void OnFocusLost() override {m_isFocused = false;}

        virtual void OnKeyDown(E2::Keyboard::Key key) override;

        virtual void OnText(const char* pText) override;

        virtual void Draw() override;

        virtual void OnNotify(Event* pEvent)override;

        const std::string& GetText() const { return m_text; }
        void ReplaceText(std::string& text);
    };
}