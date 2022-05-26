#pragma once
#include <functional>
namespace E2
{
    inline size_t HashString(const std::string& str)
    {
        return std::hash<std::string>{}(str);
    }
}