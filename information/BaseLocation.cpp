#include <sstream>

#include "TechLab/information/BaseLocation.h"
#include "TechLab/util/Util.h"

const int NearBaseLocationTileDistance = 5;

BaseLocation::BaseLocation(sc2::Agent & bot, const std::vector<const sc2::Unit*> & resources)
    : bot_(bot)
    , is_start_location_      (false)
    , left_                 (std::numeric_limits<float>::max())
    , right_                (std::numeric_limits<float>::lowest())
    , top_                  (std::numeric_limits<float>::lowest())
    , bottom_               (std::numeric_limits<float>::max())
{
    is_player_start_location_[sc2::Unit::Alliance::Self] = false;
    // Enemy start locations are simply POTENTIAL spawn locations.
    is_player_start_location_[sc2::Unit::Alliance::Enemy] = false;
    is_player_occupying_[sc2::Unit::Alliance::Self] = false;
    is_player_occupying_[sc2::Unit::Alliance::Enemy] = false;

    float resource_center_x = 0;
    float resource_center_y = 0;

    // Add each of the resources to its corresponding container.
    for (auto & resource : resources)
    {
        if (Util::IsMineral(resource))
        {
            minerals_.push_back(resource);

            // Add the position of the minerals to the center.
            resource_center_x += resource->pos.x;
            resource_center_y += resource->pos.y;
        }
        else
        {
            geysers_.push_back(resource);

            // Pull the resource center toward the geyser if it exists.
            resource_center_x += resource->pos.x;
            resource_center_y += resource->pos.y;
        }

        // Set the limits of the base location bounding box.
        const float res_width = 1;
        const float res_height = 0.5;

        left_   = std::min(left_,   resource->pos.x - res_width);
        right_  = std::max(right_,  resource->pos.x + res_width);
        top_    = std::max(top_,    resource->pos.y + res_height);
        bottom_ = std::min(bottom_, resource->pos.y - res_height);
    }

    // Calculate the center of the resources.
    size_t num_resources = minerals_.size() + geysers_.size();

    center_of_resources_ = sc2::Point2D(left_ + (right_-left_)/2.0f, top_ + (bottom_-top_)/2.0f);

    // Check to see if this is a start location for the map.
    for (auto & pos : bot_.Observation()->GetGameInfo().enemy_start_locations)
    {
        if (ContainsPosition(pos))
        {
            is_start_location_ = true;
            is_player_start_location_[sc2::Unit::Alliance::Enemy] = true;
            depot_position_ = pos;
        }
    }
    
    // If this base location position is near our own resource depot, it's our start location.
    for (auto & unit : bot_.Observation()->GetUnits())
    {
        if (Util::GetPlayer(unit) == sc2::Unit::Alliance::Self && Util::IsTownHall(unit) && ContainsPosition(unit->pos))
        {
            is_player_start_location_[sc2::Unit::Alliance::Self] = true;
            is_start_location_ = true;
            is_player_occupying_[sc2::Unit::Alliance::Self] = true;
            break;
        }
    }
    
    // if it's not a start location, we need to calculate the depot position
    //if (!IsStartLocation())
    //{
    //    // the position of the depot will be the closest spot we can build one from the resource center
    //    for (auto & tile : GetClosestTiles())
    //    {
    //        // TODO: depotPosition = depot position for this base location
    //    }
    //}
}

const sc2::Unit* BaseLocation::GetTownHall() const
{
    return town_hall_;
}

const sc2::Point2D & BaseLocation::GetTownHallPosition() const
{
    if (town_hall_)
        return town_hall_->pos;
    else
        return GetPosition();
}

const std::vector<const sc2::Unit*>& BaseLocation::GetGeysers() const
{
    return geysers_;
}

const std::vector<const sc2::Unit*>& BaseLocation::GetMinerals() const
{
    return minerals_;
}

const sc2::Point2D & BaseLocation::GetPosition() const
{
    return center_of_resources_;
}

