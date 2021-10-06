// 在Mesh的基础上，建立Sphere Mesh(球体）
//之后用三角形面片绘制球体，立方体

#include "cubemesh.h"

CubeMesh::CubeMesh(ID3D11Device* device, ID3D11DeviceContext* deviceContext, WCHAR* textureFilename, int resolution)
{
	m_resolution = resolution;

	//初始化顶点和索引缓冲区，设施基础图形为三角形
	InitBuffers(device);

	//为此obj，加载纹理
	LoadTexture(device, deviceContext, textureFilename);
}


CubeMesh::~CubeMesh()
{
	BaseMesh::~BaseMesh();
}


//立方体Mesh
void CubeMesh::InitBuffers(ID3D11Device* device)
{
	VertexType* vertices;
	unsigned long* indices;
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;

	// 计算顶点数
	// 每个四边形6个顶点, res*res = face, 每面再乘以6
	// 初始化顶点数组中的顶点数

	m_vertexCount = ((6 * m_resolution)*m_resolution) * 6;


	//初始化索引数组中的索引数
	m_indexCount = m_vertexCount;

	// 创建顶点array
	vertices = new VertexType[m_vertexCount];

	// 创建索引array
	indices = new unsigned long[m_indexCount];

	// Vertex variables
	float yincrement = 2.0f / m_resolution;
	float xincrement = 2.0f / m_resolution;
	float ystart = 1.0f;
	float xstart = -1.0f;

	//UV variables
	float txu = 0.0f;
	float txv = 0.0f;
	float txuinc = 1.0f / m_resolution;	// UV increment
	float txvinc = 1.0f / m_resolution;

	//Counters
	int v = 0;	// vertex counter
	int i = 0;	// index counter

	//front face

	for (int y = 0; y<m_resolution; y++)	// for each quad in the y direction
	{
		for (int x = 0; x < m_resolution; x++)	// for each quad in the x direction
		{
			// Load the vertex array with data.
			//0
			vertices[v].position = XMFLOAT3(xstart, ystart - yincrement, -1.0f);  // Bottom left. -1. -1. 0
			vertices[v].texture = XMFLOAT2(txu, txv + txvinc);
			vertices[v].normal = XMFLOAT3(0.0f, 0.0f, -1.0f);

			indices[i] = i;
			v++;
			i++;

			//1
			vertices[v].position = XMFLOAT3(xstart + xincrement, ystart, -1.0f);  // Top right.	1.0, 1.0 0.0
			vertices[v].texture = XMFLOAT2(txu + txuinc, txv);
			vertices[v].normal = XMFLOAT3(0.0f, 0.0f, -1.0f);

			indices[i] = i;
			v++;
			i++;

			//2
			vertices[v].position = XMFLOAT3(xstart, ystart, -1.0f);  // Top left.	-1.0, 1.0
			vertices[v].texture = XMFLOAT2(txu, txv);
			vertices[v].normal = XMFLOAT3(0.0f, 0.0f, -1.0f);

			indices[i] = i;
			v++;
			i++;

			//0
			vertices[v].position = XMFLOAT3(xstart, ystart - yincrement, -1.0f);  // Bottom left. -1. -1. 0
			vertices[v].texture = XMFLOAT2(txu, txv + txvinc);
			vertices[v].normal = XMFLOAT3(0.0f, 0.0f, -1.0f);

			indices[i] = i;
			v++;
			i++;

			//3
			vertices[v].position = XMFLOAT3(xstart + xincrement, ystart - yincrement, -1.0f);  // Bottom right.	1.0, -1.0, 0.0
			vertices[v].texture = XMFLOAT2(txu + txuinc, txv + txvinc);
			vertices[v].normal = XMFLOAT3(0.0f, 0.0f, -1.0f);

			indices[i] = i;
			v++;
			i++;

			//1
			vertices[v].position = XMFLOAT3(xstart + xincrement, ystart, -1.0f);  // Top right.	1.0, 1.0 0.0
			vertices[v].texture = XMFLOAT2(txu + txuinc, txv);
			vertices[v].normal = XMFLOAT3(0.0f, 0.0f, -1.0f);

			indices[i] = i;
			v++;
			i++;

			// increment
			xstart += xincrement;
			txu += txuinc;
			//ystart -= yincrement;

		}

		ystart -= yincrement;
		xstart = -1;

		txu = 0;
		txv += txvinc;

	}

	txv = 0;

	//back face
	ystart = 1;
	xstart = 1;
	for (int y = 0; y<m_resolution; y++)	// for each quad in the y direction
	{
		for (int x = 0; x < m_resolution; x++)	// for each quad in the x direction
		{
			// Load the vertex array with data.
			//0
			vertices[v].position = XMFLOAT3(xstart, ystart - yincrement, 1.0f);  // Bottom left. -1. -1. 0
			vertices[v].texture = XMFLOAT2(txu, txv + txvinc);
			vertices[v].normal = XMFLOAT3(0.0f, 0.0f, 1.0f);

			indices[i] = i;
			v++;
			i++;

			//2
			vertices[v].position = XMFLOAT3(xstart - xincrement, ystart, 1.0f);  // Top right.	1.0, 1.0 0.0
			vertices[v].texture = XMFLOAT2(txu + txuinc, txv);
			vertices[v].normal = XMFLOAT3(0.0f, 0.0f, 1.0f);

			indices[i] = i;
			v++;
			i++;

			//1
			vertices[v].position = XMFLOAT3(xstart, ystart, 1.0f);  // Top left.	-1.0, 1.0
			vertices[v].texture = XMFLOAT2(txu, txv);
			vertices[v].normal = XMFLOAT3(0.0f, 0.0f, 1.0f);

			indices[i] = i;
			v++;
			i++;

			//0
			vertices[v].position = XMFLOAT3(xstart, ystart - yincrement, 1.0f);  // Bottom left. -1. -1. 0
			vertices[v].texture = XMFLOAT2(txu, txv + txvinc);
			vertices[v].normal = XMFLOAT3(0.0f, 0.0f, 1.0f);

			indices[i] = i;
			v++;
			i++;

			//3
			vertices[v].position = XMFLOAT3(xstart - xincrement, ystart - yincrement, 1.0f);  // Bottom right.	1.0, -1.0, 0.0
			vertices[v].texture = XMFLOAT2(txu + txuinc, txv + txvinc);
			vertices[v].normal = XMFLOAT3(0.0f, 0.0f, 1.0f);

			indices[i] = i;
			v++;
			i++;

			//2
			vertices[v].position = XMFLOAT3(xstart - xincrement, ystart, 1.0f);  // Top right.	1.0, 1.0 0.0
			vertices[v].texture = XMFLOAT2(txu + txuinc, txv);
			vertices[v].normal = XMFLOAT3(0.0f, 0.0f, 1.0f);

			indices[i] = i;
			v++;
			i++;

			// increment
			xstart -= xincrement;
			//ystart -= yincrement;
			txu += txuinc;

		}

		ystart -= yincrement;
		xstart = 1;

		txu = 0;
		txv += txvinc;

	}

	txv = 0;

	//right face
	ystart = 1;
	xstart = -1;
	for (int y = 0; y<m_resolution; y++)	// for each quad in the y direction
	{
		for (int x = 0; x < m_resolution; x++)	// for each quad in the x direction
		{
			// Load the vertex array with data.
			//0
			vertices[v].position = XMFLOAT3(1.0f, ystart - yincrement, xstart);  // Bottom left. -1. -1. 0
			vertices[v].texture = XMFLOAT2(txu, txv + txvinc);
			vertices[v].normal = XMFLOAT3(1.0f, 0.0f, 0.0f);

			indices[i] = i;
			v++;
			i++;

			//2
			vertices[v].position = XMFLOAT3(1.0f, ystart, xstart + xincrement);  // Top right.	1.0, 1.0 0.0
			vertices[v].texture = XMFLOAT2(txu + txuinc, txv);
			vertices[v].normal = XMFLOAT3(1.0f, 0.0f, 0.0f);

			indices[i] = i;
			v++;
			i++;

			//1
			vertices[v].position = XMFLOAT3(1.0f, ystart, xstart);  // Top left.	-1.0, 1.0
			vertices[v].texture = XMFLOAT2(txu, txv);
			vertices[v].normal = XMFLOAT3(1.0f, 0.0f, 0.0f);

			indices[i] = i;
			v++;
			i++;

			//0
			vertices[v].position = XMFLOAT3(1.0f, ystart - yincrement, xstart);  // Bottom left. -1. -1. 0
			vertices[v].texture = XMFLOAT2(txu, txv + txvinc);
			vertices[v].normal = XMFLOAT3(1.0f, 0.0f, 0.0f);

			indices[i] = i;
			v++;
			i++;

			//3
			vertices[v].position = XMFLOAT3(1.0f, ystart - yincrement, xstart + xincrement);  // Bottom right.	1.0, -1.0, 0.0
			vertices[v].texture = XMFLOAT2(txu + txuinc, txv + txvinc);
			vertices[v].normal = XMFLOAT3(1.0f, 0.0f, 0.0f);

			indices[i] = i;
			v++;
			i++;

			//2
			vertices[v].position = XMFLOAT3(1.0f, ystart, xstart + xincrement);  // Top right.	1.0, 1.0 0.0
			vertices[v].texture = XMFLOAT2(txu + txuinc, txv);
			vertices[v].normal = XMFLOAT3(1.0f, 0.0f, 0.0f);

			indices[i] = i;
			v++;
			i++;

			// increment
			xstart += xincrement;
			//ystart -= yincrement;
			txu += txuinc;

		}

		ystart -= yincrement;
		xstart = -1;
		txu = 0;
		txv += txvinc;
	}

	txv = 0;

	//left face
	ystart = 1;
	xstart = 1;
	for (int y = 0; y<m_resolution; y++)	// for each quad in the y direction
	{
		for (int x = 0; x < m_resolution; x++)	// for each quad in the x direction
		{
			// Load the vertex array with data.
			//0
			vertices[v].position = XMFLOAT3(-1.0f, ystart - yincrement, xstart);  // Bottom left. -1. -1. 0
			vertices[v].texture = XMFLOAT2(txu, txv + txvinc);
			vertices[v].normal = XMFLOAT3(-1.0f, 0.0f, 0.0f);

			indices[i] = i;
			v++;
			i++;

			//2
			vertices[v].position = XMFLOAT3(-1.0f, ystart, xstart - xincrement);  // Top right.	1.0, 1.0 0.0
			vertices[v].texture = XMFLOAT2(txu + txuinc, txv);
			vertices[v].normal = XMFLOAT3(-1.0f, 0.0f, 0.0f);

			indices[i] = i;
			v++;
			i++;

			//1
			vertices[v].position = XMFLOAT3(-1.0f, ystart, xstart);  // Top left.	-1.0, 1.0
			vertices[v].texture = XMFLOAT2(txu, txv);
			vertices[v].normal = XMFLOAT3(-1.0f, 0.0f, 0.0f);

			indices[i] = i;
			v++;
			i++;

			//0
			vertices[v].position = XMFLOAT3(-1.0f, ystart - yincrement, xstart);  // Bottom left. -1. -1. 0
			vertices[v].texture = XMFLOAT2(txu, txv + txvinc);
			vertices[v].normal = XMFLOAT3(-1.0f, 0.0f, 0.0f);

			indices[i] = i;
			v++;
			i++;

			//3
			vertices[v].position = XMFLOAT3(-1.0f, ystart - yincrement, xstart - xincrement);  // Bottom right.	1.0, -1.0, 0.0
			vertices[v].texture = XMFLOAT2(txu + txuinc, txv + txvinc);
			vertices[v].normal = XMFLOAT3(-1.0f, 0.0f, 0.0f);

			indices[i] = i;
			v++;
			i++;

			//2
			vertices[v].position = XMFLOAT3(-1.0f, ystart, xstart - xincrement);  // Top right.	1.0, 1.0 0.0
			vertices[v].texture = XMFLOAT2(txu + txuinc, txv);
			vertices[v].normal = XMFLOAT3(-1.0f, 0.0f, 0.0f);

			indices[i] = i;
			v++;
			i++;

			// increment
			xstart -= xincrement;
			//ystart -= yincrement;
			txu += txuinc;
		}

		ystart -= yincrement;
		xstart = 1;
		txu = 0;
		txv += txvinc;
	}

	txv = 0;

	//top face
	ystart = 1;
	xstart = -1;

	for (int y = 0; y<m_resolution; y++)	// for each quad in the y direction
	{
		for (int x = 0; x < m_resolution; x++)	// for each quad in the x direction
		{
			// Load the vertex array with data.
			//0
			vertices[v].position = XMFLOAT3(xstart, 1.0f, ystart - yincrement);  // Bottom left. -1. -1. 0
			vertices[v].texture = XMFLOAT2(txu, txv + txvinc);
			vertices[v].normal = XMFLOAT3(0.0f, 1.0f, 0.0f);

			indices[i] = i;
			v++;
			i++;

			//2
			vertices[v].position = XMFLOAT3(xstart + xincrement, 1.0f, ystart);  // Top right.	1.0, 1.0 0.0
			vertices[v].texture = XMFLOAT2(txu + txuinc, txv);
			vertices[v].normal = XMFLOAT3(0.0f, 1.0f, 0.0f);

			indices[i] = i;
			v++;
			i++;

			//1
			vertices[v].position = XMFLOAT3(xstart, 1.0f, ystart);  // Top left.	-1.0, 1.0
			vertices[v].texture = XMFLOAT2(txu, txv);
			vertices[v].normal = XMFLOAT3(0.0f, 1.0f, 0.0f);

			indices[i] = i;
			v++;
			i++;

			//0
			vertices[v].position = XMFLOAT3(xstart, 1.0f, ystart - yincrement);  // Bottom left. -1. -1. 0
			vertices[v].texture = XMFLOAT2(txu, txv + txvinc);
			vertices[v].normal = XMFLOAT3(0.0f, 1.0f, 0.0f);

			indices[i] = i;
			v++;
			i++;

			//3
			vertices[v].position = XMFLOAT3(xstart + xincrement, 1.0f, ystart - yincrement);  // Bottom right.	1.0, -1.0, 0.0
			vertices[v].texture = XMFLOAT2(txu + txuinc, txv + txvinc);
			vertices[v].normal = XMFLOAT3(0.0f, 1.0f, 0.0f);

			indices[i] = i;
			v++;
			i++;

			//2
			vertices[v].position = XMFLOAT3(xstart + xincrement, 1.0f, ystart);  // Top right.	1.0, 1.0 0.0
			vertices[v].texture = XMFLOAT2(txu + txuinc, txv);
			vertices[v].normal = XMFLOAT3(0.0f, 1.0f, 0.0f);

			indices[i] = i;
			v++;
			i++;

			// increment
			xstart += xincrement;
			//ystart -= yincrement;
			txu += txuinc;
		}

		ystart -= yincrement;
		xstart = -1;
		txu = 0;
		txv += txvinc;
	}

	txv = 0;

	//bottom face
	ystart = -1;
	xstart = -1;

	for (int y = 0; y<m_resolution; y++)	// for each quad in the y direction
	{
		for (int x = 0; x < m_resolution; x++)	// for each quad in the x direction
		{
			// Load the vertex array with data.
			//0
			vertices[v].position = XMFLOAT3(xstart, -1.0f, ystart + yincrement);  // Bottom left. -1. -1. 0
			vertices[v].texture = XMFLOAT2(txu, txv + txvinc);
			vertices[v].normal = XMFLOAT3(0.0f, -1.0f, 0.0f);

			indices[i] = i;
			v++;
			i++;

			//2
			vertices[v].position = XMFLOAT3(xstart + xincrement, -1.0f, ystart);  // Top right.	1.0, 1.0 0.0
			vertices[v].texture = XMFLOAT2(txu + txuinc, txv);
			vertices[v].normal = XMFLOAT3(0.0f, -1.0f, 0.0f);

			indices[i] = i;
			v++;
			i++;

			//1
			vertices[v].position = XMFLOAT3(xstart, -1.0f, ystart);  // Top left.	-1.0, 1.0
			vertices[v].texture = XMFLOAT2(txu, txv);
			vertices[v].normal = XMFLOAT3(0.0f, -1.0f, 0.0f);

			indices[i] = i;
			v++;
			i++;

			//0
			vertices[v].position = XMFLOAT3(xstart, -1.0f, ystart + yincrement);  // Bottom left. -1. -1. 0
			vertices[v].texture = XMFLOAT2(txu, txv + txvinc);
			vertices[v].normal = XMFLOAT3(0.0f, -1.0f, 0.0f);

			indices[i] = i;
			v++;
			i++;

			//3
			vertices[v].position = XMFLOAT3(xstart + xincrement, -1.0f, ystart + yincrement);  // Bottom right.	1.0, -1.0, 0.0
			vertices[v].texture = XMFLOAT2(txu + txuinc, txv + txvinc);
			vertices[v].normal = XMFLOAT3(0.0f, -1.0f, 0.0f);

			indices[i] = i;
			v++;
			i++;

			//2
			vertices[v].position = XMFLOAT3(xstart + xincrement, -1.0f, ystart);  // Top right.	1.0, 1.0 0.0
			vertices[v].texture = XMFLOAT2(txu + txuinc, txv);
			vertices[v].normal = XMFLOAT3(0.0f, -1.0f, 0.0f);

			indices[i] = i;
			v++;
			i++;

			// increment
			xstart += xincrement;
			//ystart -= yincrement;
			txu += txuinc;
		}

		ystart += yincrement;
		xstart = -1;
		txu = 0;
		txv += txvinc;
	}

	
	// Set up the description of the static vertex buffer.
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(VertexType)* m_vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the vertex data.
	vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	// Now create the vertex buffer.
	device->CreateBuffer(&vertexBufferDesc, &vertexData, &m_vertexBuffer);

	// Set up the description of the static index buffer.
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long)* m_indexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the index data.
	indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	// Create the index buffer.
	device->CreateBuffer(&indexBufferDesc, &indexData, &m_indexBuffer);

	// Release the arrays now that the vertex and index buffers have been created and loaded.
	delete[] vertices;
	vertices = 0;

	delete[] indices;
	indices = 0;
}

