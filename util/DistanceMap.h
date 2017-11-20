#pragma once
#include <sc2api/sc2_api.h>

class MapTools;

class DistanceMap 
{
    int width_;
    int height_;
    sc2::Point2DI start_tile_;

    std::vector<std::vector<int>> dist_;
    std::vector<sc2::Point2DI> sorted_tile_positions_;
    
public:
    
    DistanceMap();
    void ComputeDistanceMap(const MapTools& map, const sc2::Point2DI& start_tile);

    int GetDistance(int tile_x, int tile_y) const;
    int GetDistance(const sc2::Point2DI& pos) const;

    // given a position, get the position we should move to to minimize distance
    const std::vector<sc2::Point2DI>& GetSortedTiles() const;
    const sc2::Point2DI& GetStartTile() const;
};