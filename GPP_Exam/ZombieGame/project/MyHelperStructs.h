#pragma once
class ISteeringBehavior;

#include "Action.h"

enum class SteeringState
{
	Seek,
	Flee,
	Spin,
	Face,
	SeekWhileSpinning,
	FaceAndFlee
};

struct GlobalVariables
{
	GlobalVariables() = default;

	//Inventory
	int takenSlots = 0;

	//Houses
	std::vector<HouseInfo> seenHouses{};
	std::vector<HouseInfo> visitedHouses{};
	HouseInfo currentHouse{ {0,0} };
	bool topHouseVisited = false;
	bool bottomHouseVisited = false;

	//Other
	PurgeZoneInfo purgeZone{};
	float explorationRadius{};
	bool explorationStage = true;

	//GOAP
	bool actionEnd = false;
	State currentState;
	State goalState;
	Elite::Vector2 goalPosition{};
	SteeringState steeringState;
};
