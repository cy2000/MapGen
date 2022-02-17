#include "WorldMap.h"
#include "Grid.h"
#include "HeightBiomes.h"
#include "Region.h"

#include <Random.h>
#include <Color.h>
#include <FreeFunctions.h>
#include <PerlinNoise.h>
#include <SimpleMath.h>

#include <vector>
#include <stack>
#include <queue>
#include <set>
#include <iostream>



////////////////////////////////////////////////////////////////////////
//
// World Map
//
////////////////////////////////////////////////////////////////////////
constexpr int kDefaultInputRange = 10;
constexpr int kMaxInputRange = 15;
constexpr int kDefaultOctaves = 4;
constexpr int kMaxOctaves = 5;
constexpr float kDefaultPersistence = 0.6f;
constexpr float kMaxPersistence = 1.f;

constexpr int kMinAreaTileCount = 100;
constexpr int kMinSplitSize = 20;

constexpr int kLocalHeightLevel = 12;
constexpr int kCitySize = 4;
constexpr int kMaxTryToFindCity = 10;
constexpr int kCityDistanceMin2 = 210;

constexpr float kMaxFloat = std::numeric_limits<float>::max();
constexpr int kMaxInt = std::numeric_limits<int>::max();

WorldMap::WorldMap(int width, int height, int tileSize)
    : m_mapW{ width / tileSize }
    , m_mapH{ height / tileSize }
    , m_tileSize{ tileSize }
    , m_currentInputRange{kDefaultInputRange}
    , m_currentOctaves{kDefaultOctaves}
    , m_currentPersistence{kDefaultPersistence}
    , m_pGrid{nullptr}
    , m_pRootRegion{ nullptr }
{
    m_pGrid = new Grid;
    m_pGrid->Init(m_mapW,m_mapH, m_tileSize);
    m_pRootRegion = new Region();
    m_pRootRegion->m_region.first = { m_pGrid->GetTile(0)->x, m_pGrid->GetTile(0)->y };
    m_pRootRegion->m_region.second = { m_pGrid->GetLastTile()->x, m_pGrid->GetLastTile()->y };
}

WorldMap::~WorldMap()
{
    delete m_pRootRegion;
    delete m_pGrid;
}

////////////////////////////////////////////////////////////////////////
// Generate Basic Height Noise
////////////////////////////////////////////////////////////////////////
void WorldMap::GenerateBasicHeightNoise()
{
    float longSide = (float)((m_pGrid->GetColumn() > m_pGrid->GetRow()) ? m_pGrid->GetColumn() : m_pGrid->GetRow());
    float currentAmplitude = 0;
    float totalAmplitude = 0;
    int currentInputRange = 0;

    size_t* seedForEachOctave = new size_t[m_currentOctaves];
    for (int i = 0; i < m_currentOctaves; ++i)
    {
        seedForEachOctave[i] = E2::Random();
    }

    for (int i = 0; i < m_pGrid->Size(); ++i)
    {
        Tile* pTile = m_pGrid->GetTile(i);
        pTile->rawNoise = 0;

        currentAmplitude = 1.f;
        totalAmplitude = 0;
        currentInputRange = m_currentInputRange;

        for (int j = 0; j < m_currentOctaves; ++j)
        {
            float dx = (float)pTile->x / longSide;
            float dy = (float)pTile->y / longSide;
            auto rawNoise = PerlinNoise::Perlin(dx * currentInputRange, dy * currentInputRange, seedForEachOctave[j]);    //[-1,1]
            rawNoise = rawNoise / 2 + 0.5f;     //[0,1]
            pTile->rawNoise += currentAmplitude * rawNoise;

            totalAmplitude += currentAmplitude;
            currentAmplitude *= m_currentPersistence;
            currentInputRange *= 2;
        }

        pTile->rawNoise /= totalAmplitude;
        pTile->heightNoise = pTile->rawNoise;
    }
}

