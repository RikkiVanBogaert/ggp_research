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

	/*if (totalWeight > 0.f)
	{
		auto scale = 1.f / totalWeight;
		blendedSteering.LinearVelocity *= scale;
	}*/
	
	steering = blendedSteering;
	//steering.AngularVelocity = 10.f;
	//if (pAgent->CanRenderBehavior())
	//	DEBUGRENDERER2D->DrawDirection(pAgent->GetPosition(), blendedSteering.LinearVelocity, 7, { 0, 1, 1 }, 0.40f);

	//return blendedSteering;

}

////*****************
////PRIORITY STEERING
//SteeringOutput PrioritySteering::CalculateSteering(float deltaT, SteeringAgent* pAgent)
//{
//	SteeringOutput steering = {};
//
//	for (auto pBehavior : m_PriorityBehaviors)
//	{
//		steering = pBehavior->CalculateSteering(deltaT, pAgent);
//
//		if (steering.IsValid)
//			break;
//	}
//
//	//If non of the behavior return a valid output, last behavior is returned
//	return steering;
//}