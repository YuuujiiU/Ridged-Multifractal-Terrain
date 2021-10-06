#include "CameraFrustum.h"

CameraFrustum::CameraFrustum()
{

}

CameraFrustum::~CameraFrustum()
{

}

void CameraFrustum::Initialise(float screenDepth)
{
	m_screenDepth = screenDepth;
}

void CameraFrustum::ConstructFrustum(const XMMATRIX &viewMatrix, const XMMATRIX &projMatrix)
{

	XMFLOAT4X4 pMatrix, matrix;
	float zMinimum, r, length;
	XMMATRIX finalMatrix, view, proj;

	view = viewMatrix;
	proj = projMatrix;

	// 投影矩阵转换为4x4 浮点类型
	XMStoreFloat4x4(&pMatrix, proj);

	// 计算Z值最小值
	zMinimum = -pMatrix._43 / pMatrix._33;
	r = m_screenDepth / (m_screenDepth - zMinimum);

	// 更新后的值重新加载到投影矩阵中
	pMatrix._33 = r;
	pMatrix._43 = -r * zMinimum;
	proj = XMLoadFloat4x4(&pMatrix);

	// 创建平截头体的矩阵
	//根据 视图矩阵和更新的投影矩阵
	finalMatrix = XMMatrixMultiply(view, proj);

	// 将最终矩阵转换为4x4 浮点类型
	XMStoreFloat4x4(&matrix, finalMatrix);

	// 计算frustum（平截头体）的近平面
	m_frustumPlanes[0][0] = matrix._14 + matrix._13;
	m_frustumPlanes[0][1] = matrix._24 + matrix._23;
	m_frustumPlanes[0][2] = matrix._34 + matrix._33;
	m_frustumPlanes[0][3] = matrix._44 + matrix._43;

	// 近平面normalize
	length = sqrtf((m_frustumPlanes[0][0] * m_frustumPlanes[0][0]) + (m_frustumPlanes[0][1] * m_frustumPlanes[0][1]) + (m_frustumPlanes[0][2] * m_frustumPlanes[0][2]));
	m_frustumPlanes[0][0] /= length;
	m_frustumPlanes[0][1] /= length;
	m_frustumPlanes[0][2] /= length;
	m_frustumPlanes[0][3] /= length;

	// 计算frustum（平截头体）的远平面
	m_frustumPlanes[1][0] = matrix._14 - matrix._13;
	m_frustumPlanes[1][1] = matrix._24 - matrix._23;
	m_frustumPlanes[1][2] = matrix._34 - matrix._33;
	m_frustumPlanes[1][3] = matrix._44 - matrix._43;

	// 远平面normalize
	length = sqrtf((m_frustumPlanes[1][0] * m_frustumPlanes[1][0]) + (m_frustumPlanes[1][1] * m_frustumPlanes[1][1]) + (m_frustumPlanes[1][2] * m_frustumPlanes[1][2]));
	m_frustumPlanes[1][0] /= length;
	m_frustumPlanes[1][1] /= length;
	m_frustumPlanes[1][2] /= length;
	m_frustumPlanes[1][3] /= length;

	// 计算frustum（平截头体）的左平面
	m_frustumPlanes[2][0] = matrix._14 + matrix._11;
	m_frustumPlanes[2][1] = matrix._24 + matrix._21;
	m_frustumPlanes[2][2] = matrix._34 + matrix._31;
	m_frustumPlanes[2][3] = matrix._44 + matrix._41;

	// 左平面normalize
	length = sqrtf((m_frustumPlanes[2][0] * m_frustumPlanes[2][0]) + (m_frustumPlanes[2][1] * m_frustumPlanes[2][1]) + (m_frustumPlanes[2][2] * m_frustumPlanes[2][2]));
	m_frustumPlanes[2][0] /= length;
	m_frustumPlanes[2][1] /= length;
	m_frustumPlanes[2][2] /= length;
	m_frustumPlanes[2][3] /= length;

	// 计算frustum（平截头体）的右平面
	m_frustumPlanes[3][0] = matrix._14 - matrix._11;
	m_frustumPlanes[3][1] = matrix._24 - matrix._21;
	m_frustumPlanes[3][2] = matrix._34 - matrix._31;
	m_frustumPlanes[3][3] = matrix._44 - matrix._41;

	// 右平面normalize
	length = sqrtf((m_frustumPlanes[3][0] * m_frustumPlanes[3][0]) + (m_frustumPlanes[3][1] * m_frustumPlanes[3][1]) + (m_frustumPlanes[3][2] * m_frustumPlanes[3][2]));
	m_frustumPlanes[3][0] /= length;
	m_frustumPlanes[3][1] /= length;
	m_frustumPlanes[3][2] /= length;
	m_frustumPlanes[3][3] /= length;

	// 计算frustum（平截头体）的上平面
	m_frustumPlanes[4][0] = matrix._14 - matrix._12;
	m_frustumPlanes[4][1] = matrix._24 - matrix._22;
	m_frustumPlanes[4][2] = matrix._34 - matrix._32;
	m_frustumPlanes[4][3] = matrix._44 - matrix._42;

	// 上平面normalize
	length = sqrtf((m_frustumPlanes[4][0] * m_frustumPlanes[4][0]) + (m_frustumPlanes[4][1] * m_frustumPlanes[4][1]) + (m_frustumPlanes[4][2] * m_frustumPlanes[4][2]));
	m_frustumPlanes[4][0] /= length;
	m_frustumPlanes[4][1] /= length;
	m_frustumPlanes[4][2] /= length;
	m_frustumPlanes[4][3] /= length;

	// 计算frustum（平截头体）的底部平面
	m_frustumPlanes[5][0] = matrix._14 + matrix._12;
	m_frustumPlanes[5][1] = matrix._24 + matrix._22;
	m_frustumPlanes[5][2] = matrix._34 + matrix._32;
	m_frustumPlanes[5][3] = matrix._44 + matrix._42;

	// 底平面normalize
	length = sqrtf((m_frustumPlanes[5][0] * m_frustumPlanes[5][0]) + (m_frustumPlanes[5][1] * m_frustumPlanes[5][1]) + (m_frustumPlanes[5][2] * m_frustumPlanes[5][2]));
	m_frustumPlanes[5][0] /= length;
	m_frustumPlanes[5][1] /= length;
	m_frustumPlanes[5][2] /= length;
	m_frustumPlanes[5][3] /= length;

	return;
}

