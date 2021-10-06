#include "App1.h"

App1::App1()
{
	m_Terrain = nullptr;
	m_TessellationShader = nullptr;
}

void App1::init(HINSTANCE hinstance, HWND hwnd, int screenWidth, int screenHeight, Input *in)
{
	// 调用BaseApplication::init()
	BaseApplication::init(hinstance, hwnd, screenWidth, screenHeight, in);

	// 存储屏幕的宽，高信息
	s_Width = screenWidth; s_Height = screenHeight;

	// 创建网格对象
	m_Terrain = new Terrain(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), TERRAIN_PATCHES_X, TERRAIN_PATCHES_Z);
	int totalPatches = TERRAIN_PATCHES_X * TERRAIN_PATCHES_Z;

	// 设置遮挡查询预测包围盒
	occlusionDesc.Query = D3D11_QUERY_OCCLUSION;	//创建预测
	m_Direct3D->GetDevice()->CreateQuery(&occlusionDesc, &occlusionQuery);

	// 创建 shaders
	m_TessellationShader = new TessellationShader(m_Direct3D->GetDevice(), hwnd);
	m_DepthShader = new DepthShader(m_Direct3D->GetDevice(), hwnd);
	m_TextureShader = new TextureShader(m_Direct3D->GetDevice(), hwnd);

	// 创建 render texture
	m_DepthMap = new RenderTexture(m_Direct3D->GetDevice(), 2048, 2048, SCREEN_NEAR, SCREEN_DEPTH);

	// 创建 camera
	m_Camera->SetPosition(347.0f, 88.0f, -182.0f);
	m_Camera->SetRotation(11.5f, 262.0f, 0.0f);
	m_CameraFrustum = new CameraFrustum();
	m_CameraFrustum->Initialise(SCREEN_DEPTH);

	// 创建 skybox skybox.png
	//m_Skybox = new Skybox(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), L"../res/sky.png")
	m_Skybox = new Skybox(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), L"../res/skybox1.png");


	//创建 sunlight
	m_Sunlight = new Light;
	m_Sunlight->SetDiffuseColour(1.0f, 0.6f, 0.3f, 1.0f);
	m_Sunlight->SetAmbientColour(0.4f, 0.3f, 0.3f, 1.0f);
	m_Sunlight->SetPosition(470.0f, 120.0f, -500.0f);
	m_Sunlight->SetLookAt(250.0f, 50.0f, -150.0f);
	m_LightSphere = new SphereMesh(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), L"../res/snow.jpg");

	m_Sunlight->GenerateViewMatrix(); // 只在光线移动的帧才发生
	m_Sunlight->GenerateProjectionMatrix(1.0f, 500.0f);

	// 用于调试Ortho mesh
	m_OrthoMesh = new OrthoMesh(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), screenWidth / 4, screenHeight / 4, screenWidth / 8 - (screenWidth / 2), -screenHeight / 8 + (screenHeight / 2));

	// multifractal terrain输入数据
	m_MultifractalData.H = 1.6f;
	m_MultifractalData.lacunarity = 2.0f;
	m_MultifractalData.octaves = 8.0f;
	m_MultifractalData.offset = 1.0f;
	m_MultifractalData.gain = 2.0f;
	
	// GUI
	showDepthMap = false;
	tessellationFactor = 1.0f;
	wireframe = false;
	atmosphericEffects = true;
	tessDistNear = 150.0f;
	tessDistFar = 300.0f;
	rendering = 0; culling = 0;
	num = 0;
}


