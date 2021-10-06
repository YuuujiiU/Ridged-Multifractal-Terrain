// tessellation shader.cpp
#include "tessellationshader.h"


TessellationShader::TessellationShader(ID3D11Device* device, HWND hwnd) : BaseShader(device, hwnd)
{
	InitShader(L"shaders/tessellation_vs.hlsl", L"shaders/tessellation_hs.hlsl", L"shaders/tessellation_ds.hlsl", L"shaders/tessellation_ps.hlsl");
}


TessellationShader::~TessellationShader()
{
	// 释放采样器
	if (m_sampleState)
	{
		m_sampleState->Release();
		m_sampleState = 0;
	}

	// 释放矩阵常量缓冲区
	if (m_matrixBuffer)
	{
		m_matrixBuffer->Release();
		m_matrixBuffer = 0;
	}

	// 释放tess factor常量缓冲区
	if (m_TessDistanceBuffer)
	{		  
		m_TessDistanceBuffer->Release();
		m_TessDistanceBuffer = 0;
	}

	// 释放相机常量缓冲区
	if (m_CameraBuffer)
	{
		m_CameraBuffer->Release();
		m_CameraBuffer = 0;
	}

	// 释放光照常量缓冲区
	if (m_MoonlightBuffer)
	{
		m_MoonlightBuffer->Release();
		m_MoonlightBuffer = 0;
	}

	// 释放multifractal常量缓冲区
	if (m_MultifractalBuffer)
	{
		m_MultifractalBuffer->Release();
		m_MultifractalBuffer = 0;
	}

	// 释放布局
	if (m_layout)
	{
		m_layout->Release();
		m_layout = 0;
	}

	// 释放基本着色器组件
	BaseShader::~BaseShader();
}

void TessellationShader::InitShader(WCHAR* vsFilename,  WCHAR* psFilename)
{
	D3D11_BUFFER_DESC matrixBufferDesc;
	D3D11_SAMPLER_DESC samplerDesc;
	D3D11_BUFFER_DESC tessDistanceBufferDesc;
	D3D11_BUFFER_DESC cameraBufferDesc;
	D3D11_BUFFER_DESC moonlightBufferDesc;
	D3D11_BUFFER_DESC moonlightBufferDesc2;
	D3D11_BUFFER_DESC multifractalBufferDesc;

	// 加载编译着色器文件
	loadVertexShader(vsFilename);
	loadPixelShader(psFilename);

	// 设置顶点着色器中动态矩阵常量缓冲区的说明
	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.ByteWidth = sizeof(MatrixBufferType);
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;

	// 创建常量缓冲区指针, 以便从此类中访问顶点着色器常量缓冲区
	m_device->CreateBuffer(&matrixBufferDesc, NULL, &m_matrixBuffer);

	// 创建纹理采样器的状态描述
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 1;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.BorderColor[0] = 0;
	samplerDesc.BorderColor[1] = 0;
	samplerDesc.BorderColor[2] = 0;
	samplerDesc.BorderColor[3] = 0;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	// 创建纹理采样器状态
	m_device->CreateSamplerState(&samplerDesc, &m_sampleState);

	// 采用深度图所需的 CLAMPED 采样器
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;

	// 创建纹理采样器状态参数
	m_device->CreateSamplerState(&samplerDesc, &m_sampleStateClamp);

	// 设置外壳着色器的tessellation因子缓冲区的说明
	tessDistanceBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	tessDistanceBufferDesc.ByteWidth = sizeof(TessDistanceBufferType);
	tessDistanceBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	tessDistanceBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	tessDistanceBufferDesc.MiscFlags = 0;
	tessDistanceBufferDesc.StructureByteStride = 0;

	m_device->CreateBuffer(&tessDistanceBufferDesc, NULL, &m_TessDistanceBuffer);

	// 设置外壳着色器的相机缓冲区的说明
	cameraBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	cameraBufferDesc.ByteWidth = sizeof(CameraBufferType);
	cameraBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cameraBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cameraBufferDesc.MiscFlags = 0;
	cameraBufferDesc.StructureByteStride = 0;

	m_device->CreateBuffer(&cameraBufferDesc, NULL, &m_CameraBuffer);

	// 设置像素着色器的moonlight缓冲区的说明
	moonlightBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	moonlightBufferDesc.ByteWidth = sizeof(LightBufferType);
	moonlightBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	moonlightBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	moonlightBufferDesc.MiscFlags = 0;
	moonlightBufferDesc.StructureByteStride = 0;

	m_device->CreateBuffer(&moonlightBufferDesc, NULL, &m_MoonlightBuffer);

	// 设置顶点着色器中的相机动态常量缓冲区的说明
	moonlightBufferDesc2.Usage = D3D11_USAGE_DYNAMIC;
	moonlightBufferDesc2.ByteWidth = sizeof(LightBufferType2);
	moonlightBufferDesc2.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	moonlightBufferDesc2.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	moonlightBufferDesc2.MiscFlags = 0;
	moonlightBufferDesc2.StructureByteStride = 0;

	// 创建相机常量缓冲区指针, 以便从此类中访问顶点着色器常量缓冲区 vertex shader constant buffer from within this class.
	m_device->CreateBuffer(&moonlightBufferDesc2, NULL, &m_MoonlightBuffer2);

	// 设置域着色器的multifractal缓冲区的说明
	multifractalBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	multifractalBufferDesc.ByteWidth = sizeof(MultifractalBufferType);
	multifractalBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	multifractalBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	multifractalBufferDesc.MiscFlags = 0;
	multifractalBufferDesc.StructureByteStride = 0;

	m_device->CreateBuffer(&multifractalBufferDesc, NULL, &m_MultifractalBuffer);


	// 创建纹理采样器
//	m_device->CreateSamplerState(&samplerDesc, &m_sampleState);
}

