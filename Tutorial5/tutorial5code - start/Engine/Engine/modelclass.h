////////////////////////////////////////////////////////////////////////////////
// Filename: modelclass.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _MODELCLASS_H_
#define _MODELCLASS_H_


//////////////
// INCLUDES //
//////////////
#include <d3d11.h>
#include <DirectXMath.h>


///////////////////////
// MY CLASS INCLUDES //
///////////////////////
#include "textureclass.h"
#include "vertextype.h"


using namespace DirectX;

////////////////////////////////////////////////////////////////////////////////
// Class name: ModelClass
////////////////////////////////////////////////////////////////////////////////
class ModelClass
{
public:
	ModelClass();
	ModelClass(const ModelClass&);
	ModelClass(VertexType*, int, unsigned long*, int, D3D_PRIMITIVE_TOPOLOGY = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	~ModelClass();

	bool Initialize(ID3D11Device*, WCHAR*, WCHAR*);
	void Shutdown();
	void Render(ID3D11DeviceContext*);

	int GetIndexCount();

	ID3D11ShaderResourceView* GetBodyTexture();
	ID3D11ShaderResourceView* GetArmsTexture();

private:
	bool InitializeBuffers(ID3D11Device*);
	void ShutdownBuffers();
	void RenderBuffers(ID3D11DeviceContext*);

	bool LoadTextures(ID3D11Device*, WCHAR*, WCHAR*);
	void ReleaseTextures();

private:
	D3D_PRIMITIVE_TOPOLOGY m_primitive_topology;
	ID3D11Buffer *m_vertexBuffer, *m_indexBuffer;
	int m_vertexCount, m_indexCount;
	VertexType* m_vertices;
	unsigned long* m_indices;

	TextureClass* m_BodyTexture;
	TextureClass* m_ArmsTexture;
};

#endif