////////////////////////////////////////////////////////////////////////////////
// Filename: graphicsclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "graphicsclass.h"


GraphicsClass::GraphicsClass()
{
	m_D3D = 0;
	m_Camera = 0;
	m_Model = 0;
	m_ColorShader = 0;
}


GraphicsClass::GraphicsClass(const GraphicsClass& other)
{
}


GraphicsClass::~GraphicsClass()
{
}


bool GraphicsClass::Initialize(int screenWidth, int screenHeight, HWND hwnd)
{
	bool result;


	// Create the Direct3D object.
	m_D3D = new D3DClass;
	if(!m_D3D)
	{
		return false;
	}

	// Initialize the Direct3D object.
	result = m_D3D->Initialize(screenWidth, screenHeight, VSYNC_ENABLED, hwnd, FULL_SCREEN, SCREEN_DEPTH, SCREEN_NEAR);
	if(!result)
	{
		MessageBox(hwnd, L"Could not initialize Direct3D.", L"Error", MB_OK);
		return false;
	}

	// Create the camera object.
	m_Camera = new CameraClass;
	if(!m_Camera)
	{
		return false;
	}

	// Set the initial position of the camera.
	m_Camera->SetPosition(0.0f, 0.0f, -10.0f);
	
	// Create the model object.
	m_Model = new ModelClass;
	if(!m_Model)
	{
		return false;
	}

	// Initialize the model object.
	result = m_Model->Initialize(m_D3D->GetDevice());
	if(!result)
	{
		MessageBox(hwnd, L"Could not initialize the model object.", L"Error", MB_OK);
		return false;
	}

	// Create the color shader object.
	m_ColorShader = new ColorShaderClass;
	if(!m_ColorShader)
	{
		return false;
	}

	// Initialize the color shader object.
	result = m_ColorShader->Initialize(m_D3D->GetDevice(), hwnd);
	if(!result)
	{
		MessageBox(hwnd, L"Could not initialize the color shader object.", L"Error", MB_OK);
		return false;
	}

	return true;
}


void GraphicsClass::Shutdown()
{
	// Release the color shader object.
	if(m_ColorShader)
	{
		m_ColorShader->Shutdown();
		delete m_ColorShader;
		m_ColorShader = 0;
	}

	// Release the model object.
	if(m_Model)
	{
		m_Model->Shutdown();
		delete m_Model;
		m_Model = 0;
	}

	// Release the camera object.
	if(m_Camera)
	{
		delete m_Camera;
		m_Camera = 0;
	}

	// Release the D3D object.
	if(m_D3D)
	{
		m_D3D->Shutdown();
		delete m_D3D;
		m_D3D = 0;
	}

	return;
}


bool GraphicsClass::Frame()
{
	bool result;


	// Render the graphics scene.
	result = Render();
	if(!result)
	{
		return false;
	}

	return true;
}


bool GraphicsClass::Render()
{
	const int numWorldMatrices = 3;
	XMFLOAT4X4 worldMatrices[numWorldMatrices];
	XMFLOAT4X4 viewMatrix, projectionMatrix;
	bool result;


	// Clear the buffers to begin the scene.
	m_D3D->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);

	// Generate the view matrix based on the camera's position.
	m_Camera->Render();

	// Put the model vertex and index buffers on the graphics pipeline to prepare them for drawing.
	m_Model->Render(m_D3D->GetDeviceContext());

	// Get the world, view, and projection matrices from the camera and d3d objects.
	m_Camera->GetViewMatrix(viewMatrix);
	m_D3D->GetWorldMatrix(worldMatrices[0]);
	m_D3D->GetProjectionMatrix(projectionMatrix);

	// Robot body
	XMStoreFloat4x4( &worldMatrices[0], XMMatrixScaling(0.5f, 0.5f, 1.0f) * XMMatrixRotationZ(XM_PI/4) * XMMatrixTranslation(4.0f, 3.0f, 0.0f) );

	// Robot upper arm
	XMStoreFloat4x4( &worldMatrices[1], XMMatrixScaling(0.5f, 0.5f, 1.0f) * XMMatrixRotationZ(XM_PI/2) * XMMatrixTranslation(-1.0f, 0.0f, 0.0f) * XMLoadFloat4x4(&worldMatrices[0]) );

	// Robot lower arm

	// Transformation to move the lower arm into position
	XMStoreFloat4x4( &worldMatrices[2], XMMatrixRotationZ(XM_PI/2) * XMMatrixTranslation(2.0f, 2.0f, 0.0f) * XMLoadFloat4x4(&worldMatrices[1]) );

	// Transformation to rotate lower arm around joint
	XMStoreFloat4x4( &worldMatrices[2],  XMMatrixTranslation(0.0f, -2.0f, 0.0f) * XMMatrixRotationZ(XM_PI/4) * XMMatrixTranslation(0.0f, 2.0f, 0.0f) * XMLoadFloat4x4(&worldMatrices[2]) );

	// Render the model using the color shader.
	for (int i = 0; i < numWorldMatrices; i++)
	{
		result = m_ColorShader->Render(m_D3D->GetDeviceContext(), m_Model->GetIndexCount(), worldMatrices[i], viewMatrix, projectionMatrix);
	
		if(!result)
		{
			return false;
		}
	}
	
	// Present the rendered scene to the screen.
	m_D3D->EndScene();

	return true;
}