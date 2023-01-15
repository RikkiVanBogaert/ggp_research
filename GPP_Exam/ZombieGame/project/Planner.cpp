#pragma once
#include "stdafx.h"
#include "Action.h"
#include "Planner.h"
#include "MyHelperStructs.h"
#include "MyHelperFunctions.h"

#include "IExamInterface.h"
#ifndef IEXAMINTERFACE_H
#define IEXAMINTERFACE_H

Planner::Planner(Elite::Blackboard* pBlackboard, const std::vector<Action*> actions):
	m_pBlackboard{pBlackboard}
{
	m_Actions = actions;
}

Planner::~Planner()
{
	SAFE_DELETE(m_pBlackboard);
}

void Planner::Update(Elite::Blackboard* pBlackboard)
{
	ManagePriorities(pBlackboard);
	ExecutePlan(pBlackboard);

}

void Planner::FindPlan(GlobalVariables& pGlobals)
{

	if (pGlobals.goalState != m_TempGoalState)
	{
		//if goalState changed, we reset the currentState to neutral
		pGlobals.currentState = { "Neutral", true };
		m_TempGoalState = pGlobals.goalState;
	}

	m_Plan.clear();
	m_PossiblePlans.clear();
	//still have to use possiblePlans to find cheapest plan

	std::vector<Node> nodes;
	for (Action* a : m_Actions)
	{
		nodes.push_back({ a, false });
	}
	MakePlan2(pGlobals, pGlobals.goalState, nodes);


	//std::reverse(m_Plan.begin(), m_Plan.end());

	//m_TempStates = m_Plan[0]->GetPrecondition();
	//std::cout << "FOUND PLAN\n";
}

bool Planner::MakePlan2(GlobalVariables& pGlobals, const State& parentState, std::vector<Node> usableActs)
{
	////maybe do it via using copy parameter on usableActs instead of putting them in a new nodes vector
	//std::vector<Node> nodes{};
	//for (Node a : usableActs) //Add all the usableActions into nodes
	//{
	//	nodes.push_back(a);
	//}
	//for (Node& n : nodes) //Check if the effect of the node, is the same as the precondition of the next node
	//{
	//	if (n.pAction->GetEffect() == parentState)
	//		n.isUsable = true;
	//}
	//if (nodes.size() == 0) //if there are no other usableNodes, return false
	//	return false;
	//std::vector<Node> usableNodes;
	//for (size_t i{}; i < nodes.size(); ++i) //if its not, then remove them from the vector
	//{
	//	if (nodes[i].isUsable)
	//		usableNodes.push_back(nodes[i]);
	//}
	//for (size_t i{}; i < usableNodes.size(); ++i)
	//{
	//	if (usableNodes[i].pAction->GetPrecondition() == pGlobals.currentState)
	//	{
	//		m_Plan.push_back(usableNodes[i].pAction);
	//		return true; //we found the last node in the plan
	//	}
	//	else
	//	{
	//		std::vector<Node> nodesWithoutCurrent;
	//		for (Node n : nodes)
	//		{
	//			if(n.pAction != usableNodes[i].pAction)
	//				nodesWithoutCurrent.push_back(n);
	//		}
	//		//we dont want to add the node we just used, into the node pool again so we make a new vec without the currentNode
	//		if (MakePlan2(pGlobals, usableNodes[i].pAction->GetPrecondition(), nodesWithoutCurrent))
	//		{
	//			m_Plan.push_back(usableNodes[i].pAction);
	//		}
	//	}
	//}
	//return false;

	std::vector<Node> usableNodes;
	for (Node& n : usableActs) //Check if the effect of the node, is the same as the precondition of the next node
	{
		if (n.pAction->GetEffect() == parentState)
			usableNodes.push_back(n);
	}

	for (size_t i{}; i < usableNodes.size(); ++i)
	{
		if (usableNodes[i].pAction->GetPrecondition() == pGlobals.currentState)
		{
			m_Plan.push_back(usableNodes[i].pAction);
			return true; //we found the last node in the plan
		}
		else
		{
			std::vector<Node> nodesWithoutCurrent;
			for (Node n : usableActs)
			{
				if (n.pAction != usableNodes[i].pAction)
					nodesWithoutCurrent.push_back(n);
			}

			//we dont want to add the node we just used, into the node pool again so we make a new vec without the currentNode
			if (MakePlan2(pGlobals, usableNodes[i].pAction->GetPrecondition(), nodesWithoutCurrent))
			{
				m_Plan.push_back(usableNodes[i].pAction);
			}
		}
	}

	return false;
}

