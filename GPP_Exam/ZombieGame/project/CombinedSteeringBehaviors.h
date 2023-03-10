#pragma once
#include "Exam_HelperStructs.h"
#include "SteeringBehavior.h"


//****************
//BLENDED STEERING
class BlendedSteering final : public ISteeringBehavior
{
public:
	struct WeightedBehavior
	{
		ISteeringBehavior* pBehavior = nullptr;
		float weight = 0.f;

		WeightedBehavior(ISteeringBehavior* pBehavior, float weight) :
			pBehavior(pBehavior),
			weight(weight)
		{};
	};

	BlendedSteering(std::vector<WeightedBehavior> weightedBehaviors);

	void AddBehaviour(WeightedBehavior weightedBehavior) { m_WeightedBehaviors.push_back(weightedBehavior); }
	void CalculateSteering(SteeringPlugin_Output& steering, const AgentInfo& agentInfo) override;

	// returns a reference to the weighted behaviors, can be used to adjust weighting. Is not intended to alter the behaviors themselves.
	std::vector<WeightedBehavior>& GetWeightedBehaviorsRef() { return m_WeightedBehaviors; }

private:
	std::vector<WeightedBehavior> m_WeightedBehaviors = {};

	using ISteeringBehavior::SetTarget; // made private because targets need to be set on the individual behaviors, not the combined behavior
};