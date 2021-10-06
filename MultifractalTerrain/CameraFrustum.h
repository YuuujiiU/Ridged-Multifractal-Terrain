#ifndef _CAMERAFRUSTUM_H_
#define _CAMERAFRUSTUM_H_

// 定义类来计算和存储当前摄像机截锥 
// www.rastertek.com/dx11ter10.html


#include <DirectXMath.h>
using namespace DirectX;

class CameraFrustum
{
public:

	//相机平截头体
	CameraFrustum();
	~CameraFrustum();

	void Initialise(float screenDepth);

	void ConstructFrustum(const XMMATRIX &view, const XMMATRIX &proj);

	bool CheckRectangle(float maxWidth, float maxHeight, float maxDepth, float minWidth, float minHeight, float minDepth);

private:

	float m_screenDepth;
	float m_frustumPlanes[6][4];

};

#endif