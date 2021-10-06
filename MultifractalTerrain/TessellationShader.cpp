// tessellation shader.cpp
#include "tessellationshader.h"


TessellationShader::TessellationShader(ID3D11Device* device, HWND hwnd) : BaseShader(device, hwnd)
{
	InitShader(L"shaders/tessellation_vs.hlsl", L"shaders/tessellation_hs.hlsl", L"shaders/tessellation_ds.hlsl", L"shaders/tessellation_ps.hlsl");
}


TessellationShader::~TessellationShader()
{
	// �ͷŲ�����
	if (m_sampleState)
	{
		m_sampleState->Release();
		m_sampleState = 0;
	}

	// �ͷž�����������
	if (m_matrixBuffer)
	{
		m_matrixBuffer->Release();
		m_matrixBuffer = 0;
	}

	// �ͷ�tess factor����������
	if (m_TessDistanceBuffer)
	{		  
		m_TessDistanceBuffer->Release();
		m_TessDistanceBuffer = 0;
	}

	// �ͷ��������������
	if (m_CameraBuffer)
	{
		m_CameraBuffer->Release();
		m_CameraBuffer = 0;
	}

	// �ͷŹ��ճ���������
	if (m_MoonlightBuffer)
	{
		m_MoonlightBuffer->Release();
		m_MoonlightBuffer = 0;
	}

	// �ͷ�multifractal����������
	if (m_MultifractalBuffer)
	{
		m_MultifractalBuffer->Release();
		m_MultifractalBuffer = 0;
	}

	// �ͷŲ���
	if (m_layout)
	{
		m_layout->Release();
		m_layout = 0;
	}

	// �ͷŻ�����ɫ�����
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

	// ���ر�����ɫ���ļ�
	loadVertexShader(vsFilename);
	loadPixelShader(psFilename);

	// ���ö�����ɫ���ж�̬��������������˵��
	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.ByteWidth = sizeof(MatrixBufferType);
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;

	// ��������������ָ��, �Ա�Ӵ����з��ʶ�����ɫ������������
	m_device->CreateBuffer(&matrixBufferDesc, NULL, &m_matrixBuffer);

	// ���������������״̬����
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

	// �������������״̬
	m_device->CreateSamplerState(&samplerDesc, &m_sampleState);

	// �������ͼ����� CLAMPED ������
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;

	// �������������״̬����
	m_device->CreateSamplerState(&samplerDesc, &m_sampleStateClamp);

	// ���������ɫ����tessellation���ӻ�������˵��
	tessDistanceBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	tessDistanceBufferDesc.ByteWidth = sizeof(TessDistanceBufferType);
	tessDistanceBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	tessDistanceBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	tessDistanceBufferDesc.MiscFlags = 0;
	tessDistanceBufferDesc.StructureByteStride = 0;

	m_device->CreateBuffer(&tessDistanceBufferDesc, NULL, &m_TessDistanceBuffer);

	// ���������ɫ���������������˵��
	cameraBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	cameraBufferDesc.ByteWidth = sizeof(CameraBufferType);
	cameraBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cameraBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cameraBufferDesc.MiscFlags = 0;
	cameraBufferDesc.StructureByteStride = 0;

	m_device->CreateBuffer(&cameraBufferDesc, NULL, &m_CameraBuffer);

	// ����������ɫ����moonlight��������˵��
	moonlightBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	moonlightBufferDesc.ByteWidth = sizeof(LightBufferType);
	moonlightBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	moonlightBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	moonlightBufferDesc.MiscFlags = 0;
	moonlightBufferDesc.StructureByteStride = 0;

	m_device->CreateBuffer(&moonlightBufferDesc, NULL, &m_MoonlightBuffer);

	// ���ö�����ɫ���е������̬������������˵��
	moonlightBufferDesc2.Usage = D3D11_USAGE_DYNAMIC;
	moonlightBufferDesc2.ByteWidth = sizeof(LightBufferType2);
	moonlightBufferDesc2.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	moonlightBufferDesc2.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	moonlightBufferDesc2.MiscFlags = 0;
	moonlightBufferDesc2.StructureByteStride = 0;

	// �����������������ָ��, �Ա�Ӵ����з��ʶ�����ɫ������������ vertex shader constant buffer from within this class.
	m_device->CreateBuffer(&moonlightBufferDesc2, NULL, &m_MoonlightBuffer2);

	// ��������ɫ����multifractal��������˵��
	multifractalBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	multifractalBufferDesc.ByteWidth = sizeof(MultifractalBufferType);
	multifractalBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	multifractalBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	multifractalBufferDesc.MiscFlags = 0;
	multifractalBufferDesc.StructureByteStride = 0;

	m_device->CreateBuffer(&multifractalBufferDesc, NULL, &m_MultifractalBuffer);


	// �������������
//	m_device->CreateSamplerState(&samplerDesc, &m_sampleState);
}

