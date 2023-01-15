#pragma once
#include "IExamPlugin.h"
#include "Exam_HelperStructs.h"
#include "EBlackboard.h"
#include "Action.h"
#include "MyHelperStructs.h"
//#include "MyHelperFunctions.h"
#include "SteeringBehavior.h"
#include "CombinedSteeringBehaviors.h"

class IBaseInterface;
class IExamInterface;
//class ISteeringBehavior;
class GlobalVariables;

class Planner;

class Plugin :public IExamPlugin
{
public:
	Plugin() {};
	virtual ~Plugin() {};

	void Initialize(IBaseInterface* pInterface, PluginInfo& info) override;
	void DllInit() override;
	void DllShutdown() override;

	void InitGameDebugParams(GameDebugParams& params) override;
	void Update(float dt) override;

	SteeringPlugin_Output UpdateSteering(float dt) override;
	void Render(float dt) const override;

private:
	//TESTING
	int tempTakenSlots = 0;

	//Interface, used to request data from/perform actions with the AI Framework
	IExamInterface* m_pInterface = nullptr;
	std::vector<HouseInfo> GetHousesInFOV() const;
	std::vector<EntityInfo> GetEntitiesInFOV() const;

	Elite::Vector2 m_Target = {};
	bool m_CanRun = false; //Demo purpose
	bool m_GrabItem = false; //Demo purpose
	bool m_UseItem = false; //Demo purpose
	bool m_RemoveItem = false; //Demo purpose
	float m_AngSpeed = 0.f; //Demo purpose

	//UINT m_InventorySlot = 0; //put it in pGlobals->

	Elite::Vector2 m_NextTargetPos{};

	//Own Variables--------
	GlobalVariables* m_pGlobals;
	Elite::Blackboard* m_pBlackboard;
	State* m_pCurrentState;
	State* m_pGoalState;
	SteeringState* m_pSteeringState;
	SteeringPlugin_Output* m_pSteering;
	std::vector<HouseInfo>* m_pHousesInFOV;
	std::vector<EntityInfo>* m_pEntitiesInFOV;
	//ItemInfo* m_pNearbyItem;

	//GOAP
	Planner* m_pPlanner;
	std::string m_CurrentState;
	std::string m_CurrentGoal;

	//Steering
	ISteeringBehavior* m_pSteeringBehavior;
	Seek* m_pSeek;
	Spin* m_pSpin;
	Flee* m_pFlee;
	Face* m_pFace;
	BlendedSteering* m_pBlendedSteeringSeekSpin;
	BlendedSteering* m_pBlendedSteeringFleeFace;
	BlendedSteering* m_pBlendedSteeringSeekFace; //just using seek, caused some bugs going in and out spinning

	//Own Functions-------
	bool ReachedTarget(const AgentInfo& agent, const Elite::Vector2& targetPos) const;
};

//ENTRY
//This is the first function that is called by the host program
//The plugin returned by this function is also the plugin used by the host program
extern "C"
{
	__declspec (dllexport) IPluginBase* Register()
	{
		return new Plugin();
	}
}
