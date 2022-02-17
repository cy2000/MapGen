#include "MapGenerator.h"
#include <FreeFunctions.h>

#ifdef _DEBUG
//#include <vld.h>
#endif // _DEBUG

#ifdef main
#undef main
#endif // main

int main()
{
    GetEngine().Run(MapGenerator::Get());
    return 0;
}
