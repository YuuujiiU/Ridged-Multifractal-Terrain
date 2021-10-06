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


// ��Ⱦ��patch����
const int TERRAIN_PATCHES_X = 20;
const int TERRAIN_PATCHES_Z = 20;

// ϸ�ֵ�������
const int MAX_TESS = 500;
const int MIN_TESS = 20;

class App1 : public BaseApplication
{
public:

	App1();
	~App1();

	//װ���ڴ���Դ��ID
	void init(HINSTANCE hinstance, HWND hwnd, int screenWidth, int screenHeight, Input*);

	bool Frame();

protected:
	// ��Ҫrender
	bool Render();

	// ���
	void RenderShadowMap();

	// Normal tessellation/ lighting render
	void RenderScene();

private:

	// ��պ�
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

	// ����
	void gui();
	float tessellationFactor;	
	bool wireframe;
	bool showDepthMap;
	bool atmosphericEffects;
	float tessDistNear, tessDistFar;
	int rendering, culling;
	int num;
	float frameRate;

	// ��Ļ���� 
	float s_Width, s_Height;
	CameraFrustum* m_CameraFrustum;
	ID3D11Query* occlusionQuery;
	D3D11_QUERY_DESC occlusionDesc;

	// �Ż�
	enum OptimisationMode {MANUAL, AUTO};
	int optimisationMode = MANUAL;
};

#endif