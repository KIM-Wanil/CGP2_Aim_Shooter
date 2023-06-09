//#include "stdafx.h"
#include "texturearrayclass.h"


TextureArrayClass::TextureArrayClass()
{
}


TextureArrayClass::TextureArrayClass(const TextureArrayClass& other)
{
}


TextureArrayClass::~TextureArrayClass()
{
}


bool TextureArrayClass::Initialize(ID3D11Device* device, const WCHAR* filename1, const WCHAR* filename2, const WCHAR* filename3)
{
	HRESULT result;
	// 첫번째 텍스처를 파일로부터 읽어온다
	/*if (FAILED(CreateDDSTextureFromFile(device, filename1, nullptr, &m_textures[0])))
	{
		return false;
	}*/
	result = CreateDDSTextureFromFile(device, filename1, nullptr, &m_textures[0]);
	if (FAILED(result))
	{
		return false;
	}
	// 두번째 텍스처를 파일로부터 읽어온다
	/*if (FAILED(CreateDDSTextureFromFile(device, filename2, nullptr, &m_textures[1])))
	{
		return false;
	}*/
	result = CreateDDSTextureFromFile(device, filename2, nullptr, &m_textures[1]);
	if (FAILED(result))
	{
		return false;
	}
	// 두번째 텍스처를 파일로부터 읽어온다
	/*if (FAILED(CreateDDSTextureFromFile(device, filename3, nullptr, &m_textures[2])))
	{
		return false;
	}*/
	result = CreateDDSTextureFromFile(device, filename3, nullptr, &m_textures[2]);
	if (FAILED(result))
	{
		return false;
	}
	return true;
}


void TextureArrayClass::Shutdown()
{
	// 텍스처 리소스를 해제한다.
	if (m_textures[0])
	{
		m_textures[0]->Release();
		m_textures[0] = 0;
	}

	if (m_textures[1])
	{
		m_textures[1]->Release();
		m_textures[1] = 0;
	}

	if (m_textures[2])
	{
		m_textures[2]->Release();
		m_textures[2] = 0;
	}

	return;
}


ID3D11ShaderResourceView** TextureArrayClass::GetTextureArray()
{
	return m_textures;
}