////////////////////////////////////////////////////////////////////////////////
// Filename: modelclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "modelclass.h"


ModelClass::ModelClass()
{
	m_vertexBuffer = 0;
	m_indexBuffer = 0;
}

ModelClass::ModelClass(VertexType* vertices, int numvertices, unsigned long* indices, int numindices, D3D_PRIMITIVE_TOPOLOGY topology)
{
	m_vertexBuffer = 0;
	m_indexBuffer = 0;
	m_vertices = vertices;
	m_indices = indices;
	m_vertexCount = numvertices;
	m_indexCount = numindices;
	m_primitive_topology = topology;
	
	m_BodyTexture = 0;
	m_ArmsTexture = 0;
}


ModelClass::ModelClass(const ModelClass& other)
{
}


ModelClass::~ModelClass()
{
}

bool ModelClass::Initialize(ID3D11Device* device, WCHAR* bodyTextureFilename, WCHAR* armsTextureFilename)
{
	bool result;


	// Initialize the vertex and index buffers.
	result = InitializeBuffers(device);
	if(!result)
	{
		return false;
	}

	// Load the texture for this model.
	result = LoadTextures(device, bodyTextureFilename, armsTextureFilename);
	if(!result)
	{
		return false;
	}

	return true;
}


void ModelClass::Shutdown()
{
	// Shutdown the vertex and index buffers.
	ShutdownBuffers();

	// Release the model texture.
	ReleaseTextures();

	return;
}


void ModelClass::Render(ID3D11DeviceContext* deviceContext)
{
	// Put the vertex and index buffers on the graphics pipeline to prepare them for drawing.
	RenderBuffers(deviceContext);

	return;
}


int ModelClass::GetIndexCount()
{
	return m_indexCount;
}

ID3D11ShaderResourceView* ModelClass::GetBodyTexture()
{
	return m_BodyTexture->GetTexture();
}

ID3D11ShaderResourceView* ModelClass::GetArmsTexture()
{
	return m_ArmsTexture->GetTexture();
}


bool ModelClass::InitializeBuffers(ID3D11Device* device)
{
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc; // d`escriptions of our buffers
    D3D11_SUBRESOURCE_DATA vertexData, indexData; // buffer data
	HRESULT result; // for error checking

	// Set up the description of the static vertex buffer.
    vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    vertexBufferDesc.ByteWidth = sizeof(VertexType) * m_vertexCount;
    vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vertexBufferDesc.CPUAccessFlags = 0;
    vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the vertex data.
    vertexData.pSysMem = m_vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	// Now create the vertex buffer.
    result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &m_vertexBuffer);
	if(FAILED(result))
	{
		return false;
	}

	// Set up the description of the static index buffer.
    indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    indexBufferDesc.ByteWidth = sizeof(unsigned long) * m_indexCount;
    indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    indexBufferDesc.CPUAccessFlags = 0;
    indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the index data.
    indexData.pSysMem = m_indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	// Create the index buffer.
	result = device->CreateBuffer(&indexBufferDesc, &indexData, &m_indexBuffer);
	if(FAILED(result))
	{
		return false;
	}

	return true;
}


void ModelClass::ShutdownBuffers()
{
	// Release the index buffer.
	if(m_indexBuffer)
	{
		m_indexBuffer->Release();
		m_indexBuffer = 0;
	}

	// Release the vertex buffer.
	if(m_vertexBuffer)
	{
		m_vertexBuffer->Release();
		m_vertexBuffer = 0;
	}

	return;
}


void ModelClass::RenderBuffers(ID3D11DeviceContext* deviceContext)
{
	unsigned int stride;
	unsigned int offset;


	// Set vertex buffer stride and offset.
	stride = sizeof(VertexType); 
	offset = 0;
    
	// Set the vertex buffer to active in the input assembler so it can be rendered.
	deviceContext->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);

    // Set the index buffer to active in the input assembler so it can be rendered.
	deviceContext->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);

    // Set the type of primitive that should be rendered from this vertex buffer, in this case triangles.
	deviceContext->IASetPrimitiveTopology(m_primitive_topology);

	return;
}

bool ModelClass::LoadTextures(ID3D11Device* device, WCHAR* bodyFilename, WCHAR* armsFilename)
{
	bool result;


	// Create the body texture object.
	m_BodyTexture = new TextureClass;
	if(!m_BodyTexture)
	{
		return false;
	}

	// Initialize the body texture object.
	result = m_BodyTexture->Initialize(device, bodyFilename);
	if(!result)
	{
		return false;
	}

	// Create the arms texture object.
	m_ArmsTexture = new TextureClass;
	if(!m_ArmsTexture)
	{
		return false;
	}

	// Initialize the body texture object.
	result = m_ArmsTexture->Initialize(device, armsFilename);
	if(!result)
	{
		return false;
	}

	return true;
}

void ModelClass::ReleaseTextures()
{
	// Release the texture objects.
	if(m_BodyTexture)
	{
		m_BodyTexture->Shutdown();
		delete m_BodyTexture;
		m_BodyTexture = 0;
	}
	
	if(m_ArmsTexture)
	{
		m_ArmsTexture->Shutdown();
		delete m_ArmsTexture;
		m_ArmsTexture = 0;
	}

	return;
}