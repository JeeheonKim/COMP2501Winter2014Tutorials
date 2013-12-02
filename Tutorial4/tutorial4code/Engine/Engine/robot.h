#pragma once

#include <DirectXMath.h>

using namespace DirectX;

class Robot
{
public:
	Robot(void);
	~Robot(void);

	// Access methods
	XMFLOAT3	GetPosition();
	void		SetPosition(float x, float y, float z);
	void		SetPosition(XMFLOAT3 position);
	float		GetSpeed();
	void		SetSpeed(float speed);

	// Movement
	void		MoveLeft();
	void		MoveRight();
	void		MoveUp();
	void		MoveDown();

private:
	XMFLOAT3 m_position;
	float m_speed;
};

