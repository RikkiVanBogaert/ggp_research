#pragma once
struct SteeringPlugin_Output;
class IExamInterface;
struct GlobalVariables;
struct HouseInfo;
struct EntityInfo;
struct AgentInfo;

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
	virtual void ExecuteEvent(SteeringPlugin_Output& steering, IExamInterface* interfacePtr, GlobalVariables& globals,
		std::vector<HouseInfo>& houses, std::vector<EntityInfo>& entities) {};

	State GetPrecondition() const { return m_Precondition; }
	State GetEffect() const { return m_Effect; }
	bool GetActionEnded() const { return m_ActionEnd; }

protected:
	State m_Precondition;
	State m_Effect;
	int m_Cost;
	bool m_ActionEnd;

	bool ReachedTarget(const AgentInfo& agent, const Elite::Vector2& targetPos) const;
	void InventoryManagement(IExamInterface* interfacePtr);
};

//EXPLORATION LOGIC--------------------------------------------------------------
class Explore final : public Action
{
public:
	Explore();

	virtual void ExecuteEvent(SteeringPlugin_Output& steering, IExamInterface* interfacePtr, GlobalVariables& globals,
		std::vector<HouseInfo>& houses, std::vector<EntityInfo>& entities) override;

private:
	bool m_TopLeftExplored = false;
	bool m_TopRightExplored = false;
	bool m_BottomLeftExplored = false;
	bool m_BottomRightExplored = false;
};

class GoInHouse final : public Action
{
public:
	GoInHouse();

	virtual void ExecuteEvent(SteeringPlugin_Output& steering, IExamInterface* interfacePtr, GlobalVariables& globals,
		std::vector<HouseInfo>& houses, std::vector<EntityInfo>& entities) override;

};

class SearchHouse final : public Action
{
public:
	SearchHouse();

	virtual void ExecuteEvent(SteeringPlugin_Output& steering, IExamInterface* interfacePtr, GlobalVariables& globals,
		std::vector<HouseInfo>& houses, std::vector<EntityInfo>& entities) override;

private:
	bool m_SeekingTopHouse = false;
	bool m_SeekingBottomHouse = false;
};

class GoOutHouse final : public Action
{
public:
	GoOutHouse();

	virtual void ExecuteEvent(SteeringPlugin_Output& steering, IExamInterface* interfacePtr, GlobalVariables& globals,
		std::vector<HouseInfo>& houses, std::vector<EntityInfo>& entities) override;

};


//ITEM LOGIC----------------------------------------------------------------
class GetItemAction final : public Action
{
public:
	GetItemAction();

	virtual void ExecuteEvent(SteeringPlugin_Output& steering, IExamInterface* interfacePtr, GlobalVariables& globals,
		std::vector<HouseInfo>& houses, std::vector<EntityInfo>& entities) override;

};

class CheckFood final : public Action
{
public:
	CheckFood();

	virtual void ExecuteEvent(SteeringPlugin_Output& steering, IExamInterface* interfacePtr, GlobalVariables& globals,
		std::vector<HouseInfo>& houses, std::vector<EntityInfo>& entities) override;

};

class FindFood final : public Action
{
public:
	FindFood();

	virtual void ExecuteEvent(SteeringPlugin_Output& steering, IExamInterface* interfacePtr, GlobalVariables& globals,
		std::vector<HouseInfo>& houses, std::vector<EntityInfo>& entities) override;

};

class EatFood final : public Action
{
public:
	EatFood();

	virtual void ExecuteEvent(SteeringPlugin_Output& steering, IExamInterface* interfacePtr, GlobalVariables& globals,
		std::vector<HouseInfo>& houses, std::vector<EntityInfo>& entities) override;

};


//ENEMY LOGIC----------------------------------------------------------------
class CheckBullets final : public Action
{
public:
	CheckBullets();

	virtual void ExecuteEvent(SteeringPlugin_Output& steering, IExamInterface* interfacePtr, GlobalVariables& globals,
		std::vector<HouseInfo>& houses, std::vector<EntityInfo>& entities) override;

};

class RunAway final : public Action
{
public:
	RunAway();

	virtual void ExecuteEvent(SteeringPlugin_Output& steering, IExamInterface* interfacePtr, GlobalVariables& globals,
		std::vector<HouseInfo>& houses, std::vector<EntityInfo>& entities) override;

private:
	const float m_FleeRadius = 20.f;
	Elite::Vector2 m_FirstSeenEnemyPos{};
};

class KillEnemy final : public Action
{
public:
	KillEnemy();

	virtual void ExecuteEvent(SteeringPlugin_Output& steering, IExamInterface* interfacePtr, GlobalVariables& globals,
		std::vector<HouseInfo>& houses, std::vector<EntityInfo>& entities) override;

private:
	bool m_AimingAtTarget = false;

};

//PURGEZONE------------------------------
class RunFromPurgeZone final : public Action
{
public:
	RunFromPurgeZone();

	virtual void ExecuteEvent(SteeringPlugin_Output& steering, IExamInterface* interfacePtr, GlobalVariables& globals,
		std::vector<HouseInfo>& houses, std::vector<EntityInfo>& entities) override;

private:
	
};