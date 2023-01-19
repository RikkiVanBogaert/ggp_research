#pragma once
#include "stdafx.h"
#include "IExamInterface.h"
#include "Action.h"
#include "MyHelperStructs.h"
#include "CombinedSteeringBehaviors.h"
#include "ActionHelpers.h"

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

	const int steps{ 8 };
	const float angleIncrement{ float(M_PI) * 2.f / steps };

	Elite::Vector2 target{ pInterface->World_GetInfo().Center.x + cosf(m_Angle) * pGlobals->explorationRadius,
	pInterface->World_GetInfo().Center.y + sinf(m_Angle) * pGlobals->explorationRadius };
	

	const float offset{ 5 };
	for (const HouseInfo& h : pGlobals->visitedHouses)
	{
		//check if current point is in a visitedHouse, if so, we skip it and go to the next point
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
		//end of circle
		pGlobals->explorationStage = false;
		m_Angle = 0;
	}


	//std::cout << m_Angle << '\n';
	//DEBUG Drawing
	for (int i{}; i < steps; ++i)
	{
		pInterface->Draw_Point(pInterface->World_GetInfo().Center +
			Elite::Vector2{ cosf(m_Angle + i * angleIncrement) * (pGlobals->explorationRadius),
				sinf(m_Angle + i * angleIncrement) * (pGlobals->explorationRadius) },
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
	const Elite::Vector2 topRightHouse{ houseInfo.Center.x + houseInfo.Size.x / 2 - offset, 
		houseInfo.Center.y + houseInfo.Size.y / 2 - offset };
	const Elite::Vector2 bottomLeftHouse{ houseInfo.Center.x - houseInfo.Size.x / 2 + offset
		, houseInfo.Center.y - houseInfo.Size.y / 2 + offset };

	Elite::Vector2 target{topRightHouse};


	if (ReachedTarget(agentInfo, topRightHouse))
		pGlobals->topHouseVisited = true;

	if (ReachedTarget(agentInfo, bottomLeftHouse))
		pGlobals->bottomHouseVisited = true;

	if (pGlobals->topHouseVisited)
		target = bottomLeftHouse;

	pGlobals->goalPosition = target;
	if(pGlobals->bottomHouseVisited)
	{
		pGlobals->visitedHouses.push_back(pGlobals->currentHouse);
		RemoveVisitedHouse(pBlackboard, pGlobals->currentHouse);
		pGlobals->currentHouse = { {0,0} }; //reset currentHouse
		pGlobals->topHouseVisited = false;
		pGlobals->bottomHouseVisited = false;
		pGlobals->currentState = m_Effect;
		pGlobals->actionEnd = true;

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

		if (agentInfo.Position.Distance(pGlobals->goalPosition) >= agentInfo.GrabRange)
			return;

		ItemInfo itemInfo;
		pInterface->Item_GetInfo(e, itemInfo);

		if (itemInfo.Type == eItemType::GARBAGE)
		{
			pInterface->Item_Destroy(e);
		}
		else if (pGlobals->takenSlots < pInterface->Inventory_GetCapacity()) //if we have room, pick the item
		{
			//get free inventorySlot
			int slot{ GetFreeInventorySpot(pBlackboard) };

			pInterface->Item_Grab(e, itemInfo);
			if(pInterface->Inventory_AddItem(slot, itemInfo))
				++pGlobals->takenSlots;
		}
		else if (NeedsItemA(pBlackboard)) //we dont have room but we need the item
		{
			//get item duplicate in inventory, cant be food
			int dupeSpot{};
			if(GiveDuplicateSpot(pBlackboard, dupeSpot))
				pInterface->Inventory_RemoveItem(dupeSpot);

			pInterface->Item_Grab(e, itemInfo);
			pInterface->Inventory_AddItem(dupeSpot, itemInfo);
			
		}
		else // we dont have room and we dont need the item
		{
			pInterface->Item_Destroy(e);
		}

		pGlobals->currentState = m_Effect;
		pGlobals->actionEnd = true;

	}
		


}

#pragma region //FOODandHEALING

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

	ItemInfo item;
	int foodSlot{ 10 };
	FindItemSlot(pBlackboard, eItemType::FOOD, foodSlot, item);
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


	ItemInfo item;
	int medkitSlot{ 10 };
	FindItemSlot(pBlackboard, eItemType::MEDKIT, medkitSlot, item);
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
	FindItemSlot(pBlackboard, eItemType::PISTOL, weaponSlot, item);
	FindItemSlot(pBlackboard, eItemType::SHOTGUN, weaponSlot, item);
	//shotgun slot will overwrite pistol slot if there is one since its better i want to use that first

	if (weaponSlot == 10) // slot didnt change so we dont have a gun
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

	float angleBuffer{ 0.06f }; //og 0.01f
	Elite::Vector2 desiredDirection = (pGlobals->goalPosition - agentInfo.Position);

	ItemInfo item;
	int weaponSlot{ 10 };
	FindItemSlot(pBlackboard, eItemType::PISTOL, weaponSlot, item);
	FindItemSlot(pBlackboard, eItemType::SHOTGUN, weaponSlot, item);
	//shotgun slot will overwrite pistol slot if there is one since its better i want to use that first

	//check for 0 ammo
	if (pInterface->Weapon_GetAmmo(item) <= 0)
	{
		if(pInterface->Inventory_RemoveItem(weaponSlot))
			--pGlobals->takenSlots;
		pGlobals->currentState = m_Effect;
		pGlobals->actionEnd = true;
		return;
	}

	if (std::abs(agentInfo.Orientation - std::atan2(desiredDirection.y, desiredDirection.x)) >= angleBuffer)
		return; //return if we are not aiming at the zombie yet
	
	//shoot if we are aiming at zombie
	if (!pInterface->Inventory_UseItem(weaponSlot))
	{
		//if action failed, we ran out of ammo 
		if(pInterface->Inventory_RemoveItem(weaponSlot))
			--pGlobals->takenSlots;
		pGlobals->currentState = m_Effect;
		pGlobals->actionEnd = true; 
		return;
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

	Elite::Vector2 toCenter{ agentInfo.Position - pGlobals->purgeZone.Center };
	const float offset{ 5 };

	pGlobals->goalPosition = pGlobals->purgeZone.Center + toCenter * (pGlobals->purgeZone.Radius + offset);

	if (agentInfo.Position.Distance(pGlobals->goalPosition) > pGlobals->purgeZone.Radius + offset)
	{
		pGlobals->currentState = m_Effect;
		pGlobals->actionEnd = true;
	}
}