bool CameraFrustum::CheckRectangle(float maxWidth, float maxHeight, float maxDepth, float minWidth, float minHeight, float minDepth)
{
	int i;
	float dotProduct;


	// 检查矩形的6个平面中是否有任意一个在视图平截体内
	for (i = 0; i<6; i++)
	{
		dotProduct = ((m_frustumPlanes[i][0] * minWidth) + (m_frustumPlanes[i][1] * minHeight) + (m_frustumPlanes[i][2] * minDepth) + (m_frustumPlanes[i][3] * 1.0f));
		if (dotProduct >= 0.0f)
		{
			continue;
		}

		dotProduct = ((m_frustumPlanes[i][0] * maxWidth) + (m_frustumPlanes[i][1] * minHeight) + (m_frustumPlanes[i][2] * minDepth) + (m_frustumPlanes[i][3] * 1.0f));
		if (dotProduct >= 0.0f)
		{
			continue;
		}

		dotProduct = ((m_frustumPlanes[i][0] * minWidth) + (m_frustumPlanes[i][1] * maxHeight) + (m_frustumPlanes[i][2] * minDepth) + (m_frustumPlanes[i][3] * 1.0f));
		if (dotProduct >= 0.0f)
		{
			continue;
		}

		dotProduct = ((m_frustumPlanes[i][0] * maxWidth) + (m_frustumPlanes[i][1] * maxHeight) + (m_frustumPlanes[i][2] * minDepth) + (m_frustumPlanes[i][3] * 1.0f));
		if (dotProduct >= 0.0f)
		{
			continue;
		}

		dotProduct = ((m_frustumPlanes[i][0] * minWidth) + (m_frustumPlanes[i][1] * minHeight) + (m_frustumPlanes[i][2] * maxDepth) + (m_frustumPlanes[i][3] * 1.0f));
		if (dotProduct >= 0.0f)
		{
			continue;
		}

		dotProduct = ((m_frustumPlanes[i][0] * maxWidth) + (m_frustumPlanes[i][1] * minHeight) + (m_frustumPlanes[i][2] * maxDepth) + (m_frustumPlanes[i][3] * 1.0f));
		if (dotProduct >= 0.0f)
		{
			continue;
		}

		dotProduct = ((m_frustumPlanes[i][0] * minWidth) + (m_frustumPlanes[i][1] * maxHeight) + (m_frustumPlanes[i][2] * maxDepth) + (m_frustumPlanes[i][3] * 1.0f));
		if (dotProduct >= 0.0f)
		{
			continue;
		}

		dotProduct = ((m_frustumPlanes[i][0] * maxWidth) + (m_frustumPlanes[i][1] * maxHeight) + (m_frustumPlanes[i][2] * maxDepth) + (m_frustumPlanes[i][3] * 1.0f));
		if (dotProduct >= 0.0f)
		{
			continue;
		}

		return false;
	}

	return true;
}