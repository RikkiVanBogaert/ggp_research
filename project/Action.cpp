#pragma once
#include "stdafx.h"
#include "Exam_HelperStructs.h"
#include "IExamInterface.h"
#include "Action.h"
#include "MyHelperStructs.h"

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
	m_Cost{cost},
	m_ActionEnd{false}
{}



bool Action::ConditionMet(const State& currentState)
{
	if (currentState.stateString == m_Precondition.stateString
		&& currentState.stateBool == m_Precondition.stateBool)
		return true;

	return false;
}

void Action::InventoryManagement(IExamInterface* interfacePtr)
{
	//interfacePtr->Inventory_GetItem()
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

void Explore::ExecuteEvent(SteeringPlugin_Output& steering, IExamInterface* interfacePtr, GlobalVariables& globals,
	std::vector<HouseInfo>& houses, std::vector<EntityInfo>& entities)
{
	globals.steeringState = SteeringState::SeekWhileSpinning;

	auto agentInfo = interfacePtr->Agent_GetInfo();

	//explore the world by going to the different corners
	const float offset{ 4 };
	if(!m_TopLeftExplored)
	{
		globals.goalPosition.x = interfacePtr->World_GetInfo().Center.x - interfacePtr->World_GetInfo().Dimensions.x / offset;
		globals.goalPosition.y = interfacePtr->World_GetInfo().Center.y + interfacePtr->World_GetInfo().Dimensions.y / offset;
		if (ReachedTarget(agentInfo, globals.goalPosition))
		{
			m_TopLeftExplored = true;
		}
	}
	else if (!m_TopRightExplored)
	{
		globals.goalPosition.x = interfacePtr->World_GetInfo().Center.x + interfacePtr->World_GetInfo().Dimensions.x / offset;
		globals.goalPosition.y = interfacePtr->World_GetInfo().Center.y + interfacePtr->World_GetInfo().Dimensions.y / offset;
		if (ReachedTarget(agentInfo, globals.goalPosition))
		{
			m_TopRightExplored = true;
		}
	}
	else if (!m_BottomRightExplored)
	{
		globals.goalPosition.x = interfacePtr->World_GetInfo().Center.x + interfacePtr->World_GetInfo().Dimensions.x / offset;
		globals.goalPosition.y = interfacePtr->World_GetInfo().Center.y - interfacePtr->World_GetInfo().Dimensions.y / offset;
		if (ReachedTarget(agentInfo, globals.goalPosition))
		{
			m_BottomRightExplored = true;
		}
	}
	else if (!m_BottomLeftExplored)
	{
		globals.goalPosition.x = interfacePtr->World_GetInfo().Center.x - interfacePtr->World_GetInfo().Dimensions.x / offset;
		globals.goalPosition.y = interfacePtr->World_GetInfo().Center.y - interfacePtr->World_GetInfo().Dimensions.y / offset;
		if (ReachedTarget(agentInfo, globals.goalPosition))
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
		globals.visitedHouses.clear();
	}

}

#pragma region //HOUSE
//FindHouse::FindHouse() :
//	Action("Neutral", "HouseNearby")
//{}
//
//void FindHouse::ExecuteEvent(SteeringPlugin_Output& steering, IExamInterface* interfacePtr, GlobalVariables& globals,
//	std::vector<HouseInfo>& houses, std::vector<EntityInfo>& entities)
//{
//
//	auto agentInfo = interfacePtr->Agent_GetInfo();
//	
//
//	Elite::Vector2 distance{ 50, 50 };
//	if (m_HasWanderTarget && ReachedTarget(agentInfo, globals.goalPosition))
//	{
//		distance.y *= -1;
//		m_HasWanderTarget = false;
//	}
//	if (!m_HasWanderTarget)
//	{
//		globals.goalPosition = agentInfo.Position + agentInfo.LinearVelocity.GetNormalized() * distance;
//		m_HasWanderTarget = true;
//	}
//
//
//	if (houses.size() > 0)
//	{
//		globals.currentStates[2] = m_Effect;
//	}
//
//}

GoInHouse::GoInHouse() :
	Action({ "Neutral", true }, { "InHouse", true })
{}

void GoInHouse::ExecuteEvent(SteeringPlugin_Output& steering, IExamInterface* interfacePtr, GlobalVariables& globals,
	std::vector<HouseInfo>& houses, std::vector<EntityInfo>& entities)
{
	globals.steeringState = SteeringState::SeekWhileSpinning;

	auto agentInfo = interfacePtr->Agent_GetInfo();

	globals.goalPosition = globals.currentHouse.Center;
	if (agentInfo.IsInHouse)
	{
		globals.currentState = m_Effect;
		m_ActionEnd = true;
		std::cout << "IN HOUSE\n";
	}
}

SearchHouse::SearchHouse() :
	Action({ "InHouse", true }, { "HouseSearched", true })
{}

void SearchHouse::ExecuteEvent(SteeringPlugin_Output& steering, IExamInterface* interfacePtr, GlobalVariables& globals,
	std::vector<HouseInfo>& houses, std::vector<EntityInfo>& entities)
{
	globals.steeringState = SteeringState::SeekWhileSpinning;
	auto agentInfo = interfacePtr->Agent_GetInfo();

	HouseInfo houseInfo;
	interfacePtr->Fov_GetHouseByIndex(0, houseInfo);

	const float offset{ 7 };
	const Elite::Vector2 topOfHouse{ houseInfo.Center.x + houseInfo.Size.x / 2 - offset, 
		houseInfo.Center.y + houseInfo.Size.y / 2 - offset };
	const Elite::Vector2 bottomOfHouse{ houseInfo.Center.x - houseInfo.Size.x / 2 + offset
		, houseInfo.Center.y - houseInfo.Size.y / 2 + offset };

	if (!m_SeekingBottomHouse && !m_SeekingTopHouse)//go to the top of house if hes not already seeking
	{
		m_SeekingTopHouse = true;
	}
	//Put seekingHouse bools in class member vars instead of globals
	if (m_SeekingTopHouse && !m_SeekingBottomHouse)
	{
		globals.goalPosition = topOfHouse;
		if (ReachedTarget(agentInfo, topOfHouse)) //we reached top, now go bottom
		{
			m_SeekingBottomHouse = true;
		}
	}
	else if (m_SeekingBottomHouse)
	{
		globals.goalPosition = bottomOfHouse;
		if (ReachedTarget(agentInfo, bottomOfHouse)) //we bottom, now leave
		{
			globals.currentState = m_Effect;
			m_ActionEnd = true;

			m_SeekingTopHouse = false;
			m_SeekingBottomHouse = false;
		}
	}

}



GoOutHouse::GoOutHouse() :
	Action({ "HouseSearched", true }, { "OutOfHouse", true })
{}

void GoOutHouse::ExecuteEvent(SteeringPlugin_Output& steering, IExamInterface* interfacePtr, GlobalVariables& globals,
	std::vector<HouseInfo>& houses, std::vector<EntityInfo>& entities)
{
	globals.steeringState = SteeringState::SeekWhileSpinning;
	auto agentInfo = interfacePtr->Agent_GetInfo();


	Elite::Vector2 toCenter{ interfacePtr->World_GetInfo().Center - agentInfo.Position };
	const float distance{ 100 };
	const Elite::Vector2 exitPoint{ agentInfo.Position - toCenter.GetNormalized() * distance};
	globals.goalPosition = exitPoint;

	if (!agentInfo.IsInHouse) //when the house is not in view anymore, find another one
	{
		std::cout << "OUT OF HOUSE\n";
		globals.visitedHouses[globals.visitedHouses.size() - 1].second = true;
		m_ActionEnd = true;
		globals.currentHouse = { {0,0} }; //reset currentHouse
	}

}
#pragma endregion


#pragma region //ITEM
GetItemAction::GetItemAction() :
	Action({ "Neutral", true }, { "GetItem", true })
{}

void GetItemAction::ExecuteEvent(SteeringPlugin_Output& steering, IExamInterface* interfacePtr, GlobalVariables& globals,
	std::vector<HouseInfo>& houses, std::vector<EntityInfo>& entities)
{
	globals.steeringState = SteeringState::Seek;

	auto agentInfo = interfacePtr->Agent_GetInfo();

	for (auto& e : entities)
	{
		if (e.Type == eEntityType::PURGEZONE)
		{
			PurgeZoneInfo zoneInfo;
			interfacePtr->PurgeZone_GetInfo(e, zoneInfo);
			//std::cout << "Purge Zone in FOV:" << e.Location.x << ", "<< e.Location.y << "---Radius: "<< zoneInfo.Radius << std::endl;
		}

		if (e.Type == eEntityType::ITEM)
		{
			//shotgun bug in seed 37 where he doesnt collect it, thought this was fix but guess not, so not needed for now
			//interfacePtr->Fov_GetEntityByIndex(0, e);
			//ItemInfo newItemInfo{ eItemType::GARBAGE }, ownedItem;
			//interfacePtr->Item_GetInfo(e, newItemInfo);
			//int type{};
			//switch (newItemInfo.Type)
			//{
			//case eItemType::PISTOL:
			//	type = 0;
			//	break;
			//case eItemType::SHOTGUN:
			//	type = 1;
			//	break;
			//case eItemType::FOOD:
			//	type = 2;
			//	break;
			//case eItemType::MEDKIT:
			//	type = 3;
			//	break;
			//default:
			//	break;
			//}
			//if (interfacePtr->Inventory_GetItem(type, ownedItem))
			//	continue; //we already have that item, so skip and look at the next one if there is one

			Elite::Vector2 checkpointLocation{ e.Location };
			globals.goalPosition = checkpointLocation;
			ItemInfo itemInfo;
			interfacePtr->Item_GetInfo(e, itemInfo);
			if (agentInfo.Position.Distance(checkpointLocation) < agentInfo.GrabRange)
			{
				switch (itemInfo.Type)
				{
				case eItemType::PISTOL:
					interfacePtr->Item_Grab(e, itemInfo);
					interfacePtr->Inventory_AddItem(globals.inventory["Pistol"], itemInfo);
					break;
				case eItemType::SHOTGUN:
					interfacePtr->Item_Grab(e, itemInfo);
					interfacePtr->Inventory_AddItem(globals.inventory["Shotgun"], itemInfo);
					break;
				case eItemType::FOOD:
					interfacePtr->Item_Grab(e, itemInfo);
					interfacePtr->Inventory_AddItem(globals.inventory["Food"], itemInfo);
					globals.hasFood = true;
					break;
				case eItemType::MEDKIT:
					interfacePtr->Item_Grab(e, itemInfo);
					interfacePtr->Inventory_AddItem(globals.inventory["Medkit"], itemInfo);
					globals.m_HasMedkit = true;
					break;
				case eItemType::GARBAGE:
					interfacePtr->Item_Destroy(e);
					break;
				default:
					interfacePtr->Inventory_AddItem(4, itemInfo);
					//still have to make use of this last space
					break;
				}

				globals.currentState = m_Effect;
				m_ActionEnd = true;

			}
		}
	}

}

#pragma region //FOOD
CheckFood::CheckFood() :
	Action({ "Neutral", true }, { "HasFood", false })
{}

void CheckFood::ExecuteEvent(SteeringPlugin_Output& steering, IExamInterface* interfacePtr, GlobalVariables& globals,
	std::vector<HouseInfo>& houses, std::vector<EntityInfo>& entities)
{
	globals.steeringState = SteeringState::Seek;

	auto agentInfo = interfacePtr->Agent_GetInfo();

	ItemInfo item;
	if (interfacePtr->Inventory_GetItem(globals.inventory["Food"], item))
	{
		//we have food so change the effect boolean to true
		//m_Effect.stateBool = true;
		globals.currentState = {m_Effect.stateString, true};
	}
	else
	{
		globals.currentState = { m_Effect.stateString, false };
	}
	m_ActionEnd = true;

}

FindFood::FindFood() :
	Action({ "HasFood", false }, { "HasFood", true })
{}

void FindFood::ExecuteEvent(SteeringPlugin_Output& steering, IExamInterface* interfacePtr, GlobalVariables& globals,
	std::vector<HouseInfo>& houses, std::vector<EntityInfo>& entities)
{
	globals.steeringState = SteeringState::Seek;

	auto agentInfo = interfacePtr->Agent_GetInfo();

	ItemInfo closestFood{ eItemType::FOOD, {INFINITY, INFINITY} }; 
	//initialize with infiny location so first food encountered will always overwrite as closest
	for (const ItemInfo& i : globals.storedItems)
	{
		if (i.Type != eItemType::FOOD)
			continue;

		if (i.Location.Distance(agentInfo.Position) < closestFood.Location.Distance(agentInfo.Position))
		{
			closestFood = i;
			globals.hasFood = true;
		}
	}

	if (closestFood.Location.x >= INFINITY)
	{
		//we dont have any food stored in our visitedItems
		globals.hasFood = false;
		m_Effect.stateBool = false;
		globals.currentState = m_Effect;
		m_ActionEnd = true;
		return;
	}


	globals.goalPosition = closestFood.Location;

	if (ReachedTarget(agentInfo, globals.goalPosition))
	{
		ItemInfo item;
		interfacePtr->Item_Grab(entities[0], item);
		interfacePtr->Inventory_AddItem(globals.inventory["Food"], item);
		globals.hasFood = true;

		globals.currentState = m_Effect;
		m_ActionEnd = true;
	}

}

EatFood::EatFood() :
	Action({ "HasFood", true }, { "HasEaten", true })
{}

void EatFood::ExecuteEvent(SteeringPlugin_Output& steering, IExamInterface* interfacePtr, GlobalVariables& globals,
	std::vector<HouseInfo>& houses, std::vector<EntityInfo>& entities)
{
	globals.steeringState = SteeringState::Seek;

	auto agentInfo = interfacePtr->Agent_GetInfo();

	interfacePtr->Inventory_UseItem(globals.inventory["Food"]);
	interfacePtr->Inventory_RemoveItem(globals.inventory["Food"]); //remove after use
	//globals.hasFood = false;

	m_ActionEnd = true;

}
#pragma endregion

#pragma endregion


#pragma region //ENEMY

CheckBullets::CheckBullets() :
	Action({ "Neutral", true }, { "HasBullets", true })
{}

void CheckBullets::ExecuteEvent(SteeringPlugin_Output& steering, IExamInterface* interfacePtr, GlobalVariables& globals,
	std::vector<HouseInfo>& houses, std::vector<EntityInfo>& entities)
{
	ItemInfo itemInfo;

	if (!interfacePtr->Inventory_GetItem(globals.inventory["Pistol"], itemInfo))
	{
		m_Effect.stateBool = false;
	}
	else
	{
		if(interfacePtr->Weapon_GetAmmo(itemInfo) > 0) 
			m_Effect.stateBool = true;
		else
			m_Effect.stateBool = false;
	}



	globals.currentState = m_Effect;
	m_ActionEnd = true;

}

RunAway::RunAway() :
	Action({ "HasBullets", false }, { "DealWithEnemy", true })
{}

void RunAway::ExecuteEvent(SteeringPlugin_Output& steering, IExamInterface* interfacePtr, GlobalVariables& globals,
	std::vector<HouseInfo>& houses, std::vector<EntityInfo>& entities)
{
	auto agentInfo = interfacePtr->Agent_GetInfo();

	if (globals.steeringState != SteeringState::Flee)
	{
		Elite::Vector2 toEnemy{ agentInfo.Position - entities[0].Location };
		globals.goalPosition = toEnemy.GetNormalized() * -m_FleeRadius; ///fleeradius has no effect yet since it just runs till he cant no more

		globals.steeringState = SteeringState::Flee;
	}
	if (agentInfo.Stamina <= 0)
	{
		m_ActionEnd = true;
	}
}

KillEnemy::KillEnemy() :
	Action({ "HasBullets", true }, { "DealWithEnemy", true })
{}

void KillEnemy::ExecuteEvent(SteeringPlugin_Output& steering, IExamInterface* interfacePtr, GlobalVariables& globals,
	std::vector<HouseInfo>& houses, std::vector<EntityInfo>& entities)
{
	if (entities.size() == 0)
	{
		m_ActionEnd = true;
		m_AimingAtTarget = false;
		return;
	}


	globals.steeringState = SteeringState::FaceAndFlee;
	globals.goalPosition = entities[0].Location;
	
	auto agentInfo = interfacePtr->Agent_GetInfo();
	//when facing enemy -> Shoot

	const Elite::Vector2 agentDirection{ cosf(agentInfo.Orientation), sinf(agentInfo.Orientation) };
	const Elite::Vector2 targetDirection{ (globals.goalPosition - agentInfo.Position).GetNormalized() };
	const float dotProduct{ agentDirection.Dot(targetDirection) };

	constexpr float epsilon{ 0.007f };
	if (dotProduct < 1 + epsilon && dotProduct > 1 - epsilon)
	{
		//we are aiming at target
		m_AimingAtTarget = true;
	}

	if (m_AimingAtTarget)
	{
		if(!interfacePtr->Inventory_UseItem(0))
			m_ActionEnd = true; //if action failed, we ran out of ammo
	}
}
#pragma endregion


RunFromPurgeZone::RunFromPurgeZone() :
	Action({ "Neutral", true }, { "EscapePurgeZone", true })
{}

void RunFromPurgeZone::ExecuteEvent(SteeringPlugin_Output& steering, IExamInterface* interfacePtr, GlobalVariables& globals,
	std::vector<HouseInfo>& houses, std::vector<EntityInfo>& entities)
{
	auto agentInfo = interfacePtr->Agent_GetInfo();

	globals.steeringState = SteeringState::Seek;
	//use seek because otherwise he gets stuck in houses since fleeing from a point doesnt use the navmesh

	Elite::Vector2 toCenter{ agentInfo.Position - globals.purgeZone.Center };
	const float offset{ 10 };

	globals.goalPosition = globals.purgeZone.Center + toCenter * (globals.purgeZone.Radius + offset);

	if (agentInfo.Position.Distance(globals.goalPosition) > globals.purgeZone.Radius + offset)
	{
		m_ActionEnd = true;
	}
}