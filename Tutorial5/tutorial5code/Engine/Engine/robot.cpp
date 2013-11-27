#include "robot.h"

Robot::Robot(XMFLOAT3 position, std::string boundaryType)
{
	m_position = position;
	m_speed = 0.1f;
	m_height = 2.0f;
	m_width = 3.0f;

	m_boundaryType = boundaryType;
}

Robot::~Robot(void)
{
}

void Robot::Initialize()
{
	InitializeModel();
}

void Robot::InitializeModel()
{
	m_vertexCount = 5;
	m_indexCount = 9;

	m_vertices = new VertexType[m_vertexCount];
	m_indices = new unsigned long[m_indexCount];
	
	m_vertices[0].position = XMFLOAT3(-0.5f, -0.5f, 0.0f); // Bottom left.
	//m_vertices[0].texture =` XMFLOAT2(0.0f, 1.0f);
	
	m_vertices[1].position = XMFLOAT3(-0.5f, 0.5f, 0.0f);  // Top left.
	//m_vertices[1].texture = XMFLOAT2(0.0f, 0.0f);
	
	m_vertices[2].position = XMFLOAT3(0.5f, -0.5f, 0.0f);  // Bottom right.
	//m_vertices[2].texture = XMFLOAT2(1.0f, 1.0f);
	
	m_vertices[3].position = XMFLOAT3(0.5f, 0.5f, 0.0f);   // Top right.
	//m_vertices[3].texture = XMFLOAT2(1.0f, 0.0f);
	
	m_vertices[4].position = XMFLOAT3(0.0f, 1.0f, 0.0f);   // Top of triangle
	//m_vertices[4].texture = XMFLOAT2(0.0f, 1.0f);
	
	// Load the index array with data.
	m_indices[0] = 0;  // Bottom left.
	m_indices[1] = 1;  // Top left.
	m_indices[2] = 2;  // Bottom right.
	m_indices[3] = 1;  // Top left.
	m_indices[4] = 3;  // Top right.  
	m_indices[5] = 2;  // Bottom right.
	m_indices[6] = 1;  // Top left.
	m_indices[7] = 4;  // Top of triangle.
	m_indices[8] = 3;  // Top right.
}

void Robot::Shutdown()
{
	if(m_vertices)
	{
		delete[] m_vertices;
		m_vertices = 0;
	}

	if(m_indices)
	{
		delete[] m_indices;
		m_indices = 0;
	}
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

float Robot::GetHeight() 
{
	return m_height;
}

void Robot::SetHeight(float height) 
{
	m_height = height;
}

float Robot::GetWidth() 
{
	return m_width;
}

void Robot::SetWidth(float width) 
{
	m_width = width;
}

VertexType* Robot::GetVertices()
{
	return m_vertices;
}

unsigned long* Robot::GetIndices()
{
	return m_indices;
}

int Robot::GetVertexCount() {
	return m_vertexCount;
}

int Robot::GetIndexCount() {
	return m_indexCount;
}


std::string Robot::GetBoundaryType()
{
	return m_boundaryType;
}

bool Robot::IsDancing()
{
	return m_isDancing;
}

void Robot::SetDancing(bool value)
{
	m_isDancing = value;
}

// Movement

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