#pragma once
class Action;
//class IExamInterface;
struct GlobalVariables;
struct State;

#include "EBlackboard.h"

class Planner final
{
public:
	Planner(Elite::Blackboard* pBlackboard, const std::vector<Action*> actions);
	~Planner();

	void Update(Elite::Blackboard* pBlackboard);

private:
	Elite::Blackboard* m_pBlackboard;

	struct Node
	{
		Action* pAction;
		bool isUsable = false;
	};

	std::vector<Action*> m_Actions;
	std::vector<std::vector<Action*>> m_PossiblePlans;
	std::vector<Action*> m_Plan;
	int m_CurrentAction = 0;
	State m_TempGoalState{"", false};

	void ExecutePlan(Elite::Blackboard* pBlackboard);
	void FindPlan(GlobalVariables& pGlobals);
	bool MakePlan(GlobalVariables& pGlobals, const State& parentState, std::vector<Node> usableActs); //intentional copy of the usableActs nodes
	void ManagePriorities(Elite::Blackboard* pBlackboard);

};

