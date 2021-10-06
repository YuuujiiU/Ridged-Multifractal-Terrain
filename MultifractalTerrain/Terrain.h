#pragma once
#ifndef _TERRAIN_H
#define _TERRAIN_H

#include "Patch.h"
#include "../DXFramework/Texture.h"
#include <vector>

class Terrain
{
public:

	Terrain(ID3D11Device* device, ID3D11DeviceContext* deviceContext, int rows, int columns);
	~Terrain();

	// Terrian中对于patch及其纹理
	inline std::vector<Patch*> Patches() { return patches; }
	inline Texture* GetTerrainRockTexture() { return rockTexture; }
	inline Texture* GetTerrainSnowTexture() { return snowTexture; }

protected:

	// Vector存储tessellated patches
	std::vector<Patch*> patches;

	// Textures
	Texture* rockTexture;
	Texture* snowTexture;
};

#endif
