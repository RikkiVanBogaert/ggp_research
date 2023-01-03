#include "stdafx.h"
#include "Plugin.h"
#include "IExamInterface.h"
#include "SteeringBehavior.h"
#include "Planner.h"
#include "Action.h"
#include "MyHelperFunctions.h"


using namespace std;
using namespace Elite;

//Called only once, during initialization
void Plugin::Initialize(IBaseInterface* pInterface, PluginInfo& info)
{
	//Retrieving the interface
	//This interface gives you access to certain actions the AI_Framework can perform for you
	m_pInterface = static_cast<IExamInterface*>(pInterface);

	//Bit information about the plugin
	//Please fill this in!!
	info.BotName = "Tob";
	info.Student_FirstName = "Rikki";
	info.Student_LastName = "Van Bogaert";
	info.Student_Class = "2DAE08";

	//Variables
	m_AngSpeed = 10.f;
	m_pSteeringBehavior = new Seek();

	//GOAP
	std::vector<Action*> actions
	{
		new Explore(),

		new GoInHouse(),
		new SearchHouse(),
		new GoOutHouse(),

		new GetItemAction(),

		new CheckFood(),
		new FindFood(),
		new EatFood(),

		new CheckBullets(),
		new RunAway(),
		new KillEnemy(),

		new RunFromPurgeZone()
	};
	m_pPlanner = new Planner(actions);

	m_Globals.currentState = { "Neutral", true };

	m_Globals.goalState = { "Explore", true };
	m_Globals.steeringState = SteeringState::Seek;

	//initialize inventory slots
	m_Globals.inventory["Pistol"] = 0;
	m_Globals.inventory["Shotgun"] = 1;
	m_Globals.inventory["Food"] = 2;
	m_Globals.inventory["Medkit"] = 3;
}

//Called only once
void Plugin::DllInit()
{
	//Called when the plugin is loaded
}

//Called only once
void Plugin::DllShutdown()
{
	//Called wheb the plugin gets unloaded
	//delete m_Globals.pSteeringBehavior;
	delete m_pPlanner;
}

//Called only once, during initialization
void Plugin::InitGameDebugParams(GameDebugParams& params)
{
	params.AutoFollowCam = true; //Automatically follow the AI? (Default = true)
	params.RenderUI = true; //Render the IMGUI Panel? (Default = true)
	params.SpawnEnemies = true; //Do you want to spawn enemies? (Default = true)
	params.EnemyCount = 20; //How many enemies? (Default = 20)
	params.GodMode = false; //GodMode > You can't die, can be useful to inspect certain behaviors (Default = false)
	params.LevelFile = "GameLevel.gppl";
	params.AutoGrabClosestItem = true; //A call to Item_Grab(...) returns the closest item that can be grabbed. (EntityInfo argument is ignored)
	params.StartingDifficultyStage = 1;
	params.InfiniteStamina = false;
	params.SpawnDebugPistol = true;
	params.SpawnDebugShotgun = true;
	params.SpawnPurgeZonesOnMiddleClick = true;
	params.PrintDebugMessages = true;
	params.ShowDebugItemNames = true;
	params.Seed = 10; //36 is og
}

//Only Active in DEBUG Mode
//(=Use only for Debug Purposes)
//WILL NOT BE CALLED IN RELEASE IS ONLY FOR DEBUG
void Plugin::Update(float dt)
{
	//Demo Event Code
	//In the end your AI should be able to walk around without external input
	if (m_pInterface->Input_IsMouseButtonUp(Elite::InputMouseButton::eLeft))
	{
		//Update target based on input
		Elite::MouseData mouseData = m_pInterface->Input_GetMouseData(Elite::InputType::eMouseButton, Elite::InputMouseButton::eLeft);
		const Elite::Vector2 pos = Elite::Vector2(static_cast<float>(mouseData.X), static_cast<float>(mouseData.Y));
		m_Target = m_pInterface->Debug_ConvertScreenToWorld(pos);
	}
	else if (m_pInterface->Input_IsKeyboardKeyDown(Elite::eScancode_Space))
	{
		m_CanRun = true;
	}
	else if (m_pInterface->Input_IsKeyboardKeyDown(Elite::eScancode_Left))
	{
		m_AngSpeed -= Elite::ToRadians(10);
	}
	else if (m_pInterface->Input_IsKeyboardKeyDown(Elite::eScancode_Right))
	{
		m_AngSpeed += Elite::ToRadians(10);
	}
	else if (m_pInterface->Input_IsKeyboardKeyDown(Elite::eScancode_G))
	{
		m_GrabItem = true;
	}
	else if (m_pInterface->Input_IsKeyboardKeyDown(Elite::eScancode_U))
	{
		m_UseItem = true;
	}
	else if (m_pInterface->Input_IsKeyboardKeyDown(Elite::eScancode_R))
	{
		m_RemoveItem = true;
	}
	else if (m_pInterface->Input_IsKeyboardKeyUp(Elite::eScancode_Space))
	{
		m_CanRun = false;
	}
	else if (m_pInterface->Input_IsKeyboardKeyDown(Elite::eScancode_Delete))
	{
		m_pInterface->RequestShutdown();
	}
	else if (m_pInterface->Input_IsKeyboardKeyDown(Elite::eScancode_KP_Minus))
	{
		if (m_Globals.inventorySlot > 0)
			--m_Globals.inventorySlot;
	}
	else if (m_pInterface->Input_IsKeyboardKeyDown(Elite::eScancode_KP_Plus))
	{
		if (m_Globals.inventorySlot < 4)
			++m_Globals.inventorySlot;
	}
	else if (m_pInterface->Input_IsKeyboardKeyDown(Elite::eScancode_Q))
	{
		ItemInfo info = {};
		m_pInterface->Inventory_GetItem(m_Globals.inventorySlot, info);
		std::cout << (int)info.Type << std::endl;
	}
}

