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

	// ͶӰ����ת��Ϊ4x4 ��������
	XMStoreFloat4x4(&pMatrix, proj);

	// ����Zֵ��Сֵ
	zMinimum = -pMatrix._43 / pMatrix._33;
	r = m_screenDepth / (m_screenDepth - zMinimum);

	// ���º��ֵ���¼��ص�ͶӰ������
	pMatrix._33 = r;
	pMatrix._43 = -r * zMinimum;
	proj = XMLoadFloat4x4(&pMatrix);

	// ����ƽ��ͷ��ľ���
	//���� ��ͼ����͸��µ�ͶӰ����
	finalMatrix = XMMatrixMultiply(view, proj);

	// �����վ���ת��Ϊ4x4 ��������
	XMStoreFloat4x4(&matrix, finalMatrix);

	// ����frustum��ƽ��ͷ�壩�Ľ�ƽ��
	m_frustumPlanes[0][0] = matrix._14 + matrix._13;
	m_frustumPlanes[0][1] = matrix._24 + matrix._23;
	m_frustumPlanes[0][2] = matrix._34 + matrix._33;
	m_frustumPlanes[0][3] = matrix._44 + matrix._43;

	// ��ƽ��normalize
	length = sqrtf((m_frustumPlanes[0][0] * m_frustumPlanes[0][0]) + (m_frustumPlanes[0][1] * m_frustumPlanes[0][1]) + (m_frustumPlanes[0][2] * m_frustumPlanes[0][2]));
	m_frustumPlanes[0][0] /= length;
	m_frustumPlanes[0][1] /= length;
	m_frustumPlanes[0][2] /= length;
	m_frustumPlanes[0][3] /= length;

	// ����frustum��ƽ��ͷ�壩��Զƽ��
	m_frustumPlanes[1][0] = matrix._14 - matrix._13;
	m_frustumPlanes[1][1] = matrix._24 - matrix._23;
	m_frustumPlanes[1][2] = matrix._34 - matrix._33;
	m_frustumPlanes[1][3] = matrix._44 - matrix._43;

	// Զƽ��normalize
	length = sqrtf((m_frustumPlanes[1][0] * m_frustumPlanes[1][0]) + (m_frustumPlanes[1][1] * m_frustumPlanes[1][1]) + (m_frustumPlanes[1][2] * m_frustumPlanes[1][2]));
	m_frustumPlanes[1][0] /= length;
	m_frustumPlanes[1][1] /= length;
	m_frustumPlanes[1][2] /= length;
	m_frustumPlanes[1][3] /= length;

	// ����frustum��ƽ��ͷ�壩����ƽ��
	m_frustumPlanes[2][0] = matrix._14 + matrix._11;
	m_frustumPlanes[2][1] = matrix._24 + matrix._21;
	m_frustumPlanes[2][2] = matrix._34 + matrix._31;
	m_frustumPlanes[2][3] = matrix._44 + matrix._41;

	// ��ƽ��normalize
	length = sqrtf((m_frustumPlanes[2][0] * m_frustumPlanes[2][0]) + (m_frustumPlanes[2][1] * m_frustumPlanes[2][1]) + (m_frustumPlanes[2][2] * m_frustumPlanes[2][2]));
	m_frustumPlanes[2][0] /= length;
	m_frustumPlanes[2][1] /= length;
	m_frustumPlanes[2][2] /= length;
	m_frustumPlanes[2][3] /= length;

	// ����frustum��ƽ��ͷ�壩����ƽ��
	m_frustumPlanes[3][0] = matrix._14 - matrix._11;
	m_frustumPlanes[3][1] = matrix._24 - matrix._21;
	m_frustumPlanes[3][2] = matrix._34 - matrix._31;
	m_frustumPlanes[3][3] = matrix._44 - matrix._41;

	// ��ƽ��normalize
	length = sqrtf((m_frustumPlanes[3][0] * m_frustumPlanes[3][0]) + (m_frustumPlanes[3][1] * m_frustumPlanes[3][1]) + (m_frustumPlanes[3][2] * m_frustumPlanes[3][2]));
	m_frustumPlanes[3][0] /= length;
	m_frustumPlanes[3][1] /= length;
	m_frustumPlanes[3][2] /= length;
	m_frustumPlanes[3][3] /= length;

	// ����frustum��ƽ��ͷ�壩����ƽ��
	m_frustumPlanes[4][0] = matrix._14 - matrix._12;
	m_frustumPlanes[4][1] = matrix._24 - matrix._22;
	m_frustumPlanes[4][2] = matrix._34 - matrix._32;
	m_frustumPlanes[4][3] = matrix._44 - matrix._42;

	// ��ƽ��normalize
	length = sqrtf((m_frustumPlanes[4][0] * m_frustumPlanes[4][0]) + (m_frustumPlanes[4][1] * m_frustumPlanes[4][1]) + (m_frustumPlanes[4][2] * m_frustumPlanes[4][2]));
	m_frustumPlanes[4][0] /= length;
	m_frustumPlanes[4][1] /= length;
	m_frustumPlanes[4][2] /= length;
	m_frustumPlanes[4][3] /= length;

	// ����frustum��ƽ��ͷ�壩�ĵײ�ƽ��
	m_frustumPlanes[5][0] = matrix._14 + matrix._12;
	m_frustumPlanes[5][1] = matrix._24 + matrix._22;
	m_frustumPlanes[5][2] = matrix._34 + matrix._32;
	m_frustumPlanes[5][3] = matrix._44 + matrix._42;

	// ��ƽ��normalize
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


	// �����ε�6��ƽ�����Ƿ�������һ������ͼƽ������
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