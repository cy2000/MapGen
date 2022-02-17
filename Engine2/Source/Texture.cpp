#include "pch.h"
#include "Texture.h"

E2::Texture::Texture()
    : type{ TextureType::Unknown }
    , dememsion{ 0,0 }
    , pTexture{ nullptr }
{}