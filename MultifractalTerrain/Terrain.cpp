#include "Terrain.h"

Terrain::Terrain(ID3D11Device* device, ID3D11DeviceContext* deviceContext, int rows, int columns)
{
	// �����Ⱥ͸߶�
	int width = PATCHWIDTH * columns;
	int height = PATCHHEIGHT * rows;
	float uvWidth = 1.0f / columns;
	float uvHeight = 1.0f / rows;

	// ʹ�ø�����������������������
	for (int i = 0; i < rows; i++)
	{
		for (int j = 0; j < columns; j++)
		{
			float uvX = j * uvWidth;
			float uvY = i * uvHeight;

			Patch* temp = new Patch(device, deviceContext, L"604.jpg", j * PATCHWIDTH, i * -(PATCHHEIGHT), uvX, uvY, uvWidth, uvHeight);

			patches.push_back(temp);
		}
	}

	// �߶�ͼ�ϼ�������
	rockTexture = new Texture(device, deviceContext, L"../res/rock.jpg");
	snowTexture = new Texture(device, deviceContext, L"../res/snow.jpg");

}

Terrain::~Terrain()
{
	// Release
	if (rockTexture)
	{
		delete rockTexture;
		rockTexture = 0;
	}

	// Release
	if (snowTexture)
	{
		delete snowTexture;
		snowTexture = 0;
	}

}