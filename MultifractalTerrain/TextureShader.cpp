// texture shader.cpp
#include "textureshader.h"


TextureShader::TextureShader(ID3D11Device* device, HWND hwnd) : BaseShader(device, hwnd)
{
	InitShader(L"shaders/texture_vs.hlsl", L"shaders/texture_ps.hlsl");
}


TextureShader::~TextureShader()
{
	// �ͷŲ�����״̬
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

	// �ͷŲ���
	if (m_layout)
	{
		m_layout->Release();
		m_layout = 0;
	}

	// �ͷŻ�����ɫ�����
	BaseShader::~BaseShader();
}


void TextureShader::InitShader(WCHAR* vsFilename, WCHAR* psFilename)
{
	D3D11_BUFFER_DESC matrixBufferDesc;
	D3D11_SAMPLER_DESC samplerDesc;

	// ���ء�������ɫ���ļ�
	loadVertexShader(vsFilename);
	loadPixelShader(psFilename);

	// ���ö�����ɫ���еĶ�̬��������������˵��
	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.ByteWidth = sizeof(MatrixBufferType);
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;

	// ��������������ָ��, �Ա������ʶ�����ɫ��������������
	m_device->CreateBuffer(&matrixBufferDesc, NULL, &m_matrixBuffer);


	// �������������״̬����
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


	// ���������״̬����
	m_device->CreateSamplerState(&samplerDesc, &m_sampleState);

}


void TextureShader::SetShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX &worldMatrix, const XMMATRIX &viewMatrix, const XMMATRIX &projectionMatrix, ID3D11ShaderResourceView* texture)
{
	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	MatrixBufferType* dataPtr;
	unsigned int bufferNumber;
	XMMATRIX tworld, tview, tproj;


	// ת������, Ϊ��ɫ������׼��
	tworld = XMMatrixTranspose(worldMatrix);
	tview = XMMatrixTranspose(viewMatrix);
	tproj = XMMatrixTranspose(projectionMatrix);

	// ��������������, �Ա�д��
	result = deviceContext->Map(m_matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);

	// ��ȡָ�����������е����ݵ�ָ��
	dataPtr = (MatrixBufferType*)mappedResource.pData;

	// �������Ƶ�������������
	dataPtr->world = tworld;// worldMatrix;
	dataPtr->view = tview;
	dataPtr->projection = tproj;

	// ��������������
	deviceContext->Unmap(m_matrixBuffer, 0);

	// ���ó����������ڶ�����ɫ���е�λ��
	bufferNumber = 0;

	// ʹ�ø��µ�ֵ�ڶ�����ɫ�������ó���������
	deviceContext->VSSetConstantBuffers(bufferNumber, 1, &m_matrixBuffer);

	// ��������ɫ����������ɫ��������Դ
	deviceContext->PSSetShaderResources(0, 1, &texture);
}

void TextureShader::Render(ID3D11DeviceContext* deviceContext, int indexCount)
{
	// ��������ɫ�������ò�����״̬
	deviceContext->PSSetSamplers(0, 1, &m_sampleState);

	// Base render function.
	BaseShader::Render(deviceContext, indexCount);
}



