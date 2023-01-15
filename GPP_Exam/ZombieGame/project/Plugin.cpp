#include "stdafx.h"
#include "Plugin.h"
#include "IExamInterface.h"
#include "Planner.h"
//#include "SteeringBehavior.h"
//#include "CombinedSteeringBehaviors.h"
//#include "Action.h"
//#include "MyHelperStructs.h"
//#include "MyHelperFunctions.h"
//TEST GIT


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
	//m_AngSpeed = 10.f;
	m_pSeek = new Seek();
	m_pSpin = new Spin();
	m_pFlee = new Flee();
	m_pFace = new Face();
	m_pBlendedSteeringSeekSpin = new BlendedSteering({ {m_pSeek, 1.f},  {m_pSpin, 1.f} });
	m_pBlendedSteeringFleeFace = new BlendedSteering({ {m_pFlee, 1.f},  {m_pFace, 1.f} });
	m_pBlendedSteeringSeekFace = new BlendedSteering({ {m_pSeek, 1.f},  {m_pFace, 1.f} });

	m_pSteeringBehavior = m_pSeek;

	//GlobalVariables globals;
	m_pGlobals = new GlobalVariables{};
	m_pGlobals->currentState = { "Neutral", true };
	m_pGlobals->goalState = { "Explore", true };
	m_pGlobals->steeringState = SteeringState::Seek;
	m_pGlobals->beginRadius = m_pInterface->World_GetInfo().Dimensions.x / 21.f;
	m_pGlobals->minRadius = m_pInterface->World_GetInfo().Dimensions.x / 4.5f;
	m_pGlobals->maxRadius = m_pInterface->World_GetInfo().Dimensions.x / 4.f;

	//initialize inventory slots
	//for (int i{}; i < m_pInterface->Inventory_GetCapacity(); ++i)
	//{
	//	m_pGlobals->inventorySlots[i] = false;
	//}


	//Blackboard
	m_pBlackboard = new Elite::Blackboard();
	m_pBlackboard->AddData("Interface", m_pInterface);
	m_pBlackboard->AddData("Globals", m_pGlobals);
	m_pSteering = new SteeringPlugin_Output();
	m_pBlackboard->AddData("Steering", m_pSteering);
	m_pHousesInFOV = new std::vector<HouseInfo>();
	m_pBlackboard->AddData("HousesInFOV", m_pHousesInFOV);
	m_pEntitiesInFOV = new std::vector<EntityInfo>();
	m_pBlackboard->AddData("EntitiesInFOV", m_pEntitiesInFOV);
	m_pBlackboard->AddData("SteeringBehavior", m_pSteeringBehavior);
	m_pBlackboard->AddData("BlendedSeekSpin", m_pBlendedSteeringSeekSpin);
	m_pBlackboard->AddData("Seek", m_pSeek);

	//GOAP
	std::vector<Action*> actions
	{
		//new Explore(),
		//new ExploreSpiral(),
		new ExploreCircle(),

		new GoInHouse(),
		new SearchHouse(),
		new GoOutHouse(),

		new GetItemAction(),

		//new CheckFood(),
		//new FindFood(),
		new EatFood(),
		new Heal(),

		new CheckBullets(),
		//new RunAway(),
		new KillEnemy(),

		new RunFromPurgeZone()
	};
	m_pPlanner = new Planner(m_pBlackboard, actions);
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
	//delete m_pBlackboard;
}

//Called only once, during initialization
void Plugin::InitGameDebugParams(GameDebugParams& params)
{
	params.AutoFollowCam = true; //Automatically follow the AI? (Default = true)
	params.RenderUI = true; //Render the IMGUI Panel? (Default = true)
	params.SpawnEnemies = false; //Do you want to spawn enemies? (Default = true)
	params.EnemyCount = 20; //How many enemies? (Default = 20)
	params.GodMode = true; //GodMode > You can't die, can be useful to inspect certain behaviors (Default = false)
	params.LevelFile = "GameLevel.gppl";
	params.AutoGrabClosestItem = false; //A call to Item_Grab(...) returns the closest item that can be grabbed. (EntityInfo argument is ignored)
	params.StartingDifficultyStage = 1;
	params.InfiniteStamina = false;
	params.SpawnDebugPistol = true;
	params.SpawnDebugShotgun = true;
	params.SpawnPurgeZonesOnMiddleClick = true;
	params.PrintDebugMessages = true;
	params.ShowDebugItemNames = true;
	params.Seed = 2; //36 is og
	//11 has weird navmesh glitch or something
}