//Update
//This function calculates the new SteeringOutput, called once per frame
SteeringPlugin_Output Plugin::UpdateSteering(float dt)
{
	auto steering = SteeringPlugin_Output();

	//Use the Interface (IAssignmentInterface) to 'interface' with the AI_Framework
	auto agentInfo = m_pInterface->Agent_GetInfo();

	auto vHousesInFOV = GetHousesInFOV();//uses m_pInterface->Fov_GetHouseByIndex(...)
	auto vEntitiesInFOV = GetEntitiesInFOV();


	m_pPlanner->ExecutePlan(steering, m_pInterface, m_Globals, vHousesInFOV, vEntitiesInFOV);

	ManagePriorities();

	//DebugRendering
	m_pInterface->Draw_Point(m_NextTargetPos, 5, { 1,1,1 });



	if (Distance(m_NextTargetPos, agentInfo.Position) < 2.f)
	{
		steering.LinearVelocity = Elite::ZeroVector2;
	}

	//steering.RunMode = false; //If RunMode is True > MaxLinSpd is increased for a limited time (till your stamina runs out)

	//Handle steeringBehavior
	switch (m_Globals.steeringState)
	{
	case SteeringState::Seek:
		steering.RunMode = false;
		m_pSteeringBehavior = new Seek();
		break;
	case SteeringState::Flee:
		steering.RunMode = true;
		m_pSteeringBehavior = new Flee();
		break;
	case SteeringState::Face:
		steering.RunMode = false;
		m_pSteeringBehavior = new Face();
		break;
	case SteeringState::SeekWhileSpinning:
		steering.RunMode = false;
		m_pSteeringBehavior = new SeekWhileSpinning();
		break;
	case SteeringState::FaceAndFlee:
		steering.RunMode = false;
		m_pSteeringBehavior = new FaceAndFlee();
		break;
	default:
		break;
	}

	m_NextTargetPos = m_pInterface->NavMesh_GetClosestPathPoint(m_Globals.goalPosition);
	m_pSteeringBehavior->SetTarget(m_NextTargetPos);
	m_pSteeringBehavior->CalculateSteering(steering, agentInfo);

	//SteeringPlugin_Output is works the exact same way a SteeringBehaviour output

//@End (Demo Purposes)
	m_GrabItem = false; //Reset State
	m_UseItem = false;
	m_RemoveItem = false;

	return steering;
}

//This function should only be used for rendering debug elements
void Plugin::Render(float dt) const
{
	//This Render function should only contain calls to Interface->Draw_... functions
	m_pInterface->Draw_SolidCircle(m_Target, .7f, { 0,0 }, { 1, 0, 0 });
}

std::vector<HouseInfo> Plugin::GetHousesInFOV() const
{
	std::vector<HouseInfo> vHousesInFOV = {};

	HouseInfo hi = {};
	for (int i = 0;; ++i)
	{
		if (m_pInterface->Fov_GetHouseByIndex(i, hi))
		{
			vHousesInFOV.push_back(hi);
			continue;
		}

		break;
	}

	return vHousesInFOV;
}

std::vector<EntityInfo> Plugin::GetEntitiesInFOV() const
{
	std::vector<EntityInfo> vEntitiesInFOV = {};

	EntityInfo ei = {};
	for (int i = 0;; ++i)
	{
		if (m_pInterface->Fov_GetEntityByIndex(i, ei))
		{
			vEntitiesInFOV.push_back(ei);
			continue;
		}

		break;
	}

	return vEntitiesInFOV;
}


