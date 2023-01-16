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