#pragma once


struct lua_State;

namespace E2
{
    class UIElement;
    struct UIElementData;
    struct GameSetting;
    class ScriptManager
    {
    private:
        lua_State* m_pLua = nullptr;
    public:
        ScriptManager();
        ~ScriptManager();

        bool LoadUI(const char* pScript);
        UIElement* LoadUIElement(const char* pScript);
        static int Host_LoadUIElement(lua_State* pState);
        bool LoadFile(const char* pScript);
        bool LoadGameSettings(const char* pScript, GameSetting& gameSetting);
    private:
        UIElement* LoadUILabel();
        UIElement* LoadUIImage();
        UIElement* LoadUITextInput();
        void LoadPositionInfo(UIElementData& data);
    };
}