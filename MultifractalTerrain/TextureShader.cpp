// texture shader.cpp
#include "textureshader.h"


TextureShader::TextureShader(ID3D11Device* device, HWND hwnd) : BaseShader(device, hwnd)
{
	InitShader(L"shaders/texture_vs.hlsl", L"shaders/texture_ps.hlsl");
}


TextureShader::~TextureShader()
{
	// 释放采样器状态
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

	// 释放布局
	if (m_layout)
	{
		m_layout->Release();
		m_layout = 0;
	}

	// 释放基本着色器组件
	BaseShader::~BaseShader();
}


void TextureShader::InitShader(WCHAR* vsFilename, WCHAR* psFilename)
{
	D3D11_BUFFER_DESC matrixBufferDesc;
	D3D11_SAMPLER_DESC samplerDesc;

	// 加载、编译着色器文件
	loadVertexShader(vsFilename);
	loadPixelShader(psFilename);

	// 设置顶点着色器中的动态矩阵常量缓冲区的说明
	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.ByteWidth = sizeof(MatrixBufferType);
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;

	// 创建常量缓冲区指针, 以便此类访问顶点着色器常量缓冲区。
	m_device->CreateBuffer(&matrixBufferDesc, NULL, &m_matrixBuffer);


	// 创建纹理采样器状态描述
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


	// 纹理采样器状态描述
	m_device->CreateSamplerState(&samplerDesc, &m_sampleState);

}


void TextureShader::SetShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX &worldMatrix, const XMMATRIX &viewMatrix, const XMMATRIX &projectionMatrix, ID3D11ShaderResourceView* texture)
{
	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	MatrixBufferType* dataPtr;
	unsigned int bufferNumber;
	XMMATRIX tworld, tview, tproj;


	// 转换矩阵, 为着色器做好准备
	tworld = XMMatrixTranspose(worldMatrix);
	tview = XMMatrixTranspose(viewMatrix);
	tproj = XMMatrixTranspose(projectionMatrix);

	// 锁定常量缓冲区, 以便写入
	result = deviceContext->Map(m_matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);

	// 获取指向常量缓冲区中的数据的指针
	dataPtr = (MatrixBufferType*)mappedResource.pData;

	// 将矩阵复制到常量缓冲区中
	dataPtr->world = tworld;// worldMatrix;
	dataPtr->view = tview;
	dataPtr->projection = tproj;

	// 解锁常量缓存区
	deviceContext->Unmap(m_matrixBuffer, 0);

	// 设置常量缓冲区在顶点着色器中的位置
	bufferNumber = 0;

	// 使用更新的值在顶点着色器中设置常量缓冲区
	deviceContext->VSSetConstantBuffers(bufferNumber, 1, &m_matrixBuffer);

	// 在像素着色器中设置着色器纹理资源
	deviceContext->PSSetShaderResources(0, 1, &texture);
}

void TextureShader::Render(ID3D11DeviceContext* deviceContext, int indexCount)
{
	// 在像素着色器中设置采样器状态
	deviceContext->PSSetSamplers(0, 1, &m_sampleState);

	// Base render function.
	BaseShader::Render(deviceContext, indexCount);
}



