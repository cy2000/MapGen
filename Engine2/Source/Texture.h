#pragma once
#include "Vector2.h"
#include "Macro.h"
namespace E2
{
    enum class TextureType
    {
        BMP,
        JPG,
        PNG,
        Text,
        Unknown,
    };
    struct Texture
    {
        TextureType type;
        Vector2 dememsion;
        _Texture* pTexture;
        Texture();
    };
}
