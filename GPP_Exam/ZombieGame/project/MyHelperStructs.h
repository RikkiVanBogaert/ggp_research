#pragma once
class ISteeringBehavior;

#include "Action.h"
//#include "Exam_HelperStructs.h"

enum class SteeringState
{
	Seek,
	Flee,
	Spin,
	Face,
	SeekWhileSpinning,
	FaceAndFlee
};

//make a map of strings and inventorySlot numbers so i can call the name of an item and get the inventory slot
struct GlobalVariables
{
	GlobalVariables() = default;

	//Inventory
	int takenSlots = 0;

	//Houses
	std::vector<HouseInfo> seenHouses{}; //will replace visitedHouses
	std::vector<HouseInfo> visitedHouses{};
	HouseInfo currentHouse{ {0,0} };
	bool topHouseVisited = false;
	bool bottomHouseVisited = false;

	//Other
	PurgeZoneInfo purgeZone{};
	float minRadius{};
	float maxRadius{};
	float beginRadius{};
	bool explorationStage = true;

	//GOAP
	bool actionEnd = false;
	State currentState;
	State goalState;
	Elite::Vector2 goalPosition{};
	SteeringState steeringState;
};