void TessellationShader::InitShader(WCHAR* vsFilename, WCHAR* hsFilename, WCHAR* dsFilename, WCHAR* psFilename)
{
	// ��дInitShader()�������¼��ض�����ɫ����������ɫ��
	InitShader(vsFilename, psFilename);

	// ������������shaders
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

	// ת������Ϊ��ɫ����׼��
	tworld = XMMatrixTranspose(worldMatrix);
	tview = XMMatrixTranspose(viewMatrix);
	tproj = XMMatrixTranspose(projectionMatrix);
	tLightViewMatrix = XMMatrixTranspose(moonlight->GetViewMatrix());
	tLightProjectionMatrix = XMMatrixTranspose(moonlight->GetProjectionMatrix());

	// ���������������Ա�д��
	result = deviceContext->Map(m_matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);

	// ��ȡָ�����������е����ݵ�ָ��
	dataPtr = (MatrixBufferType*)mappedResource.pData;

	// �������Ƶ�������������
	dataPtr->world = tworld;// �������;
	dataPtr->view = tview;
	dataPtr->projection = tproj;
	dataPtr->lightView = tLightViewMatrix;
	dataPtr->lightProjection = tLightProjectionMatrix;

	// ��������������
	deviceContext->Unmap(m_matrixBuffer, 0);

	// ���ó����������ڶ�����ɫ���е�λ��
	bufferNumber = 0;

	// ���ø��µ�ֵ��������ɫ���еĳ���������
	deviceContext->DSSetConstantBuffers(bufferNumber, 1, &m_matrixBuffer);

	// ��tessellation���ݷ���Ϊ�����ɫ��
	deviceContext->Map(m_TessDistanceBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	tessPtr = (TessDistanceBufferType*)mappedResource.pData;
	tessPtr->tessFar = tessDistFar;
	tessPtr->tessNear = tessDistNear;
	tessPtr->padding = XMFLOAT2(1.0f, 1.0f);
	deviceContext->Unmap(m_TessDistanceBuffer, 0);
	bufferNumber = 0;
	deviceContext->HSSetConstantBuffers(bufferNumber, 1, &m_TessDistanceBuffer);

	// ��������ݷ��͵���Ǻ�������ɫ��
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

	// �������ݷ��͵�������ɫ��
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

	// �������ݷ��͵�������ɫ��
	deviceContext->Map(m_MoonlightBuffer2, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	lightPtr2 = (LightBufferType2*)mappedResource.pData;
	lightPtr2->position = moonlight->GetPosition();
	lightPtr2->padding = 1.0f;
	deviceContext->Unmap(m_MoonlightBuffer2, 0);
	bufferNumber = 1;
	deviceContext->DSSetConstantBuffers(bufferNumber, 1, &m_MoonlightBuffer2);

	// ��multifractal���ݷ��͵�����ɫ��
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

	// �ڶ��㡢���������ɫ����������ɫ��������Դ
	deviceContext->PSSetShaderResources(0, 1, &rockTexture);
	deviceContext->PSSetShaderResources(1, 1, &snowTexture);
	deviceContext->PSSetShaderResources(3, 1, &depthMap);
}

void TessellationShader::Render(ID3D11DeviceContext* deviceContext, int indexCount)
{
	// ��������ɫ�������ò�����״̬
	deviceContext->PSSetSamplers(0, 1, &m_sampleState);
	deviceContext->PSSetSamplers(1, 1, &m_sampleStateClamp);

	// Base render function.
	BaseShader::Render(deviceContext, indexCount);
}



