#include "AITest.h"
#include <FreeFunctions.h>

#ifdef _DEBUG
//#include <vld.h>
#endif // _DEBUG

#ifdef main
#undef main
#endif // main

int main()
{
    GetEngine().Run(AITest::Get());
    return 0;
}

//MOD01
//AITest::Init() creates 2 triangles.
//AITest::Update() moves and spins them.
//E2::TransformComponent has transform of the object.
//E2::ShapeComponent::Update() calculates the position of the polygon.
// 
//There are no 'dancing' functions.
//