App1::~App1()
{

	BaseApplication::~BaseApplication();

	// 释放 Direct3D objects
	if (m_Skybox)
	{
		delete m_Skybox;
		m_Skybox = 0;
	}

	if (m_OrthoMesh)
	{
		delete m_OrthoMesh;
		m_OrthoMesh = 0;
	}

	if (m_DepthMap)
	{
		delete m_DepthMap;
		m_DepthMap = 0;
	}


	if (m_Terrain)
	{
		delete m_Terrain;
		m_Terrain = 0;
	}

	if (m_TessellationShader)
	{
		delete m_TessellationShader;
		m_TessellationShader = 0;
	}

	if (m_DepthShader)
	{
		delete m_DepthShader;
		m_DepthShader = 0;
	}

	if (m_TextureShader)
	{
		delete m_TextureShader;
		m_TextureShader = 0;
	}

	if (m_Sunlight)
	{
		delete m_Sunlight;
		m_Sunlight = 0;
	}

	if (m_LightSphere)
	{
		delete m_LightSphere;
		m_LightSphere = 0;
	}

	if (m_CameraFrustum)
	{
		delete m_CameraFrustum;
		m_CameraFrustum = 0;
	}
}


bool App1::Frame()
{
	bool result;

	// 线框切换
	if (m_Input->isKeyDown(VK_SPACE))
	{
		if (!wireframe)
		{
			m_Direct3D->TurnOnWireframe();
			wireframe = true;
			m_Input->SetKeyUp(VK_SPACE);
		}
		else
		{
			m_Direct3D->TurnOffWireframe();
			wireframe = false;
			m_Input->SetKeyUp(VK_SPACE);
		}
	}

	result = BaseApplication::Frame();
	if (!result)
	{
		return false;
	}

	// 渲染图形
	result = Render();
	if (!result)
	{
		return false;
	}

	return true;
}


bool App1::Render()
{
	// 首先获取光照和阴影的深度图
	RenderShadowMap();

	//开始绘制（将背景色初始化为蓝色）
	m_Direct3D->BeginScene(0.39f, 0.58f, 0.92f, 1.0f);

	// 测试闭合遮挡
	//OcclusionTest();
	
	// 最终帧渲染
	RenderScene();	

	// 更新帧时间
	frameRate = 1 / m_Timer->GetTime();

	// 检查自动优化
	if (optimisationMode == AUTO)
	{
		// 如果刷新帧频率不等于(~30fps)，调整tessellation值
		if (frameRate < 29.0f)
		{
			// 减少细节范围 (在限制范围内)
			if (tessDistFar > tessDistNear + 0.5)
				tessDistFar -= 0.5;
			if (tessDistNear > MIN_TESS + 0.1)
				tessDistNear -= 0.5;
		}
		else if (frameRate > 31.0f)
		{
			// 增加细节范围
			if (tessDistFar < MAX_TESS - 0.5)
				tessDistFar += 0.5;
			if (tessDistNear < tessDistFar - 0.1)
				tessDistNear += 0.5;
		}
	}

	// 显示圆框帧速率
	frameRate = roundf(frameRate);

	// 绘制 GUI
	gui();

	// 刷新显示
	m_Direct3D->EndScene();

	return true;
}