void Planner::ExecutePlan(Elite::Blackboard* pBlackboard)
{
	GlobalVariables* pGlobals;
	if (!pBlackboard->GetData("Globals", pGlobals) || pGlobals == nullptr)
		return;

	IExamInterface* pInterface;
	if (!pBlackboard->GetData("Interface", pInterface) || pInterface == nullptr)
		return;

	if(m_Plan.size() > 0)
	{
		//if (!m_Plan[m_CurrentAction]->ConditionMet(pGlobals->currentHouseState)) //if next action doesnt meet condition anymore, find new plan
			//FindPlan(pGlobals->;

		m_Plan[0]->ExecuteEvent(pBlackboard);

		if (pGlobals->actionEnd)
		{
			//If a state changes, we need to check if a higher priority goal now became available by trying to make a new plan
			FindPlan(*pGlobals);
			pGlobals->actionEnd = false;
		}
	}
	else
	{
		std::cout << "NO PLAN YET\n";
		FindPlan(*pGlobals);
	}

	if (pGlobals->currentState.stateString == pGlobals->goalState.stateString)
	{
		std::cout << "GOAL IS REACHED\n";
		pGlobals->currentState = { "Neutral", true };
	}

	ManageInventory(*pGlobals, pInterface);
}

void Planner::ManagePriorities(Elite::Blackboard* pBlackboard)
{
	GlobalVariables* pGlobals;
	if (!m_pBlackboard->GetData("Globals", pGlobals) || pGlobals == nullptr)
		return;

	IExamInterface* pInterface;
	if (!pBlackboard->GetData("Interface", pInterface) || pInterface == nullptr)
		return;

	std::vector<EntityInfo>* pEntities;
	if (!pBlackboard->GetData("EntitiesInFOV", pEntities))
		return;

	//std::cout << pGlobals->currentState.stateString << "   " << pGlobals->goalState.stateString << '\n';
	//std::cout << pGlobals->goalPosition.x << "  " << pGlobals->goalPosition.y << '\n';

	auto agentInfo = pInterface->Agent_GetInfo();

	if (PurgeZoneInFOV(pBlackboard))
	{
		//loop through all entities in FOV and if any of them is a zone, do zone,
		//if any is an enemy, do enemy.....etc
		pInterface->PurgeZone_GetInfo((*pEntities)[0], pGlobals->purgeZone);
		pGlobals->goalState = { "EscapePurgeZone", true };
		FindPlan(*pGlobals);
	}
	else if (agentInfo.Energy < 4.f && HasItem(m_pBlackboard, eItemType::FOOD))
	{
		pGlobals->goalState = { "HasEaten", true };
		FindPlan(*pGlobals);
	}
	else if (agentInfo.Health < 8.f && HasItem(m_pBlackboard, eItemType::MEDKIT))
	{
		pGlobals->goalState = { "HasHealed", true };
		FindPlan(*pGlobals);
	}
	else if (pEntities->size() > 0 && (*pEntities)[0].Type == eEntityType::ENEMY &&
		(HasItem(m_pBlackboard, eItemType::PISTOL) || HasItem(m_pBlackboard, eItemType::SHOTGUN)))
	{
		pGlobals->goalState = { "DealWithEnemy", true };
		FindPlan(*pGlobals);
	}
	else if (pEntities->size() > 0 && (*pEntities)[0].Type == eEntityType::ITEM)
	{
		pGlobals->goalState = { "GetItem", true };
		FindPlan(*pGlobals);
	}
	else if (IsNewHouse(pBlackboard))
	{
		pGlobals->goalState = { "OutOfHouse", true };
		FindPlan(*pGlobals);
	}
	else
	{
		pGlobals->currentState = { "Neutral", true };
		pGlobals->goalState = { "Explore", true };
	}
}

void Planner::ManageInventory(GlobalVariables& pGlobals, IExamInterface* interfacePtr)
{
	//function not in actions because it should act seperatly from doing normal actions

	auto agentInfo = interfacePtr->Agent_GetInfo();
}

void Planner::RemoveItemVec(std::vector<Action*>& vec, int index)
{
	vec[index] = vec[vec.size() - 1];
	vec.pop_back();
}

std::vector<Planner::Node> Planner::VecWithoutItem(std::vector<Node> vec, int index)
{

	vec[index] = vec[vec.size() - 1];
	vec.pop_back();

	return vec;
}

#endif