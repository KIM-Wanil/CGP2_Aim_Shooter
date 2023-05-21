#pragma once

#include <d3d11.h>
#include <directxmath.h>
#include <d3dcompiler.h>
#include "AlignedAllocationPolicy.h"
#include <string>
#include <fstream>

using namespace std;
using namespace DirectX;
class FontClass;
class FontShaderClass;


class TextClass : public AlignedAllocationPolicy<16>
{
private:
	struct SentenceType
	{
		ID3D11Buffer *vertexBuffer, *indexBuffer;
		int vertexCount, indexCount, maxLength;
		float red, green, blue;
	};

	struct VertexType
	{
		XMFLOAT3 position;
		XMFLOAT2 texture;
	};

public:
	TextClass();
	TextClass(const TextClass&);
	~TextClass();

	bool Initialize(ID3D11Device*, ID3D11DeviceContext*, HWND, int, int, XMMATRIX);
	void Shutdown();
	bool Render(ID3D11DeviceContext*, XMMATRIX, XMMATRIX);
	bool RenderScore(ID3D11DeviceContext*, XMMATRIX, XMMATRIX);
	bool SetFps(int, ID3D11DeviceContext*);
	bool SetCpu(int, ID3D11DeviceContext*);
	bool SetHit(int, ID3D11DeviceContext*);
	bool SetSize(int,int, ID3D11DeviceContext*);
	bool SetScore(int, ID3D11DeviceContext*);
	bool SetCount(int,int, ID3D11DeviceContext*);

private:
	bool InitializeSentence(SentenceType**, int, ID3D11Device*);
	bool UpdateSentence(SentenceType*, const char*, int, int, float, float, float, ID3D11DeviceContext*);
	void ReleaseSentence(SentenceType**);
	bool RenderSentence(ID3D11DeviceContext*, SentenceType*, XMMATRIX, XMMATRIX);

private:
	FontClass* m_Font = nullptr;
	FontShaderClass* m_FontShader = nullptr;
	int m_screenWidth = 0;
	int m_screenHeight = 0;
	int m_score=0;
	XMMATRIX m_baseViewMatrix;
	SentenceType* m_sentence1;
	SentenceType* m_sentence2;
	SentenceType* m_sentence3;
	SentenceType* m_sentence4;
	SentenceType* m_sentence5;
	SentenceType* m_sentence6;
	SentenceType* m_sentence7;
	SentenceType* m_sentence8;
};