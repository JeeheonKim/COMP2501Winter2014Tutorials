#include "collisionmanager.h"


CollisionManager::CollisionManager(void)
{
}


CollisionManager::~CollisionManager(void)
{
}

void CollisionManager::MoveIfNotColliding(Robot* a, Robot* b, void (Robot::*FuncPtr)(void))
{
	// Make a copy of the robot position before it moves
	XMFLOAT3 initialRobotPosition = a->GetPosition();

	// Move the robot
	(a->*FuncPtr)();

	// If the move caused a collision, restore the robot to its initial position
	if (AreColliding(a, b))
	{
		a->SetPosition(initialRobotPosition);
	}
}

bool CollisionManager::AreColliding(Robot* a, Robot* b)
{
	if (a->GetBoundaryType() == "circle")
	{
		// Add logic to determine if circles overlap
		return false;
	}
	else if (a->GetBoundaryType() == "square")
	{
		// Add logic to determine if squares overlap
		return false;
	}

	return false;
}