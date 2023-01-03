#pragma once
#include "IExamPlugin.h"
#include "Exam_HelperStructs.h"
#include "MyHelperStructs.h"

class IBaseInterface;
class IExamInterface;
class ISteeringBehavior;

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

	//UINT m_InventorySlot = 0; //put it in globals

	Elite::Vector2 m_NextTargetPos{};

	//Own Variables--------

	GlobalVariables m_Globals;

	//GOAP
	Planner* m_pPlanner;
	std::string m_CurrentState;
	std::string m_CurrentGoal;

	//Steering
	ISteeringBehavior* m_pSteeringBehavior;

	//PUT IN BEHAVIOR CLASS
	enum class AgentState
	{
		goInHouse,
		searchHouse,
		seekClosestItem,
		getOutHouse,
		lookForHouse
	};
	AgentState m_AgentState{ AgentState::lookForHouse };

	//Own Functions-------

	//void GoInHouse(const AgentInfo& agentInfo, HouseInfo& houseInfo, Elite::Vector2& nextTargetPos);
	//void GetClosestItem(const AgentInfo& agentInfo, Elite::Vector2& nextTargetPos);
	//void AgentBehavior();

	void ManagePriorities();

	bool ReachedTarget(const AgentInfo& agent, const Elite::Vector2& targetPos) const;
	bool IsNewHouse();
	bool IsNewItem();
	//Miscellanious
	//bool ReachedTarget(const AgentInfo& agent, const Elite::Vector2& targetPos) const;
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