////////////////////////////////////////////////////////////////////////
// Adjust Height Noise for each region
////////////////////////////////////////////////////////////////////////
void WorldMap::AdjustRegionHeightNoise()
{
    auto availableNodes = Region::FindSplittableRegions(m_pRootRegion);
    for (auto& node : availableNodes)
    {
        int startX = node->m_region.first.x;
        int startY = node->m_region.first.y;
        int endX = node->m_region.second.x;
        int endY = node->m_region.second.y;

        auto emptiness = RegionEmptiness(node->TileCount(), 10);
        for (int y = startY; y <= endY; ++y)
        {
            for (int x = startX; x <= endX; ++x)
            {
                Tile* pTile = m_pGrid->GetTile(x, y);
                {
                    auto nx = (float)(pTile->x - startX) / (float)(endX - startX) - 0.5f;
                    auto ny = (float)(pTile->y - startY) / (float)(endY - startY) - 0.5f;

                    auto d = std::sqrtf(nx * nx + ny * ny) / std::sqrtf(0.5f);
                    //auto d = 2 * std::max(abs(nx), abs(ny));
                    //auto d = abs(nx) + abs(ny);
                    d = (emptiness + 3 * d) / 4;
                    auto layerNoise = (1 - d + pTile->rawNoise) / 2;
                    pTile->heightNoise = (layerNoise * 0.7f + pTile->rawNoise * 0.3f);
                    pTile->heightNoise *= emptiness;
                    pTile->heightNoise = E2::SmoothStep(pTile->heightNoise);
                }
            }
        }
    }
}

////////////////////////////////////////////////////////////////////////
// ReShape Height Noise to [0,1]
////////////////////////////////////////////////////////////////////////
void WorldMap::ReShapeHeightNoise()
{
    float minNoise = 1.f;
    float maxNoise = 0;
    for (int i = 0; i < m_pGrid->Size(); ++i)
    {
        auto* pTile = m_pGrid->GetTile(i);
        if (pTile->heightNoise <= minNoise)
        {
            minNoise = pTile->heightNoise;
        }
        else if (pTile->heightNoise >= maxNoise)
        {
            maxNoise = pTile->heightNoise;
        }
    }
    float noiseRange = maxNoise - minNoise;
    for (int i = 0; i < m_pGrid->Size(); ++i)
    {
        auto* pTile = m_pGrid->GetTile(i);
        float weight = (pTile->heightNoise - minNoise) / noiseRange;
        pTile->heightNoise = E2::Lerp(0, 1.f, weight);
    }
}

////////////////////////////////////////////////////////////////////////
// This is VERY important!
// not calling this changes the result of reshaping height noise
//
// Split lines causes noise to change abruptly 
// TODO: Make it smoother
////////////////////////////////////////////////////////////////////////
void WorldMap::SmoothNoiseAroundSplitLines()
{
    auto func = [](Region* pRegion,Grid* pGrid)
    {
        if (pRegion->m_splitLine.first.x == kMaxInt)
        {
            return;
        }
        auto& splitLine = pRegion->m_splitLine;
        bool lineIsVertical = (splitLine.first.x == splitLine.second.x) ? true : false;

        if (lineIsVertical)
        {
            for (int y = splitLine.first.y; y <= splitLine.second.y; ++y)
            {
                auto* pTile = pGrid->GetTile(splitLine.first.x, y);
                auto* pLeftTile = pGrid->GetTile(splitLine.first.x - 1, y);
                auto* pRightTile = pGrid->GetTile(splitLine.first.x + 1, y);
                pTile->heightNoise = (pLeftTile->heightNoise + pRightTile->heightNoise) / 2;
            }
        }
        else
        {
            for (int x = splitLine.first.x; x <= splitLine.second.x; ++x)
            {
                auto* pTile = pGrid->GetTile(x, splitLine.first.y);
                auto* pUpTile = pGrid->GetTile(x, splitLine.first.y - 1);
                auto* pDownTile = pGrid->GetTile(x, splitLine.first.y + 1);
                pTile->heightNoise = (pUpTile->heightNoise + pDownTile->heightNoise) / 2;
            }
        }
    };
    Region::WalkTheTree(m_pRootRegion, func, m_pGrid);
}


void WorldMap::DrawHeightNoise()
{
    for (int i = 0; i < m_pGrid->Size(); ++i)
    {
        Tile* pTile = m_pGrid->GetTile(i);

        auto gray = (uint8_t)E2::Lerp(0, 255, pTile->heightNoise);
        DrawRect(pTile->rect, E2::Color{ gray ,gray ,gray, 255});
    }
}