//Only Active in DEBUG Mode
//(=Use only for Debug Purposes)
//WILL NOT BE CALLED IN RELEASE IS ONLY FOR DEBUG
void Plugin::Update(float dt)
{
	GlobalVariables* pGlobals;
	if(!m_pBlackboard->GetData("Globals", pGlobals) || pGlobals == nullptr)
		return;

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
		if (pGlobals->takenSlots > 0);
			//--pGlobals->takenSlots;
	}
	else if (m_pInterface->Input_IsKeyboardKeyDown(Elite::eScancode_KP_Plus))
	{
		if (pGlobals->takenSlots < 4);
			//++pGlobals->takenSlots;
	}
	else if (m_pInterface->Input_IsKeyboardKeyDown(Elite::eScancode_Q))
	{
		ItemInfo info = {};
		m_pInterface->Inventory_GetItem(pGlobals->takenSlots, info);
		std::cout << (int)info.Type << std::endl;
	}
}

//Update
//This function calculates the new SteeringOutput, called once per frame
SteeringPlugin_Output Plugin::UpdateSteering(float dt)
{
	//TESTING
	

	//auto steering = SteeringPlugin_Output();

	GlobalVariables* pGlobals;
	if (!m_pBlackboard->GetData("Globals", pGlobals) || pGlobals == nullptr)
		return *m_pSteering;


	//Use the Interface (IAssignmentInterface) to 'interface' with the AI_Framework
	auto agentInfo = m_pInterface->Agent_GetInfo();


	//this will put the info in the blackboard
	*m_pHousesInFOV = GetHousesInFOV(); 
	*m_pEntitiesInFOV = GetEntitiesInFOV();

	m_pPlanner->Update(m_pBlackboard);

	//std::cout << m_pGlobals->currentState.stateString << ' ' << m_pGlobals->goalState.stateString << "\n";
	//std::cout << m_pEntitiesInFOV->size() << "\n";

	if (Distance(m_NextTargetPos, agentInfo.Position) < 2.f)
	{
		m_pSteering->LinearVelocity = Elite::ZeroVector2;
	}

	switch (pGlobals->steeringState)
	{
	case SteeringState::Seek:
		m_pSteering->RunMode = false;
		m_pSteeringBehavior = m_pBlendedSteeringSeekFace;
		break;
	case SteeringState::Flee:
		m_pSteering->RunMode = true;
		m_pSteeringBehavior = m_pSeek;
		//use seek because otherwise he gets stuck in houses since fleeing from a point doesnt use the navmesh (used for escaping purgezone)
		break;
	case SteeringState::Face:
		m_pSteering->RunMode = false;
		m_pSteeringBehavior = m_pFace;
		break;
	case SteeringState::SeekWhileSpinning:
		m_pSteering->RunMode = false;
		m_pSteeringBehavior = m_pBlendedSteeringSeekSpin;
		break;
	case SteeringState::FaceAndFlee:
		m_pSteering->RunMode = false;
		m_pSteeringBehavior = m_pBlendedSteeringFleeFace;
		break;
	default:
		break;
	}

	//std::cout << pGlobals->takenSlots << '\n';
	//SteeringPlugin_Output is works the exact same way a SteeringBehaviour output

//@End (Demo Purposes)
	m_GrabItem = false; //Reset State
	m_UseItem = false;
	m_RemoveItem = false;


	//SteeringPlugin_Output* pSteering{};
	m_NextTargetPos = m_pInterface->NavMesh_GetClosestPathPoint(m_pGlobals->goalPosition);
	m_pSteeringBehavior->SetTarget(m_NextTargetPos);
	m_pSteeringBehavior->CalculateSteering(*m_pSteering, agentInfo);


	return *m_pSteering;
}

//This function should only be used for rendering debug elements
void Plugin::Render(float dt) const
{
	//This Render function should only contain calls to Interface->Draw_... functions

	m_pInterface->Draw_SolidCircle(m_Target, .7f, { 0,0 }, { 1, 0, 0 });

	//DebugRendering
	//std::cout << m_pGlobals->goalPosition.x << "  " << m_pGlobals->goalPosition.y << '\n';
	m_pInterface->Draw_Point(m_NextTargetPos, 5, { 1,1,1 });
	for (const HouseInfo& h : m_pGlobals->visitedHouses)
	{
		m_pInterface->Draw_Point(h.Center, 10, {0, 0, 1});
	}

	for (const HouseInfo& h : m_pGlobals->seenHouses)
	{
		m_pInterface->Draw_Point(h.Center, 10, { 1, 0, 0 });
	}

	m_pInterface->Draw_Point(m_pGlobals->currentHouse.Center, 15, { 0, 1, 0 });

	//map circles
	//m_pInterface->Draw_Circle(m_pInterface->World_GetInfo().Center, m_pGlobals->beginRadius, { 0, 1, 0 });
	//m_pInterface->Draw_Circle(m_pInterface->World_GetInfo().Center, m_pGlobals->minRadius, { 1, 0, 1 });
	m_pInterface->Draw_Circle(m_pInterface->World_GetInfo().Center, m_pGlobals->maxRadius, { 1, 0, 0 });

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

bool Plugin::ReachedTarget(const AgentInfo& agent, const Elite::Vector2& targetPos) const
{
	if (agent.Position.Distance(targetPos) <= agent.GrabRange)
		return true;

	return false;
}
