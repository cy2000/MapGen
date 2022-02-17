#pragma once

namespace E2
{
    inline float Lerp(float min, float max, float weight)
    {
        return  (1 - weight) * min + weight * max;
    }

    inline float SmoothStep(float x)
    {
        if (x < 0)
            return 0;
        if (x > 1)
            return 1;

        return x * x * (3.f - 2.f * x);
    }

    inline float Absolute(float x)
    {
        if (x > 0)
        {
            return x;
        }
        else if(x < 0)
        {
            return x * -1;
        }
        else
        {
            return x;
        }
    }

    template<typename type>
    type Clamp(type min, type max, type value)
    {
        if (value > max)
        {
            return max;
        }
        if (value < min)
        {
            return min;
        }
        return value;
    }
}