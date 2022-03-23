#include "StateMachineComponent.h"
#include "Actor.h"
#include "SeekComponent.h"
#include "WanderComponent.h"
#include "PathfindComponent.h"
#include "Transform2D.h"

void StateMachineComponent::start()
{
	Component::start();

	m_seekComponent = getOwner()->getComponent<SeekComponent>();
	m_seekForce = m_seekComponent->getSteeringForce();

	m_wanderComponent = getOwner()->getComponent<WanderComponent>();
	m_wanderForce = m_wanderComponent->getSteeringForce();
	
	m_pathfindComponent = getOwner()->getComponent<PathfindComponent>();

	m_currentState = SEEKINGWANDER;
}

void StateMachineComponent::update(float deltaTime)
{
	Component::update(deltaTime);

	MathLibrary::Vector2 targetPos = m_seekComponent->getTarget()->getTransform()->getWorldPosition();
	MathLibrary::Vector2 ownerPos = getOwner()->getTransform()->getWorldPosition();
	float distanceFromTarget = (targetPos - ownerPos).getMagnitude();

	bool targetInRange = distanceFromTarget <= m_seekRange;
	bool targetInPathfindRange = distanceFromTarget <= m_pathfindRange;

	switch (m_currentState)
	{
	case PATHFIND:
		m_seekComponent->setSteeringForce(0);
		m_wanderComponent->setSteeringForce(150);
		m_pathfindComponent->setEnabled(true);

		if (!targetInPathfindRange)
			setCurrentState(WANDER);
		if (targetInRange)
			setCurrentState(SEEK);

		break;
	case WANDER:
		m_seekComponent->setSteeringForce(0);
		m_wanderComponent->setSteeringForce(m_wanderForce);
		m_pathfindComponent->setEnabled(false);

		if (targetInRange)
			setCurrentState(SEEK);
		if (targetInPathfindRange)
			setCurrentState(PATHFIND);

		break;
	case SEEK:
		m_seekComponent->setSteeringForce(160);
		m_wanderComponent->setSteeringForce(0);
		m_pathfindComponent->setEnabled(false);

		if (!targetInRange)
			setCurrentState(SEEKINGWANDER);

		break;
	case SEEKINGWANDER:
		m_seekComponent->setSteeringForce(50);
		m_wanderComponent->setSteeringForce(150);
		m_pathfindComponent->setEnabled(false);

		if (targetInPathfindRange)
			setCurrentState(PATHFIND);
		if (!targetInPathfindRange)
			setCurrentState(WANDER);

		break;
	}
}
