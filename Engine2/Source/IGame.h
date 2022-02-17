#pragma once
#include <string>
namespace E2
{
    struct GameInfo
    {
        std::string m_gameName;
        int m_windowWidth;
        int m_windowHeight;
        int m_tileSize;
    };

    const GameInfo kDefaultSetting{ {"Default"},{720},{720},{16} };
    
    class IGame
    {
    public:
        virtual ~IGame() = default;
        virtual bool Init() = 0;
        virtual void Update() = 0;
        virtual void ShutDown() = 0;
        virtual const GameInfo& GetInfo() = 0;
    };
}
