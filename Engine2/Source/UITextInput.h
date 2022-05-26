#pragma once
#include "UIElement.h"
#include "Texture.h"
#include "Font.h"
namespace E2
{
    struct UITextInputData : UIElementData
    {
        std::string font;
        int fontSize = 0;
        uint8_t textR = 0;
        uint8_t textG = 0;
        uint8_t textB = 0;
        uint8_t backGroundR = 0;
        uint8_t backGroundG = 0;
        uint8_t backGroundB = 0;
    };


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
        UITextInput(Font font, Color textColor, Color backGroundcolor);
        virtual ~UITextInput();

        virtual bool SupportsKeyFocus() const override {return true;}
        virtual void OnFocusGained() override {m_isFocused = true;}
        virtual void OnFocusLost() override {m_isFocused = false;}

        virtual void OnKeyDown(E2::Keyboard::Key key) override;

        virtual void OnText(const char c) override;

        virtual void Draw() override;

        virtual void OnNotify(Event evt)override;

        const std::string& GetText() const { return m_text; }
        void ReplaceText(const std::string& text);
    };
}