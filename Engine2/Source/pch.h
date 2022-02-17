// pch.h: This is a precompiled header file.
// Files listed below are compiled only once, improving build performance for future builds.
// This also affects IntelliSense performance, including code completion and many code browsing features.
// However, files listed here are ALL re-compiled if any one of them is updated between builds.
// Do not add files here that you will be updating frequently as this negates the performance advantage.

#ifndef PCH_H
#define PCH_H

// add headers that you want to pre-compile here
#include "framework.h"

#include <unordered_map>
#include <vector>
#include <assert.h>

#if defined(USING_SDL)
#include <SDL.h>
//#include <SDL_image.h>

#elif defined(USING_SFML)


#endif
#endif //PCH_H
