#pragma once
#include "stdafx.h"
#include "Action.h"
#include "Planner.h"
#include "MyHelperStructs.h"

#include "IExamInterface.h"
#ifndef IEXAMINTERFACE_H
#define IEXAMINTERFACE_H

Planner::Planner(const std::vector<Action*> actions)
{
	m_Actions = actions;
}

void Planner::FindPlan(GlobalVariables& globals)
{
	//MakePlan(globals, currentState, globals.currentEntityState);

	if (globals.goalState != m_TempGoalState)
	{
		globals.currentState = { "Neutral", true };
		m_TempGoalState = globals.goalState;
	}

	m_Plan.clear();
	m_PossiblePlans.clear();
	//still have to use possiblePlans to find cheapest plan

	std::vector<Node> nodes;
	for (Action* a : m_Actions)
	{
		nodes.push_back({ a, false });
	}
	MakePlan2(globals, globals.goalState, nodes);


	//std::reverse(m_Plan.begin(), m_Plan.end());

	//m_TempStates = m_Plan[0]->GetPrecondition();
	//std::cout << "FOUND PLAN\n";
}

bool Planner::MakePlan(GlobalVariables& globals, std::vector<Action*>& usableActions)
{
	State curState = globals.goalState;
	m_Plan.clear();

	//FIND CHEAPEST PLAN still make that
	findNextAction:
	for (size_t i{}; i < usableActions.size(); ++i)
	{
		if (curState == globals.currentState)
			return true; //then plan is completed

		if (usableActions[i]->GetEffect() == curState)
		{
			m_Plan.push_back(usableActions[i]);
			curState = usableActions[i]->GetPrecondition();
			RemoveItemVec(usableActions, i);
			goto findNextAction;
		}

	}
	//MakePlan(globals, usableActions);
	return false;
	//plan done
}

bool Planner::MakePlan2(GlobalVariables& globals, const State& parentState, const std::vector<Node>& usableActs)
{
	//maybe do it via using copy parameter on usableActs instead of putting them in a new nodes vector
	std::vector<Node> nodes{};
	for (Node a : usableActs) //Add all the usableActions into nodes
	{
		nodes.push_back(a);
	}

	for (Node& n : nodes) //Check if the effect of the node, is the same as the precondition of the next node
	{
		if (n.pAction->GetEffect() == parentState)
			n.isUsable = true;
	}

	if (nodes.size() == 0) //if there are no other usableNodes, return false
		return false;

	std::vector<Node> usableNodes;
	for (size_t i{}; i < nodes.size(); ++i) //if its not, then remove them from the vector
	{
		if (nodes[i].isUsable)
			usableNodes.push_back(nodes[i]);
	}
	for (size_t i{}; i < usableNodes.size(); ++i)
	{
		if (usableNodes[i].pAction->GetPrecondition() == globals.currentState)
		{
			m_Plan.push_back(usableNodes[i].pAction);
			return true; //we found the last node in the plan
		}
		else
		{
			std::vector<Node> nodesWithoutCurrent;
			for (Node n : nodes)
			{
				if(n.pAction != usableNodes[i].pAction)
					nodesWithoutCurrent.push_back(n);
			}

			//we dont want to add the node we just used, into the node pool again so we make a new vec without the currentNode
			if (MakePlan2(globals, usableNodes[i].pAction->GetPrecondition(), nodesWithoutCurrent))
			{
				m_Plan.push_back(usableNodes[i].pAction);
			}
		}
	}

	return false;
}

void Planner::ExecutePlan(SteeringPlugin_Output& steering, IExamInterface* interfacePtr, GlobalVariables& globals,
	std::vector<HouseInfo>& houses, std::vector<EntityInfo>& entities)
{

	if(m_Plan.size() > 0)
	{
		//if (!m_Plan[m_CurrentAction]->ConditionMet(globals.currentHouseState)) //if next action doesnt meet condition anymore, find new plan
			//FindPlan(globals);

		m_Plan[0]->ExecuteEvent(steering, interfacePtr, globals, houses, entities);

		if (m_Plan[0]->GetActionEnded())
		{
			//If a state changes, we need to check if a higher priority goal now became available by trying to make a new plan
			FindPlan(globals);
		}
	}
	else
	{
		std::cout << "NO PLAN YET\n";
		FindPlan(globals);
	}

	if (globals.currentState == globals.goalState)
	{
		std::cout << "GOAL IS REACHED\n";
		globals.currentState = { "Neutral", true };
	}

	ManageInventory(globals, interfacePtr);
}

void Planner::ManageInventory(GlobalVariables& globals, IExamInterface* interfacePtr)
{
	//function not in actions because it should act seperatly from doing normal actions

	auto agentInfo = interfacePtr->Agent_GetInfo();

	//if (agentInfo.Energy <= 3 && globals.m_HasFood)
	//{
	//	interfacePtr->Inventory_UseItem(globals.inventory["Food"]);

	//	ItemInfo food; //if item is now 0 after using, discard it
	//	interfacePtr->Inventory_GetItem(globals.inventory["Food"], food);
	//	if (interfacePtr->Food_GetEnergy(food) <= 0)
	//	{
	//		interfacePtr->Inventory_RemoveItem(globals.inventory["Food"]);
	//		globals.m_HasFood = false;
	//	}
	//}

	if (agentInfo.Health < 10 && globals.m_HasMedkit)
	{
		interfacePtr->Inventory_UseItem(globals.inventory["Medkit"]);

		ItemInfo medkit; //if item is now 0 after using, discard it
		interfacePtr->Inventory_GetItem(globals.inventory["Medkit"], medkit);
		if (interfacePtr->Medkit_GetHealth(medkit) <= 0)
		{
			interfacePtr->Inventory_RemoveItem(globals.inventory["Medkit"]);
			globals.m_HasMedkit = false;
		}
	}

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