#pragma once
#include <Vector2.h>

#include <vector>
#include <utility>

struct Tile;
class Grid;

using RegionTile = E2::Vector2;
using Zone = std::pair<RegionTile, RegionTile>;
struct Region
{
    Region* m_pParent;
    Region* m_pLeft;
    Region* m_pRight;
    Zone m_region;
    Zone m_innerRegion;
    Zone m_splitLine;
    int m_depth;

public:
    Region();
    ~Region();

    int TileCount();
    
    void Split(int minTileCount,int minSplitSize);
    static std::vector<Region*> FindSplittableRegions(Region* pRoot);

    template<typename Func, typename... Targs>
    static void WalkTheTree(Region* pRoot, Func&& function, Targs... inArgs);
};

template<typename Func, typename... Targs>
inline static void Region::WalkTheTree(Region* pRoot, Func&& function, Targs... inArgs)
{
    if (!pRoot)
    {
        return;
    }
    WalkTheTree(pRoot->m_pLeft, function, inArgs...);
    WalkTheTree(pRoot->m_pRight, function, inArgs...);
    function(pRoot, inArgs...);
}
