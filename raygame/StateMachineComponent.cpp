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

	//bools for the player being in certain ranges
	bool targetInRange = distanceFromTarget <= m_seekRange;
	bool targetInPathfindRange = distanceFromTarget <= m_pathfindRange;

	switch (m_currentState)
	{
	case PATHFIND:
		//Sets seeking to 0, and pathfinds using wander to mitigate
		//the enemy getting stuck on walls
		m_seekComponent->setSteeringForce(0);
		m_wanderComponent->setSteeringForce(150);
		m_pathfindComponent->setEnabled(true);

		//If the target isnt in pathfinding range set to wander
		if (!targetInPathfindRange)
			setCurrentState(WANDER);
		//If the targets in close range set to seek
		if (targetInRange)
			setCurrentState(SEEK);

		break;
	case WANDER:
		//Sets seeking and pathfinding to 0
		m_seekComponent->setSteeringForce(0);
		m_wanderComponent->setSteeringForce(m_wanderForce);
		m_pathfindComponent->setEnabled(false);

		//if the target is in seek range set to seek, and 
		//if the target is in pathfinding range set to pathfind
		if (targetInRange)
			setCurrentState(SEEK);
		if (targetInPathfindRange)
			setCurrentState(PATHFIND);

		break;
	case SEEK:
		//Sets seeking to a certain value and turns off wandering and pathfinding
		m_seekComponent->setSteeringForce(160);
		m_wanderComponent->setSteeringForce(0);
		m_pathfindComponent->setEnabled(false);

		//If the target leaves the range set to SeekingWander
		if (!targetInRange)
			setCurrentState(SEEKINGWANDER);

		break;
	case SEEKINGWANDER:
		//Turns off pathfinding and turns on both seeking and wandering
		m_seekComponent->setSteeringForce(50);
		m_wanderComponent->setSteeringForce(150);
		m_pathfindComponent->setEnabled(false);

		//If the target is in pathfinding range set to pathfind
		if (targetInPathfindRange)
			setCurrentState(PATHFIND);
		//If the target isnt in range set back to wander
		if (!targetInPathfindRange)
			setCurrentState(WANDER);

		break;
	}
}
