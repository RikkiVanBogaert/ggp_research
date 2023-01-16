#pragma once
#include "stdafx.h"
#include "Exam_HelperStructs.h"
#include "IExamInterface.h"
#include "EBlackboard.h"
#include "MyHelperStructs.h"

bool HasItem(Elite::Blackboard* pBlackboard, const eItemType& type)
{
	GlobalVariables* pGlobals;
	if (!pBlackboard->GetData("Globals", pGlobals) || pGlobals == nullptr)
		return false;

	IExamInterface* pInterface;
	if (!pBlackboard->GetData("Interface", pInterface) || pInterface == nullptr)
		return false;


	for (size_t i{}; i < pInterface->Inventory_GetCapacity(); ++i)
	{
		ItemInfo item;
		if(!pInterface->Inventory_GetItem(i, item))
			continue;

		if (item.Type == type)
			return true;
	}

	return false;
}

bool NeedsItem(Elite::Blackboard* pBlackboard)
{
	GlobalVariables* pGlobals;
	if (!pBlackboard->GetData("Globals", pGlobals) || pGlobals == nullptr)
		return false;

	IExamInterface* pInterface;
	if (!pBlackboard->GetData("Interface", pInterface) || pInterface == nullptr)
		return false;

	std::vector<EntityInfo>* pEntities;
	if (!pBlackboard->GetData("EntitiesInFOV", pEntities))
		return false;

	ItemInfo item;
	pInterface->Item_GetInfo((*pEntities)[0], item);
	
	if (!HasItem(pBlackboard, item.Type) || item.Type == eItemType::FOOD)
		return true;

	return false;
}
//


HouseInfo GetClosestSeenHouse(Elite::Blackboard* pBlackboard)
{
	GlobalVariables* pGlobals;
	if (!pBlackboard->GetData("Globals", pGlobals) || pGlobals == nullptr)
		return {};

	IExamInterface* pInterface;
	if (!pBlackboard->GetData("Interface", pInterface) || pInterface == nullptr)
		return {};

	HouseInfo closestHouse{ {INFINITY, INFINITY} };
	Elite::Vector2 agentPos{ pInterface->Agent_GetInfo().Position };
	for (const auto& sh : pGlobals->seenHouses)
	{
		if (Elite::Distance(sh.Center, agentPos) < Elite::Distance(closestHouse.Center, agentPos))
		{
			closestHouse = sh;
		}
	}

	return closestHouse;
}

bool HasHouseToLoot(Elite::Blackboard* pBlackboard)
{
	GlobalVariables* pGlobals;
	if (!pBlackboard->GetData("Globals", pGlobals) || pGlobals == nullptr)
		return false;

	std::vector<HouseInfo>* pHouses;
	if (!pBlackboard->GetData("HousesInFOV", pHouses))
		return false;

	if (!pGlobals->explorationStage && 
		pGlobals->visitedHouses.size() > 0 && pGlobals->seenHouses.size() == 0)
	{
		//we are done exploring so we just use the visited houses again to go to them
		pGlobals->seenHouses = pGlobals->visitedHouses;
		pGlobals->visitedHouses.clear();
	}

	for (const auto& h : *pHouses)
	{
		bool isVisited{false};
		for (const auto& vh : pGlobals->visitedHouses)
		{
			//we check if a house in fov has been visited, if not, we add it to the seenhouses
			if (h.Center == vh.Center)
				isVisited = true;
		}

		for (const auto& sh : pGlobals->seenHouses)
		{
			//we check if a house in fov has been visited, if not, we add it to the seenhouses
			if (h.Center == sh.Center)
				isVisited = true;
		}

		if (isVisited)
			continue;

		pGlobals->seenHouses.push_back(h);
	}

	//we still have a currentHouse so continue with that one
	if (pGlobals->currentHouse.Center != Elite::Vector2{ 0,0 })
		return true;


	//there are no more houses to go to, so continue exploring
	if (pGlobals->seenHouses.size() == 0)
		return false;

	//we take (closest by) seenHouse and set that as the house to go to
	pGlobals->currentHouse = GetClosestSeenHouse(pBlackboard);
	return true;

	//OG LOGIC
	////auto vHousesInFOV = pHouses;
	//if (pHouses->size() == 0 &&
	//	pGlobals->currentHouse.Center == Elite::Vector2{ 0,0 }) // no houses in FOV and no current house
	//	return false;
	////else, there is a house in fov, and we dont have a current house
	////check if we have visited the house
	//for (const HouseInfo& h : pGlobals->visitedHouses)
	//{
	//	//if the center of the found house is the same as a house in the vector visitedHouses, 
	//	//and if we have searched the house, we dont want to go in there anymore
	//	if ((*pHouses)[0].Center == h.Center)
	//	{
	//		return false;
	//	}
	//}
	//if (pGlobals->currentHouse.Center != Elite::Vector2{ 0,0 })
	//	return true; //we have a current house, so just continue behavior
	////else, we add the house as currentHouse
	//pGlobals->currentHouse = (*pHouses)[0];
	//return true;
}

bool EntityTypeInFOV(Elite::Blackboard* pBlackboard, const eEntityType& type)
{
	std::vector<EntityInfo>* pEntities;
	if (!pBlackboard->GetData("EntitiesInFOV", pEntities))
		return false;

	for (const auto& e : *pEntities)
	{
		if (e.Type == type)
			return true;
	}
	return false;
}


#pragma endregion