void WorldMap::DrawHeightBiome()
{
    for (int i = 0; i < m_pGrid->Size(); ++i)
    {
        Tile* pTile = m_pGrid->GetTile(i);

        switch (pTile->type)
        {
        case Tile::Type::Empty: DrawRect(pTile->rect, PickColor(pTile->heightNoise)); break;
        case Tile::Type::City:
        case Tile::Type::CityRoad: DrawRect(pTile->rect, E2::Red::kRed); break;
        case Tile::Type::Road:DrawRect(pTile->rect, E2::Red::kPink); break;
        }
    }
}

////////////////////////////////////////////////////////////////////////
// Randomly choose a region and split it once
////////////////////////////////////////////////////////////////////////
void WorldMap::SplitRegion()
{
    auto availableNodes = Region::FindSplittableRegions(m_pRootRegion);
    // TODO: make weighted random
    auto choice = E2::Random(0, availableNodes.size() - 1);
    availableNodes[choice]->Split(kMinAreaTileCount,kMinSplitSize);
}

////////////////////////////////////////////////////////////////////////
// Draw the split lines with red color
////////////////////////////////////////////////////////////////////////
void WorldMap::DrawSplitLines()
{
    auto drawLine = [](Region* pRegion, int tileSize)
    {
        if (pRegion->m_splitLine.first.x != kMaxInt)
        {
            E2::Rect rect{ pRegion->m_splitLine.first.x * tileSize
                         , pRegion->m_splitLine.first.y * tileSize
                         , (pRegion->m_splitLine.second.x - pRegion->m_splitLine.first.x +1) * tileSize
                         , (pRegion->m_splitLine.second.y - pRegion->m_splitLine.first.y +1) * tileSize };

            DrawRect(rect, E2::Red::kRed);
        }

        E2::Rect rect1{ pRegion->m_region.first.x * tileSize ,pRegion->m_region.first.y * tileSize,tileSize,tileSize };
        E2::Rect rect2{ pRegion->m_region.second.x * tileSize,pRegion->m_region.second.y * tileSize,tileSize,tileSize };
        DrawRect(rect1,E2::Red::kPink);
        DrawRect(rect2,E2::Red::kPink);
    };

    Region::WalkTheTree(m_pRootRegion, drawLine ,m_tileSize);
}

////////////////////////////////////////////////////////////////////////
// Found cities based on height 
////////////////////////////////////////////////////////////////////////
void WorldMap::FoundCity(float minHeight, float maxHeight)
{
    m_cities.clear();

    auto availableNodes = Region::FindSplittableRegions(m_pRootRegion);

    for (auto* node : availableNodes)
    {
        std::vector<Tile*> okTiles;
        int startX = node->m_region.first.x;
        int startY = node->m_region.first.y;
        int endX = node->m_region.second.x;
        int endY = node->m_region.second.y;
        
        for (int y = startY; y <= endY; ++y)
        {
            for (int x = startX; x <= endX; ++x)
            {
                auto* pTile = m_pGrid->GetTile(x,y);

                if (pTile->heightNoise > minHeight && pTile->heightNoise < maxHeight)
                {
                    okTiles.push_back(pTile);
                }
            }
        }

        int cityToFound = GetCityCount(node);
        int maxTry = kMaxTryToFindCity;
        while (maxTry>0 && cityToFound > 0 && cityToFound < okTiles.size())
        {
            Tile* pTile = nullptr;
            int maxTryToFindACity = kMaxTryToFindCity;
            while (maxTryToFindACity > 0)
            {
                pTile = okTiles[E2::Random(0, okTiles.size() - 1)];
                bool thisTileIsGood = true;
                //check if this tile is too close to current cities
                for (auto* pCity : m_cities)
                {
                    auto deltaX = pTile->x - pCity->x;
                    auto deltaY = pTile->y - pCity->y;
                    auto distance2 = deltaX * deltaX + deltaY * deltaY;
                    if (distance2 < kCityDistanceMin2)
                    {
                        --maxTryToFindACity;
                        thisTileIsGood = false;
                        break;
                    }
                }
                if (thisTileIsGood)
                {
                    break;
                }
            }

            if (pTile)
            {
                m_cities.push_back(pTile);
                ExpandCityTiles(pTile, kCitySize);
                --cityToFound;
            }
            
            --maxTry;
        }
        okTiles.clear();
    }
}