void TessellationShader::InitShader(WCHAR* vsFilename, WCHAR* hsFilename, WCHAR* dsFilename, WCHAR* psFilename)
{
	// 重写InitShader()，并重新加载顶点着色器和像素着色器
	InitShader(vsFilename, psFilename);

	// 加载其他所需shaders
	loadHullShader(hsFilename);
	loadDomainShader(dsFilename);
}


void TessellationShader::SetShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX &worldMatrix, const XMMATRIX &viewMatrix, const XMMATRIX &projectionMatrix, 
												ID3D11ShaderResourceView* rockTexture, ID3D11ShaderResourceView* snowTexture, ID3D11ShaderResourceView*depthMap,
												float tessDistNear, float tessDistFar, Camera* camera, Light* moonlight, MultifractalBufferType multifractalData, bool fog)
{
	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	MatrixBufferType* dataPtr;
	TessDistanceBufferType* tessPtr;
	CameraBufferType* cameraPtr;
	LightBufferType* lightPtr;
	LightBufferType2* lightPtr2;
	MultifractalBufferType* multifracPtr;
	unsigned int bufferNumber;
	XMMATRIX tworld, tview, tproj, tLightViewMatrix, tLightProjectionMatrix;

	// 转换矩阵为着色器做准备
	tworld = XMMatrixTranspose(worldMatrix);
	tview = XMMatrixTranspose(viewMatrix);
	tproj = XMMatrixTranspose(projectionMatrix);
	tLightViewMatrix = XMMatrixTranspose(moonlight->GetViewMatrix());
	tLightProjectionMatrix = XMMatrixTranspose(moonlight->GetProjectionMatrix());

	// 锁定常量缓冲区以便写入
	result = deviceContext->Map(m_matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);

	// 获取指向常量缓冲区中的数据的指针
	dataPtr = (MatrixBufferType*)mappedResource.pData;

	// 将矩阵复制到常量缓冲区中
	dataPtr->world = tworld;// 世界矩阵;
	dataPtr->view = tview;
	dataPtr->projection = tproj;
	dataPtr->lightView = tLightViewMatrix;
	dataPtr->lightProjection = tLightProjectionMatrix;

	// 解锁常量缓冲区
	deviceContext->Unmap(m_matrixBuffer, 0);

	// 设置常量缓冲区在顶点着色器中的位置
	bufferNumber = 0;

	// 改用更新的值设置域着色器中的常量缓冲区
	deviceContext->DSSetConstantBuffers(bufferNumber, 1, &m_matrixBuffer);

	// 将tessellation数据发送为外壳着色器
	deviceContext->Map(m_TessDistanceBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	tessPtr = (TessDistanceBufferType*)mappedResource.pData;
	tessPtr->tessFar = tessDistFar;
	tessPtr->tessNear = tessDistNear;
	tessPtr->padding = XMFLOAT2(1.0f, 1.0f);
	deviceContext->Unmap(m_TessDistanceBuffer, 0);
	bufferNumber = 0;
	deviceContext->HSSetConstantBuffers(bufferNumber, 1, &m_TessDistanceBuffer);

	// 将相机数据发送到外壳和像素着色器
	deviceContext->Map(m_CameraBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	cameraPtr = (CameraBufferType*)mappedResource.pData;
	cameraPtr->cameraPos = camera->GetPosition();
	if (fog)
		cameraPtr->camPadding = FLOAT(1.0f);
	else
		cameraPtr->camPadding = FLOAT(0.0f);
	deviceContext->Unmap(m_CameraBuffer, 0);
	bufferNumber = 1;
	deviceContext->HSSetConstantBuffers(bufferNumber, 1, &m_CameraBuffer);
	deviceContext->PSSetConstantBuffers(bufferNumber, 1, &m_CameraBuffer);

	// 将光数据发送到像素着色器
	deviceContext->Map(m_MoonlightBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	lightPtr = (LightBufferType*)mappedResource.pData;
	lightPtr->diffuse = moonlight->GetDiffuseColour();
	lightPtr->ambient = moonlight->GetAmbientColour();
	lightPtr->position.x = moonlight->GetPosition().x;
	lightPtr->position.y = moonlight->GetPosition().y;
	lightPtr->position.z = moonlight->GetPosition().z;
	lightPtr->position.w = 0.0f;
	deviceContext->Unmap(m_MoonlightBuffer, 0);
	bufferNumber = 0;
	deviceContext->PSSetConstantBuffers(bufferNumber, 1, &m_MoonlightBuffer);

	// 将光数据发送到顶点着色器
	deviceContext->Map(m_MoonlightBuffer2, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	lightPtr2 = (LightBufferType2*)mappedResource.pData;
	lightPtr2->position = moonlight->GetPosition();
	lightPtr2->padding = 1.0f;
	deviceContext->Unmap(m_MoonlightBuffer2, 0);
	bufferNumber = 1;
	deviceContext->DSSetConstantBuffers(bufferNumber, 1, &m_MoonlightBuffer2);

	// 将multifractal数据发送到域着色器
	deviceContext->Map(m_MultifractalBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	multifracPtr = (MultifractalBufferType*)mappedResource.pData;
	multifracPtr->H = multifractalData.H; 
	multifracPtr->lacunarity = multifractalData.lacunarity;
	multifracPtr->octaves = multifractalData.octaves;
	multifracPtr->offset = multifractalData.offset;
	multifracPtr->gain = multifractalData.gain;
	multifracPtr->padding = XMFLOAT3(0, 0, 0);
	deviceContext->Unmap(m_MultifractalBuffer, 0);
	bufferNumber = 2;
	deviceContext->DSSetConstantBuffers(bufferNumber, 1, &m_MultifractalBuffer);

	// 在顶点、域和像素着色器中设置着色器纹理资源
	deviceContext->PSSetShaderResources(0, 1, &rockTexture);
	deviceContext->PSSetShaderResources(1, 1, &snowTexture);
	deviceContext->PSSetShaderResources(3, 1, &depthMap);
}

void TessellationShader::Render(ID3D11DeviceContext* deviceContext, int indexCount)
{
	// 在像素着色器中设置采样器状态
	deviceContext->PSSetSamplers(0, 1, &m_sampleState);
	deviceContext->PSSetSamplers(1, 1, &m_sampleStateClamp);

	// Base render function.
	BaseShader::Render(deviceContext, indexCount);
}



