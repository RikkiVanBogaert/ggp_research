#pragma once
#include "Exam_HelperStructs.h"
#include "EBlackboard.h"
#include "MyHelperStructs.h"
#include "IExamInterface.h"      


void FindItemSlot(Elite::Blackboard* pBlackboard, const eItemType& type, int& slot, ItemInfo& item)
{
	GlobalVariables* pGlobals;
	if (!pBlackboard->GetData("Globals", pGlobals) || pGlobals == nullptr)
		return;

	IExamInterface* pInterface;
	if (!pBlackboard->GetData("Interface", pInterface) || pInterface == nullptr)
		return;

	ItemInfo tempItem{};
	for (size_t i{}; i < pInterface->Inventory_GetCapacity(); ++i)
	{
		if (!pInterface->Inventory_GetItem(i, tempItem))
		{
			continue;
		}

		if (tempItem.Type == type)
		{
			item = tempItem;
			slot = i;
			return;
		}
	}
}

bool HasItemA(Elite::Blackboard* pBlackboard, const eItemType& type)
{
	GlobalVariables* pGlobals;
	if (!pBlackboard->GetData("Globals", pGlobals) || pGlobals == nullptr)
		return false;

	IExamInterface* pInterface;
	if (!pBlackboard->GetData("Interface", pInterface) || pInterface == nullptr)
		return false;


	for (int i{}; i < pInterface->Inventory_GetCapacity(); ++i)
	{
		ItemInfo item;
		if (!pInterface->Inventory_GetItem(i, item))
			continue;

		if (item.Type == type)
			return true;
	}

	return false;
}

bool NeedsItemA(Elite::Blackboard* pBlackboard)
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

	if (item.Type == eItemType::GARBAGE)
		return false;

	if (!HasItemA(pBlackboard, item.Type) || item.Type == eItemType::FOOD)
		return true;

	return false;
}

bool GiveDuplicateSpot(Elite::Blackboard* pBlackboard, int& spot)
{
	IExamInterface* pInterface;
	if (!pBlackboard->GetData("Interface", pInterface) || pInterface == nullptr)
		return false;

	int pistolCounter{}, shotgunCounter{}, medkitCounter{};
	int duplicateSpot{};
	for (int i{}; i < pInterface->Inventory_GetCapacity(); ++i)
	{
		ItemInfo item;
		if (!pInterface->Inventory_GetItem(i, item))
			continue;

		switch (item.Type)
		{
		case eItemType::PISTOL:
			++pistolCounter;
			if (pistolCounter == 2)
			{
				spot = i;
				return true;
			}
			break;
		case eItemType::SHOTGUN:
			++shotgunCounter;
			if (shotgunCounter == 2)
			{
				spot = i;
				return true;
			}
			break;
		case eItemType::MEDKIT:
			++medkitCounter;
			if (medkitCounter == 2)
			{
				spot = i;
				return true;
			}
			break;
		}

	}
	return false;
}

int GetFreeInventorySpot(Elite::Blackboard* pBlackboard)
{
	IExamInterface* pInterface;
	if (!pBlackboard->GetData("Interface", pInterface) || pInterface == nullptr)
		return false;

	for (int i{}; i < pInterface->Inventory_GetCapacity(); ++i)
	{
		ItemInfo item;
		if (!pInterface->Inventory_GetItem(i, item))
		{
			//if we dont get an item, that means the spot is free
			return i;
			break;
		}
	}
	return -1;
}

void RemoveVisitedHouse(Elite::Blackboard* pBlackboard, const HouseInfo& house)
{
	GlobalVariables* pGlobals;
	if (!pBlackboard->GetData("Globals", pGlobals) || pGlobals == nullptr)
		return;

	for (int i{}; i < pGlobals->seenHouses.size(); ++i)
	{
		if (house.Center != pGlobals->seenHouses[i].Center)
			continue;

		pGlobals->seenHouses[i] = pGlobals->seenHouses[pGlobals->seenHouses.size() - 1];
		pGlobals->seenHouses.pop_back();
	}

}
