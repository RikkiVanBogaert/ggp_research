#pragma once
#include "Exam_HelperStructs.h"

#pragma region **ISTEERINGBEHAVIOR** (BASE)
	class ISteeringBehavior
	{
	public:
		ISteeringBehavior() = default;
		virtual ~ISteeringBehavior() = default;

		virtual void CalculateSteering(SteeringPlugin_Output& steering, const AgentInfo& agentInfo) = 0;

		//Seek Functions
		void SetTarget(const Elite::Vector2& target) { m_Target = target; }

		template<class T, typename std::enable_if<std::is_base_of<ISteeringBehavior, T>::value>::type* = nullptr>
		T* As()
		{
			return static_cast<T*>(this);
		}

	protected:
		Elite::Vector2 m_Target;
	};
#pragma endregion

	///////////////////////////////////////
	//SEEK
	//****
	class Seek : public ISteeringBehavior
	{
	public:
		Seek() = default;
		virtual ~Seek() = default;

		//Seek Behaviour
		void CalculateSteering(SteeringPlugin_Output& steering, const AgentInfo& agentInfo) override;
	};

	///////////////////////////////////////
	//SPIN
	//****
	class Spin : public ISteeringBehavior
	{
	public:
		Spin() = default;
		virtual ~Spin() = default;

		// Behaviour
		void CalculateSteering(SteeringPlugin_Output& steering, const AgentInfo& agentInfo) override;
	};

	///////////////////////////////////////
	//SPINANDSEEK
	//****
	class SeekWhileSpinning : public ISteeringBehavior
	{
	public:
		SeekWhileSpinning() = default;
		virtual ~SeekWhileSpinning() = default;

		// Behaviour
		void CalculateSteering(SteeringPlugin_Output& steering, const AgentInfo& agentInfo) override;
	};

	/////////////////////////////////////////
	////FLEE
	////****
	class Flee : public ISteeringBehavior
	{
	public:
		Flee() = default;
		virtual ~Flee() = default;

		// behavior
		void CalculateSteering(SteeringPlugin_Output& steering, const AgentInfo& agentInfo) override;
		void SetFleeRadius(float fleeRadius) { m_FleeRadius = fleeRadius; }
	private:
		float m_FleeRadius = 20.f;
	};

	/////////////////////////////////////////
	////FACE
	////****
	class Face : public ISteeringBehavior
	{
	public:
		Face() = default;
		virtual ~Face() = default;

		void CalculateSteering(SteeringPlugin_Output& steering, const AgentInfo& agentInfo) override;
	};

	/////////////////////////////////////////
	////FACEAndFlee
	////****
	class FaceAndFlee : public ISteeringBehavior
	{
	public:
		FaceAndFlee() = default;
		virtual ~FaceAndFlee() = default;

		void CalculateSteering(SteeringPlugin_Output& steering, const AgentInfo& agentInfo) override;
	};

	class ZigZag : public ISteeringBehavior
	{
	public:
		ZigZag() = default;
		virtual ~ZigZag() = default;

		void CalculateSteering(SteeringPlugin_Output& steering, const AgentInfo& agentInfo) override;

	private:
		int dir = 1;
		bool goingUp = false;
	};

	/////////////////////////////////////////
	////ARRIVE
	////****
	//class Arrive : public ISteeringBehavior
	//{
	//public:
	//	Arrive() = default;
	//	virtual ~Arrive() = default;

	//	void CalculateSteering(float deltaT, SteeringPlugin_Output& steering) override;
	//};


	/////////////////////////////////////////
	////WANDER
	////****
	//class Wander : public Seek
	//{
	//public:
	//	Wander() = default;
	//	virtual ~Wander() = default;

	//	//Wander Behaviour
	//	void CalculateSteering(float deltaT, SteeringPlugin_Output& steering) override;

	//	void SetWanderOffset(float offset) { m_OffsetDistance = offset; }
	//	void SetWanderRadius(float radius) { m_Radius = radius; }
	//	void SetMaxAngleChange(float rad) { m_MaxAngleChange = rad; }

	//protected:
	//	float m_Timer = 0.f;
	//	float m_OffsetDistance = 6.f; //Offset (Agent Direction)
	//	float m_Radius = 4.f; //WanderRadius
	//	float m_MaxAngleChange = Elite::ToRadians(45); //Max WanderAngle change per frame
	//	float m_WanderAngle = 0.f; //Internal
	//};

	/////////////////////////////////////////
	////EVADE
	////****
	//class Evade : public ISteeringBehavior
	//{
	//public:
	//	Evade() = default;
	//	virtual ~Evade() = default;

	//	void CalculateSteering(float deltaT, SteeringPlugin_Output& steering) override;
	//};


