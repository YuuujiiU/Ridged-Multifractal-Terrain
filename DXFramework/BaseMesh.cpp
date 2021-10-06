// BaseMesh.cpp
// 继承基本Mesh功能

#include "basemesh.h"

BaseMesh::BaseMesh()
{
}


BaseMesh::~BaseMesh()
{
	// Release the index buffer.
	if (m_indexBuffer)
	{
		m_indexBuffer->Release();
		m_indexBuffer = 0;
	}

	// Release the vertex buffer.
	if (m_vertexBuffer)
	{
		m_vertexBuffer->Release();
		m_vertexBuffer = 0;
	}

	// Release the texture object.
	if (m_Texture)
	{
		delete m_Texture;
		m_Texture = 0;
	}
}



//返回值
int BaseMesh::GetIndexCount()
{
	return m_indexCount;
}

ID3D11ShaderResourceView* BaseMesh::GetTexture()
{
	return m_Texture->GetTexture();
}



void BaseMesh::SendData(ID3D11DeviceContext* deviceContext)
{
	unsigned int stride;
	unsigned int offset;
	
	//设置顶点缓冲大小和偏移
	stride = sizeof(VertexType);
	offset = 0;

	//在渲染管线中，设置顶点缓冲区为‘active'，以便之后渲染
	deviceContext->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);

	//在渲染管线中，设置索引缓冲区为‘active'，以便之后渲染
	deviceContext->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	// 设置基本体为，三角形
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

void BaseMesh::LoadTexture(ID3D11Device* device, ID3D11DeviceContext* deviceContext, WCHAR* filename)
{
	// 新建纹理
	m_Texture = new Texture(device, deviceContext, filename);
}



