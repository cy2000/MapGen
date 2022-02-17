#include "LocalMap.h"
#include "HeightBiomes.h"
#include "Grid.h"
#include "Region.h"

#include <Random.h>
#include <FreeFunctions.h>
#include <map>

constexpr float kColorValue = 0.7f;
constexpr int kCitySplitTimes = 10;
constexpr int kMinSplitTileCount = 50;
constexpr int kMinSplitSize = 8;
constexpr int kBuildingSizeInverse = 3;
LocalMap::LocalMap()
    : m_pGrid{nullptr}
{
}

LocalMap::~LocalMap()
{
    delete m_pGrid;
}

Tile* LocalMap::GetTile(int x, int y)
{
    return m_pGrid->GetTile(x, y);
}

void LocalMap::Reset(int maxX, int maxY, int tileSize)
{
    delete m_pGrid;
    for (auto* pRegion : m_rootRegions)
    {
        delete pRegion;
    }
    m_rootRegions.clear();
    m_cities.clear();
    m_pGrid = new Grid;
    m_pGrid->Init(maxX,maxY,tileSize);
}

void LocalMap::CopyTileInfo(int index, Tile* pInTile)
{
    auto* pTile = m_pGrid->GetTile(index);
    pTile->type = pInTile->type;
    pTile->heightNoise = pInTile->heightNoise;
}

void LocalMap::CopyTileInfo(int x, int y, Tile* pInTile)
{
    auto* pTile = m_pGrid->GetTile(x,y);
    pTile->type = pInTile->type;
    pTile->heightNoise = pInTile->heightNoise;
}

void LocalMap::Terrace(int level)
{
    assert(level > 0);
    for (int i = 0; i < m_pGrid->Size(); ++i)
    {
        Tile* pTile = m_pGrid->GetTile(i);
        pTile->heightNoise = (float)(std::lroundf(pTile->heightNoise * (float)level)) / (float)level;
    }
}

void LocalMap::SplitRegion(Region* pRoot)
{
    auto availableNodes = Region::FindSplittableRegions(pRoot);
    // TODO: make weighted random
    auto choice = E2::Random(0, availableNodes.size() - 1);
    availableNodes[choice]->Split(kMinSplitTileCount,kMinSplitSize);
}

void LocalMap::GenerateBuildings()
{
    FindCities();
    for (auto& [start, end] : m_cities)
    {
        auto* pRegion = new Region();
        pRegion->m_region = { {start->x,start->y},{end->x,end->y} };
        m_rootRegions.push_back(pRegion);
        for (int i = 0; i < kCitySplitTimes; ++i)
        {
            SplitRegion(pRegion);
        }
    }
    SpawnBuildings();
}

