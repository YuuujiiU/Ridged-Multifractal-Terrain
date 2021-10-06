#include "Patch.h"

Patch::Patch(ID3D11Device* device, ID3D11DeviceContext* deviceContext, WCHAR* textureFilename, float x, float z, float uvX, float uvY, float uvWidth, float uvHeight)
	: TessellationMesh(device, deviceContext, textureFilename)
{
	// 初始化保存三角形几何的顶点和索引缓冲区
	InitBuffers(device, x, z, uvX, uvY, uvWidth, uvHeight);

	// 加载模型纹理
	LoadTexture(device, deviceContext, textureFilename);

	// Fill permutation table
	perm = new int[256] { 151,160,137,91,90,15,
		131,13,201,95,96,53,194,233,7,225,140,36,103,30,69,142,8,99,37,240,21,10,23,
		190, 6,148,247,120,234,75,0,26,197,62,94,252,219,203,117,35,11,32,57,177,33,
		88,237,149,56,87,174,20,125,136,171,168, 68,175,74,165,71,134,139,48,27,166,
		77,146,158,231,83,111,229,122,60,211,133,230,220,105,92,41,55,46,245,40,244,
		102,143,54, 65,25,63,161, 1,216,80,73,209,76,132,187,208, 89,18,169,200,196,
		135,130,116,188,159,86,164,100,109,198,173,186, 3,64,52,217,226,250,124,123,
		5,202,38,147,118,126,255,82,85,212,207,206,59,227,47,16,58,17,182,189,28,42,
		223,183,170,213,119,248,152, 2,44,154,163, 70,221,153,101,155,167, 43,172,9,
		129,22,39,253, 19,98,108,110,79,113,224,232,178,185, 112,104,218,246,97,228,
		251,34,242,193,238,210,144,12,191,179,162,241, 81,51,145,235,249,14,239,107,
		49,192,214, 31,181,199,106,157,184, 84,204,176,115,121,50,45,127, 4,150,254,
		138,236,205,93,222,114,67,29,24,72,243,141,128,195,78,66,215,61,156,180
	};

	// 将以上permutation table置换
	DoublePerm();
}

Patch::~Patch()
{
	TessellationMesh::~TessellationMesh();

	delete[] perm;
}

void Patch::InitBuffers(ID3D11Device* device, float x, float z, float uvX, float uvY, float uvWidth, float uvHeight)
{
	// 右手坐标系
	//顶点，索引
	VType* vertices;
	unsigned long* indices;
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;
	FLOAT coordScale = 0.007, heightScale = 60.0f;

	// 设置顶点数组中的顶点数
	m_vertexCount = 4;

	// 设置索引数组中的索引数
	m_indexCount = 4;

	// 创建顶点数组
	vertices = new VType[m_vertexCount];

	// 创建索引数组
	indices = new unsigned long[m_indexCount];

	// 用数据加载顶点数组
	float vertYs[4];
	vertices[0].position = XMFLOAT3((x - PATCHWIDTH), 0.0f, (z - PATCHHEIGHT));  // Bottom left.
	vertYs[0] = (float)RidgedMultifractal(XMFLOAT3(vertices[0].position.x * coordScale, vertices[0].position.y * coordScale, vertices[0].position.z * coordScale), 1.6f, 2.0f, 8.0f, 1.0f, 2.0f) * heightScale;
	vertices[0].heightTex = XMFLOAT2(uvX, uvY + uvHeight);
	vertices[0].texture = XMFLOAT2(0.0f, 10.0f);
	vertices[0].normal = XMFLOAT3(0.0f, 1.0f, 0.0f);

	vertices[1].position = XMFLOAT3((x - PATCHWIDTH) , 0.0f, z);  // Top left.
	vertYs[1] = (float)RidgedMultifractal(XMFLOAT3(vertices[1].position.x * coordScale, vertices[1].position.y * coordScale, vertices[1].position.z * coordScale), 1.6f, 2.0f, 8.0f, 1.0f, 2.0f) * heightScale;
	vertices[1].heightTex = XMFLOAT2(uvX, uvY);
	vertices[1].texture = XMFLOAT2(0.0f, 0.0f);
	vertices[1].normal = XMFLOAT3(0.0f, 1.0f, 0.0f);

	vertices[2].position = XMFLOAT3(x, 0.0f, z);  // Top right.
	vertYs[2] = (float)RidgedMultifractal(XMFLOAT3(vertices[2].position.x * coordScale, vertices[2].position.y * coordScale, vertices[2].position.z * coordScale), 1.6f, 2.0f, 8.0f, 1.0f, 2.0f) * heightScale;
	vertices[2].heightTex = XMFLOAT2(uvX + uvWidth, uvY);
	vertices[2].texture = XMFLOAT2(10.0f, 0.0f);
	vertices[2].normal = XMFLOAT3(0.0f, 1.0f, 0.0f);

	vertices[3].position = XMFLOAT3(x, 0.0f, (z - PATCHHEIGHT));  // Bottom right.
	vertYs[3] = (float)RidgedMultifractal(XMFLOAT3(vertices[3].position.x * coordScale, vertices[3].position.y * coordScale, vertices[3].position.z * coordScale), 1.6f, 2.0f, 8.0f, 1.0f, 2.0f) * heightScale;
	vertices[3].heightTex = XMFLOAT2(uvX + uvWidth, uvY + uvHeight);
	vertices[3].texture = XMFLOAT2(10.0f, 10.0f);
	vertices[3].normal = XMFLOAT3(0.0f, 1.0f, 0.0f);

	// 用数据加载索引数组
	indices[0] = 1;  // Top left.
	indices[1] = 0;  // Bottom left.
	indices[2] = 2;  // Top Right.
	indices[3] = 3;	// bottom right

	// box values
	// Width
	boundingBox.maxX = vertices[2].position.x;
	boundingBox.minX = vertices[0].position.x;

	// Height
	float height = 0.0f;
	for (int i = 0; i < m_vertexCount; i++)
	{
		height = vertYs[i];

		if (height > boundingBox.maxY)
		{
			boundingBox.maxY = height;
		}
		if (height < boundingBox.minY)
		{
			boundingBox.minY = height;
		}
	}

	// Depth 
	boundingBox.maxZ = vertices[1].position.z;
	boundingBox.minZ = vertices[0].position.z;

	// 设置静态顶点缓冲区的说明
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(VType)* m_vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	// 给子资源提供指针指向顶点数据
	vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	// 创建顶点缓存区
	device->CreateBuffer(&vertexBufferDesc, &vertexData, &m_vertexBuffer);

	// 设置静态索引缓冲区的说明
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

void Patch::SendData(ID3D11DeviceContext* deviceContext)
{
	unsigned int stride;
	unsigned int offset;

	// 设置顶点缓冲区大小以及偏移量
	stride = sizeof(VType);
	offset = 0;

	// 设置顶点缓冲区为active，为输入管线做准备，IA render
	deviceContext->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);

	// 设置索引缓冲区为active，为输入管线做准备，IA render
	deviceContext->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	//设置基元类型应用于顶点缓冲区,为之后的细分做准备
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_4_CONTROL_POINT_PATCHLIST);

}
