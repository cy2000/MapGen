#pragma once

namespace E2
{
    struct Vector2
    {
        int x;
        int y;

        Vector2()
            :Vector2(0,0)
        {
        }

        Vector2(int inX, int inY)
            : x{inX}
            , y{inY}
        {
        }
    };

    struct Vector2f
    {
        float x;
        float y;

        Vector2f()
            :Vector2f(0, 0)
        {
        }

        Vector2f(float inX, float inY)
            : x{ inX }
            , y{ inY }
        {
        }

        //dot product
        float operator*(Vector2f& left)
        {
            return x * left.x + y * left.y;
        }

        Vector2f operator/(float value)
        {
            return Vector2f{ x / value, y / value };
        }

        Vector2f operator+(Vector2f& left)
        {
            return Vector2f{ x + left.x , y + left.y };
        }
    };
}