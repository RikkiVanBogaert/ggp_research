#pragma once
class ISteeringBehavior;

#include "Action.h"
#include "Exam_HelperStructs.h"


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
	//Could be stored in a blackBoard aswell
	State currentState;
	State goalState;
	Elite::Vector2 goalPosition{};
	UINT inventorySlot = 0;
	SteeringState steeringState;
	std::vector<std::pair<HouseInfo, bool>> visitedHouses;
	HouseInfo currentHouse{ {0,0} };
	PurgeZoneInfo purgeZone{};
	std::map<std::string, int> inventory;
	std::vector<ItemInfo> storedItems;

	bool hasFood;
	bool m_HasMedkit;
};