////////////////////////////////////////////////////////////////////////
// Bigger the region, more the cities
// TODO: magic numbers
////////////////////////////////////////////////////////////////////////
int WorldMap::GetCityCount(Region* pNode)
{
    //return 3;
    float weight = (float)pNode->TileCount() / m_pGrid->Size();
    if (weight < 0.05f)
    {
        return 1;
    }
    else if (weight < 0.2f)
    {
        return 2;
    }
    else if (weight < 0.3f)
    {
        return 5;
    }
    else if (weight < 0.5f)
    {
        return 7;
    }
    else if (weight < 0.7f)
    {
        return 12;
    }
    else if (weight < 0.9f)
    {
        return 16;
    }
    else
    {
        return 20;
    }
}

void WorldMap::LinkCities()
{
    m_cityAdjacencyList.clear();
    m_cityAdjacencyList = std::vector<std::vector<int>>(m_cities.size());

    for (int i = 0; i < (int)m_cities.size(); ++i)
    {
        int targetCityId = -1;
        int minDistance2 = kMaxInt;

        for (int j = 0; j < (int)m_cities.size(); ++j)
        {
            if (i == j)
            {
                continue;
            }

            bool alreadyLinked = false;
            for (auto& id : m_cityAdjacencyList[j])
            {
                if (id == i)
                {
                    alreadyLinked = true;
                    break;
                }
            }
            if (alreadyLinked)
            {
                continue;
            }

            int distance2 = Grid::Distance2(m_cities[i], m_cities[j]);
            if (distance2 < minDistance2)
            {
                minDistance2 = distance2;
                targetCityId = j;
            }
        }

        if (targetCityId != -1)
        {
            m_cityAdjacencyList[i].push_back(targetCityId);
        }
    }
}

void WorldMap::BuildRoads()
{
    m_roads.clear();
    for (int i = 0; i < (int) m_cityAdjacencyList.size(); ++i)
    {
        for (auto id : m_cityAdjacencyList[i])
        {
            FindPath(m_cities[i], m_cities[id]);
        }
    }
}

float WorldMap::RegionEmptiness(int regionTileCount, int slope)
{
    // when slope is 1, the chance of getting negative value is too high for small areas
    assert(slope > 1);
    float areaRatio = (float)regionTileCount / (float)m_pGrid->Size();
    float areaWeight = std::log10f(areaRatio) / (float)slope + 1;
    //assert(areaWeight > 0 && areaWeight < 1.f);
    if (areaWeight < 0)
    {
        return 0;
    }
    return areaWeight;
}