void Plugin::ManagePriorities()
{
	//ManagePriorities
	auto agentInfo = m_pInterface->Agent_GetInfo();
	auto vHousesInFOV = GetHousesInFOV();//uses m_pInterface->Fov_GetHouseByIndex(...)
	auto vEntitiesInFOV = GetEntitiesInFOV();

	if (vEntitiesInFOV.size() > 0 && vEntitiesInFOV[0].Type == eEntityType::PURGEZONE)
	{
		//loop through all entities in FOV and if any of them is a zone, do zone,
		//if any is an enemy, do enemy.....etc
		m_pInterface->PurgeZone_GetInfo(vEntitiesInFOV[0], m_Globals.purgeZone);
		m_Globals.goalState = { "EscapePurgeZone", true };
		m_pPlanner->FindPlan(m_Globals);
	}
	else if (agentInfo.Energy < 5.f && m_Globals.hasFood)
	{
		m_Globals.goalState = { "HasEaten", true };
		m_pPlanner->FindPlan(m_Globals);
	}
	else if(vEntitiesInFOV.size() > 0 && vEntitiesInFOV[0].Type == eEntityType::ENEMY)
	{
		m_Globals.goalState = { "DealWithEnemy", true };
		m_pPlanner->FindPlan(m_Globals);
	}
	else if (vEntitiesInFOV.size() > 0 && vEntitiesInFOV[0].Type == eEntityType::ITEM &&
		IsNewItem())
	{
		m_Globals.goalState = { "GetItem", true };
		m_pPlanner->FindPlan(m_Globals);
	}
	else if (IsNewHouse())
	{
		m_Globals.goalState = { "OutOfHouse", true };
		m_pPlanner->FindPlan(m_Globals);
	}
	else
	{
		m_Globals.currentState = { "Neutral", true };
		m_Globals.goalState = { "Explore", true };
	}
}


bool Plugin::ReachedTarget(const AgentInfo& agent, const Elite::Vector2& targetPos) const
{
	if (agent.Position.Distance(targetPos) <= agent.GrabRange)
		return true;

	return false;
}

bool Plugin::IsNewHouse()
{
	auto vHousesInFOV = GetHousesInFOV();
	if (vHousesInFOV.size() == 0 && 
		m_Globals.currentHouse.Center == Elite::Vector2{0,0}) // no houses in FOV and no current house
		return false;

	if (m_Globals.currentHouse.Center != Elite::Vector2{ 0,0 })
		return true; //we have a current house, so just continue behavior

	//else, there is a house in fov, and we dont have a current house
	//check if we have visited the house
	for (const std::pair<HouseInfo, bool>& h : m_Globals.visitedHouses)
	{
		//if the center of the found house is the same as a house in the vector visitedHouses, 
		//and if we have searched the house, we dont want to go in there anymore
		if (vHousesInFOV[0].Center == h.first.Center
			&& h.second)
		{
			return false;
		}
	}

	//else, we add the house as currentHouse
	m_Globals.currentHouse = vHousesInFOV[0];
	m_Globals.visitedHouses.push_back({ m_Globals.currentHouse, false });
	return true;
}

bool Plugin::IsNewItem()
{
	auto vEntitiesInFOV = GetEntitiesInFOV();
	bool newItemInFOV{ false };

	for ( EntityInfo e: vEntitiesInFOV)
	{
		m_pInterface->Fov_GetEntityByIndex(0, e);
		ItemInfo newItemInfo{ eItemType::GARBAGE };
		ItemInfo ownedItemInfo;
		m_pInterface->Item_GetInfo(e, newItemInfo);

		int type{};
		int valueInventoryItem{};

		switch (newItemInfo.Type)
		{
		case eItemType::PISTOL:
			type = m_Globals.inventory["Pistol"];
			break;
		case eItemType::SHOTGUN:
			type = m_Globals.inventory["Shotgun"];
			break;
		case eItemType::FOOD:
			type = m_Globals.inventory["Food"];
			m_Globals.hasFood = true;
			break;
		case eItemType::MEDKIT:
			type = m_Globals.inventory["Medkit"];
			break;
		default:
			break;
		}

		//if one of the items in the FOV is new, the state is set to itemNearby, or if its garbage which we have to destroy
		if(!m_pInterface->Inventory_GetItem(type, ownedItemInfo) ||
			newItemInfo.Type == eItemType::GARBAGE)
		{
			return true;
		}
		//if not a new item, and not already visited, store itemInfo in the storedItems vector
		for (const ItemInfo& i : m_Globals.storedItems)
		{
			if (newItemInfo.Location == i.Location)
				return false;
		}

		m_Globals.storedItems.push_back(newItemInfo);
		std::cout << "STORED ITEM IN VISITEDITEMS\n";
		return true;


		//not done yet because u cant drop items from inventory without discarding them, so would be stupid
		//else if(m_pInterface->Inventory_GetItem(type, ownedItemInfo))
		//{
		//	//we have item already, but we check if the value is better than our current item
		//	int valueInventoryItem{};
		//	int valueNewItem{};
		//	switch (newItemInfo.Type)
		//	{
		//	case eItemType::PISTOL:
		//		type = m_Globals.inventory["Pistol"];
		//		valueInventoryItem = m_pInterface->Weapon_GetAmmo(ownedItemInfo);
		//		valueNewItem = m_pInterface->Weapon_GetAmmo(newItemInfo);
		//		break;
		//	case eItemType::SHOTGUN:
		//		type = m_Globals.inventory["Shotgun"];
		//		break;
		//	case eItemType::FOOD:
		//		type = m_Globals.inventory["Food"];
		//		break;
		//	case eItemType::MEDKIT:
		//		type = m_Globals.inventory["Medkit"];
		//		break;
		//	default:
		//		break;
		//	}
		//}
	}
}