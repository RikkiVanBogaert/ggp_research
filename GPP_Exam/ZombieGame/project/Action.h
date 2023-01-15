#pragma once
struct SteeringPlugin_Output;
class IExamInterface;
struct GlobalVariables;
struct HouseInfo;
struct EntityInfo;
struct AgentInfo;

#include "Exam_HelperStructs.h"


#include "EBlackboard.h"
#ifndef EBLACKBOARD_H
#define EBLACKBOARD_H

struct State
{
	State(std::string string, bool ean) : stateString{ string }, stateBool{ ean } {}
	State() {}
	std::string stateString = {};
	bool stateBool = false; 

	bool operator==(const State& STATE) const;
	bool operator!=(const State& STATE) const;
};

//helperFunctions
//void FindItemSlot(const eItemType& type, int& slot, 
	//const GlobalVariables& pGlobals, IExamInterface& pInterface, ItemInfo& item);
//bool HasItemA(Elite::Blackboard* pBlackboard, const eItemType& type);
//bool NeedsItemA(Elite::Blackboard* pBlackboard);
//bool GiveDuplicateSpot(Elite::Blackboard* pBlackboard, int& spot);


class Action
{
public:
	Action(const State& precondition, const State& effect, int cost = 1);

	bool ConditionMet(const State& currentState);
	virtual void ExecuteEvent(Elite::Blackboard* pBlackboard) {};

	State GetPrecondition() const { return m_Precondition; }
	State GetEffect() const { return m_Effect; }
	//bool GetActionEnded() const { return m_ActionEnd; }

protected:
	State m_Precondition;
	State m_Effect;
	int m_Cost;

	bool ReachedTarget(const AgentInfo& agent, const Elite::Vector2& targetPos) const;
};

//EXPLORATION LOGIC--------------------------------------------------------------
class Explore final : public Action
{
public:
	Explore();

	virtual void ExecuteEvent(Elite::Blackboard* pBlackboard) override;

private:
	bool m_TopLeftExplored = false;
	bool m_TopMidExplored = false;
	bool m_TopRightExplored = false;
	bool m_BottomLeftExplored = false;
	bool m_BottomRightExplored = false;
};

class ExploreSpiral final : public Action
{
public:
	ExploreSpiral();

	virtual void ExecuteEvent(Elite::Blackboard* pBlackboard) override;

private:
	float m_Angle = 0;
	float m_Radius = 0;
	float m_SearchRadius = 0;
	int m_Steps = 16;

	bool m_GoingOut = true;
};

class ExploreCircle final : public Action
{
public:
	ExploreCircle();

	virtual void ExecuteEvent(Elite::Blackboard* pBlackboard) override;

private:
	float m_Angle = 0;
	float m_SearchRadius = 0;
	int m_CircleState = 0;
};

class GoInHouse final : public Action
{
public:
	GoInHouse();

	virtual void ExecuteEvent(Elite::Blackboard* pBlackboard) override;

};

class SearchHouse final : public Action
{
public:
	SearchHouse();

	virtual void ExecuteEvent(Elite::Blackboard* pBlackboard) override;

private:
	bool m_SeekingTopHouse = false;
	bool m_SeekingBottomHouse = false;

	bool m_TopChecked = false;
	bool m_BottomChecked = false;
};

class GoOutHouse final : public Action
{
public:
	GoOutHouse();

	virtual void ExecuteEvent(Elite::Blackboard* pBlackboard) override;

};


//ITEM LOGIC----------------------------------------------------------------
class GetItemAction final : public Action
{
public:
	GetItemAction();

	virtual void ExecuteEvent(Elite::Blackboard* pBlackboard) override;

};

//class CheckFood final : public Action
//{
//public:
//	CheckFood();
//
//	virtual void ExecuteEvent(Elite::Blackboard* pBlackboard) override;
//
//};
//
//class FindFood final : public Action
//{
//public:
//	FindFood();
//
//	virtual void ExecuteEvent(Elite::Blackboard* pBlackboard) override;
//
//};

class EatFood final : public Action
{
public:
	EatFood();

	virtual void ExecuteEvent(Elite::Blackboard* pBlackboard) override;

};

class Heal final : public Action
{
public:
	Heal();

	virtual void ExecuteEvent(Elite::Blackboard* pBlackboard) override;

};

//ENEMY LOGIC----------------------------------------------------------------
class CheckBullets final : public Action
{
public:
	CheckBullets();

	virtual void ExecuteEvent(Elite::Blackboard* pBlackboard) override;

};

class RunAway final : public Action
{
public:
	RunAway();

	virtual void ExecuteEvent(Elite::Blackboard* pBlackboard) override;

private:
	const float m_FleeRadius = 20.f;
	Elite::Vector2 m_FirstSeenEnemyPos{};
};

class KillEnemy final : public Action
{
public:
	KillEnemy();

	virtual void ExecuteEvent(Elite::Blackboard* pBlackboard) override;

private:
	bool m_AimingAtTarget = false;

};

//PURGEZONE------------------------------
class RunFromPurgeZone final : public Action
{
public:
	RunFromPurgeZone();

	virtual void ExecuteEvent(Elite::Blackboard* pBlackboard) override;

private:
	
};

#endif