////////////////////////////////////////////////////////////////////////
// Path finding for cities, using A*
////////////////////////////////////////////////////////////////////////
void WorldMap::FindPath(Tile* pStart, Tile* pEnd)
{
    if (!(pStart && pEnd))
    {
        return;
    }
    std::unordered_map<int,PathNode*> testedNodes;
    PathNode* pStartNode = new PathNode{ pStart,kMaxFloat,kMaxFloat,nullptr };
    PathNode* pEndNode = new PathNode{ pEnd,kMaxFloat,kMaxFloat,nullptr };
    testedNodes.emplace(pStartNode->m_pTile->id, pStartNode);
    testedNodes.emplace(pStartNode->m_pTile->id, pEndNode);

    auto comp = [](PathNode* left, PathNode* right)
    {
        return left->m_fScore > right->m_fScore;
    };

    auto getGScore = [](Tile* pCurrent, Tile* pStart, Tile* pEnd)
    {
        auto a = pCurrent->heightNoise - pStart->heightNoise;
        auto b = pCurrent->heightNoise - pEnd->heightNoise;
        return (std::fabs(a) + std::fabs(b)) * 700.f;
    };

    auto heuristic = [](Tile* pCurrent, Tile* pEnd)
    {
        auto deltaX = pCurrent->x - pEnd->x;
        auto deltaY = pCurrent->y - pEnd->y;
        return deltaX * deltaX + deltaY * deltaY;
    };

    std::priority_queue<PathNode*, std::vector<PathNode*>, decltype(comp)> openSet(comp);
    std::unordered_map<Tile*,bool> closeSet;

    auto testNeighbor = [&](Tile* pTile, PathNode* pCurrent)
    {
        if (pTile && closeSet.find(pTile) == closeSet.end())
        {
            float gNew = 0.f;
            float hNew = 0.f;
            float fNew = 0.f;
            if (pTile->type == Tile::Type::Road|| pTile->type == Tile::Type::City)
            {
                
            }
            else
            {
                gNew = pCurrent->m_gScore + getGScore(pTile, pStartNode->m_pTile, pEndNode->m_pTile);
                hNew = (float)heuristic(pTile, pEndNode->m_pTile);
                fNew = gNew + hNew;
            }
            
            //not tested yet
            if (testedNodes.find(pTile->id) == testedNodes.end())
            {
                testedNodes.emplace(pTile->id, new PathNode{ pTile ,gNew,fNew,pCurrent });
                openSet.push(testedNodes[pTile->id]);
            }
            //if tested, relax
            else if (testedNodes[pTile->id]->m_fScore > fNew)
            {
                testedNodes[pTile->id]->m_gScore = gNew;
                testedNodes[pTile->id]->m_fScore = fNew;
                testedNodes[pTile->id]->m_cameFrom = pCurrent;
                openSet.push(testedNodes[pTile->id]);
            }
        }
    };

    pStartNode->m_gScore = 0;
    pStartNode->m_fScore = (float)heuristic(pStartNode->m_pTile, pEndNode->m_pTile);
    openSet.push(pStartNode);


    while(openSet.top()->m_pTile != pEnd)
    {
        auto* pCurrent = openSet.top();
        closeSet[pCurrent->m_pTile]= true;
        openSet.pop();
        // find neighbors
        auto* pNorthTile = m_pGrid->GetNeighborTile(pCurrent->m_pTile,Grid::Direction::North);
        testNeighbor(pNorthTile, pCurrent);

        auto* pSouthTile = m_pGrid->GetNeighborTile(pCurrent->m_pTile, Grid::Direction::South);
        testNeighbor(pSouthTile, pCurrent);

        auto* pEastTile = m_pGrid->GetNeighborTile(pCurrent->m_pTile, Grid::Direction::East);
        testNeighbor(pEastTile, pCurrent);

        auto* pWestTile = m_pGrid->GetNeighborTile(pCurrent->m_pTile, Grid::Direction::West);
        testNeighbor(pWestTile, pCurrent);

#ifdef _DEBUG
        DrawRect(pCurrent->m_pTile->rect, E2::Red::kPink);
        GetEngine().ForceRender();
#endif // _DEBUG
    }

    auto* pNode = openSet.top();
    while (pNode)
    {
        if (pNode->m_pTile->type == Tile::Type::City)
        {
            
        }
        else
        {
            pNode->m_pTile->type = Tile::Type::Road;
        }
        m_roads.push_back(pNode->m_pTile);
        pNode = pNode->m_cameFrom;
    }

    while (!openSet.empty())
    {
        openSet.pop();
    }

    for (auto [f, s] : testedNodes)
    {
        delete s;
    }
    testedNodes.clear();
}

void WorldMap::ExpandCityTiles(Tile* pCenter, int range)
{
    int startX = pCenter->x - range;
    int startY = pCenter->y - range;
    int endX = pCenter->x + range;
    int endY = pCenter->y + range;

    for (int y = startY; y < endY; ++y)
    {
        for (int x = startX; x < endX; ++x)
        {
            auto* pTile = m_pGrid->GetTile(x,y);
            if (pTile)
            {
                pTile->type = Tile::Type::City;
            }
        }
    }
}

void WorldMap::DrawCities()
{
    for (auto* pTile : m_cities)
    {
        E2::Rect city{ pTile->rect.x - kCitySize * pTile->rect.w
                      ,pTile->rect.y - kCitySize * pTile->rect.w
                      ,(kCitySize * 2 + 1) * pTile->rect.w
                      ,(kCitySize * 2 + 1) * pTile->rect.h };

        DrawRect(city, E2::Red::kRed);
    }
}

