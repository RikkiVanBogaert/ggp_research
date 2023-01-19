#pragma once
#include "IExamPlugin.h"
#include "Exam_HelperStructs.h"
#include "EBlackboard.h"
#include "Action.h"
#include "MyHelperStructs.h"
#include "SteeringBehavior.h"
#include "CombinedSteeringBehaviors.h"

class IBaseInterface;
class IExamInterface;
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

	//UINT m_InventorySlot = 0; // I put it in m_pGlobals

	Elite::Vector2 m_NextTargetPos{};

	//Own Variables--------
	GlobalVariables* m_pGlobals;
	Elite::Blackboard* m_pBlackboard;
	SteeringPlugin_Output* m_pSteering;
	std::vector<HouseInfo>* m_pHousesInFOV;
	std::vector<EntityInfo>* m_pEntitiesInFOV;

	//GOAP
	Planner* m_pPlanner;

	//Steering
	ISteeringBehavior* m_pSteeringBehavior;
	Seek* m_pSeek;
	Spin* m_pSpin;
	Flee* m_pFlee;
	Face* m_pFace;
	BlendedSteering* m_pBlendedSteeringSeekSpin;
	BlendedSteering* m_pBlendedSteeringFleeFace;
	BlendedSteering* m_pBlendedSteeringSeekFace; //only using seek caused some bugs going in and out spinning when switching to seek


	//Own Functions
	void UpdateSteering();
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
