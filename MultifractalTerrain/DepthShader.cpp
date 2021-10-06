// depth shader.cpp
#include "depthshader.h"


DepthShader::DepthShader(ID3D11Device* device, HWND hwnd) : BaseShader(device, hwnd)
{
	InitShader(L"shaders/depth_vs.hlsl", L"shaders/depth_hs.hlsl", L"shaders/depth_ds.hlsl", L"shaders/depth_ps.hlsl");
}


DepthShader::~DepthShader()
{
	// �ͷž�����������
	if (m_matrixBuffer)
	{
		m_matrixBuffer->Release();
		m_matrixBuffer = 0;
	}

	// �ͷŲ���
	if (m_layout)
	{
		m_layout->Release();
		m_layout = 0;
	}

	// �ͷ�tess���ӳ���������
	if (m_TessDistanceBuffer)
	{
		m_TessDistanceBuffer->Release();
		m_TessDistanceBuffer = 0;
	}

	// �ͷ���Ӱ������������
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

	//�����ļ�
	// Load (+ compile) shader files
	loadVertexShader(vsFilename);
	loadPixelShader(psFilename);


	//���ö�����ɫ���ж�̬��������������˵����VS��
	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.ByteWidth = sizeof(MatrixBufferType);
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;


	//���������ɫ���������������˵����HS��
	cameraBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	cameraBufferDesc.ByteWidth = sizeof(CameraBufferType);
	cameraBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cameraBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cameraBufferDesc.MiscFlags = 0;
	cameraBufferDesc.StructureByteStride = 0;

	m_device->CreateBuffer(&cameraBufferDesc, NULL, &m_CameraBuffer);


	//��������������ָ��, �Ա�Ӵ����з��ʶ�����ɫ������������
	m_device->CreateBuffer(&matrixBufferDesc, NULL, &m_matrixBuffer);

	//���������ɫ���Ķ�̬ϸ������(tess)������
	tessDistanceBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	tessDistanceBufferDesc.ByteWidth = sizeof(TessDistanceBufferType);
	tessDistanceBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	tessDistanceBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	tessDistanceBufferDesc.MiscFlags = 0;
	tessDistanceBufferDesc.StructureByteStride = 0;

	m_device->CreateBuffer(&tessDistanceBufferDesc, NULL, &m_TessDistanceBuffer);


	// multifractal buffer
	//��������ɫ���Ķ��ط���(multi)��������˵��(DS)
	multifractalBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	multifractalBufferDesc.ByteWidth = sizeof(TessellationShader::MultifractalBufferType);
	multifractalBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	multifractalBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	multifractalBufferDesc.MiscFlags = 0;
	multifractalBufferDesc.StructureByteStride = 0;

	m_device->CreateBuffer(&multifractalBufferDesc, NULL, &m_MultifractalBuffer);

}

//��дInitShader
void DepthShader::InitShader(WCHAR* vsFilename, WCHAR* hsFilename, WCHAR* dsFilename, WCHAR* psFilename)
{
	//InitShader ���뱻����, ����ͬʱ���ض����������ɫ�� + ���û�����
	InitShader(vsFilename, psFilename);

	// ����Shaders
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

	//׼������
	// ת������Ϊ��ɫ����׼��
	tworld = XMMatrixTranspose(worldMatrix);
	tview = XMMatrixTranspose(viewMatrix);
	tproj = XMMatrixTranspose(projectionMatrix);

	// ��������������, �Ա㽫��д��
	//mappedResource����������
	result = deviceContext->Map(m_matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);

	// ��ȡָ���������������ݵ�ָ��
	dataPtr = (MatrixBufferType*)mappedResource.pData;

	// �������Ƶ�������������
	dataPtr->world = tworld;// worldMatrix;
	dataPtr->view = tview;
	dataPtr->projection = tproj;

	// ��������������
	deviceContext->Unmap(m_matrixBuffer, 0);

	// ���ó����������ڶ�����ɫ���е�λ��
	bufferNumber = 0;

	//ʹ�ø��µ�ֵ��������ɫ���еĳ���������
	deviceContext->DSSetConstantBuffers(bufferNumber, 1, &m_matrixBuffer);

	// ������Ӱ�����ݽ������ɫ��
	deviceContext->Map(m_CameraBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	cameraPtr = (CameraBufferType*)mappedResource.pData;
	cameraPtr->cameraPos = camera->GetPosition();
	cameraPtr->camPadding = FLOAT(1.0f);
	deviceContext->Unmap(m_CameraBuffer, 0);
	bufferNumber = 0;
	deviceContext->HSSetConstantBuffers(bufferNumber, 1, &m_CameraBuffer);

	// ����ϸ���������ݽ������ɫ��
	deviceContext->Map(m_TessDistanceBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	tessPtr = (TessDistanceBufferType*)mappedResource.pData;
	tessPtr->tessFar = tessDistFar;
	tessPtr->tessNear = tessDistNear;
	tessPtr->padding = XMFLOAT2(1.0f, 1.0f);
	deviceContext->Unmap(m_TessDistanceBuffer, 0);
	bufferNumber = 1;
	deviceContext->HSSetConstantBuffers(bufferNumber, 1, &m_TessDistanceBuffer);

	//����multifractal data������ɫ��
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



