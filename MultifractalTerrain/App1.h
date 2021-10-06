// Application.h

#ifndef _APP1_H
#define _APP1_H

// Includes
#include <math.h>
#include "../DXFramework/baseapplication.h"
#include "D3D.h"
#include "../DXFramework/TriangleMesh.h"
#include "../DXFramework/SphereMesh.h"
#include "../DXFramework/Light.h"
#include "../DXFramework/RenderTexture.h"
#include "../DXFramework/OrthoMesh.h"
#include "SkyBox.h"
#include "TessellationShader.h"
#include "DepthShader.h"
#include "TextureShader.h"
#include "Patch.h"
#include "Terrain.h"
#include "CameraFrustum.h"


// 渲染的patch数量
const int TERRAIN_PATCHES_X = 20;
const int TERRAIN_PATCHES_Z = 20;

// 细分的上下限
const int MAX_TESS = 500;
const int MIN_TESS = 20;

class App1 : public BaseApplication
{
public:

	App1();
	~App1();

	//装入内存资源的ID
	void init(HINSTANCE hinstance, HWND hwnd, int screenWidth, int screenHeight, Input*);

	bool Frame();

protected:
	// 主要render
	bool Render();

	// 深度
	void RenderShadowMap();

	// Normal tessellation/ lighting render
	void RenderScene();

private:

	// 天空盒
	Skybox* m_Skybox;

	// Terrain
	TessellationShader* m_TessellationShader;
	Terrain* m_Terrain;
	TessellationShader::MultifractalBufferType m_MultifractalData;

	// Lighting and Shadows 
	DepthShader* m_DepthShader;
	TextureShader* m_TextureShader; //ortho mesh
	OrthoMesh* m_OrthoMesh;
	Light* m_Sunlight;
	RenderTexture* m_DepthMap;
	SphereMesh* m_LightSphere;

	// 交互
	void gui();
	float tessellationFactor;	
	bool wireframe;
	bool showDepthMap;
	bool atmosphericEffects;
	float tessDistNear, tessDistFar;
	int rendering, culling;
	int num;
	float frameRate;

	// 屏幕属性 
	float s_Width, s_Height;
	CameraFrustum* m_CameraFrustum;
	ID3D11Query* occlusionQuery;
	D3D11_QUERY_DESC occlusionDesc;

	// 优化
	enum OptimisationMode {MANUAL, AUTO};
	int optimisationMode = MANUAL;
};

#endif