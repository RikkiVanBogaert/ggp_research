#include "stdafx.h"
#include "CombinedSteeringBehaviors.h"


BlendedSteering::BlendedSteering(std::vector<WeightedBehavior> weightedBehaviors)
	:m_WeightedBehaviors(weightedBehaviors)
{
};

//****************
//BLENDED STEERING
void BlendedSteering::CalculateSteering(SteeringPlugin_Output& steering, const AgentInfo& agentInfo)
{
	SteeringPlugin_Output blendedSteering = {};
	auto totalWeight = 0.f;

	for (auto weightedBehavior : m_WeightedBehaviors)
	{
		SteeringPlugin_Output permSteering{};
		weightedBehavior.pBehavior->SetTarget(m_Target);
		weightedBehavior.pBehavior->CalculateSteering(permSteering, agentInfo);
		blendedSteering.LinearVelocity += weightedBehavior.weight * permSteering.LinearVelocity;
		blendedSteering.AngularVelocity += weightedBehavior.weight * permSteering.AngularVelocity;
		blendedSteering.AutoOrient = permSteering.AutoOrient;

		totalWeight += weightedBehavior.weight;
	}

	//we dont use this, so we can use full angular velocity from one behavior, and full linear vel from another
	/*if (totalWeight > 0.f)
	{
		auto scale = 1.f / totalWeight;
		blendedSteering.LinearVelocity *= scale;
	}*/
	
	steering = blendedSteering;

}
