#pragma once
#ifndef _PATCH_H
#define _PATCH_H

// 全局变量
// 定义每个地形补丁的世界空间大小 (补丁越大, 顶点分辨率越低)

#define PATCHWIDTH 25
#define PATCHHEIGHT 25

#include "../DXFramework/TessellationMesh.h"

// 自定义顶点类型
struct VType
{
	XMFLOAT3 position;
	XMFLOAT2 heightTex;
	XMFLOAT2 texture;
	XMFLOAT3 normal;
};

// 设置边界框体积
struct Box
{
	
	float maxX = -100000.0f, maxY = -100000.0f, maxZ = -100000.0f;
	float minX = 100000.0f, minY = 100000.0f, minZ =  100000.0f;
};

// Permutation table forPerlin Noise
// 噪点图置换
static int* perm;
static int p[512];

class Patch : public TessellationMesh
{
public:

	Patch(ID3D11Device* device, ID3D11DeviceContext* deviceContext, WCHAR* textureFilename, float x, float z, float uvX, float uvY, float uvWidth, float uvHeight);
	~Patch();

	void SendData(ID3D11DeviceContext*);

	// Frustum Culling（视截体裁剪）
	Box boundingBox;
	bool toRender = true;

protected:
	void InitBuffers(ID3D11Device* device, float x, float z, float uvX, float uvY, float uvWidth, float uvHeight);

	// 噪点图Function

	inline static double fade(double t) { return t * t * t * (t * (t * 6 - 15) + 10); }
	inline static double lInterpolate(double t, double a, double b) { return a + t * (b - a); }

	inline static double grad(int hash, double x, double y, double z)
	{// （转换 LO 4 位哈希代码进入12个数据方向）
		int h = hash & 15;
		double u = h<8 ? x : y,
			v = h<4 ? y : h == 12 || h == 14 ? x : z;
		return ((h & 1) == 0 ? u : -u) + ((h & 2) == 0 ? v : -v);
	}

	inline static void DoublePerm() { for (int i = 0; i<512; i++) p[i] = perm[i & 255]; }
	
	inline static double SampleImprovedPerlin(double x, double y, double z)
	{ // 顶点收到返回的噪点值
	
	   // 找到包含点的立方体单元，限制在255的范围内
		int X = (int)floor(x) & 255,
			Y = (int)floor(y) & 255,
			Z = (int)floor(z) & 255;
	
		// 找到立方体的x,y,z点，计算左边部分
		x -= floor(x);
		y -= floor(y);
		z -= floor(z);
	
		// 对x,y,z点平滑
		double u = fade(x),
			v = fade(y),
			w = fade(z);
	
		// 立方体8个角的散列坐标
		int A = p[X] + Y, AA = p[A] + Z, AB = p[A + 1] + Z,
			B = p[X + 1] + Y, BA = p[B] + Z, BB = p[B + 1] + Z;
	
		// 添加8个立方体脚的插值计算结果
		return lInterpolate(w, lInterpolate(v, lInterpolate(u, grad(p[AA], x, y, z),
			grad(p[BA], x - 1, y, z)),
			lInterpolate(u, grad(p[AB], x, y - 1, z),
				grad(p[BB], x - 1, y - 1, z))),
			lInterpolate(v, lInterpolate(u, grad(p[AA + 1], x, y, z - 1),
				grad(p[BA + 1], x - 1, y, z - 1)),
				lInterpolate(u, grad(p[AB + 1], x, y - 1, z - 1),
					grad(p[BB + 1], x - 1, y - 1, z - 1))));
	
	}
	
	// Ridged multifractal terrain model
	inline double RidgedMultifractal(XMFLOAT3 pnt, double H, double lacunarity, double octaves, double offset, double gain)
	{
		double result, signal, weight;
		int i;
		static bool first = true;
		static double exponent_array[8];
	
		// 计算spectral weights
		if (first)
		{
			for (i = 0; i < 8; i++)
			{
				// 计算each frequency 
				exponent_array[i] = pow(lacunarity, -i*H);
			}
			first = false;
		}

		// 赋予第一个octave值 (线性非线性数值计算) 
		signal = SampleImprovedPerlin(pnt.x, pnt.y, pnt.z);
	
		// 计算绝对值 (this creates the ridges) 
		if (signal < 0.0)
			signal = -signal;
	
		// Invert and translate
		signal = offset - signal;
	
		// signal=signal*signal
		signal *= signal;
	
		// 设定原始值
		result = signal;
		weight = 1.0;
	
		// 提高frequency
		for (i = 1; weight > 0.001 && i < 8; i++)
		{
			pnt.x *= (float)lacunarity;
			pnt.y *= (float)lacunarity;
			pnt.z *= (float)lacunarity;
	
			// Weight successive contributions by previous signal
			weight = signal * gain;
			if (weight > 1.0)
				weight = 1.0;
			if (weight < 0.0)
				weight = 0.0;
			signal = SampleImprovedPerlin(pnt.x, pnt.y, pnt.z);
			if (signal < 0.0)
				signal = -signal;
			signal = offset - signal;
			signal *= signal;
	
			// 计算权重
			signal *= weight;
			result += signal * exponent_array[i];
		}
		return (result);
	}
	//Function for Perline噪点图

};

#endif 
