#include "Region.h"

#include <Random.h>
#include <stack>

constexpr int kInvalid = std::numeric_limits<int>::max();

Region::Region()
    : m_pParent{ nullptr }
    , m_pLeft{ nullptr }
    , m_pRight{ nullptr }
    , m_region{ {kInvalid,kInvalid},{kInvalid,kInvalid} }
    , m_innerRegion{ {kInvalid,kInvalid},{kInvalid,kInvalid} }
    , m_splitLine{ {kInvalid,kInvalid},{kInvalid,kInvalid} }
    , m_depth{ 0 }
{
}

Region::~Region()
{
    delete m_pLeft;
    delete m_pRight;
}

int Region::TileCount()
{
    return (m_region.second.x - m_region.first.x + 1) * (m_region.second.y - m_region.first.y + 1);
}

void Region::Split(int minTileCount, int minSplitSize)
{
    //get the long side 
    int horizontalLength = m_region.second.x - m_region.first.x;
    int verticalLength = m_region.second.y - m_region.first.y;

    //Split vertically
    if (horizontalLength > verticalLength)
    {
        //the split shall happen closer to the center
        int leftX = m_region.first.x + minSplitSize;
        int rightX = m_region.second.x - minSplitSize;
        if (leftX >= rightX)
        {
            return;
        }
        int splitX = (int)E2::Random(leftX, rightX);
        m_pLeft = new Region();
        m_pRight = new Region();
        m_pLeft->m_pParent = this;
        m_pRight->m_pParent = this;
        m_pLeft->m_depth = m_depth + 1;
        m_pRight->m_depth = m_depth + 1;

        m_pLeft->m_region.first = m_region.first;
        m_pLeft->m_region.second = { splitX - 1, m_region.second.y };

        m_pRight->m_region.first = { splitX + 1, m_region.first.y };
        m_pRight->m_region.second = m_region.second;

        RegionTile splitLineStart = { splitX , m_region.first.y };
        RegionTile splitLineEnd = { splitX , m_region.second.y };
        m_splitLine = { splitLineStart ,splitLineEnd };
    }
    //Split horizontally
    else
    {
        //the split shall happen closer to the center
        int topY = m_region.first.y + minSplitSize;
        int bottomY = m_region.second.y - minSplitSize;
        if (topY >= bottomY)
        {
            return;
        }
        int splitY = (int)E2::Random(topY, bottomY);

        m_pLeft = new Region();
        m_pRight = new Region();
        m_pLeft->m_pParent = this;
        m_pRight->m_pParent = this;
        m_pLeft->m_depth = m_depth + 1;
        m_pRight->m_depth = m_depth + 1;

        m_pLeft->m_region.first = m_region.first;
        m_pLeft->m_region.second = { m_region.second.x, splitY - 1 };

        m_pRight->m_region.first = { m_region.first.x, splitY + 1};
        m_pRight->m_region.second = m_region.second;

        RegionTile splitLineStart = {m_region.first.x, splitY};
        RegionTile splitLineEnd = {m_region.second.x, splitY};
        m_splitLine = {splitLineStart,splitLineEnd};
    }
}

std::vector<Region*> Region::FindSplittableRegions(Region* pRoot)
{
    std::vector<Region*> availableNodes;
    std::stack<Region*> helper;
    // do an inorder walk
    Region* pCurrentNode = pRoot;
    while (pCurrentNode || !helper.empty())
    {
        while (pCurrentNode)
        {
            if (!pCurrentNode->m_pLeft && !pCurrentNode->m_pRight)
            {
                availableNodes.push_back(pCurrentNode);
            }

            helper.push(pCurrentNode);
            pCurrentNode = pCurrentNode->m_pLeft;
        }

        pCurrentNode = helper.top();
        helper.pop();

        pCurrentNode = pCurrentNode->m_pRight;
    }
    return availableNodes;
}
