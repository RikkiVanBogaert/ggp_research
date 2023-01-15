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
	//steering.AngularVelocity = 0.f;
	// Shows a green line that show the velocity
	//if (pAgent->CanRenderBehavior())
	//DEBUGRENDERER2D->DrawDirection(pAgent->GetPosition(), steering.LinearVelocity, 5, { 0,1,0 }, 0);
}

//SPIN
//****
void Spin::CalculateSteering(SteeringPlugin_Output& steering, const AgentInfo& agentInfo)
{
	steering.AutoOrient = false;
	steering.AngularVelocity = agentInfo.MaxAngularSpeed;
}

////SPINANDSEEK
////****
//void SeekWhileSpinning::CalculateSteering(SteeringPlugin_Output& steering, const AgentInfo& agentInfo)
//{
//	steering.AutoOrient = false;
//	steering.LinearVelocity = m_Target - agentInfo.Position;
//	steering.LinearVelocity.Normalize();
//	steering.LinearVelocity *= agentInfo.MaxLinearSpeed;
//	steering.AngularVelocity = agentInfo.MaxAngularSpeed;
//}

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
	//steering.AutoOrient = false;
	//const Elite::Vector2 agentDirection{ cosf(agentInfo.Orientation), sinf(agentInfo.Orientation) };
	//const Elite::Vector2 targetDirection{ (m_Target - agentInfo.Position).GetNormalized() };
	//const float dotProduct{ agentDirection.Dot(targetDirection) };            // - Projection of normalized vectors to check "overlap" (through projection)
	//float turnDirection{};
	////constexpr float epsilon{ 0.004f };                                        // - Chosen by iterative testing, (0.01f was og)
	////if (dotProduct > 1 + epsilon || dotProduct < 1 - epsilon)                // - If agent & target vector overlap, result of dot will be 1
	////{
	//	const float crossResult{ agentDirection.Cross(targetDirection) };    // - Determines turning direction
	//	if(crossResult != 0)
	//		turnDirection = crossResult / abs(crossResult);                      // - "Unitize" turnDirection so speed of turning is not 
	////}                                                                        //	  determined by angular distance to target
	//steering.AngularVelocity = turnDirection * agentInfo.MaxAngularSpeed;


	steering.AutoOrient = false;
	Elite::Vector2 toTarget = (m_Target - agentInfo.Position);
	toTarget.Normalize();
	const float agentRot{ agentInfo.Orientation + 0.5f * float(M_PI) };
	Elite::Vector2 agentDirection{ std::cosf(agentRot),std::sinf(agentRot) };
	steering.AngularVelocity = (toTarget.Dot(agentDirection)) * agentInfo.MaxAngularSpeed;
}

//////FACEandFLEE
//////****
//void FaceAndFlee::CalculateSteering(SteeringPlugin_Output& steering, const AgentInfo& agentInfo)
//{
//	
//	//FACE
//	steering.AutoOrient = false;
//	Elite::Vector2 toTarget = (m_Target - agentInfo.Position);
//	toTarget.Normalize();
//	const float agentRot{ agentInfo.Orientation + 0.5f * float(M_PI) };
//	Elite::Vector2 agentDirection{ std::cosf(agentRot),std::sinf(agentRot) };
//	steering.AngularVelocity = (toTarget.Dot(agentDirection)) * agentInfo.MaxAngularSpeed;
//
//
//	//FLEE
//	steering.LinearVelocity = m_Target - agentInfo.Position;
//	steering.LinearVelocity.Normalize();
//	steering.LinearVelocity *= -agentInfo.MaxLinearSpeed;
//}

////ZIGZAG
////****
void ZigZag::CalculateSteering(SteeringPlugin_Output& steering, const AgentInfo& agentInfo)
{
	//SPIN
	steering.AutoOrient = false;
	Elite::Vector2 toTarget = (m_Target - agentInfo.Position);
	toTarget.Normalize();
	const float agentRot{ agentInfo.Orientation + 0.5f * float(M_PI) };
	Elite::Vector2 agentDirection{ std::cosf(agentRot),std::sinf(agentRot) };
	steering.AngularVelocity = (toTarget.Dot(agentDirection)) * agentInfo.MaxAngularSpeed;


	Elite::Vector2 perpendicular{ toTarget.y, -toTarget.x };
	Elite::Vector2 diagonal{ (perpendicular + toTarget).GetNormalized() };
}


