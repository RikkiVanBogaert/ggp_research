#include "stdafx.h"
#include "SteeringBehavior.h"

using namespace Elite;
//SEEK
//****
void Seek::CalculateSteering( SteeringPlugin_Output& steering, const AgentInfo& agentInfo)
{
	steering.AutoOrient = true;
	steering.LinearVelocity = m_Target - agentInfo.Position;
	steering.LinearVelocity.Normalize();
	steering.LinearVelocity *= agentInfo.MaxLinearSpeed;

}

//SPIN
//****
void Spin::CalculateSteering(SteeringPlugin_Output& steering, const AgentInfo& agentInfo)
{
	steering.AutoOrient = false;
	steering.AngularVelocity = agentInfo.MaxAngularSpeed;
}


////FLEE
////****
void Flee::CalculateSteering(SteeringPlugin_Output& steering, const AgentInfo& agentInfo)
{
	//Elite::Vector2 toTarget = agentInfo.Position -  m_Target;
	//float distanceSqrd = toTarget.MagnitudeSquared();

	steering.AutoOrient = true;
	steering.LinearVelocity = m_Target - agentInfo.Position;
	steering.LinearVelocity.Normalize();
	steering.LinearVelocity *= -agentInfo.MaxLinearSpeed;
}

////FACE
////****
void Face::CalculateSteering(SteeringPlugin_Output& steering, const AgentInfo& agentInfo)
{
	steering.AutoOrient = false;
	Elite::Vector2 toTarget = (m_Target - agentInfo.Position).GetNormalized();
	const float agentRotation{ agentInfo.Orientation + 0.5f * float(M_PI) };
	Elite::Vector2 agentDirection{ std::cosf(agentRotation),std::sinf(agentRotation) };
	steering.AngularVelocity = (toTarget.Dot(agentDirection)) * agentInfo.MaxAngularSpeed;
}