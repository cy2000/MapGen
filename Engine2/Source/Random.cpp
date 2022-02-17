#include "pch.h"
#include "Random.h"
#include "SimpleMath.h"
#include <assert.h>

static uint64_t s_seed = 0;

void E2::Seed(uint64_t seed)
{
    s_seed = seed;
}

uint64_t E2::GetSeed()
{
    return s_seed;
}

uint64_t E2::Random()
{
    uint64_t x = s_seed;
    x ^= x >> 12;
    x ^= x << 25;
    x ^= x >> 27;
    s_seed = x;

    return x * 0x2545F4914F6CDD1DULL;
}

uint64_t E2::Random(uint64_t min, uint64_t max)
{
    assert(max >= min);
    if (max == min)
    {
        return min;
    }
    auto range = max - min +1;
    auto randomNum = Random();

    return randomNum % range + min;
}


float E2::RandomF(float min, float max)
{
    assert(max > min);
    float weight = (float)Random(0, 1000) / 1000.f;
    return E2::Lerp(min, max, weight);
}