void App1::RenderScene()
{
	XMMATRIX worldMatrix, viewMatrix, projectionMatrix, baseViewMatrix, orthoMatrix;

	// 根据摄像机的位置生成视图矩阵
	m_Camera->Update();

	//从相机和 Direct3D 对象获取世界、视图、投影和正交矩阵
	m_Direct3D->GetWorldMatrix(worldMatrix);
	m_Camera->GetViewMatrix(viewMatrix);
	m_Direct3D->GetProjectionMatrix(projectionMatrix);
	
	// 重建 camera frustum（平截头体）
	m_CameraFrustum->ConstructFrustum(viewMatrix, projectionMatrix);

	// 渲染天空盒，关闭深度测试
	m_Direct3D->TurnZBufferOff();
	m_Direct3D->TurnOffWireframe();

		// 将天空盒装换位相机位置
		worldMatrix = XMMatrixTranslation(m_Camera->GetPosition().x, m_Camera->GetPosition().y, m_Camera->GetPosition().z + 2.0f);
		
		// 发送几何数据 (从网格）
		m_Skybox->SendData(m_Direct3D->GetDeviceContext());
		// 设置着色器参数 (矩阵和纹理)
		m_TextureShader->SetShaderParameters(m_Direct3D->GetDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, m_Skybox->GetTexture());
		//// 渲染对象 (几何网格 和 着色器进程 的组合）
		m_TextureShader->Render(m_Direct3D->GetDeviceContext(), m_Skybox->GetIndexCount());
		
		// 重设世界矩阵
		m_Direct3D->GetWorldMatrix(worldMatrix);

	if (wireframe) { m_Direct3D->TurnOnWireframe(); }
	m_Direct3D->TurnZBufferOn();


	// 发送几何数据 (从地形)
	rendering = 0; culling = 0;
	const std::vector<Patch*> localVec = m_Terrain->Patches();
	std::vector<Patch*>::const_iterator i = localVec.begin();

	while (i != localVec.end())
	{
		// Render之前检测patch是否在相机frustum（平截头体）内
		if (m_CameraFrustum->CheckRectangle((*i)->boundingBox.maxX, (*i)->boundingBox.maxY, (*i)->boundingBox.maxZ, (*i)->boundingBox.minX, (*i)->boundingBox.minY, (*i)->boundingBox.minZ) && (*i)->toRender)
		{
			(*i)->SendData(m_Direct3D->GetDeviceContext());
			// 设置着色器参数 (矩阵和纹理)
			m_TessellationShader->SetShaderParameters(m_Direct3D->GetDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, // Matrices
														 m_Terrain->GetTerrainRockTexture()->GetTexture(), m_Terrain->GetTerrainSnowTexture()->GetTexture(), m_DepthMap->GetShaderResourceView(), // Textures and maps
														 tessDistNear, tessDistFar, m_Camera, m_Sunlight, m_MultifractalData, atmosphericEffects); // Other
			// 设置物体 (几何网格和着色器过程)
			m_TessellationShader->Render(m_Direct3D->GetDeviceContext(), (*i)->GetIndexCount());

			// 增量Render计数器
			rendering++;
		}
		else
		{
			// 增加 culling 计数器
			culling++;
		}
		i++;
	}


	// 转换至光源位置
	worldMatrix = XMMatrixTranslation(m_Sunlight->GetPosition().x, m_Sunlight->GetPosition().y, m_Sunlight->GetPosition().z);
		// 发送 light sphere 数据
		m_LightSphere->SendData(m_Direct3D->GetDeviceContext());
		// 设置着色器参数 (矩阵和纹理)
		m_TextureShader->SetShaderParameters(m_Direct3D->GetDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, m_LightSphere->GetTexture());
		// 设置物体 (几何网格和着色器过程)
		m_TextureShader->Render(m_Direct3D->GetDeviceContext(), m_LightSphere->GetIndexCount());
	// 重设世界矩阵
	m_Direct3D->GetWorldMatrix(worldMatrix);



	// 渲染正交网格
	//关闭 Z 缓冲区，开始2D 渲染
	if (showDepthMap)
	{
		m_Direct3D->TurnZBufferOff();

			m_Direct3D->GetOrthoMatrix(orthoMatrix);// 用于二维渲染的正交矩阵
			m_Camera->GetBaseViewMatrix(baseViewMatrix);

			m_OrthoMesh->SendData(m_Direct3D->GetDeviceContext());
			m_TextureShader->SetShaderParameters(m_Direct3D->GetDeviceContext(), worldMatrix, baseViewMatrix, orthoMatrix, m_DepthMap->GetShaderResourceView());
			m_TextureShader->Render(m_Direct3D->GetDeviceContext(), m_OrthoMesh->GetIndexCount());

		m_Direct3D->TurnZBufferOn();
	}
}

