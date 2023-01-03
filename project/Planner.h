#pragma once
class Action;
//class IExamInterface;
struct GlobalVariables;
struct State;

class Planner
{
public:
	Planner(const std::vector<Action*> actions);

	void FindPlan(GlobalVariables& globals);
	void ExecutePlan(SteeringPlugin_Output& steering, IExamInterface* interfacePtr, GlobalVariables& globals,
		std::vector<HouseInfo>& houses, std::vector<EntityInfo>& entities);

private:

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

	void RemoveItemVec(std::vector<Action*>& vec, int index);
	std::vector<Node> Planner::VecWithoutItem(std::vector<Node> vec, int index);

	bool MakePlan(GlobalVariables& globals, std::vector<Action*>& usableActs);
	bool MakePlan2(GlobalVariables& globals, const State& parentState, const std::vector<Node>& usableActs);
	void ManageInventory(GlobalVariables& globals, IExamInterface* interfacePtr);
};

