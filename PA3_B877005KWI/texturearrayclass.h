#pragma once
#include <d3d11.h>
#include <directxmath.h>
#include <d3dcompiler.h>
#include "DDSTextureLoader.h"

#include <fstream>

using namespace std;
using namespace DirectX;

class TextureArrayClass
{
public:
	TextureArrayClass();
	TextureArrayClass(const TextureArrayClass&);
	~TextureArrayClass();

	bool Initialize(ID3D11Device*, const WCHAR*, const WCHAR*, const WCHAR*);
	void Shutdown();

	ID3D11ShaderResourceView** GetTextureArray();

private:
	ID3D11ShaderResourceView* m_textures[3] = { nullptr, nullptr, nullptr };
};