void App1::RenderShadowMap()
{
	// 传递渲染，至纹理

	XMMATRIX worldMatrix, lightViewMatrix, lightProjectionMatrix;

	// 将渲染目标设置为渲染到纹理
	m_DepthMap->SetRenderTarget(m_Direct3D->GetDeviceContext());

	// 清除渲染
	m_DepthMap->ClearRenderTarget(m_Direct3D->GetDeviceContext(), 0.0f, 0.0f, 0.0f, 1.0f);

	// 根据摄像机的位置生成视图矩阵
	m_Camera->Update();

	// 从光和 d3d 对象中获取世界、视图和投影矩阵
	lightViewMatrix = m_Sunlight->GetViewMatrix();
	lightProjectionMatrix = m_Sunlight->GetProjectionMatrix();
	m_Direct3D->GetWorldMatrix(worldMatrix);


	// 发送几何数据 (从地形)
	const std::vector<Patch*> localVec = m_Terrain->Patches();
	std::vector<Patch*>::const_iterator i = localVec.begin();

	while (i != localVec.end())
	{
		(*i)->SendData(m_Direct3D->GetDeviceContext());


		//设置着色器参数 (矩阵和纹理)
		m_DepthShader->SetShaderParameters(m_Direct3D->GetDeviceContext(), worldMatrix, lightViewMatrix, lightProjectionMatrix, 
										   m_Camera, m_MultifractalData, tessDistNear, tessDistFar);

		// 渲染对象 (网格几何和着色器的组合）
		m_DepthShader->Render(m_Direct3D->GetDeviceContext(), (*i)->GetIndexCount());

		i++;

	}

	// 将渲染目标重置回原始后缓冲区, 不再将渲染设置作纹理
	m_Direct3D->SetBackBufferRenderTarget();

	// Reset viewport
	m_Direct3D->ResetViewport();

}



void App1::gui()
{
	//几何着色器上强制关闭
	m_Direct3D->GetDeviceContext()->GSSetShader(NULL, NULL, 0);

	// 建立GUI
	ImGui::InputInt("Patches rendering", &rendering);
	ImGui::InputInt("Patches culling", &culling);
	ImGui::InputFloat("FPS", &frameRate, 0.0f, 0.0f, 0);


	if(optimisationMode == MANUAL)
		ImGui::Text("Optimisation Mode: (Manual)");
	else
		ImGui::Text("Optimisation Mode: (Automatic)");
	//if (ImGui::Button("Manual"))
	//{
	//	if (optimisationMode == AUTO)
	//	{
	//		optimisationMode = MANUAL;
	//	}
	//}

	if(ImGui::Button("Automatic"))
	{
		if (optimisationMode == MANUAL)
		{
			optimisationMode = AUTO;
		}
	}


	ImGui::Text("Dynamic Tessellation Range:");
	ImGui::SliderFloat("Near", &tessDistNear, MIN_TESS, tessDistFar);
	ImGui::SliderFloat("Far", &tessDistFar, tessDistNear, MAX_TESS);

	if (ImGui::Button("Wireframe", ImVec2(70, 20)))
	{
		if (!wireframe)
		{
			m_Direct3D->TurnOnWireframe();
			wireframe = true;
		}
		else
		{
			m_Direct3D->TurnOffWireframe();
			wireframe = false;
		}
	}


	if (ImGui::Button("Show Depth Map", ImVec2(110, 20)))
	{
		if (!showDepthMap)
		{			
			showDepthMap = true;
		}
		else
		{
			showDepthMap = false;
		}
	}
	//if (ImGui::Button("Atmospheric Effects", ImVec2(140, 20)))
	//{
	//	if (!atmosphericEffects)
	//	{
	//		atmosphericEffects = true;
	//	}
	//	else
	//	{
	//		atmosphericEffects = false;
	//	}
	//}

	ImGui::Text("Number of triangular patches:");
	ImGui::Text("Ridged Multifractal Terrain:");
	ImGui::SliderFloat("Highest Fractal Dimension", &m_MultifractalData.H, 0.0f, 10.0f);
	ImGui::SliderFloat("Lacunarity", &m_MultifractalData.lacunarity, 0.0f, 2.000f);
	//ImGui::SliderFloat("Octaves", &m_MultifractalData.octaves, 0.0f, 8.0f);
	ImGui::SliderFloat("Offset", &m_MultifractalData.offset, 0.0f, 5.0f);
	ImGui::SliderFloat("Gain", &m_MultifractalData.gain, 0.0f, 10.0f);

	// Render UI
	ImGui::Render();
}