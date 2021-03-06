#pragma once
#include <sc2api/sc2_api.h>

namespace Util
{
    sc2::Unit::Alliance GetPlayer(const sc2::Unit* unit);
    bool IsCombatUnit(const sc2::Unit* unit, sc2::Agent & bot);
    bool IsCombatUnitType(const sc2::UnitTypeID type, sc2::Agent & bot);
    bool IsSupplyProvider(const sc2::Unit* unit);
    bool IsSupplyProviderType(const sc2::UnitTypeID type);
    bool IsTownHall(const sc2::Unit* unit);
    bool IsTownHallType(const sc2::UnitTypeID & type);
    bool IsRefinery(const sc2::Unit* unit);
    bool IsRefineryType(const sc2::UnitTypeID & type);
    bool IsDetector(const sc2::Unit* unit);
    bool IsDetectorType(const sc2::UnitTypeID & type);
    bool IsGeyser(const sc2::Unit* unit);
    bool IsMineral(const sc2::Unit* unit);
    bool IsWorker(const sc2::Unit* unit);
    bool IsWorkerType(const sc2::UnitTypeID & unit);
    bool IsIdle(const sc2::Unit* unit);
    sc2::UnitTypeData GetUnitTypeData(const sc2::UnitTypeID type, const sc2::Agent & bot);
	int GetSupplyProvided(const sc2::UnitTypeID type, const sc2::Agent & bot);
    bool IsCompleted(const sc2::Unit* unit);
	float GetAttackRate(const sc2::UnitTypeID & type, const sc2::Agent & bot);
    float GetAttackRange(const sc2::UnitTypeID & type, const sc2::Agent & bot);
    int   GetAttackDamage(const sc2::UnitTypeID & type, const sc2::Agent & bot);
    
    bool UnitHasAbilityAvailable(const sc2::Unit * unit, const sc2::AbilityID & ability, sc2::Agent & bot);
    bool UnitCanBuildTypeNow(const sc2::Unit* unit, const sc2::UnitTypeID & type, sc2::Agent & bot);
    sc2::UnitTypeID WhatBuilds(const sc2::UnitTypeID & type);
	int DPSAtPoint(const sc2::Point2D unit_pos, const sc2::Agent& bot);
    int PredictFutureDPSAtPoint(const sc2::Point2D unit_pos, const float future_time, const sc2::Agent & bot);
    int GetUnitTypeWidth(const sc2::UnitTypeID type, const sc2::Agent & bot);
    int GetUnitTypeHeight(const sc2::UnitTypeID type, const sc2::Agent & bot);
    bool IsUnitOfType(const sc2::Unit * unit, sc2::UnitTypeID unit_type, const sc2::Agent & bot);
    int GetMineralPrice(const sc2::UnitTypeID type, const sc2::Agent & bot);
    int GetGasPrice(const sc2::UnitTypeID type, const sc2::Agent & bot);
    sc2::UnitTypeID GetTownHall(const sc2::Race & race);
    sc2::UnitTypeID GetSupplyProvider(const sc2::Race & race);
    std::string GetStringFromRace(const sc2::Race & race);
    sc2::Race GetRaceFromString(const std::string & race);
    sc2::Point2D CalcCenterOfUnitGroup(const std::vector<const sc2::Unit*>& units);
    sc2::UnitTypeID GetUnitTypeIDFromName(const sc2::ObservationInterface * obs, const std::string & name);

    float Dist(const int x1, const int y1, const int x2, const int y2);
    float Dist(const sc2::Point2D & p1, const sc2::Point2D & p2);
    float Dist(const sc2::Point2DI & p1, const sc2::Point2D & p2);
    float DistSq(const sc2::Point2D & p1, const sc2::Point2D & p2);
    float DistSq(const sc2::Point2DI & p1, const sc2::Point2D & p2);
    
    // Kevin-provided helper functions
    bool    CanPlace(const sc2::GameInfo& info, const sc2::Point2D& point);
    bool    Pathable(const sc2::GameInfo& info, const sc2::Point2D& point);
    
    bool    IsBuilding(const sc2::UnitTypeID & type);
    bool    IsMorphCommand(const sc2::AbilityID & ability);
    sc2::AbilityID  UnitTypeIDToAbilityID(const sc2::UnitTypeID & id);
    bool CanAttackAir(std::vector<sc2::Weapon> weapons);
    int  GetGameTimeInSeconds(const sc2::Agent& bot);

	sc2::Point2DI ToPoint2DI(sc2::Point2D point);
	sc2::Point2D ToPoint2D(sc2::Point2DI point);
};