void BaseLocation::SetTownHall(const sc2::Unit* unit)
{
    town_hall_ = unit;
}

void BaseLocation::SetPlayerOccupying(const sc2::Unit::Alliance player, const bool occupying)
{
    is_player_occupying_[player] = occupying;

    // If this base is a start location that's occupied by the enemy, it's that enemy's start location.
    if (occupying && player == sc2::Unit::Alliance::Enemy && IsStartLocation() && is_player_start_location_[player] == false)
    {
        is_player_start_location_[player] = true;
    }
}

bool BaseLocation::IsInResourceBox(const int x, const int y) const
{
    return x >= left_ && x < right_ && y < top_ && y >= bottom_;
}

bool BaseLocation::IsOccupiedByPlayer(const sc2::Unit::Alliance player) const
{
    return is_player_occupying_.at(player);
}

bool BaseLocation::IsExplored() const
{
    return false;
}

// IsPlayerStartLocation returns if you spawned at the given location. 
// IsPlayerStartLocation.at(sc2::Unit::Alliance::Enemy) only gives POTENTIAL enemy spawn locations. 
// For clarity, there is a separate function for that. 
bool BaseLocation::IsPlayerStartLocation() const
{
    return is_player_start_location_.at(sc2::Unit::Alliance::Self);
}

bool BaseLocation::IsPotentialEnemyStartLocation() const
{
    return is_player_start_location_.at(sc2::Unit::Alliance::Enemy);
}

bool BaseLocation::IsMineralOnly() const
{
    return GetGeysers().empty();
}

bool BaseLocation::IsBaseFullySaturated() const
{
    if (town_hall_)
        return town_hall_->assigned_harvesters >= town_hall_->ideal_harvesters;
    return false;
}

// Warning: does not check to see if the tile is on the map. 
bool BaseLocation::ContainsPosition(const sc2::Point2D & pos) const
{
    return GetGroundDistance(pos) < NearBaseLocationTileDistance;
}

// Returns -1 if no workers are Mining.
int BaseLocation::TotalWorkersMining() const
{
    if(town_hall_)
        return town_hall_->assigned_harvesters;
    // No base exists, therefore no workers are mining
    return -1;
}

// Provides a rough approximation of current mineral income. 
float BaseLocation::MineralIncomePerSecond() const
{
    // More accurate income can be calculated by knowing which minerals are currently being mined from.
    // h is harvest time
    // Mineral Harvest time = 2.786;
    // Gas Harvest time = 1.9810
    // mineralspertrip = 5
    // t is travel time
    // w is wait time (waiting at patch to be able to mine)
    // worker acceleration = 2.5
    // max worker speed = 2.813
    // d = cc.pos + m.pos - cc.radius - m.radius - scv.diamater (the distance the scv would actually have to walk)
    // vf = sqrt( 2 * a * d)
    // t = 2*d / vf

    // The mineral income equation is something like this
    // Minerals per scv per second = mineralspertrip / (h + t + w) 
    // if (harvest workers == max workers)
    // Minerals per scv per second = mineralspertrip / h

    // For now, we will use a simplified version of the equation.
    // A worker will on average return 55 minerals per minute
    // 55/60 = 0.91666 = minerals per second per harvester. 
    return town_hall_->assigned_harvesters * 0.91666;
}

// Provides a rough approximation of current gas income. 
float BaseLocation::GasIncomePerSecond() const
{
    int j = 0;
    for (const auto & geyser : geysers_)
    {
        j += geyser->assigned_harvesters;
    }
    // 35/60 to get harester income rate. 
    return j * 0.58333;
}

int BaseLocation::GetGroundDistance(const sc2::Point2D & pos) const
{
    return Util::Dist(pos, center_of_resources_);
    //return distanceMap.getDistance(pos);
}

bool BaseLocation::IsStartLocation() const
{
    return is_start_location_;
}