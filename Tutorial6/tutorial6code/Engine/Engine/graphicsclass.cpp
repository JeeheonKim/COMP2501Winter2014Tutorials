////////////////////////////////////////////////////////////////////////////////
// Filename: graphicsclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "graphicsclass.h"


GraphicsClass::GraphicsClass()
{
	m_D3D = 0;
	m_Camera = 0;
	m_Model = 0;
	m_TextureShader = 0;
}


GraphicsClass::GraphicsClass(const GraphicsClass& other)
{
}


GraphicsClass::~GraphicsClass()
{
}


bool GraphicsClass::Initialize(int screenWidth, int screenHeight, HWND hwnd, Robot* robot)
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
	
	// Set robot member variable
	m_Robot = robot;

	// Create the model for a robot.
	m_Model = new ModelClass(m_Robot->GetVertices(), m_Robot->GetVertexCount(), m_Robot->GetIndices(), m_Robot->GetIndexCount());
	if(!m_Model)
	{
		return false;
	}

	// Initialize the model object.
	result = m_Model->Initialize(m_D3D->GetDevice(), L"../Engine/robotBody.dds", L"../Engine/robotArms.dds");
	if(!result)
	{
		MessageBox(hwnd, L"Could not initialize the model object.", L"Error", MB_OK);
		return false;
	}

	// Create the texture shader object.
	m_TextureShader = new TextureShaderClass;
	if(!m_TextureShader)
	{
		return false;
	}

	// Initialize the texture shader object.
	result = m_TextureShader->Initialize(m_D3D->GetDevice(), hwnd);
	if(!result)
	{
		MessageBox(hwnd, L"Could not initialize the texture shader object.", L"Error", MB_OK);
		return false;
	}

	return true;
}


void GraphicsClass::Shutdown()
{
	// Release the texture shader object.
	if(m_TextureShader)
	{
		m_TextureShader->Shutdown();
		delete m_TextureShader;
		m_TextureShader = 0;
	}

	// Release the model objects.
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
	// Update our time	
	static float t = 0.0f;

    static DWORD dwTimeStart = 0;
    DWORD dwTimeCur = GetTickCount();
    if( dwTimeStart == 0 )
        dwTimeStart = dwTimeCur;
	t = ( dwTimeCur - dwTimeStart ) / 1000.0f;

	// Matrices
	XMFLOAT4X4 worldBody, worldLeftUpper, worldLeftForearm, worldRightUpper, worldRightForearm;
	XMFLOAT4X4 viewMatrix, projectionMatrix;

	// Initialize matrices to Identity matrices
	
	bool result;

	// Clear the buffers to begin the scene.
	m_D3D->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);

	// Generate the view matrix based on the camera's position.
	m_Camera->Render();

	// Get the view and projection matrices from the camera and d3d objects.
	m_Camera->GetViewMatrix(viewMatrix);
	m_D3D->GetProjectionMatrix(projectionMatrix);
	m_D3D->GetWorldMatrix(worldBody);

	// Put the model vertex and index buffers on the graphics pipeline to prepare them for drawing.
	m_Model->Render(m_D3D->GetDeviceContext());

	// Set up player robot

	XMStoreFloat4x4( &worldBody, XMMatrixScaling(3.0f, 3.0f, 3.0f) * XMMatrixTranslation(m_Robot->GetPosition().x, m_Robot->GetPosition().y, m_Robot->GetPosition().z) );
	XMStoreFloat4x4( &worldLeftUpper, XMMatrixScaling(0.5f, 0.5f, 1.0f) * XMMatrixRotationZ(XM_PI/2) * XMMatrixTranslation(-0.5f, 0.0f, 0.0f) * XMLoadFloat4x4(&worldBody) );
	XMStoreFloat4x4( &worldLeftForearm, XMMatrixRotationZ(XM_PI/2) * XMMatrixTranslation(1.0f, 1.0f, -0.1f) * XMLoadFloat4x4(&worldLeftUpper) );
	XMStoreFloat4x4( &worldRightUpper, XMMatrixScaling(0.5f, 0.5f, 1.0f) * XMMatrixRotationZ(3 * XM_PI/2) * XMMatrixTranslation(0.5f, 0.0f, 0.0f) * XMLoadFloat4x4(&worldBody) );
	XMStoreFloat4x4( &worldRightForearm, XMMatrixRotationZ(3 * XM_PI/2) * XMMatrixTranslation(-1.0f, 1.0f, -0.1f) * XMLoadFloat4x4(&worldRightUpper) );

	// If robot is dancing, move his arms
	if (m_Robot->IsDancing())
	{
		XMStoreFloat4x4( &worldLeftForearm, XMMatrixTranslation(0.0f, -1.0f, 0.0f) * XMMatrixRotationZ(sin(t)) * XMMatrixTranslation(0.0f, 1.0f, 0.0f) * XMLoadFloat4x4(&worldLeftForearm));
		XMStoreFloat4x4( &worldRightForearm, XMMatrixTranslation(0.0f, -1.0f, 0.0f) * XMMatrixRotationZ(sin(t)) * XMMatrixTranslation(0.0f, 1.0f, 0.0f) * XMLoadFloat4x4(&worldRightForearm));
	}

	XMFLOAT4X4 worldMatrices[5] = { worldBody, worldLeftUpper, worldLeftForearm, worldRightUpper, worldRightForearm };

	result = m_TextureShader->Render(m_D3D->GetDeviceContext(), m_Model->GetIndexCount(), worldMatrices[0], viewMatrix, projectionMatrix, m_Model->GetBodyTexture());
	
	if(!result)
	{
		return false;
	}

	// Render the model for the first robot using the color shader.
	for (int i = 1; i < 5; i++)
	{
		result = m_TextureShader->Render(m_D3D->GetDeviceContext(), m_Model->GetIndexCount(), worldMatrices[i], viewMatrix, projectionMatrix, m_Model->GetArmsTexture());
	
		if(!result)
		{
			return false;
		}
	}

	// Present the rendered scene to the screen.
	m_D3D->EndScene();

	return true;
}