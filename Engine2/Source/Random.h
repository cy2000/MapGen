#pragma once
#include <cstdint>

// Xorshift* 
// https://en.wikipedia.org/wiki/Xorshift
namespace E2
{
    void Seed(uint64_t seed);
    uint64_t GetSeed();

    uint64_t Random();
    uint64_t Random(uint64_t min, uint64_t max);

    float RandomF(float min, float max);
}