void WorldMap::DrawRoads()
{
    for (auto* pTile : m_roads)
    {
        DrawRect(pTile->rect,E2::Red::kPink);
    }
}

////////////////////////////////////////////////////////////////////////
// MISC.
////////////////////////////////////////////////////////////////////////
const E2::Color& WorldMap::PickColor(float noise)
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

void WorldMap::Reset()
{
    delete m_pRootRegion;
    m_pRootRegion = new Region();
    m_pRootRegion->m_region.first = { m_pGrid->GetTile(0)->x, m_pGrid->GetTile(0)->y };
    m_pRootRegion->m_region.second = { m_pGrid->GetLastTile()->x, m_pGrid->GetLastTile()->y };

    m_cities.clear();
    m_roads.clear();
    m_pGrid->ClearObjects();
}

Tile* WorldMap::GetTile(int x, int y)
{
    return m_pGrid->GetTile(x, y);
}


void WorldMap::ChangeInputRange(int delta)
{
    m_currentInputRange += delta;
    m_currentInputRange = E2::Clamp<int>(1, kMaxInputRange, m_currentInputRange);
}

void WorldMap::ChangePersistence(float delta)
{
    m_currentPersistence += delta;
    m_currentPersistence = E2::Clamp<float>(0, kMaxPersistence, m_currentPersistence);
}

void WorldMap::ChangeOctave(int delta)
{
    m_currentOctaves += delta;
    m_currentOctaves = E2::Clamp<int>(0, kMaxOctaves, m_currentOctaves);
}




void WorldMap::DoSurvey()
{
    m_survey.clear();
    m_survey = std::vector<int>(11);
    
    for (int i = 0; i < m_pGrid->Size(); ++i)
    {
        Tile* pTile = m_pGrid->GetTile(i);
        if (pTile->heightNoise < 0 || pTile->heightNoise > 1.f)
        {
            ++m_survey[10];
        }
        else if (pTile->heightNoise < 0.1f)
        {
            ++m_survey[0];
        }
        else if (pTile->heightNoise < 0.2f)
        {
            ++m_survey[1];
        }
        else if (pTile->heightNoise < 0.3f)
        {
            ++m_survey[2];
        }
        else if (pTile->heightNoise < 0.4f)
        {
            ++m_survey[3];
        }
        else if (pTile->heightNoise < 0.5f)
        {
            ++m_survey[4];
        }
        else if (pTile->heightNoise < 0.6f)
        {
            ++m_survey[5];
        }
        else if (pTile->heightNoise < 0.7f)
        {
            ++m_survey[6];
        }
        else if (pTile->heightNoise < 0.8f)
        {
            ++m_survey[7];
        }
        else if (pTile->heightNoise < 0.9f)
        {
            ++m_survey[8];
        }
        else
        {
            ++m_survey[9];
        }
    };
}

void WorldMap::PrintInfo()
{
    system("cls");
    std::cout << "***************" << '\n';
    std::cout << "Input Range " << m_currentInputRange << '\n';
    std::cout << "Persistence " << m_currentPersistence << '\n';
    std::cout << "Octaves " << m_currentOctaves << '\n';
    std::cout << "< 0.1 : " << m_survey[0] << '\n';
    std::cout << "< 0.2 : " << m_survey[1] << '\n';
    std::cout << "< 0.3 : " << m_survey[2] << '\n';
    std::cout << "< 0.4 : " << m_survey[3] << '\n';
    std::cout << "< 0.5 : " << m_survey[4] << '\n';
    std::cout << "< 0.6 : " << m_survey[5] << '\n';
    std::cout << "< 0.7 : " << m_survey[6] << '\n';
    std::cout << "< 0.8 : " << m_survey[7] << '\n';
    std::cout << "< 0.9 : " << m_survey[8] << '\n';
    std::cout << "< 1.0 : " << m_survey[9] << '\n';
    std::cout << "Error : " << m_survey[10] << '\n';
    std::cout << "***************" << '\n';
}