void LocalMap::ConnectBuildings()
{
    std::map<int , std::vector<Region*>> openSet;
    std::vector<Region*> closeSet;

    auto sortRegion = [](Region* pRegion, std::map<int, std::vector<Region*>>* openSet)
    {
        (*openSet)[pRegion->m_depth].push_back(pRegion);
    };

    for (auto* pRoot : m_rootRegions)
    {
        Region::WalkTheTree(pRoot, sortRegion, &openSet);

        for (int i = (int)openSet.size(); i>=0; --i)
        {
            auto& regionVector = openSet[i];
            for (auto* pRegion : regionVector)
            {
                //check if this region is done already
                bool regionIsDone = false;
                for (auto* pClosedRegion : closeSet)
                {
                    if (pClosedRegion == pRegion)
                    {
                        regionIsDone = true;
                        break;
                    }
                }
                if (regionIsDone)
                {
                    continue;
                }

                //find sibling
                if (pRegion->m_pParent == nullptr)
                {
                    break;
                }
                Region* pSibling = nullptr;
                if (pRegion->m_pParent->m_pLeft == pRegion)
                {
                    pSibling = pRegion->m_pParent->m_pRight;
                }
                else
                {
                    pSibling = pRegion->m_pParent->m_pLeft;
                }
                assert(pSibling != pRegion);
                assert(pSibling->m_depth == pRegion->m_depth);

                //connect siblings

                int midX1 = 0;
                int midY1 = 0;
                int midX2 = 0;
                int midY2 = 0;

                if (!(pRegion->m_pLeft && pRegion->m_pRight))
                {
                    midX1 = (pRegion->m_innerRegion.first.x + pRegion->m_innerRegion.second.x) / 2;
                    midY1 = (pRegion->m_innerRegion.first.y + pRegion->m_innerRegion.second.y) / 2;
                }
                else
                {
                    std::vector<Tile*> roadTiles;
                    int startX = pRegion->m_innerRegion.first.x;
                    int startY = pRegion->m_innerRegion.first.y;
                    int endX = pRegion->m_innerRegion.second.x;
                    int endY = pRegion->m_innerRegion.second.y;
                    for (int y = startY; y <= endY; ++y)
                    {
                        for (int x = startX; x <= endX; ++x)
                        {
                            auto* pTile = m_pGrid->GetTile(x, y);
                            if (pTile->type == Tile::Type::CityRoad)
                            {
                                roadTiles.push_back(pTile);
                            }
                        }
                    }
                    auto* pRoadMid = roadTiles[roadTiles.size()/2];
                    midX1 = pRoadMid->x;
                    midY1 = pRoadMid->y;
                }
                  

                if (!(pSibling->m_pLeft && pSibling->m_pRight))
                {
                    midX2 = (pSibling->m_innerRegion.first.x + pSibling->m_innerRegion.second.x) / 2;
                    midY2 = (pSibling->m_innerRegion.first.y + pSibling->m_innerRegion.second.y) / 2;
                }
                else
                {
                    std::vector<Tile*> roadTiles;
                    int startX = pSibling->m_innerRegion.first.x;
                    int startY = pSibling->m_innerRegion.first.y;
                    int endX = pSibling->m_innerRegion.second.x;
                    int endY = pSibling->m_innerRegion.second.y;
                    for (int y = startY; y <= endY; ++y)
                    {
                        for (int x = startX; x <= endX; ++x)
                        {
                            auto* pTile = m_pGrid->GetTile(x, y);
                            if (pTile->type == Tile::Type::CityRoad)
                            {
                                roadTiles.push_back(pTile);
                            }
                        }
                    }
                    auto* pRoadMid = roadTiles[roadTiles.size() / 2];
                    midX2 = pRoadMid->x;
                    midY2 = pRoadMid->y;
                }

                int startX = midX1 > midX2 ? midX2 : midX1;
                int endX = midX1 > midX2 ? midX1 : midX2;
                int startY = midY1 > midY2 ? midY2 : midY1;
                int endY = midY1 > midY2 ? midY1 : midY2;
                //TODO:Road Bug 
                //sometimes one tile off the road
                int usingX = 0;
                int usingY = 0;
                if (startY == midY1)
                {
                    usingX = midX2;
                }
                else
                {
                    usingX = midX1;
                }

                if (startX == midX1)
                {
                    usingY = midY2;
                }
                else
                {
                    usingY = midY1;
                }
                for (int y = startY; y <= endY; ++y)
                {
                    if (m_pGrid->GetTile(usingX, y)->type != Tile::Type::Building)
                    {
                        m_pGrid->GetTile(usingX, y)->type = Tile::Type::CityRoad;
                    }
                }
                for (int x = startX; x <= endX; ++x)
                {
                    if (m_pGrid->GetTile(x, usingY)->type != Tile::Type::Building)
                    {
                        m_pGrid->GetTile(x, usingY)->type = Tile::Type::CityRoad;
                    }
                }

                closeSet.push_back(pRegion);
                closeSet.push_back(pSibling);
            }
        }
        openSet.clear();
        closeSet.clear();
    }
    
}

void LocalMap::Draw()
{
    for (int i = 0; i < m_pGrid->Size(); ++i)
    {
        Tile* pTile = m_pGrid->GetTile(i);
        switch (pTile->type)
        {
        case Tile::Type::Empty: DrawRect(pTile->rect, PickColor(pTile->heightNoise)); break;
        case Tile::Type::City: DrawRect(pTile->rect, E2::Mono::kLightGray); break;
        case Tile::Type::CityRoad: DrawRect(pTile->rect, E2::Red::kRed); break;
        case Tile::Type::Road:DrawRect(pTile->rect, E2::Red::kPink); break;
        case Tile::Type::Building:DrawRect(pTile->rect, E2::Mono::kDark); break;
        }
    }
}

void LocalMap::DrawHeight()
{
    for (int i = 0; i < m_pGrid->Size(); ++i)
    {
        Tile* pTile = m_pGrid->GetTile(i);
        E2::Color c = E2::Color_hsva(240.f - 120.f * (float)pTile->heightNoise, 1.f, kColorValue);
        DrawRect(pTile->rect, c);
    }
}

void LocalMap::DrawHeightTerrace(int level)
{
    for (int i = 0; i < m_pGrid->Size(); ++i)
    {
        Tile* pTile = m_pGrid->GetTile(i);
        auto newHeightNoise = (float)(std::lroundf(pTile->heightNoise * (float)level)) / (float)level;
        E2::Color c = E2::Color_hsva(240.f - 120.f * newHeightNoise,1.f, kColorValue);
        DrawRect(pTile->rect, c);
    }
}

