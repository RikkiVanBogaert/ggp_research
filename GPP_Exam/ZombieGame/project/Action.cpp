#pragma once
#include "stdafx.h"
#include "IExamInterface.h"
#include "Action.h"
#include "MyHelperStructs.h"
#include "CombinedSteeringBehaviors.h"
//#include "MyIncludes.h"

bool State::operator==(const State& STATE) const
{
	if (this->stateString == STATE.stateString
		&& this->stateBool == STATE.stateBool) 
		return true;
	else 
		return false;
}
bool State::operator!=(const State& STATE) const
{
	if (this->stateString != STATE.stateString
		|| this->stateBool != STATE.stateBool)
		return true;
	else
		return false;
}

#pragma region //HELPERS
void FindItemSlot(const eItemType& type, int& slot, const GlobalVariables& pGlobals, IExamInterface& pInterface, ItemInfo& item)
{
	ItemInfo tempItem{};
	for (size_t i{}; i < pInterface.Inventory_GetCapacity(); ++i)
	{
		if(!pInterface.Inventory_GetItem(i, tempItem))
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

#pragma endregion

Action::Action(const State& precondition, const State& effect, int cost):
	m_Precondition{precondition},
	m_Effect{effect},
	m_Cost{cost}
{}



bool Action::ConditionMet(const State& currentState)
{
	if (currentState.stateString == m_Precondition.stateString
		&& currentState.stateBool == m_Precondition.stateBool)
		return true;

	return false;
}

//Helper Function
bool Action::ReachedTarget(const AgentInfo& agent, const Elite::Vector2& targetPos) const
{
	if (agent.Position.Distance(targetPos) <= agent.GrabRange)
		return true;

	return false;
}

Explore::Explore() :
	Action({ "Neutral", true }, { "Explore", true }, 100) //give high cost so it will only do this if no other options are available
{}

void Explore::ExecuteEvent(Elite::Blackboard* pBlackboard)
{
	IExamInterface* pInterface;
	if (!pBlackboard->GetData("Interface", pInterface) || pInterface == nullptr)
		return;

	auto agentInfo = pInterface->Agent_GetInfo();

	GlobalVariables* pGlobals;
	if (!pBlackboard->GetData("Globals", pGlobals) || pGlobals == nullptr)
		return;

	pGlobals->steeringState = SteeringState::SeekWhileSpinning;

	//explore the world by going to the different corners
	const float cornerOffset{ 4 };
	if(!m_TopLeftExplored)
	{
		pGlobals->goalPosition.x = pInterface->World_GetInfo().Center.x - pInterface->World_GetInfo().Dimensions.x / cornerOffset;
		pGlobals->goalPosition.y = pInterface->World_GetInfo().Center.y + pInterface->World_GetInfo().Dimensions.y / cornerOffset;
		if (ReachedTarget(agentInfo, pGlobals->goalPosition))
		{
			m_TopLeftExplored = true;
		}
	}
	else if (!m_TopRightExplored)
	{
		pGlobals->goalPosition.x = pInterface->World_GetInfo().Center.x + pInterface->World_GetInfo().Dimensions.x / cornerOffset;
		pGlobals->goalPosition.y = pInterface->World_GetInfo().Center.y + pInterface->World_GetInfo().Dimensions.y / cornerOffset;
		if (ReachedTarget(agentInfo, pGlobals->goalPosition))
		{
			m_TopRightExplored = true;
		}
	}
	else if (!m_BottomRightExplored)
	{
		pGlobals->goalPosition.x = pInterface->World_GetInfo().Center.x + pInterface->World_GetInfo().Dimensions.x / cornerOffset;
		pGlobals->goalPosition.y = pInterface->World_GetInfo().Center.y - pInterface->World_GetInfo().Dimensions.y / cornerOffset;
		if (ReachedTarget(agentInfo, pGlobals->goalPosition))
		{
			m_BottomRightExplored = true;
		}
	}
	else if (!m_BottomLeftExplored)
	{
		pGlobals->goalPosition.x = pInterface->World_GetInfo().Center.x - pInterface->World_GetInfo().Dimensions.x / cornerOffset;
		pGlobals->goalPosition.y = pInterface->World_GetInfo().Center.y - pInterface->World_GetInfo().Dimensions.y / cornerOffset;
		if (ReachedTarget(agentInfo, pGlobals->goalPosition))
		{
			m_BottomLeftExplored = true;
		}
	}
	else
	{
		//all corners have been explored, so we reset them and also the visitedHouses since the houses normally have spawned new items
		m_TopLeftExplored = false;
		m_TopRightExplored = false;
		m_BottomRightExplored = false;
		m_TopLeftExplored = false;
		pGlobals->visitedHouses.clear();
	}

}

ExploreSpiral::ExploreSpiral() :
	Action({ "Neutral", true }, { "Explore", true }, 100) //give high cost so it will only do this if no other options are available
{}

void ExploreSpiral::ExecuteEvent(Elite::Blackboard* pBlackboard)
{
	IExamInterface* pInterface;
	if (!pBlackboard->GetData("Interface", pInterface) || pInterface == nullptr)
		return;

	auto agentInfo = pInterface->Agent_GetInfo();

	GlobalVariables* pGlobals;
	if (!pBlackboard->GetData("Globals", pGlobals) || pGlobals == nullptr)
		return;

	pGlobals->steeringState = SteeringState::SeekWhileSpinning;


	Elite::Vector2 target { pInterface->World_GetInfo().Center.x + cosf(m_Angle) * m_Radius,
	pInterface->World_GetInfo().Center.y + sinf(m_Angle) * m_Radius };

	if (m_SearchRadius == 0) 
		m_SearchRadius = pGlobals->beginRadius;


	const float xOffset{ 7 };
	if (agentInfo.Position.x >= pGlobals->beginRadius - xOffset && m_SearchRadius == pGlobals->beginRadius)
	{
		//we searched the inner circle, go to the outer circle
		m_SearchRadius = pGlobals->maxRadius - pGlobals->minRadius;

		target = { pInterface->World_GetInfo().Center + Elite::Vector2{pGlobals->minRadius, 0} };
		m_Radius = pGlobals->minRadius;
	}
	else if (agentInfo.Position.x >= pGlobals->maxRadius) //no
	{
		//we searched the outer circle, go to all the seenhouses again

	}

	const float angleIncrement{ float(M_PI) * 2.f / m_Steps };
	float radiusIncrement{ m_SearchRadius / m_Steps };
	if (!m_GoingOut)
	{
		radiusIncrement *= -1;
	}

	if (agentInfo.Position.x >= pGlobals->maxRadius - xOffset && m_GoingOut)
	{
		m_GoingOut = false;
	}

	//if new targetPoint is in a visitedHouse, get a new point
	//use offset so the point isnt too close to a house (caused a bug in which he got stuck in a corner)
	const float offset{ 5 };
	for (const HouseInfo& h : pGlobals->visitedHouses)
	{
		if (target.x > h.Center.x - h.Size.x / 2 - offset && target.x < h.Center.x + h.Size.x / 2 + offset &&
			target.y > h.Center.y - h.Size.y / 2 - offset && target.y < h.Center.y + h.Size.y / 2 + offset)
		{
			m_Angle += angleIncrement;
			m_Radius += radiusIncrement;
			return;
		}
	}

	pGlobals->goalPosition = target;

	//we reached point, get next point on spiral
	if (Elite::Distance(agentInfo.Position, pGlobals->goalPosition) < 3)
	{
		m_Angle += angleIncrement;
		m_Radius += radiusIncrement;
	}


	//DEBUG Drawing
	for (int i{}; i < m_Steps; ++i)
	{
		pInterface->Draw_Point(pInterface->World_GetInfo().Center + 
		Elite::Vector2{cosf(m_Angle + i * angleIncrement)* (m_Radius + i * radiusIncrement), 
			sinf(m_Angle + i * angleIncrement)* (m_Radius + i * radiusIncrement) },
			10, {0,0,0});
	}

}

ExploreCircle::ExploreCircle() :
	Action({ "Neutral", true }, { "Explore", true }, 100) //give high cost so it will only do this if no other options are available
{}

void ExploreCircle::ExecuteEvent(Elite::Blackboard* pBlackboard)
{
	IExamInterface* pInterface;
	if (!pBlackboard->GetData("Interface", pInterface) || pInterface == nullptr)
		return;

	auto agentInfo = pInterface->Agent_GetInfo();

	GlobalVariables* pGlobals;
	if (!pBlackboard->GetData("Globals", pGlobals) || pGlobals == nullptr)
		return;

	pGlobals->steeringState = SteeringState::SeekWhileSpinning;

	int steps{ 8 };
	switch (m_CircleState)
	{
	case 0:
		m_SearchRadius = pGlobals->maxRadius;
		break;
	default:
		pGlobals->explorationStage = false;
		
		//we are done exploring the circle, we just revisit the previously seen houses.
		break;
	}

	Elite::Vector2 target{ pInterface->World_GetInfo().Center.x + cosf(m_Angle) * m_SearchRadius,
	pInterface->World_GetInfo().Center.y + sinf(m_Angle) * m_SearchRadius };
	
	const float angleIncrement{ float(M_PI) * 2.f / steps };

	const float offset{ 5 };
	for (const HouseInfo& h : pGlobals->visitedHouses)
	{
		if (target.x > h.Center.x - h.Size.x / 2 - offset && target.x < h.Center.x + h.Size.x / 2 + offset &&
			target.y > h.Center.y - h.Size.y / 2 - offset && target.y < h.Center.y + h.Size.y / 2 + offset)
		{
			m_Angle += angleIncrement;
			return;
		}
	}

	pGlobals->goalPosition = target;

	if (Elite::Distance(agentInfo.Position, pGlobals->goalPosition) < 3)
	{
		m_Angle += angleIncrement;
	}
	
	if (m_Angle >= 2 * M_PI)
	{
		//end of current circle
		++m_CircleState;
		m_Angle = 0;
	}


	//std::cout << m_Angle << '\n';
	//DEBUG Drawing
	for (int i{}; i < steps; ++i)
	{
		pInterface->Draw_Point(pInterface->World_GetInfo().Center +
			Elite::Vector2{ cosf(m_Angle + i * angleIncrement) * (m_SearchRadius),
				sinf(m_Angle + i * angleIncrement) * (m_SearchRadius) },
			10, { 0,0,0 });
	}
}


#pragma region //HOUSE

GoInHouse::GoInHouse() :
	Action({ "Neutral", true }, { "InHouse", true })
{}

void GoInHouse::ExecuteEvent(Elite::Blackboard* pBlackboard)
{
	GlobalVariables* pGlobals;
	if (!pBlackboard->GetData("Globals", pGlobals) || pGlobals == nullptr)
		return;

	IExamInterface* pInterface;
	if (!pBlackboard->GetData("Interface", pInterface) || pInterface == nullptr)
		return;

	pGlobals->steeringState = SteeringState::SeekWhileSpinning;

	pGlobals->goalPosition = pGlobals->currentHouse.Center;

	auto agentInfo = pInterface->Agent_GetInfo();


	const Elite::Vector2 agentPos{ agentInfo.Position };
	const HouseInfo curHouse{ pGlobals->currentHouse };
	const float offset{ 3 };
	if (agentPos.x > curHouse.Center.x - curHouse.Size.x / 2 + offset && 
		agentPos.x < curHouse.Center.x + curHouse.Size.x / 2 - offset && 
		agentPos.y > curHouse.Center.y - curHouse.Size.y / 2 + offset &&
		agentPos.y < curHouse.Center.y + curHouse.Size.y / 2 - offset) 
	{
		//use this instead of agentInfo.isInHouse, to make sure he is in current house and not in another random hosue
		pGlobals->currentState = m_Effect;
		pGlobals->actionEnd = true;
		std::cout << "IN HOUSE\n";
	}
}

SearchHouse::SearchHouse() :
	Action({ "InHouse", true }, { "HouseSearched", true })
{}

void SearchHouse::ExecuteEvent(Elite::Blackboard* pBlackboard)
{
	GlobalVariables* pGlobals;
	if (!pBlackboard->GetData("Globals", pGlobals) || pGlobals == nullptr)
		return;

	IExamInterface* pInterface;
	if (!pBlackboard->GetData("Interface", pInterface) || pInterface == nullptr)
		return;

	
	pGlobals->steeringState = SteeringState::SeekWhileSpinning;
	auto agentInfo = pInterface->Agent_GetInfo();

	HouseInfo houseInfo;
	pInterface->Fov_GetHouseByIndex(0, houseInfo);

	const float offset{ 7 };
	const Elite::Vector2 topOfHouse{ houseInfo.Center.x + houseInfo.Size.x / 2 - offset, 
		houseInfo.Center.y + houseInfo.Size.y / 2 - offset };
	const Elite::Vector2 bottomOfHouse{ houseInfo.Center.x - houseInfo.Size.x / 2 + offset
		, houseInfo.Center.y - houseInfo.Size.y / 2 + offset };

	Elite::Vector2 target{topOfHouse};


	if (ReachedTarget(agentInfo, topOfHouse))
		pGlobals->topHouseVisited = true;

	if (ReachedTarget(agentInfo, bottomOfHouse))
		pGlobals->bottomHouseVisited = true;

	if (pGlobals->topHouseVisited)
		target = bottomOfHouse;

	pGlobals->goalPosition = target;
	if(pGlobals->bottomHouseVisited)
	{
		pGlobals->visitedHouses.push_back(pGlobals->currentHouse);
		RemoveVisitedHouse(pBlackboard, pGlobals->currentHouse);
		pGlobals->currentHouse = { {0,0} }; //reset currentHouse
		pGlobals->currentState = m_Effect;
		pGlobals->actionEnd = true;

	}
}



GoOutHouse::GoOutHouse() :
	Action({ "HouseSearched", true }, { "OutOfHouse", true })
{}

void GoOutHouse::ExecuteEvent(Elite::Blackboard* pBlackboard)
{

	GlobalVariables* pGlobals;
	if (!pBlackboard->GetData("Globals", pGlobals) || pGlobals == nullptr)
		return;

	IExamInterface* pInterface;
	if (!pBlackboard->GetData("Interface", pInterface) || pInterface == nullptr)
		return;

	pGlobals->steeringState = SteeringState::SeekWhileSpinning;
	auto agentInfo = pInterface->Agent_GetInfo();


	Elite::Vector2 toCenter{ pInterface->World_GetInfo().Center - agentInfo.Position };
	const float distance{ 100 };
	const Elite::Vector2 exitPoint{ agentInfo.Position + toCenter.GetNormalized() * distance}; //was minus -
	pGlobals->goalPosition = exitPoint;

	if (!agentInfo.IsInHouse && pGlobals->bottomHouseVisited) //when the house is not in view anymore, find another one
	{
		std::cout << "OUT OF HOUSE\n";
		pGlobals->actionEnd = true;
		pGlobals->currentState = m_Effect;
		pGlobals->topHouseVisited = false;
		pGlobals->bottomHouseVisited = false;
	}

}
#pragma endregion


#pragma region //ITEM
GetItemAction::GetItemAction() :
	Action({ "Neutral", true }, { "GetItem", true })
{}

void GetItemAction::ExecuteEvent(Elite::Blackboard* pBlackboard)
{
	GlobalVariables* pGlobals;
	if (!pBlackboard->GetData("Globals", pGlobals) || pGlobals == nullptr)
		return;
	IExamInterface* pInterface;
	if (!pBlackboard->GetData("Interface", pInterface) || pInterface == nullptr)
		return;
	std::vector<EntityInfo>* pEntities;
	if (!pBlackboard->GetData("EntitiesInFOV", pEntities))
		return;

	pGlobals->steeringState = SteeringState::Seek;

	auto agentInfo = pInterface->Agent_GetInfo();

	if (pEntities->size() <= 0)
	{
		return;
	}

	for( const auto& e : *pEntities)
	{

		pGlobals->goalPosition = e.Location;

		if (agentInfo.Position.Distance(pGlobals->goalPosition) < agentInfo.GrabRange)
		{
			ItemInfo itemInfo;
			pInterface->Item_GetInfo(e, itemInfo);

			//VAN HERE
			if (itemInfo.Type == eItemType::GARBAGE)
			{
				pInterface->Item_Destroy(e);
			}
			else if (pGlobals->takenSlots < pInterface->Inventory_GetCapacity()) //if we have room, pick the item
			{
				//get free inventorySlot, check if we have room
				int slot{ GetFreeInventorySpot(pBlackboard) };

				pInterface->Item_Grab(e, itemInfo);
				if(pInterface->Inventory_AddItem(slot, itemInfo))
					++pGlobals->takenSlots;
			}
			else if (NeedsItemA(pBlackboard)) //we dont have room but we need the item
			{
				//get item duplicate, not food
				int dupeSpot{};
				if(GiveDuplicateSpot(pBlackboard, dupeSpot))
					pInterface->Inventory_RemoveItem(dupeSpot);

				pInterface->Item_Grab(e, itemInfo);
				pInterface->Inventory_AddItem(dupeSpot, itemInfo);
				
			}
			else
			{
				pInterface->Item_Destroy(e);
			}
			//TOT HERR
			//if ((NeedsItemA(pBlackboard) && pGlobals->takenSlots < pInterface->Inventory_GetCapacity())
			//	|| itemInfo.Type == eItemType::FOOD) //either he needs the item and has space, or its food
			//{
			//	//get free inventorySlot, check if we have room
			//	int slot{ 10 };
			//	for (int i{}; i < pInterface->Inventory_GetCapacity(); ++i)
			//	{
			//		ItemInfo item;
			//		if (!pInterface->Inventory_GetItem(i, item))
			//		{
			//			//if we dont get an item, that means the spot is free
			//			slot = i;
			//			break;
			//		}
			//	}
			//	if (slot == 10)
			//	{
			//		std::cout << "NO FREE SLOTS\n";
			//		pGlobals->actionEnd = true;
			//		return;
			//	}
			//	pInterface->Item_Grab(e, itemInfo);
			//	pInterface->Inventory_AddItem(slot, itemInfo);
			//	++pGlobals->takenSlots;
			//}
			//else
			//{
			//	pInterface->Item_Destroy(e);
			//}

			pGlobals->currentState = m_Effect;
			pGlobals->actionEnd = true;
		}

	}
		


}

#pragma region //FOOD
//CheckFood::CheckFood() :
//	Action({ "Neutral", false }, { "HasFood", false })
//{}
//
//void CheckFood::ExecuteEvent(Elite::Blackboard* pBlackboard)
//{
//	GlobalVariables* pGlobals;
//	if (!pBlackboard->GetData("Globals", pGlobals) || pGlobals == nullptr)
//		return;
//
//	IExamInterface* pInterface;
//	if (!pBlackboard->GetData("Interface", pInterface) || pInterface == nullptr)
//		return;
//
//	pGlobals->steeringState = SteeringState::Seek;
//
//	auto agentInfo = pInterface->Agent_GetInfo();
//
//	ItemInfo item;
//	if (pInterface->Inventory_GetItem(pGlobals->inventory["Food"], item))
//	{
//		//we have food so change the effect boolean to true
//		//m_Effect.stateBool = true;
//		pGlobals->currentState = {m_Effect.stateString, true};
//	}
//	else
//	{
//		pGlobals->currentState = { m_Effect.stateString, false };
//	}
//	pGlobals->actionEnd = true;
//
//}
//FindFood::FindFood() :
//	Action({ "Neutral", true }, { "HasFood", true })
//{}
//
//void FindFood::ExecuteEvent(Elite::Blackboard* pBlackboard)
//{
//	GlobalVariables* pGlobals;
//	if (!pBlackboard->GetData("Globals", pGlobals) || pGlobals == nullptr)
//		return;
//
//	IExamInterface* pInterface;
//	if (!pBlackboard->GetData("Interface", pInterface) || pInterface == nullptr)
//		return;
//
//	std::vector<EntityInfo>* pEntities;
//	if (!pBlackboard->GetData("EntitiesInFOV", pEntities))
//		return;
//
//	pGlobals->steeringState = SteeringState::Seek;
//
//	auto agentInfo = pInterface->Agent_GetInfo();
//
//	ItemInfo closestFood{ eItemType::FOOD, {INFINITY, INFINITY} }; 
//	//initialize with infiny location so first food encountered will always overwrite as closest
//	for (const ItemInfo& i : pGlobals->storedItems)
//	{
//		if (i.Type != eItemType::FOOD)
//			continue;
//
//		if (i.Location.Distance(agentInfo.Position) < closestFood.Location.Distance(agentInfo.Position))
//		{
//			closestFood = i;
//		}
//	}
//
//	if (closestFood.Location.x >= INFINITY)
//	{
//		//we dont have any food stored in our visitedItems
//		m_Effect.stateBool = false;
//		pGlobals->currentState = m_Effect;
//		pGlobals->actionEnd = true;
//		return;
//	}
//
//
//	pGlobals->goalPosition = closestFood.Location;
//
//	if (ReachedTarget(agentInfo, pGlobals->goalPosition))
//	{
//		ItemInfo item;
//		pInterface->Item_Grab((*pEntities)[0], item);
//		pInterface->Inventory_AddItem(pGlobals->inventory["Food"], item);
//
//		pGlobals->currentState = m_Effect;
//		pGlobals->actionEnd = true;
//	}
//
//}

EatFood::EatFood() :
	Action({ "Neutral", true }, { "HasEaten", true })
{}

void EatFood::ExecuteEvent(Elite::Blackboard* pBlackboard)
{
	GlobalVariables* pGlobals;
	if (!pBlackboard->GetData("Globals", pGlobals) || pGlobals == nullptr)
		return;

	IExamInterface* pInterface;
	if (!pBlackboard->GetData("Interface", pInterface) || pInterface == nullptr)
		return;

	//pGlobals->steeringState = SteeringState::Seek;

	//auto agentInfo = pInterface->Agent_GetInfo();

	ItemInfo item;
	int foodSlot{ 10 };
	FindItemSlot(eItemType::FOOD, foodSlot, *pGlobals, *pInterface, item);
	if (foodSlot == 10) 
	{
		pGlobals->actionEnd = true;
		return;
	}

	pInterface->Inventory_UseItem(foodSlot);
	if(pInterface->Inventory_RemoveItem(foodSlot)) //remove after use
		--pGlobals->takenSlots;

	pGlobals->actionEnd = true;

}


Heal::Heal() :
	Action({ "Neutral", true }, { "HasHealed", true })
{}

void Heal::ExecuteEvent(Elite::Blackboard* pBlackboard)
{
	GlobalVariables* pGlobals;
	if (!pBlackboard->GetData("Globals", pGlobals) || pGlobals == nullptr)
		return;

	IExamInterface* pInterface;
	if (!pBlackboard->GetData("Interface", pInterface) || pInterface == nullptr)
		return;

	//pGlobals->steeringState = SteeringState::Seek;

	//auto agentInfo = pInterface->Agent_GetInfo();

	ItemInfo item;
	int medkitSlot{ 10 };
	FindItemSlot(eItemType::MEDKIT, medkitSlot, *pGlobals, *pInterface, item);
	if (medkitSlot == 10)
	{
		pGlobals->actionEnd = true;
		return;
	}

	pInterface->Inventory_UseItem(medkitSlot);
	if(pInterface->Inventory_RemoveItem(medkitSlot)) //remove after use
		--pGlobals->takenSlots;

	pGlobals->actionEnd = true;

}
#pragma endregion

#pragma endregion


#pragma region //ENEMY

CheckBullets::CheckBullets() :
	Action({ "Neutral", true }, { "HasBullets", true })
{}

void CheckBullets::ExecuteEvent(Elite::Blackboard* pBlackboard)
{
	GlobalVariables* pGlobals;
	if (!pBlackboard->GetData("Globals", pGlobals) || pGlobals == nullptr)
		return;

	IExamInterface* pInterface;
	if (!pBlackboard->GetData("Interface", pInterface) || pInterface == nullptr)
		return;

	ItemInfo item;

	//find weaponSlot
	int weaponSlot{10};
	FindItemSlot(eItemType::PISTOL, weaponSlot, *pGlobals, *pInterface, item);
	FindItemSlot(eItemType::SHOTGUN, weaponSlot, *pGlobals, *pInterface, item); 
	//shotgun slot will overwrite pistol slot if there is one since its better i want to use that first

	if (weaponSlot == 10) // we dont have a gun
	{
		//we dont have a running away action that works for now
		//m_Effect.stateBool = false;
	}
	
	if (pInterface->Weapon_GetAmmo(item) <= 0)
	{
		if(pInterface->Inventory_RemoveItem(weaponSlot))
			--pGlobals->takenSlots;
		//m_Effect.stateBool = false;
	}


	m_Effect.stateBool = true;

	pGlobals->currentState = m_Effect;
	pGlobals->actionEnd = true;

}

RunAway::RunAway() :
	Action({ "HasBullets", false }, { "DealWithEnemy", true })
{}

void RunAway::ExecuteEvent(Elite::Blackboard* pBlackboard)
{
	GlobalVariables* pGlobals;
	if (!pBlackboard->GetData("Globals", pGlobals) || pGlobals == nullptr)
		return;

	IExamInterface* pInterface;
	if (!pBlackboard->GetData("Interface", pInterface) || pInterface == nullptr)
		return;

	std::vector<EntityInfo>* pEntities;
	if (!pBlackboard->GetData("EntitiesInFOV", pEntities))
		return;

	auto agentInfo = pInterface->Agent_GetInfo();


	pGlobals->steeringState = SteeringState::Seek;

	//run opposite way, kinda wank
	/*if (pGlobals->steeringState != SteeringState::Flee)
	{
		Elite::Vector2 toEnemy{ agentInfo.Position - (*pEntities)[0].Location };
		pGlobals->goalPosition = toEnemy.GetNormalized() * -m_FleeRadius; ///fleeradius has no effect yet since it just runs till he cant no more

		pGlobals->steeringState = SteeringState::Flee;
	}*/

	SteeringPlugin_Output* pSteering;
	if (!pBlackboard->GetData("Steering", pSteering) || pSteering == nullptr)
		return;

	pSteering->RunMode = true;

	if (agentInfo.Stamina <= 3)
	{
		pSteering->RunMode = false;
		pGlobals->currentState = m_Effect;
		pGlobals->actionEnd = true;
	}
}

KillEnemy::KillEnemy() :
	Action({ "HasBullets", true }, { "DealWithEnemy", true })
{}

void KillEnemy::ExecuteEvent(Elite::Blackboard* pBlackboard)
{
	GlobalVariables* pGlobals;
	if (!pBlackboard->GetData("Globals", pGlobals) || pGlobals == nullptr)
		return;

	IExamInterface* pInterface;
	if (!pBlackboard->GetData("Interface", pInterface) || pInterface == nullptr)
		return;

	std::vector<EntityInfo>* pEntities;
	if (!pBlackboard->GetData("EntitiesInFOV", pEntities))
		return;

	if (pEntities->size() == 0)
	{
		pGlobals->actionEnd = true;
		m_AimingAtTarget = false;
		return;
	}


	pGlobals->steeringState = SteeringState::FaceAndFlee;
	pGlobals->goalPosition = (*pEntities)[0].Location;
	
	auto agentInfo = pInterface->Agent_GetInfo();
	//when facing enemy -> Shoot

	//const Elite::Vector2 agentDirection{ cosf(agentInfo.Orientation), sinf(agentInfo.Orientation) };
	//const Elite::Vector2 targetDirection{ (pGlobals->goalPosition - agentInfo.Position).GetNormalized() };
	//const float dotProduct{ agentDirection.Dot(targetDirection) };

	float angleBuffer{ 0.06f }; //og 0.01f
	Elite::Vector2 desiredDirection = (pGlobals->goalPosition - agentInfo.Position);

	ItemInfo item;
	int weaponSlot{ 10 };
	FindItemSlot(eItemType::PISTOL, weaponSlot, *pGlobals, *pInterface, item);
	FindItemSlot(eItemType::SHOTGUN, weaponSlot, *pGlobals, *pInterface, item);
	//shotgun slot will overwrite pistol slot if there is one since its better i want to use that first

	if (std::abs(agentInfo.Orientation - std::atan2(desiredDirection.y, desiredDirection.x)) < angleBuffer)
	{
		//shoot if we are aiming at zombie
		if (!pInterface->Inventory_UseItem(weaponSlot))
		{
			//if action failed, we ran out of ammo 
			if(pInterface->Inventory_RemoveItem(weaponSlot))
				--pGlobals->takenSlots;
			pGlobals->currentState = m_Effect;
			pGlobals->actionEnd = true; 
		}
	}

	if (pInterface->Weapon_GetAmmo(item) <= 0)
	{
		if(pInterface->Inventory_RemoveItem(weaponSlot))
			--pGlobals->takenSlots;
		pGlobals->currentState = m_Effect;
		pGlobals->actionEnd = true;
	}
}
#pragma endregion


RunFromPurgeZone::RunFromPurgeZone() :
	Action({ "Neutral", true }, { "EscapePurgeZone", true })
{}

void RunFromPurgeZone::ExecuteEvent(Elite::Blackboard* pBlackboard)
{
	GlobalVariables* pGlobals;
	if (!pBlackboard->GetData("Globals", pGlobals) || pGlobals == nullptr)
		return;

	IExamInterface* pInterface;
	if (!pBlackboard->GetData("Interface", pInterface) || pInterface == nullptr)
		return;

	auto agentInfo = pInterface->Agent_GetInfo();

	pGlobals->steeringState = SteeringState::Flee;
	//use seek because otherwise he gets stuck in houses since fleeing from a point doesnt use the navmesh

	Elite::Vector2 toCenter{ agentInfo.Position - pGlobals->purgeZone.Center };
	const float offset{ 5 };

	pGlobals->goalPosition = pGlobals->purgeZone.Center + toCenter * (pGlobals->purgeZone.Radius + offset);
	//pGlobals->goalPosition = pGlobals->purgeZone.Center;


	if (agentInfo.Position.Distance(pGlobals->goalPosition) > pGlobals->purgeZone.Radius + offset)
	{
		pGlobals->currentState = m_Effect;
		pGlobals->actionEnd = true;
	}
}
