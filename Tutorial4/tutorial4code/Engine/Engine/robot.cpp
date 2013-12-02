#include "robot.h"


Robot::Robot(void)
{
	m_speed = 0.1f;
	m_position = XMFLOAT3(0.0f, 0.0f, 0.0f);
}


Robot::~Robot(void)
{
}

XMFLOAT3 Robot::GetPosition() 
{
	return m_position;
}

void Robot::SetPosition(float x, float y, float z) 
{
	m_position = XMFLOAT3(x, y, z);
}

void Robot::SetPosition(XMFLOAT3 position) 
{
	m_position = position;
}

float Robot::GetSpeed() 
{
	return m_speed;
}

void Robot::SetSpeed(float speed) 
{
	m_speed = speed;
}

void Robot::MoveLeft()
{
	m_position = XMFLOAT3(m_position.x - m_speed, m_position.y, m_position.z);
}

void Robot::MoveRight()
{
	m_position = XMFLOAT3(m_position.x + m_speed, m_position.y, m_position.z);
}

void Robot::MoveUp()
{
	m_position = XMFLOAT3(m_position.x, m_position.y  + m_speed, m_position.z);
}

void Robot::MoveDown()
{
	m_position = XMFLOAT3(m_position.x, m_position.y - m_speed, m_position.z);
}