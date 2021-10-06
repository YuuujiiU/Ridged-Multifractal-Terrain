#include "SkyBox.h"
//天空盒

Skybox::Skybox(ID3D11Device* device, ID3D11DeviceContext* deviceContext, WCHAR* textureFilename)
	: QuadMesh(device, deviceContext, textureFilename)
{
	// Init 顶点和索引缓冲区，设置基本图元为triangle
	InitBuffers(device);

	// 加载纹理
	LoadTexture(device, deviceContext, textureFilename);
}

Skybox::~Skybox()
{

}

void Skybox::InitBuffers(ID3D11Device* device)
{
	VertexType* vertices;
	unsigned long* indices;
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;

	// 设置顶点数组中的顶点数
	m_vertexCount = 24;

	// 设置索引数组中的索引数
	m_indexCount = 36;

	// 创建顶点序列
	vertices = new VertexType[m_vertexCount];

	// 创建索引序列
	indices = new unsigned long[m_indexCount];

	// 加载顶点序列数据

	// Back face
	vertices[0].position = XMFLOAT3(-4.0f, -4.0f, 0.0f);  // Bottom left.
	vertices[0].texture = XMFLOAT2(0.25f, 0.66666f);
	vertices[0].normal = XMFLOAT3(0.0f, 0.0f, -1.0f);

	vertices[1].position = XMFLOAT3(-4.0f, 4.0f, 0.0f);  // Top left.
	vertices[1].texture = XMFLOAT2(0.25f, 0.33333f);
	vertices[1].normal = XMFLOAT3(0.0f, 0.0f, -1.0f);

	vertices[2].position = XMFLOAT3(4.0f, 4.0f, 0.0f);  // Top right.
	vertices[2].texture = XMFLOAT2(0.5f, 0.33333f);
	vertices[2].normal = XMFLOAT3(0.0f, 0.0f, -1.0f);

	vertices[3].position = XMFLOAT3(4.0f, -4.0f, 0.0f);  // Bottom right.
	vertices[3].texture = XMFLOAT2(0.5f, 0.66666f);
	vertices[3].normal = XMFLOAT3(0.0f, 0.0f, -1.0f);

	// Top face
	vertices[4].position = XMFLOAT3(-4.0f, 4.0f, 0.0f);  // Bottom left.
	vertices[4].texture = XMFLOAT2(0.25f, 0.33333f);
	vertices[4].normal = XMFLOAT3(0.0f, -1.0f, 0.0f);

	vertices[5].position = XMFLOAT3(-4.0f, 4.0f, -8.0f);  // Top left.
	vertices[5].texture = XMFLOAT2(0.25f, 0.0f);
	vertices[5].normal = XMFLOAT3(0.0f, -1.0f, 0.0f);

	vertices[6].position = XMFLOAT3(4.0f, 4.0f, -8.0f);  // Top right.
	vertices[6].texture = XMFLOAT2(0.5f, 0.0f);
	vertices[6].normal = XMFLOAT3(0.0f, -1.0f, 0.0f);

	vertices[7].position = XMFLOAT3(4.0f, 4.0f, 0.0f);  // Bottom right.
	vertices[7].texture = XMFLOAT2(0.5f, 0.33333f);
	vertices[7].normal = XMFLOAT3(0.0f, -1.0f, 0.0f);

	// Bottom face
	vertices[8].position = XMFLOAT3(-4.0f, -4.0f, -8.0f);  // Bottom left.
	vertices[8].texture = XMFLOAT2(0.25f, 1.0f);
	vertices[8].normal = XMFLOAT3(0.0f, 1.0f, 0.0f);

	vertices[9].position = XMFLOAT3(-4.0f, -4.0f, 0.0f);  // Top left.
	vertices[9].texture = XMFLOAT2(0.25f, 0.66666f);
	vertices[9].normal = XMFLOAT3(0.0f, 1.0f, 0.0f);

	vertices[10].position = XMFLOAT3(4.0f, -4.0f, 0.0f);  // Top right.
	vertices[10].texture = XMFLOAT2(0.5f, 0.66666f);
	vertices[10].normal = XMFLOAT3(0.0f, 1.0f, 0.0f);

	vertices[11].position = XMFLOAT3(4.0f, -4.0f, -8.0f);  // Bottom right.
	vertices[11].texture = XMFLOAT2(0.5f, 1.0f);
	vertices[11].normal = XMFLOAT3(0.0f, 1.0f, 0.0f);

	// Left face
	vertices[12].position = XMFLOAT3(-4.0f, -4.0f, -8.0f);  // Bottom left.
	vertices[12].texture = XMFLOAT2(0.0f, 0.66666f);
	vertices[12].normal = XMFLOAT3(1.0f, 0.0f, 0.0f);

	vertices[13].position = XMFLOAT3(-4.0f, 4.0f, -8.0f);  // Top left.
	vertices[13].texture = XMFLOAT2(0.0f, 0.33333f);
	vertices[13].normal = XMFLOAT3(1.0f, 0.0f, 0.0f);

	vertices[14].position = XMFLOAT3(-4.0f, 4.0f, 0.0f);  // Top right.
	vertices[14].texture = XMFLOAT2(0.25f, 0.33333f);
	vertices[14].normal = XMFLOAT3(1.0f, 0.0f, 0.0f);

	vertices[15].position = XMFLOAT3(-4.0f, -4.0f, 0.0f);  // Bottom right.
	vertices[15].texture = XMFLOAT2(0.25f, 0.66666f);
	vertices[15].normal = XMFLOAT3(1.0f, 0.0f, 0.0f);

	// Right face
	vertices[16].position = XMFLOAT3(4.0f, -4.0f, 0.0f);  // Bottom left.
	vertices[16].texture = XMFLOAT2(0.5f, 0.66666f);
	vertices[16].normal = XMFLOAT3(-1.0f, 0.0f, 0.0f);

	vertices[17].position = XMFLOAT3(4.0f, 4.0f, 0.0f);  // Top left.
	vertices[17].texture = XMFLOAT2(0.5f, 0.33333f);
	vertices[17].normal = XMFLOAT3(-1.0f, 0.0f, 0.0f);

	vertices[18].position = XMFLOAT3(4.0f, 4.0f, -8.0f);  // Top right.
	vertices[18].texture = XMFLOAT2(0.75f, 0.33333f);
	vertices[18].normal = XMFLOAT3(-1.0f, 0.0f, 0.0f);

	vertices[19].position = XMFLOAT3(4.0f, -4.0f, -8.0f);  // Bottom right.
	vertices[19].texture = XMFLOAT2(0.75f, 0.66666f);
	vertices[19].normal = XMFLOAT3(-1.0f, 0.0f, 0.0f);

	// Front face
	vertices[20].position = XMFLOAT3(4.0f, -4.0f, -8.0f);  // Bottom left.
	vertices[20].texture = XMFLOAT2(0.75f, 0.66666f);
	vertices[20].normal = XMFLOAT3(0.0f, 0.0f, 1.0f);

	vertices[21].position = XMFLOAT3(4.0f, 4.0f, -8.0f);  // Top left.
	vertices[21].texture = XMFLOAT2(0.75f, 0.33333f);
	vertices[21].normal = XMFLOAT3(0.0f, 0.0f, 1.0f);

	vertices[22].position = XMFLOAT3(-4.0f, 4.0f, -8.0f);  // Top right.
	vertices[22].texture = XMFLOAT2(1.0f, 0.33333f);
	vertices[22].normal = XMFLOAT3(0.0f, 0.0f, 1.0f);

	vertices[23].position = XMFLOAT3(-4.0f, -4.0f, -4.0f);  // Bottom right.
	vertices[23].texture = XMFLOAT2(1.0f, 0.66666f);
	vertices[23].normal = XMFLOAT3(0.0f, 0.0f, 1.0f);

	// 加载索引序列数据

	// Back face
	indices[0] = 0;  // Bottom left.
	indices[1] = 2;  // Top right.
	indices[2] = 1;  // Top left.

	indices[3] = 0;	// bottom left
	indices[4] = 3;	// bottom right
	indices[5] = 2;	// top right
	// Top face
	indices[6] = 4;  // Bottom left.
	indices[7] = 6;  // Top right.
	indices[8] = 5;  // Top left.

	indices[9] = 4;	// bottom left
	indices[10] = 7;	// bottom right
	indices[11] = 6;	// top right
	// Bottom face
	indices[12] = 8;  // Bottom left.
	indices[13] = 10;  // Top right.
	indices[14] = 9;  // Top left.

	indices[15] = 8;	// bottom left
	indices[16] = 11;	// bottom right
	indices[17] = 10;	// top right
	// Left face
	indices[18] = 12;  // Bottom left.
	indices[19] = 14;  // Top right.
	indices[20] = 13;  // Top left.

	indices[21] = 12;	// bottom left
	indices[22] = 15;	// bottom right
	indices[23] = 14;	// top right
	// Right face
	indices[24] = 16;  // Bottom left.
	indices[25] = 18;  // Top right.
	indices[26] = 17;  // Top left.

	indices[27] = 16;	// bottom left
	indices[28] = 19;	// bottom right
	indices[29] = 18;	// top right
	// Front face
	indices[30] = 20;  // Bottom left.
	indices[31] = 22;  // Top right.
	indices[32] = 21;  // Top left.

	indices[33] = 20;	// bottom left
	indices[34] = 23;	// bottom right
	indices[35] = 22;	// top right


	// 设置静态顶点缓冲区参数
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(VertexType)* m_vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	// 给子资源提供指针指向顶点数据
	vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	// 创建顶点缓冲区
	device->CreateBuffer(&vertexBufferDesc, &vertexData, &m_vertexBuffer);

	// 设置静态索引缓冲区参数
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long)* m_indexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	// 给子资源提供指针指向索引数据
	indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	// 创建索引缓冲区
	device->CreateBuffer(&indexBufferDesc, &indexData, &m_indexBuffer);

	// 释放缓冲区
	delete[] vertices;
	vertices = 0;

	delete[] indices;
	indices = 0;
}