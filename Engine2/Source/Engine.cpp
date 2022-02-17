#include "pch.h"
#include "Engine.h"
#include "HashString.h"
#include "Random.h"
#include <iostream>
#include <ctime>

E2::Engine::Engine()
    : m_quit{ false }
    , m_manuallyUpdate{false}
    , m_canUpdate{true}
{
}

bool E2::Engine::Init(const GameInfo& info)
{
    std::cout << "Engine Init\n";
    bool ready = false;
    if (info.m_windowWidth == 0 || info.m_windowHeight == 0)
    {
        std::cout << "Using default values to init engine\n";
        ready = m_device.Init(kDefaultSetting);
    }
    else
    {
        ready = m_device.Init(info);
    }
    if (GetSeed() == 0)
    {
        Seed(std::time(nullptr));
    }
    return ready;
}

void E2::Engine::Update(IGame& game,void(IGame::* pGameUpdate)())
{
    m_eventManger.Update();
    (game.*pGameUpdate)();
    m_UIManager.Update();
    m_UIManager.Draw();
}

bool E2::Engine::Run(IGame& game)
{
    if (!(Init(game.GetInfo())&& game.Init()))
    {
        return false;
    }

    std::cout << "Engine Runs Game\n";
    while (!m_quit)
    {
        m_device.ProcessInput();
        if (m_canUpdate)
        {
            Update(game, &IGame::Update);
            m_device.Render();
        }
        if (m_manuallyUpdate && m_canUpdate)
        {
            m_canUpdate = false;
        }
        m_keyboard.NextFrame();
        m_mouse.NextFrame();
    }

    game.ShutDown();
    ShutDown();
    return true;
}

bool E2::Engine::PointInRect(int x, int y, Rect& rect)
{
    return m_device.PointInRect(x,y,rect.x,rect.y,rect.w,rect.h);
}

E2::Vector2 E2::Engine::GetWindowSize()
{
    E2::Vector2 vec2;
    m_device.GetWindowSize(vec2.x, vec2.y);
    return vec2;
}

void E2::Engine::ToggleManuallyUpdate()
{
    m_manuallyUpdate = !m_manuallyUpdate;
}

void E2::Engine::DrawRect(const Rect& rect, const Color& color)
{
    m_device.DrawRect(rect.x,rect.y,rect.w,rect.h,color.r,color.g,color.b,color.a);
}

void E2::Engine::DrawRectOutline(const Rect& rect, const Color& color)
{
    m_device.DrawRectOutline(rect.x, rect.y, rect.w, rect.h, color.r, color.g, color.b, color.a);
}

void E2::Engine::DrawLine(const Vector2& vec1, const Vector2& vec2, const Color& color)
{
    m_device.DrawLine(vec1.x,vec1.y,vec2.x,vec2.y, color.r, color.g, color.b, color.a);
}

void E2::Engine::DrawImage(const char* path)
{
    auto hash = HashString(path);
    auto p = m_resourceManager.GetResource(hash);
    if (p)
    {
        auto size = m_resourceManager.GetResourceSize(hash);
        DrawImageFromMem(p, size, nullptr, nullptr);
    }
}

void E2::Engine::DrawImageFromMem(std::byte* pSource, size_t size, Rect* pSrc, Rect* pDest)
{
    m_device.DrawImageFromMem(pSource,size,pSrc,pDest);
}

void E2::Engine::DrawTexture(Texture& texture, Rect* pSrc, Rect* pDest)
{
    m_device.DrawTexture(texture, pSrc, pDest);
}

bool E2::Engine::LoadFile(const char* path)
{
    return m_resourceManager.Load(path);
}

std::byte* E2::Engine::GetResource(const char* path)
{
    return m_resourceManager.GetResource(HashString(path));
}

size_t E2::Engine::GetResourceSize(const char* path)
{
    return m_resourceManager.GetResourceSize(HashString(path));
}

E2::Texture E2::Engine::CreateTexture(const char* path)
{
    auto id = HashString(path);
    auto texture = m_resourceManager.GetTexture(id);

    if (texture.pTexture)
    {
        return texture;
    }
    else
    {
        auto* pResource = GetResource(path);
        if (pResource)
        {
            auto size = GetResourceSize(path);
            TextureType type = m_resourceManager.GetTextureType(id);
            auto newtexture = m_device.CreateTexture(pResource, size, type);
            m_resourceManager.SaveTexture(id, newtexture);
            return newtexture;
        }
        return {};
    }
}

void E2::Engine::ForceRender()
{
    m_device.ForceRender();
}

E2::Font E2::Engine::CreateFont(const char* path, int height)
{
    auto* pResource = GetResource(path);
    if (pResource)
    {
        auto size = GetResourceSize(path);
        auto pNewFont = m_device.CreateFont(pResource, size, height);
        return pNewFont;
    }
    return {};
}

E2::Texture E2::Engine::CreateTextTexture(Font& font, std::string& text, Color color)
{
    auto newtexture = m_device.CreateTextTexture(font, text, color.r, color.g, color.b, color.a);
    return newtexture;
}

void E2::Engine::Notify(Event* pEvent)
{
    m_eventManger.NotifyUI(pEvent);
}

void E2::Engine::AddUIElement(UIElement* pElement)
{
    m_UIManager.AddElement(pElement);
}

void E2::Engine::ShutDown()
{
    m_device.ShutDown();
    std::cout << "Engine Shuts Down\n";
}

E2::Engine& E2::Engine::Get()
{
    static Engine instance;
    return instance;
}
