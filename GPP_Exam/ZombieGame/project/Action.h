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
	std::string stateString = {};
	bool stateBool = false; 

	bool operator==(const State& STATE) const;
	bool operator!=(const State& STATE) const;
};

class Action
{
public:
	Action(const State& precondition, const State& effect, int cost = 1);

	bool ConditionMet(const State& currentState);
	virtual void ExecuteEvent(Elite::Blackboard* pBlackboard) {};

	State GetPrecondition() const { return m_Precondition; }
	State GetEffect() const { return m_Effect; }

protected:
	State m_Precondition;
	State m_Effect;
	int m_Cost;

	bool ReachedTarget(const AgentInfo& agent, const Elite::Vector2& targetPos) const;
};

class ExploreCircle final : public Action
{
public:
	ExploreCircle();

	virtual void ExecuteEvent(Elite::Blackboard* pBlackboard) override;

private:
	float m_Angle = 0;
};

//HOUSE LOGIC--------------------------------------------------------------
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

};

//ITEM LOGIC----------------------------------------------------------------
class GetItemAction final : public Action
{
public:
	GetItemAction();

	virtual void ExecuteEvent(Elite::Blackboard* pBlackboard) override;

};

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

class KillEnemy final : public Action
{
public:
	KillEnemy();

	virtual void ExecuteEvent(Elite::Blackboard* pBlackboard) override;

private:
	bool m_AimingAtTarget = false;

};

//PURGEZONE LOGIC------------------------------
class RunFromPurgeZone final : public Action
{
public:
	RunFromPurgeZone();

	virtual void ExecuteEvent(Elite::Blackboard* pBlackboard) override;

private:
	
};

#endif