////////////////////////////////////////////////////////////////////////////////
// Filename: modelclass.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _MODELCLASS_H_
#define _MODELCLASS_H_


//////////////
// INCLUDES //
//////////////
#include <d3d11.h>
#include <directxmath.h>
#include <vector>
using namespace DirectX;

///////////////////////
// MY CLASS INCLUDES //
///////////////////////
#include "textureclass.h"
#include "texturearrayclass.h"

#include <fstream>
using namespace std;

////////////////////////////////////////////////////////////////////////////////
// Class name: ModelClass
////////////////////////////////////////////////////////////////////////////////
class TextureArrayClass;
class ModelClass
{
private:
	struct VertexType
	{
		XMFLOAT3 position;
		XMFLOAT2 texture;
		XMFLOAT3 normal;
		XMFLOAT3 tangent;
		XMFLOAT3 binormal;
	};
	struct VertexType2
	{
		XMFLOAT3 position;
		XMFLOAT2 texture;
		XMFLOAT3 normal;
		XMFLOAT3 tangent;
		XMFLOAT3 binormal;
	};

	struct FaceType
	{
		int vIndex1, vIndex2, vIndex3;
		int tIndex1, tIndex2, tIndex3;
		int nIndex1, nIndex2, nIndex3;
	};

	struct ModelType
	{
		float x, y, z;
		float tu, tv;
		float nx, ny, nz;
		float tx, ty, tz;
		float bx, by, bz;
	};
	struct ModelType2
	{
		float x, y, z;
		float tu, tv;
		float nx, ny, nz;
	};
	struct TempVertexType
	{
		float x, y, z;
		float tu, tv;
		float nx, ny, nz;
	};
	struct InstanceType
	{
		XMFLOAT3 position;
		bool isHit ;
	};
	struct VectorType
	{
		float x, y, z;
	};


public:
	ModelClass();
	ModelClass(const ModelClass&);
	~ModelClass();

	bool Initialize(ID3D11Device*, const WCHAR*, const WCHAR*, int);
	bool Initialize2D(ID3D11Device*,const WCHAR*, int, int,int,XMMATRIX);
	bool InitializeMulti(ID3D11Device*, const WCHAR*, const WCHAR*, const WCHAR*, const WCHAR*,int);
	bool InitializeEffect(ID3D11Device*, const char*, const WCHAR*, const WCHAR*, const WCHAR*);
	void Shutdown();
	void ShutdownMulti();
	void ShutdownEffect();
	void Render(ID3D11DeviceContext*);
	bool Render2D(ID3D11DeviceContext*, int, int);
	void RenderMulti(ID3D11DeviceContext*);
	void RenderEffect(ID3D11DeviceContext*);

	int GetIndexCount();
	int GetInstanceCount(int i);
	ID3D11ShaderResourceView* GetTexture();

	ID3D11ShaderResourceView* GetTexture1();
	ID3D11ShaderResourceView* GetTexture2();
	ID3D11ShaderResourceView* GetTexture3();

	ID3D11ShaderResourceView** GetTextureArray();

	bool LoadModel(const WCHAR*);
	bool LoadModelMulti(const WCHAR*);
	bool LoadModelEffect(const char*);
	void ReleaseModel();
	XMMATRIX GetViewMatrix2D() { return m_viewMatrix2D; };
	//std::vector<XMFLOAT3>& GetVertexPosArray(){return m_vertPosArray; };

private:
	bool InitializeBuffers(ID3D11Device*, int);
	bool InitializeBuffers2D(ID3D11Device*);
	bool InitializeBuffersMulti(ID3D11Device*, int);
	bool InitializeBuffersEffect(ID3D11Device*);
	void ShutdownBuffers();
	void RenderBuffers(ID3D11DeviceContext*);
	void RenderBuffersMulti(ID3D11DeviceContext*);
	void RenderBuffersEffect(ID3D11DeviceContext*);

	bool LoadTexture(ID3D11Device*, const WCHAR*);
	bool LoadTextures(ID3D11Device*, const WCHAR*, const WCHAR*, const WCHAR*);
	bool LoadTexturesEffect(ID3D11Device*, const WCHAR*, const WCHAR*, const WCHAR*);
	void ReleaseTexture();
	void ReleaseTextures();
	void ReleaseTexturesEffect();

	bool ReadFileCounts(const WCHAR*);
	bool LoadDataStructures(const WCHAR*, int, int, int, int);
	void SetPosition(InstanceType*, int);

	bool UpdateBuffers(ID3D11DeviceContext*, int, int);

	void CalculateModelVectors();
	void CalculateTangentBinormal(TempVertexType, TempVertexType, TempVertexType, VectorType&, VectorType&);
	void CalculateNormal(VectorType, VectorType, VectorType&);


private:
	ID3D11Buffer* m_vertexBuffer, * m_indexBuffer;
	ID3D11Buffer* m_instanceBuffer = nullptr;
	TextureArrayClass* m_TextureArray = nullptr;

	int m_vertexCount, m_indexCount, m_textureCount, m_normalCount, m_faceCount;
	int m_instanceCount[5] = { 0 };

	int m_screenWidth, m_screenHeight;
	int m_bitmapWidth, m_bitmapHeight;
	int m_previousPosX, m_previousPosY;

	//int m_modelCount;
	TextureClass* m_Texture;
	TextureClass* m_Texture1 = nullptr;
	TextureClass* m_Texture2 = nullptr;
	TextureClass* m_Texture3 = nullptr;
	ModelType* m_model;
	XMMATRIX m_viewMatrix2D;
public:
	std::vector<XMFLOAT3> m_vertPosArray;
	bool isHit = false;
	//bool* isHit;
};

#endif