////ARRIVE
////****
//SteeringOutput Arrive::CalculateSteering(float deltaT, SteeringAgent* pAgent)
//{
//	SteeringOutput steering = {};
//	const float slowRadius = 15.f;
//	const float arrivalRadius = 1.f;
//
//	steering.LinearVelocity = m_Target.Position - pAgent->GetPosition();
//	const float distanceSquared = steering.LinearVelocity.MagnitudeSquared();
//	steering.LinearVelocity.Normalize();
//
//	if (distanceSquared < (arrivalRadius * arrivalRadius))
//	{
//		steering.LinearVelocity.x = 0.f;
//		steering.LinearVelocity.y = 0.f;
//		return steering;
//	}
//
//	if (distanceSquared < (slowRadius * slowRadius))
//	{
//		steering.LinearVelocity *= pAgent->GetMaxLinearSpeed() * distanceSquared / (slowRadius * slowRadius);
//	}
//	else
//	{
//		steering.LinearVelocity *= pAgent->GetMaxLinearSpeed();
//	}
//
//	return steering;
//}
//

////WANDER
////****
//SteeringOutput Wander::CalculateSteering(float deltaT, SteeringAgent* pAgent)
//{
//	//Get the angle of the agent
//	//Find the direction where it's looking at
//	//Set a circle at the correct distance of the agent at the correct direction
//	const float agentAngle{ pAgent->GetRotation() };
//	const Elite::Vector2 agentDirection{ cosf(agentAngle), sinf(agentAngle) };
//	const Elite::Vector2 wanderCenter{ m_OffsetDistance * agentDirection + pAgent->GetPosition() };
//
//	m_Timer += deltaT;
//	if (m_Timer > 1.f)
//	{
//		m_Timer = 0.f;
//		const float minAngle{ m_WanderAngle - m_MaxAngleChange };
//		m_WanderAngle = float(rand() % int((minAngle + 90) + 1) + minAngle);
//	}
//	const Elite::Vector2 targetPointOnWander{ cosf(m_WanderAngle) * m_Radius + wanderCenter.x
//											, sinf(m_WanderAngle) * m_Radius + wanderCenter.y };
//
//	SteeringOutput steering{};
//	steering.LinearVelocity = targetPointOnWander - pAgent->GetPosition();
//	steering.LinearVelocity.GetNormalized() *= pAgent->GetMaxLinearSpeed();
//
//	const Elite::Vector2 agentDirectionVector{ targetPointOnWander - pAgent->GetPosition() };
//	if (pAgent->CanRenderBehavior())
//	{
//		DEBUGRENDERER2D->DrawCircle(wanderCenter, m_Radius, { 0,0,1 }, 0.9f);
//		DEBUGRENDERER2D->DrawDirection(pAgent->GetPosition(), agentDirectionVector, agentDirectionVector.Magnitude(), { 0,1,0 });
//		DEBUGRENDERER2D->DrawPoint(targetPointOnWander, 4, { 1,0,0 });
//	}
//
//	return steering;
//}
//
////EVADE
////****
//SteeringOutput Evade::CalculateSteering(float deltaT, SteeringAgent* pAgent)
//{
//	SteeringOutput steering{};
//	Elite::Vector2 direction{};
//	const Elite::Vector2 evadeMagnitude = m_Target.Position - pAgent->GetPosition();
//
//	if (evadeMagnitude.MagnitudeSquared() <= 15 * 15)
//	{
//		//			'-' because we want to get away and not towards the target
//		direction = -(evadeMagnitude)+m_Target.LinearVelocity;
//	}
//
//	steering.LinearVelocity = direction;
//	steering.LinearVelocity.Normalize();
//	steering.LinearVelocity *= pAgent->GetMaxLinearSpeed();
//	return steering;
//}