void LocalMap::FindCities()
{
    for (int i = 0; i < m_pGrid->Size(); ++i)
    {
        Tile* pTile = m_pGrid->GetTile(i);
        if (IsTileInCity(pTile))
        {
            continue;
        }
        if (pTile->type == Tile::Type::City || pTile->type == Tile::Type::CityRoad)
        {
            auto* pStartTile = pTile;
            //find the end tile
            auto* pEastEnd = pTile;
            auto* pNeighborEast = m_pGrid->GetNeighborTile(pTile, Grid::Direction::East);
            while (pNeighborEast
                && (pNeighborEast->type == Tile::Type::City || pNeighborEast->type == Tile::Type::CityRoad))
            {
                pEastEnd = pNeighborEast;
                pNeighborEast = m_pGrid->GetNeighborTile(pNeighborEast, Grid::Direction::East);
            }

            auto* pSouthEnd = pTile;
            auto* pNeighborSouth = m_pGrid->GetNeighborTile(pTile, Grid::Direction::South);
            while (pNeighborSouth
                && (pNeighborSouth->type == Tile::Type::City || pNeighborSouth->type == Tile::Type::CityRoad))
            {
                pSouthEnd = pNeighborSouth;
                pNeighborSouth = m_pGrid->GetNeighborTile(pNeighborSouth, Grid::Direction::South);
            }

            auto* pEndTile = m_pGrid->GetTile(pEastEnd->x, pSouthEnd->y);
            m_cities.push_back({ pStartTile ,pEndTile });
            //return;
        }
    }
}

void LocalMap::SpawnBuildings()
{
    auto spawnBuilding = [](Region* pRegion, Grid* pGrid)
    {
        if (pRegion->m_pLeft && pRegion->m_pRight)
        {
            auto& leftRegion = pRegion->m_pLeft->m_innerRegion;
            auto& rightRegion = pRegion->m_pRight->m_innerRegion;
            auto startX = leftRegion.first.x < rightRegion.first.x ? leftRegion.first.x : rightRegion.first.x;
            auto startY = leftRegion.first.y < rightRegion.first.y ? leftRegion.first.y : rightRegion.first.y;
            auto endX = leftRegion.second.x > rightRegion.second.x ? leftRegion.second.x : rightRegion.second.x;
            auto endY = leftRegion.second.y > rightRegion.second.y ? leftRegion.second.y : rightRegion.second.y;
            pRegion->m_innerRegion = { {startX,startY},{endX,endY} };
        }
        else
        {
            auto startX = pRegion->m_region.first.x + (int)E2::Random(1, kBuildingSizeInverse);
            auto startY = pRegion->m_region.first.y + (int)E2::Random(1, kBuildingSizeInverse);
            auto endX = pRegion->m_region.second.x - (int)E2::Random(1, kBuildingSizeInverse);
            auto endY = pRegion->m_region.second.y - (int)E2::Random(1, kBuildingSizeInverse);
            pRegion->m_innerRegion = { {startX,startY},{endX,endY} };

            for (int y = startY; y < endY; ++y)
            {
                for (int x = startX; x < endX; ++x)
                {
                    auto* pTile = pGrid->GetTile(x, y);
                    pTile->type = Tile::Type::Building;
                }
            }
        }
    };

    for (auto* pRegion : m_rootRegions)
    {
        Region::WalkTheTree(pRegion, spawnBuilding, m_pGrid);
    }
}

bool LocalMap::IsTileInCity(Tile* pTile)
{
    for (auto& [start, end] : m_cities)
    {
        if (pTile->x >= start->x && pTile->y >= start->y)
        {
            if (pTile->x <= end->x && pTile->y <= end->y)
            {
                return true;
            }
        }
    }
    return false;
}

const E2::Color& LocalMap::PickColor(float noise)
{
    if (noise < kDeepWater)
    {
        return E2::Blue::kNavy;
    }
    else if (noise < kShallowWater)
    {
        return E2::Blue::kRoyalBlue;
    }
    else if (noise < kBeach)
    {
        return E2::Yellow::kLemon;
    }
    else if (noise < kSwamp)
    {
        return E2::Red::kRedWood;
    }
    else if (noise < kGrass)
    {
        return E2::Green::kLightGreen;
    }
    else if (noise < kForest)
    {
        return E2::Green::kForest;
    }
    else if (noise < kJungle)
    {
        return E2::Green::kYellowGreen;
    }
    else if (noise < kSavannah)
    {
        return E2::Green::kOlive;
    }
    else if (noise < kTundra)
    {
        return E2::Red::kTuscan;
    }
    else if (noise < kRock)
    {
        return E2::Red::kApricot;
    }
    else
    {
        return E2::Mono::kWhite;
    }
}
