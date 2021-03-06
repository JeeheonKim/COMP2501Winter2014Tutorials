#pragma once

#include <DirectXMath.h>
#include "vertextype.h"
#include <string>

using namespace DirectX;

class Robot
{
public:
	Robot(XMFLOAT3 position, std::string boundaryType);
	~Robot(void);

	void Initialize();
	void Shutdown();

	// Access methods
	XMFLOAT3		GetPosition();
	void			SetPosition(float x, float y, float z);
	void			SetPosition(XMFLOAT3 position);
	XMFLOAT3		GetRotation();
	float			GetSpeed();
	void			SetSpeed(float speed);
	float			GetHeight();
	void			SetHeight(float height);
	float			GetWidth();
	void			SetWidth(float height);
	
	VertexType*		GetVertices();
	unsigned long*	GetIndices();
	int				GetVertexCount();
	int				GetIndexCount();


	std::string		GetBoundaryType();

	bool			IsDancing();
	void			SetDancing(bool);

	// Movement
	void			MoveLeft();
	void			MoveRight();
	void			MoveUp();
	void			MoveDown();
	void			MoveForward();
	void			MoveBackward();

	void			RotateLeft();
	void			RotateRight();

private:
	void			InitializeModel();
private:
	XMFLOAT3		m_position;
	XMFLOAT3		m_rotation;
	float			m_speed;
	float			m_height;
	float			m_width;

	VertexType*		m_vertices;
	unsigned long*	m_indices;
	int				m_vertexCount;
	int				m_indexCount;

	std::string		m_boundaryType;

	bool			m_isDancing;
};

