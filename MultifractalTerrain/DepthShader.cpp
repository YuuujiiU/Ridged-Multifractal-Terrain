// depth shader.cpp
#include "depthshader.h"


DepthShader::DepthShader(ID3D11Device* device, HWND hwnd) : BaseShader(device, hwnd)
{
	InitShader(L"shaders/depth_vs.hlsl", L"shaders/depth_hs.hlsl", L"shaders/depth_ds.hlsl", L"shaders/depth_ps.hlsl");
}


DepthShader::~DepthShader()
{
	// 释放矩阵常数缓冲区
	if (m_matrixBuffer)
	{
		m_matrixBuffer->Release();
		m_matrixBuffer = 0;
	}

	// 释放布局
	if (m_layout)
	{
		m_layout->Release();
		m_layout = 0;
	}

	// 释放tess因子常量缓冲区
	if (m_TessDistanceBuffer)
	{
		m_TessDistanceBuffer->Release();
		m_TessDistanceBuffer = 0;
	}

	// 释放摄影机常量缓冲区
	if (m_CameraBuffer)
	{
		m_CameraBuffer->Release();
		m_CameraBuffer = 0;
	}

	BaseShader::~BaseShader();
}


//InitShader
void DepthShader::InitShader(WCHAR* vsFilename, WCHAR* psFilename)
{
	D3D11_BUFFER_DESC matrixBufferDesc;
	D3D11_BUFFER_DESC cameraBufferDesc;
	D3D11_BUFFER_DESC multifractalBufferDesc;
	D3D11_BUFFER_DESC tessDistanceBufferDesc;

	//加载文件
	// Load (+ compile) shader files
	loadVertexShader(vsFilename);
	loadPixelShader(psFilename);


	//设置顶点着色器中动态矩阵常量缓冲区的说明（VS）
	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.ByteWidth = sizeof(MatrixBufferType);
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;


	//设置外壳着色器的相机缓冲区的说明（HS）
	cameraBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	cameraBufferDesc.ByteWidth = sizeof(CameraBufferType);
	cameraBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cameraBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cameraBufferDesc.MiscFlags = 0;
	cameraBufferDesc.StructureByteStride = 0;

	m_device->CreateBuffer(&cameraBufferDesc, NULL, &m_CameraBuffer);


	//创建常量缓冲区指针, 以便从此类中访问顶点着色器常量缓冲区
	m_device->CreateBuffer(&matrixBufferDesc, NULL, &m_matrixBuffer);

	//设置外壳着色器的动态细分因子(tess)缓冲区
	tessDistanceBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	tessDistanceBufferDesc.ByteWidth = sizeof(TessDistanceBufferType);
	tessDistanceBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	tessDistanceBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	tessDistanceBufferDesc.MiscFlags = 0;
	tessDistanceBufferDesc.StructureByteStride = 0;

	m_device->CreateBuffer(&tessDistanceBufferDesc, NULL, &m_TessDistanceBuffer);


	// multifractal buffer
	//设置域着色器的多重分形(multi)缓冲区的说明(DS)
	multifractalBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	multifractalBufferDesc.ByteWidth = sizeof(TessellationShader::MultifractalBufferType);
	multifractalBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	multifractalBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	multifractalBufferDesc.MiscFlags = 0;
	multifractalBufferDesc.StructureByteStride = 0;

	m_device->CreateBuffer(&multifractalBufferDesc, NULL, &m_MultifractalBuffer);

}

//重写InitShader
void DepthShader::InitShader(WCHAR* vsFilename, WCHAR* hsFilename, WCHAR* dsFilename, WCHAR* psFilename)
{
	//InitShader 必须被覆盖, 它将同时加载顶点和像素着色器 + 设置缓冲区
	InitShader(vsFilename, psFilename);

	// 加载Shaders
	loadHullShader(hsFilename);
	loadDomainShader(dsFilename);

}


void DepthShader::SetShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX &worldMatrix, const XMMATRIX &viewMatrix, const XMMATRIX &projectionMatrix,
									  Camera* camera, TessellationShader::MultifractalBufferType multifractalData, float tessDistNear, float tessDistFar)
{

	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	MatrixBufferType* dataPtr;
	CameraBufferType* cameraPtr;
	TessDistanceBufferType* tessPtr;
	TessellationShader::MultifractalBufferType* multifracPtr;
	unsigned int bufferNumber;
	XMMATRIX tworld, tview, tproj;

	//准备矩阵
	// 转换矩阵，为着色器做准备
	tworld = XMMatrixTranspose(worldMatrix);
	tview = XMMatrixTranspose(viewMatrix);
	tproj = XMMatrixTranspose(projectionMatrix);

	// 锁定常量缓冲区, 以便将其写入
	//mappedResource常量缓冲区
	result = deviceContext->Map(m_matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);

	// 获取指向常量缓冲区中数据的指针
	dataPtr = (MatrixBufferType*)mappedResource.pData;

	// 将矩阵复制到常量缓冲区中
	dataPtr->world = tworld;// worldMatrix;
	dataPtr->view = tview;
	dataPtr->projection = tproj;

	// 解锁常量缓冲区
	deviceContext->Unmap(m_matrixBuffer, 0);

	// 设置常量缓冲区在顶点着色器中的位置
	bufferNumber = 0;

	//使用更新的值设置域着色器中的常量缓冲区
	deviceContext->DSSetConstantBuffers(bufferNumber, 1, &m_matrixBuffer);

	// 传递摄影机数据进外壳着色器
	deviceContext->Map(m_CameraBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	cameraPtr = (CameraBufferType*)mappedResource.pData;
	cameraPtr->cameraPos = camera->GetPosition();
	cameraPtr->camPadding = FLOAT(1.0f);
	deviceContext->Unmap(m_CameraBuffer, 0);
	bufferNumber = 0;
	deviceContext->HSSetConstantBuffers(bufferNumber, 1, &m_CameraBuffer);

	// 传递细分数据数据进外壳着色器
	deviceContext->Map(m_TessDistanceBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	tessPtr = (TessDistanceBufferType*)mappedResource.pData;
	tessPtr->tessFar = tessDistFar;
	tessPtr->tessNear = tessDistNear;
	tessPtr->padding = XMFLOAT2(1.0f, 1.0f);
	deviceContext->Unmap(m_TessDistanceBuffer, 0);
	bufferNumber = 1;
	deviceContext->HSSetConstantBuffers(bufferNumber, 1, &m_TessDistanceBuffer);

	//传递multifractal data入域着色器
	deviceContext->Map(m_MultifractalBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	multifracPtr = (TessellationShader::MultifractalBufferType*)mappedResource.pData;
	multifracPtr->H = multifractalData.H;
	multifracPtr->lacunarity = multifractalData.lacunarity;
	multifracPtr->octaves = multifractalData.octaves;
	multifracPtr->offset = multifractalData.offset;
	multifracPtr->gain = multifractalData.gain;
	multifracPtr->padding = XMFLOAT3(0, 0, 0);
	deviceContext->Unmap(m_MultifractalBuffer, 0);
	bufferNumber = 1;
	deviceContext->DSSetConstantBuffers(bufferNumber, 1, &m_MultifractalBuffer);

}



