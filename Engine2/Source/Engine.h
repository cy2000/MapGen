#pragma once
#include "IGame.h"
#include "Device.h"
#include "ResourceManager.h"
#include "GameObjectManager.h"
#include "UIManager.h"
#include "EventManager.h"
#include "ScriptManager.h"
#include "Keyboard.h"
#include "Mouse.h"
#include "Color.h"
#include "Rect.h"
#include "Vector2.h"
#include "Event.h"
#include "Timer.h"
#include "TryCast.h"
#include <unordered_map>

namespace E2
{
    class Engine
    {
    private:
        Device m_device;
        Keyboard m_keyboard;
        Mouse m_mouse;
        ResourceManager m_resourceManager;
        EventManager m_eventManger;
        UIManager m_UIManager;
        GameObjectManager m_gameObjectManager;

        ScriptManager m_scriptManager;
        Timer m_timer;

        GameSetting m_gameSetting;

        bool m_quit;
        bool m_manuallyUpdate;
        bool m_canUpdate;
        bool m_showStatus;
    public:
        //Engine Control//////////////////////////////////////////
        static Engine& Get();
        void Quit() { m_quit = true; }
        bool Run(IGame& game);
        Vector2 GetWindowSize();
        void ToggleManuallyUpdate();
        void UpdateOnce() { m_canUpdate = true; }
        void PrintFPS(float deltaTime);
        void ToggleEngineShowStatus() { m_showStatus = !m_showStatus; }
        void DestroyTexture(E2::Texture& texture);

        //Input//////////////////////////////////////////
        void SetKeyState(Keyboard::Key key, bool state) { m_keyboard.SetState(key, state); }
        void SetMouseState(Mouse::Button button, bool state) { m_mouse.SetState(button, state); }
        void SetMousePos(int x, int y) { m_mouse.SetMousePos(Vector2{ x,y }); }
        Vector2 GetMousePos()const { return m_mouse.GetMousePos(); }
        bool PointInRect(int x, int y, Rect& rect);
        bool IsAnyMouseButtonPressed() { return m_mouse.IsAnyButtonPressed(); }
        bool IsAnyKeyPressed() { return m_keyboard.IsAnykeyPressed(); }
        bool IsKeyPressed(Keyboard::Key key) { return m_keyboard.IsKeyPressed(key); }
        bool IsKeyDown(Keyboard::Key key) { return m_keyboard.IsKeyDown(key); }
        const Keyboard::Key GetLastKeyPressed() const { return m_keyboard.GetLastKeyPressed(); }

        //Graphics//////////////////////////////////////////
        void DrawRect(const Rect& rect, const Color& color);
        void DrawRectOutline(const Rect& rect, const Color& color);
        void DrawLine(const Vector2& vec1, const Vector2& vec2, const Color& color);
        void DrawImage(const char* path);
        void DrawImageFromMem(std::byte* pSource, size_t size, Rect* pSrc, Rect* pDest); //bad
        void DrawTexture(Texture& texture, Rect* pSrc, Rect* pDest);
        Texture CreateTexture(const char* path);
        Texture CombineCurrentView();
        void RenderNow();
        void CleanRenderer();

        //FileIO//////////////////////////////////////////
        bool LoadFile(const char* path);
        std::byte* GetResource(const char* path);
        size_t GetResourceSize(const char* path);

        //Event//////////////////////////////////////////
        void RegisterListener(EventListener* pListener,const char* pEventType) { m_eventManger.AddListener(pListener, pEventType); }
        void ClearListener() { m_eventManger.ClearListener(); }
        void Notify(Event evt);

        //UI
        void DrawUIFrame(bool b) { m_UIManager.WillDrawDebugFrame(b); }
        Font CreateFont(const char* path,int height);
        Texture CreateTextTexture(Font& font, std::string& text, Color color);
        void AddUIElement(UIElement* pElement);
        void ClearUI() { m_UIManager.ClearUI(); }
        UIElement* GetUIElement(const char* pName);

        //Lua
        //Test
        UIElement* Lua_LoadUIElement(const char* pPath);
        bool LoadUI(const char* pPath);
        bool LoadFilesFromScript(const char* pPath);
        bool LoadGameSettings(const char* pPath);
        
        //GameObjects
        void AddGameObject(GameObject* pObject) { m_gameObjectManager.AddGameObject(pObject); }
        void DestroyGameObject(size_t id) { m_gameObjectManager.RemoveGameObject(id); }
        void DestroyAllGameObject() { m_gameObjectManager.DestroyAll(); }

        //Misc
        const GameSetting& GetGameSetting() const { return m_gameSetting; }
        float GetDeltaTime() const { return m_timer.DeltaTime(); }

    private:
        Engine();
        bool Init(const char* pGameConfig);
        void Update(IGame& game,void (IGame::*pGameUpdate)(float),float deltaTime);
        void ShutDown();
        void ShowEngineStatus(float deltaTime);
    };
}