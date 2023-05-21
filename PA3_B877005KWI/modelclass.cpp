////////////////////////////////////////////////////////////////////////////////
// Filename: modelclass.cpp
////////////////////////////////////////////////////////////////////////////////
//#include "stdafx.h"
#include "modelclass.h"
#include "TextureArrayClass.h"
#include <fstream>
using namespace std;

ModelClass::ModelClass()
{
	m_vertexBuffer = 0;
	m_indexBuffer = 0;
	m_Texture = 0;
	m_model = 0;
	m_instanceBuffer = 0;

	m_textureCount = 0;
	m_normalCount = 0;
	m_faceCount = 0;

	m_instanceCount[0] = 1;
	m_instanceCount[1] = 15;
	m_instanceCount[2] = 1;
	m_instanceCount[3] = 1;
	m_instanceCount[4] = 1;

	m_screenWidth = 1600;
	m_screenHeight = 900;

}


ModelClass::ModelClass(const ModelClass& other)
{
}


ModelClass::~ModelClass()
{
}


bool ModelClass::Initialize(ID3D11Device* device, const WCHAR* modelFilename, const WCHAR* textureFilename,int objNum)
{
	bool result;

	// Load in the model data,
	result = LoadModel(modelFilename);
	if (!result)
	{
		return false;
	}

	// Initialize the vertex and index buffers.
	result = InitializeBuffers(device, objNum);
	if(!result)
	{
		return false;
	}

	// Load the texture for this model.
	result = LoadTexture(device, textureFilename);
	if(!result)
	{
		return false;
	}

	return true;
}

bool ModelClass::Initialize2D(ID3D11Device* device, const WCHAR* textureFilename, int bitmapWidth, int bitmapHeight, int objNum, XMMATRIX ViewMatrix2D)
{
	bool result;

	// Store the size in pixels that this bitmap should be rendered at.
	m_bitmapWidth = bitmapWidth;
	m_bitmapHeight = bitmapHeight;

	// Initialize the previous rendering position to negative one.
	m_previousPosX = -1;
	m_previousPosY = -1;

	m_viewMatrix2D = ViewMatrix2D;
	// Initialize the vertex and index buffers.
	result = InitializeBuffers2D(device);
	if (!result)
	{
		return false;
	}

	// Load the texture for this bitmap.
	result = LoadTexture(device, textureFilename);
	if (!result)
	{
		return false;
	}

	return true;
}
bool ModelClass::InitializeMulti(ID3D11Device* device, const WCHAR* modelFilename,
	const WCHAR* textureFilename1, const WCHAR* textureFilename2, const WCHAR* textureFilename3, int objNum)
{
	bool result;
	result = LoadModel(modelFilename);
	// 모델 데이터를 로드합니다.
	if (!result)
	{
		return false;
	}

	// 모델의 법선, 접선 및 이항 벡터를 계산합니다.
	CalculateModelVectors();
	result = InitializeBuffersMulti(device, objNum);
	// 정점 및 인덱스 버퍼를 초기화합니다.
	if (!result)
	{
		return false;
	}

	result = LoadTextures(device, textureFilename1, textureFilename2, textureFilename3);
	if (!result)
	{
		return false;
	}

		// 이 모델의 텍스처를 로드합니다.
	return true;
	
}

bool ModelClass::InitializeEffect(ID3D11Device* device, const char* modelFilename, const WCHAR* textureFilename1, const WCHAR* textureFilename2,
	const WCHAR* textureFilename3)
{
	// 모델 데이터를 로드합니다.
	if (!LoadModelEffect(modelFilename))
	{
		return false;
	}

	// 정점 및 인덱스 버퍼를 초기화합니다.
	if (!InitializeBuffersEffect(device))
	{
		return false;
	}

	// 이 모델의 텍스처를 로드합니다.
	return LoadTexturesEffect(device, textureFilename1, textureFilename2, textureFilename3);
}



void ModelClass::Shutdown()
{
	// Release the model texture.
	ReleaseTexture();

	// Shutdown the vertex and index buffers.
	ShutdownBuffers();

	// Release the model data.
	ReleaseModel();

	return;
}
void ModelClass::ShutdownMulti()
{
	// Release the model texture.
	ReleaseTextures();

	// Shutdown the vertex and index buffers.
	ShutdownBuffers();

	// Release the model data.
	ReleaseModel();

	return;
}
void ModelClass::ShutdownEffect()
{
	// Release the model texture.
	ReleaseTexturesEffect();

	// Shutdown the vertex and index buffers.
	ShutdownBuffers();

	// Release the model data.
	ReleaseModel();

	return;
}


void ModelClass::Render(ID3D11DeviceContext* deviceContext)
{
	// Put the vertex and index buffers on the graphics pipeline to prepare them for drawing.
	RenderBuffers(deviceContext);

	return;
}

bool ModelClass::Render2D(ID3D11DeviceContext* deviceContext,int positionX, int positionY)
{
	bool result;
	result = UpdateBuffers(deviceContext, positionX, positionY);
	if (!result)
	{
		return false;
	}
	// Put the vertex and index buffers on the graphics pipeline to prepare them for drawing.
	RenderBuffers(deviceContext);

	return true;
}
void ModelClass::RenderMulti(ID3D11DeviceContext* deviceContext)
{
	// Put the vertex and index buffers on the graphics pipeline to prepare them for drawing.
	RenderBuffersMulti(deviceContext);

	return;
}

void ModelClass::RenderEffect(ID3D11DeviceContext* deviceContext)
{
	// 그리기를 준비하기 위해 그래픽 파이프 라인에 꼭지점과 인덱스 버퍼를 놓습니다.
	RenderBuffersEffect(deviceContext);
}

int ModelClass::GetIndexCount()
{
	return m_indexCount;
}

int ModelClass::GetInstanceCount(int i)
{
	return m_instanceCount[i];
}

ID3D11ShaderResourceView* ModelClass::GetTexture()
{
	return m_Texture->GetTexture();
}

ID3D11ShaderResourceView** ModelClass::GetTextureArray()
{
	return m_TextureArray->GetTextureArray();
}

bool ModelClass::InitializeBuffers(ID3D11Device* device, int objNum)
{
	VertexType* vertices;
	unsigned long* indices;
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
    D3D11_SUBRESOURCE_DATA vertexData, indexData;
	HRESULT result;
	int i;

	// Create the vertex array.
	vertices = new VertexType[m_vertexCount];
	if(!vertices)
	{
		return false;
	}

	// Create the index array.
	indices = new unsigned long[m_indexCount];
	if(!indices)
	{
		return false;
	}

	// Load the vertex array and index array with data.
	for (i = 0; i < m_vertexCount; i++)
	{
		vertices[i].position = XMFLOAT3(m_model[i].x, m_model[i].y, m_model[i].z);
		vertices[i].texture = XMFLOAT2(m_model[i].tu, m_model[i].tv);
		vertices[i].normal = XMFLOAT3(m_model[i].nx, m_model[i].ny, m_model[i].nz);

		indices[i] = i;
	}

	// Set up the description of the static vertex buffer.
    vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    vertexBufferDesc.ByteWidth = sizeof(VertexType) * m_vertexCount;
    vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vertexBufferDesc.CPUAccessFlags = 0;
    vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the vertex data.
    vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	// Now create the vertex buffer.
    result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &m_vertexBuffer);
	if(FAILED(result))
	{
		return false;
	}

	// Set up the description of the static index buffer.
    indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    indexBufferDesc.ByteWidth = sizeof(unsigned long) * m_indexCount;
    indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    indexBufferDesc.CPUAccessFlags = 0;
    indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the index data.
    indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	// Create the index buffer.
	result = device->CreateBuffer(&indexBufferDesc, &indexData, &m_indexBuffer);
	if(FAILED(result))
	{
		return false;
	}


	// 배열의 인스턴스 수를 설정합니다.

	// 인스턴스 배열을 만듭니다.
	InstanceType* instances = new InstanceType[m_instanceCount[objNum]];
	if (!instances)
	{
		return false;
	}
	if (objNum==0)

	SetPosition(instances, objNum);


	// 인스턴스 버퍼의 설명을 설정합니다.
	D3D11_BUFFER_DESC instanceBufferDesc;
	instanceBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	instanceBufferDesc.ByteWidth = sizeof(InstanceType) * m_instanceCount[objNum];
	instanceBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	instanceBufferDesc.CPUAccessFlags = 0;
	instanceBufferDesc.MiscFlags = 0;
	instanceBufferDesc.StructureByteStride = 0;

	// 하위 리소스 구조에 인스턴스 데이터에 대한 포인터를 제공합니다.
	D3D11_SUBRESOURCE_DATA instanceData;
	instanceData.pSysMem = instances;
	instanceData.SysMemPitch = 0;
	instanceData.SysMemSlicePitch = 0;

	// 인스턴스 버퍼를 만듭니다.
	if (FAILED(device->CreateBuffer(&instanceBufferDesc, &instanceData, &m_instanceBuffer)))
	{
		return false;
	}

	
	// Release the arrays now that the vertex and index buffers have been created and loaded.
	delete [] vertices;
	vertices = 0;

	delete [] indices;
	indices = 0;

	// 인스턴스 버퍼가 생성되고로드되었으므로 인스턴스 배열을 해제합니다.
	delete[] instances;
	instances = 0;

	return true;
}

bool ModelClass::InitializeBuffers2D(ID3D11Device* device)
{
	VertexType* vertices;
	unsigned long* indices;
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;
	HRESULT result;
	int i;
	// Set the number of vertices in the vertex array.
	m_vertexCount = 18;

	// Set the number of indices in the index array.
	m_indexCount = m_vertexCount;

	// Create the vertex array.
	vertices = new VertexType[m_vertexCount];
	if (!vertices)
	{
		return false;
	}

	// Create the index array.
	indices = new unsigned long[m_indexCount];
	if (!indices)
	{
		return false;
	}
	// Initialize vertex array to zeros at first.
	memset(vertices, 0, (sizeof(VertexType) * m_vertexCount));

	// Load the index array with data.
	for (i = 0; i < m_indexCount; i++)
	{
		indices[i] = i;
	}

	// Set up the description of the static vertex buffer.
	vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	vertexBufferDesc.ByteWidth = sizeof(VertexType) * m_vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the vertex data.
	vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	// Now create the vertex buffer.
	result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &m_vertexBuffer);
	if (FAILED(result))
	{
		return false;
	}

	// Set up the description of the static index buffer.
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * m_indexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the index data.
	indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	// Create the index buffer.
	result = device->CreateBuffer(&indexBufferDesc, &indexData, &m_indexBuffer);
	if (FAILED(result))
	{
		return false;
	}

	// Release the arrays now that the vertex and index buffers have been created and loaded.
	delete[] vertices;
	vertices = 0;

	delete[] indices;
	indices = 0;

	return true;
}

bool ModelClass::InitializeBuffersMulti(ID3D11Device* device, int objNum)
{
	// 정점 배열을 만듭니다.
	VertexType* vertices = new VertexType[m_vertexCount];
	if (!vertices)
	{
		return false;
	}

	// 인덱스 배열을 만듭니다.
	unsigned long* indices = new unsigned long[m_indexCount];
	if (!indices)
	{
		return false;
	}

	// 정점 배열과 인덱스 배열을 데이터로 읽어옵니다.
	for (int i = 0; i < m_vertexCount; i++)
	{
		vertices[i].position = XMFLOAT3(m_model[i].x, m_model[i].y, m_model[i].z);
		vertices[i].texture = XMFLOAT2(m_model[i].tu, m_model[i].tv);
		vertices[i].normal = XMFLOAT3(m_model[i].nx, m_model[i].ny, m_model[i].nz);
		vertices[i].tangent = XMFLOAT3(m_model[i].tx, m_model[i].ty, m_model[i].tz);
		vertices[i].binormal = XMFLOAT3(m_model[i].bx, m_model[i].by, m_model[i].bz);

		indices[i] = i;
	}

	// 정적 정점 버퍼의 구조체를 설정합니다.
	D3D11_BUFFER_DESC vertexBufferDesc;
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(VertexType) * m_vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	// subresource 구조에 정점 데이터에 대한 포인터를 제공합니다.
	D3D11_SUBRESOURCE_DATA vertexData;
	vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	// 이제 정점 버퍼를 만듭니다.
	if (FAILED(device->CreateBuffer(&vertexBufferDesc, &vertexData, &m_vertexBuffer)))
	{
		return false;
	}

	// 정적 인덱스 버퍼의 구조체를 설정합니다.
	D3D11_BUFFER_DESC indexBufferDesc;
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * m_indexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	// 인덱스 데이터를 가리키는 보조 리소스 구조체를 작성합니다.
	D3D11_SUBRESOURCE_DATA indexData;
	indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	// 인덱스 버퍼를 생성합니다.
	if (FAILED(device->CreateBuffer(&indexBufferDesc, &indexData, &m_indexBuffer)))
	{
		return false;
	}

	// 인스턴스 배열을 만듭니다.
	InstanceType* instances = new InstanceType[m_instanceCount[objNum]];
	if (!instances)
	{
		return false;
	}

	SetPosition(instances, objNum);


	// 인스턴스 버퍼의 설명을 설정합니다.
	D3D11_BUFFER_DESC instanceBufferDesc;
	instanceBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	instanceBufferDesc.ByteWidth = sizeof(InstanceType) * m_instanceCount[objNum];
	instanceBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	instanceBufferDesc.CPUAccessFlags = 0;
	instanceBufferDesc.MiscFlags = 0;
	instanceBufferDesc.StructureByteStride = 0;

	// 하위 리소스 구조에 인스턴스 데이터에 대한 포인터를 제공합니다.
	D3D11_SUBRESOURCE_DATA instanceData;
	instanceData.pSysMem = instances;
	instanceData.SysMemPitch = 0;
	instanceData.SysMemSlicePitch = 0;

	// 인스턴스 버퍼를 만듭니다.
	if (FAILED(device->CreateBuffer(&instanceBufferDesc, &instanceData, &m_instanceBuffer)))
	{
		return false;
	}


	// Release the arrays now that the vertex and index buffers have been created and loaded.
	delete[] vertices;
	vertices = 0;

	delete[] indices;
	indices = 0;

	// 인스턴스 버퍼가 생성되고로드되었으므로 인스턴스 배열을 해제합니다.
	delete[] instances;
	instances = 0;

	return true;
}

bool ModelClass::InitializeBuffersEffect(ID3D11Device* device)
{
	// 정점 배열을 만듭니다.
	VertexType* vertices = new VertexType[m_vertexCount];
	if (!vertices)
	{
		return false;
	}

	// 인덱스 배열을 만듭니다.
	unsigned long* indices = new unsigned long[m_indexCount];
	if (!indices)
	{
		return false;
	}

	// 정점 배열과 인덱스 배열을 데이터로 읽어옵니다.
	for (int i = 0; i < m_vertexCount; i++)
	{
		vertices[i].position = XMFLOAT3(m_model[i].x, m_model[i].y, m_model[i].z);
		vertices[i].texture = XMFLOAT2(m_model[i].tu, m_model[i].tv);
		indices[i] = i;
	}

	// 정적 정점 버퍼의 구조체를 설정합니다.
	D3D11_BUFFER_DESC vertexBufferDesc;
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(VertexType) * m_vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	// subresource 구조에 정점 데이터에 대한 포인터를 제공합니다.
	D3D11_SUBRESOURCE_DATA vertexData;
	vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	// 이제 정점 버퍼를 만듭니다.
	if (FAILED(device->CreateBuffer(&vertexBufferDesc, &vertexData, &m_vertexBuffer)))
	{
		return false;
	}

	// 정적 인덱스 버퍼의 구조체를 설정합니다.
	D3D11_BUFFER_DESC indexBufferDesc;
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * m_indexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	// 인덱스 데이터를 가리키는 보조 리소스 구조체를 작성합니다.
	D3D11_SUBRESOURCE_DATA indexData;
	indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	// 인덱스 버퍼를 생성합니다.
	if (FAILED(device->CreateBuffer(&indexBufferDesc, &indexData, &m_indexBuffer)))
	{
		return false;
	}

	// 생성되고 값이 할당된 정점 버퍼와 인덱스 버퍼를 해제합니다.
	delete[] vertices;
	vertices = 0;

	delete[] indices;
	indices = 0;

	return true;
}

void ModelClass::SetPosition(InstanceType* instances, int objNum)
{
	switch (objNum)
	{
	case 0:
		instances[0].position = XMFLOAT3(0.0f, 0.0f, 0.0f);
		/*instances[1].position = XMFLOAT3(0.0f, 0.0f, 500.0f);
		instances[2].position = XMFLOAT3(300.0f, 0.0f, 400.0f);
		instances[3].position = XMFLOAT3(150.0f, 0.0f, 850.0f);
		instances[4].position = XMFLOAT3(-150.0f, 0.0f, 650.0f);*/
		break;
	case 1: 
		instances[0].position = XMFLOAT3(-120.0f, 0.0f, 0.0f);
		instances[1].position = XMFLOAT3(-60.0f, 0.0f, 0.0f);
		instances[2].position = XMFLOAT3(0.0f, 0.0f, 0.0f);
		instances[3].position = XMFLOAT3(60.0f, 0.0f, 0.0f);
		instances[4].position = XMFLOAT3(120.0f, 0.0f, 0.0f);

		instances[5].position = XMFLOAT3(-120.0f, 0.0f, 100.0f);
		instances[6].position = XMFLOAT3(-60.0f, 0.0f, 100.0f);
		instances[7].position = XMFLOAT3(0.0f, 0.0f, 100.0f);
		instances[8].position = XMFLOAT3(60.0f, 0.0f, 100.0f);
		instances[9].position = XMFLOAT3(120.0f, 0.0f, 100.0f);

		instances[10].position = XMFLOAT3(-120.0f, 0.0f, 200.0f);
		instances[11].position = XMFLOAT3(-60.0f, 0.0f, 200.0f);
		instances[12].position = XMFLOAT3(0.0f, 0.0f, 200.0f);
		instances[13].position = XMFLOAT3(60.0f, 0.0f, 200.0f);
		instances[14].position = XMFLOAT3(120.0f, 0.0f, 200.0f);
		break;
	//case 2:
	//	instances[0].position = XMFLOAT3(0.0f, 0.0f, 0.0f);
	//	break;
	/*case 3:

		break;*/
	//case 4:
	//	instances[0].position = XMFLOAT3(0.0f, 0.0f, 0.0f);
	//	break;

	//case 5:
	//	instances[0].position = XMFLOAT3(0.0f, 0.0f, 0.0f);
	//	break;
	//case 6:
	//	instances[0].position = XMFLOAT3(0.0f, 0.0f, 0.0f);
	//	break;
	//case 7:
	//	instances[0].position = XMFLOAT3(0.0f, 0.0f, 0.0f);
	//	break;
	default:
		instances[0].position = XMFLOAT3(0.0f, 0.0f, 0.0f);
		break;
	}
	return;
}


void ModelClass::ShutdownBuffers()
{
	// Release the index buffer.
	if(m_indexBuffer)
	{
		m_indexBuffer->Release();
		m_indexBuffer = 0;
	}

	// Release the vertex buffer.
	if(m_vertexBuffer)
	{
		m_vertexBuffer->Release();
		m_vertexBuffer = 0;
	}

	// 인스턴스 버퍼를 해제합니다.
	if (m_instanceBuffer)
	{
		m_instanceBuffer->Release();
		m_instanceBuffer = 0;
	}

	return;
}
bool ModelClass::UpdateBuffers(ID3D11DeviceContext* deviceContext, int positionX, int positionY)
{
	float left, right, top, bottom;
	VertexType* vertices;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	VertexType* verticesPtr;
	HRESULT result;


	// If the position we are rendering this bitmap to has not changed then don't update the vertex buffer since it
	// currently has the correct parameters.
	/*if ((positionX == m_previousPosX) && (positionY == m_previousPosY))
	{
		return true;
	}*/

	// If it has changed then update the position it is being rendered to.
	m_previousPosX = positionX;
	m_previousPosY = positionY;

	// Calculate the screen coordinates of the left side of the bitmap.
	left = (float)((m_screenWidth / 2) * -1) + (float)positionX;

	// Calculate the screen coordinates of the right side of the bitmap.
	right = left + (float)m_bitmapWidth;

	// Calculate the screen coordinates of the top of the bitmap.
	top = (float)(m_screenHeight / 2) - (float)positionY;

	// Calculate the screen coordinates of the bottom of the bitmap.
	bottom = top - (float)m_bitmapHeight;

	// Create the vertex array.
	vertices = new VertexType[m_vertexCount];
	if (!vertices)
	{
		return false;
	}

	// Load the vertex array with data.
	// First triangle.
	//vertices[0].position = XMFLOAT3(left, top, 0.0f);  // Top left.
	//vertices[0].texture = XMFLOAT2(0.0f, 0.0f);

	//vertices[1].position = XMFLOAT3(right, bottom, 0.0f);  // Bottom right.
	//vertices[1].texture = XMFLOAT2(1.0f, 1.0f);

	//vertices[2].position = XMFLOAT3(left, bottom, 0.0f);  // Bottom left.
	//vertices[2].texture = XMFLOAT2(0.0f, 1.0f);

	//// Second triangle.
	//vertices[3].position = XMFLOAT3(left, top, 0.0f);  // Top left.
	//vertices[3].texture = XMFLOAT2(0.0f, 0.0f);

	//vertices[4].position = XMFLOAT3(right, top, 0.0f);  // Top right.
	//vertices[4].texture = XMFLOAT2(1.0f, 0.0f);

	//vertices[5].position = XMFLOAT3(right, bottom, 0.0f);  // Bottom right.
	//vertices[5].texture = XMFLOAT2(1.0f, 1.0f);

	vertices[0].position = XMFLOAT3(0.0f, 27.5f, 0.0f);  
	vertices[0].texture = XMFLOAT2(0.0f, 0.55f);
	vertices[1].position = XMFLOAT3(50.0f, 22.5f, 0.0f);  
	vertices[1].texture = XMFLOAT2(1.0f, 0.45f);
	vertices[2].position = XMFLOAT3(0.0f, 22.5, 0.0f);  
	vertices[2].texture = XMFLOAT2(0.0f, 0.45f);


	vertices[3].position = XMFLOAT3(0.0f, 27.5f, 0.0f);
	vertices[3].texture = XMFLOAT2(0.0f, 0.55f);
	vertices[4].position = XMFLOAT3(50.0f, 27.5f, 0.0f);
	vertices[4].texture = XMFLOAT2(1.0f, 0.55f);
	vertices[5].position = XMFLOAT3(50.0f, 22.5, 0.0f);
	vertices[5].texture = XMFLOAT2(1.0f, 0.45f);

	vertices[6].position = XMFLOAT3(22.5f, 50.0f, 0.0f);
	vertices[6].texture = XMFLOAT2(0.45f, 1.0f);
	vertices[7].position = XMFLOAT3(27.5f, 27.5f, 0.0f);
	vertices[7].texture = XMFLOAT2(0.55f, 0.55f);
	vertices[8].position = XMFLOAT3(22.5f, 27.5, 0.0f);
	vertices[8].texture = XMFLOAT2(0.45f, 0.55f);

	vertices[9].position = XMFLOAT3(22.5f, 50.0f, 0.0f);
	vertices[9].texture = XMFLOAT2(0.45f, 1.0f);
	vertices[10].position = XMFLOAT3(27.5f, 50.0f, 0.0f);
	vertices[10].texture = XMFLOAT2(0.55f, 1.0f);
	vertices[11].position = XMFLOAT3(27.5f, 27.5f, 0.0f);
	vertices[11].texture = XMFLOAT2(0.55f, 0.55f);

	vertices[12].position = XMFLOAT3(22.5f, 22.5f, 0.0f);
	vertices[12].texture = XMFLOAT2(0.45f, 0.45f);
	vertices[13].position = XMFLOAT3(27.5f, 0.0f, 0.0f);
	vertices[13].texture = XMFLOAT2(0.55f, 0.0f);
	vertices[14].position = XMFLOAT3(22.5f, 0.0f, 0.0f);
	vertices[14].texture = XMFLOAT2(0.45f, 0.0f);

	vertices[15].position = XMFLOAT3(22.5f, 22.5f, 0.0f);
	vertices[15].texture = XMFLOAT2(0.45f, 0.45f);
	vertices[16].position = XMFLOAT3(27.5f, 22.5f, 0.0f);
	vertices[16].texture = XMFLOAT2(0.55f, 0.45f);
	vertices[17].position = XMFLOAT3(27.5f, 0.0f, 0.0f);
	vertices[17].texture = XMFLOAT2(0.55f, 0.0f);


	// Lock the vertex buffer so it can be written to.
	result = deviceContext->Map(m_vertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result))
	{
		return false;
	}

	// Get a pointer to the data in the vertex buffer.
	verticesPtr = (VertexType*)mappedResource.pData;

	// Copy the data into the vertex buffer.
	memcpy(verticesPtr, (void*)vertices, (sizeof(VertexType) * m_vertexCount));

	// Unlock the vertex buffer.
	deviceContext->Unmap(m_vertexBuffer, 0);

	// Release the vertex array as it is no longer needed.
	delete[] vertices;
	vertices = 0;

	return true;
}


void ModelClass::RenderBuffers(ID3D11DeviceContext* deviceContext)
{
	//unsigned int stride;
	//unsigned int offset;


	//// Set vertex buffer stride and offset.
	//stride = sizeof(VertexType); 
	//offset = 0;
 //   
	//// Set the vertex buffer to active in the input assembler so it can be rendered.
	//deviceContext->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);

 //   // Set the index buffer to active in the input assembler so it can be rendered.
	//deviceContext->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);

 //   // Set the type of primitive that should be rendered from this vertex buffer, in this case triangles.
	//deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	//
	// 버퍼 오프셋과 스트라이드를 설정합니다.
	unsigned int strides[2] = { sizeof(VertexType), sizeof(InstanceType) };
	unsigned int offsets[2] = { 0, 0 };

	// 포인터의 배열을 정점 버퍼와 인스턴스 버퍼로 설정합니다.
	ID3D11Buffer* bufferPointers[2] = { m_vertexBuffer, m_instanceBuffer };

	// 렌더링 할 수 있도록 입력 어셈블러에서 정점 버퍼를 활성으로 설정합니다.
	deviceContext->IASetVertexBuffers(0, 2, bufferPointers, strides, offsets);

	deviceContext->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	// 이 꼭지점 버퍼에서 렌더링되어야하는 프리미티브 유형을 설정합니다.이 경우에는 삼각형입니다.
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	return;
}
void ModelClass::RenderBuffersMulti(ID3D11DeviceContext* deviceContext)
{
	//unsigned int stride;
	//unsigned int offset;


	//// Set vertex buffer stride and offset.
	//stride = sizeof(VertexType); 
	//offset = 0;
 //   
	//// Set the vertex buffer to active in the input assembler so it can be rendered.
	//deviceContext->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);

 //   // Set the index buffer to active in the input assembler so it can be rendered.
	//deviceContext->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);

 //   // Set the type of primitive that should be rendered from this vertex buffer, in this case triangles.
	//deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	//
	// 버퍼 오프셋과 스트라이드를 설정합니다.
	unsigned int strides[2] = { sizeof(VertexType), sizeof(InstanceType) };
	unsigned int offsets[2] = { 0, 0 };

	// 포인터의 배열을 정점 버퍼와 인스턴스 버퍼로 설정합니다.
	ID3D11Buffer* bufferPointers[2] = { m_vertexBuffer, m_instanceBuffer };

	// 렌더링 할 수 있도록 입력 어셈블러에서 정점 버퍼를 활성으로 설정합니다.
	deviceContext->IASetVertexBuffers(0, 2, bufferPointers, strides, offsets);

	deviceContext->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	// 이 꼭지점 버퍼에서 렌더링되어야하는 프리미티브 유형을 설정합니다.이 경우에는 삼각형입니다.
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	return;
}

void ModelClass::RenderBuffersEffect(ID3D11DeviceContext* deviceContext)
{
	// 정점 버퍼의 단위와 오프셋을 설정합니다.
	UINT stride = sizeof(VertexType);
	UINT offset = 0;

	// 렌더링 할 수 있도록 입력 어셈블러에서 정점 버퍼를 활성으로 설정합니다.
	deviceContext->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);

	// 렌더링 할 수 있도록 입력 어셈블러에서 인덱스 버퍼를 활성으로 설정합니다.
	deviceContext->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	// 정점 버퍼로 그릴 기본형을 설정합니다. 여기서는 삼각형으로 설정합니다.
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}


bool ModelClass::LoadTexture(ID3D11Device* device, const WCHAR* filename)
{
	bool result;


	// Create the texture object.
	m_Texture = new TextureClass;
	if(!m_Texture)
	{
		return false;
	}

	// Initialize the texture object.
	result = m_Texture->Initialize(device, filename);
	if(!result)
	{
		return false;
	}

	return true;
}

bool ModelClass::LoadTextures(ID3D11Device* device, const WCHAR* filename1, const WCHAR* filename2, const WCHAR* filename3)
{

	// 텍스처 배열 오브젝트를 생성한다.
	m_TextureArray = new TextureArrayClass;
	if (!m_TextureArray)
	{
		return false;
	}

	// 텍스처 배열 오브젝트를 초기화한다.
	return m_TextureArray->Initialize(device, filename1, filename2, filename3);
}

bool ModelClass::LoadTexturesEffect(ID3D11Device* device, const WCHAR* textureFilename1, const WCHAR* textureFilename2, const WCHAR* textureFilename3)
{
	// 첫번째 텍스처 오브젝트를 생성한다.
	m_Texture1 = new TextureClass;
	if (!m_Texture1)
	{
		return false;
	}

	// 첫번째 텍스처 오브젝트를 초기화한다
	bool result = m_Texture1->Initialize(device, textureFilename1);
	if (!result)
	{
		return false;
	}

	// 두번째 텍스처 오브젝트를 생성한다.
	m_Texture2 = new TextureClass;
	if (!m_Texture2)
	{
		return false;
	}

	// 두번째 텍스처 오브젝트를 초기화한다
	result = m_Texture2->Initialize(device, textureFilename2);
	if (!result)
	{
		return false;
	}

	// 세번째 텍스처 오브젝트를 생성한다.
	m_Texture3 = new TextureClass;
	if (!m_Texture3)
	{
		return false;
	}

	// 세번째 텍스처 오브젝트를 초기화한다
	result = m_Texture3->Initialize(device, textureFilename3);
	if (!result)
	{
		return false;
	}

	return true;
}

void ModelClass::ReleaseTexture()
{
	// Release the texture object.
	if(m_Texture)
	{
		m_Texture->Shutdown();
		delete m_Texture;
		m_Texture = 0;
	}

	return;
}
void ModelClass::ReleaseTextures()
{
	// 텍스처 배열 오브젝트를 릴리즈한다.
	if (m_TextureArray)
	{
		m_TextureArray->Shutdown();
		delete m_TextureArray;
		m_TextureArray = 0;
	}
}
void ModelClass::ReleaseTexturesEffect()
{
	// 텍스처 오브젝트를 해제한다
	if (m_Texture1)
	{
		m_Texture1->Shutdown();
		delete m_Texture1;
		m_Texture1 = 0;
	}

	if (m_Texture2)
	{
		m_Texture2->Shutdown();
		delete m_Texture2;
		m_Texture2 = 0;
	}

	if (m_Texture3)
	{
		m_Texture3->Shutdown();
		delete m_Texture3;
		m_Texture3 = 0;
	}
}

bool ModelClass::LoadModel(const WCHAR* filename)
{
	ReadFileCounts(filename);

	return true;
}

bool ModelClass::LoadModelMulti(const WCHAR* filename)
{
	// 모델 파일을 엽니다.
	ifstream fin;
	fin.open(filename);

	// 파일을 열 수 없으면 종료합니다.
	if (fin.fail())
	{
		return false;
	}

	// 버텍스 카운트의 값까지 읽는다.
	char input = 0;
	fin.get(input);
	while (input != ':')
	{
		fin.get(input);
	}

	// 버텍스 카운트를 읽는다.
	fin >> m_vertexCount;

	// 인덱스의 수를 정점 수와 같게 설정합니다.
	m_indexCount = m_vertexCount;

	// 읽어 들인 정점 개수를 사용하여 모델을 만듭니다.
	m_model = new ModelType[m_vertexCount];
	if (!m_model)
	{
		return false;
	}

	// 데이터의 시작 부분까지 읽는다.
	fin.get(input);
	while (input != ':')
	{
		fin.get(input);
	}
	fin.get(input);
	fin.get(input);

	// 버텍스 데이터를 읽습니다.
	for (int i = 0; i < m_vertexCount; i++)
	{
		fin >> m_model[i].x >> m_model[i].y >> m_model[i].z;
		fin >> m_model[i].tu >> m_model[i].tv;
		fin >> m_model[i].nx >> m_model[i].ny >> m_model[i].nz;
	}

	// 모델 파일을 닫는다.
	fin.close();

	return true;
}
bool ModelClass::LoadModelEffect(const char* filename)
{
	// 모델 파일을 엽니다.
	ifstream fin;
	fin.open(filename);

	// 파일을 열 수 없으면 종료합니다.
	if (fin.fail())
	{
		return false;
	}

	// 버텍스 카운트의 값까지 읽는다.
	char input = 0;
	fin.get(input);
	while (input != ':')
	{
		fin.get(input);
	}

	// 버텍스 카운트를 읽는다.
	fin >> m_vertexCount;

	// 인덱스의 수를 정점 수와 같게 설정합니다.
	m_indexCount = m_vertexCount;

	// 읽어 들인 정점 개수를 사용하여 모델을 만듭니다.
	m_model = new ModelType[m_vertexCount];
	if (!m_model)
	{
		return false;
	}

	// 데이터의 시작 부분까지 읽는다.
	fin.get(input);
	while (input != ':')
	{
		fin.get(input);
	}
	fin.get(input);
	fin.get(input);

	// 버텍스 데이터를 읽습니다.
	for (int i = 0; i < m_vertexCount; i++)
	{
		fin >> m_model[i].x >> m_model[i].y >> m_model[i].z;
		fin >> m_model[i].tu >> m_model[i].tv;
		fin >> m_model[i].nx >> m_model[i].ny >> m_model[i].nz;
	}

	// 모델 파일을 닫는다.
	fin.close();

	return true;
}
void ModelClass::ReleaseModel()
{
	if (m_model)
	{
		delete[] m_model;
		m_model = 0;
	}

	return;
}


bool ModelClass::ReadFileCounts(const WCHAR* filename)
{

	ifstream fin;
	char input;
	// Initialize the counts.
	int vertexCount = 0;
	int textureCount = 0;
	int normalCount = 0;
	int faceCount = 0;
	// Open the file.
	fin.open(filename);
	// Check if it was successful in opening the file.
	if (fin.fail() == true)
	{
		return false;
	}
	// Read from the file and continue to read until the end of the file is reached.
	fin.get(input);
	while (!fin.eof())
	{
		// If the line starts with 'v' then count either the vertex, the texture coordinates, or the normal vector.
		if (input == 'v')
		{
			fin.get(input);
			if (input == ' ') { vertexCount++; }
			if (input == 't') { textureCount++; }
			if (input == 'n') { normalCount++; }
		}

		// If the line starts with 'f' then increment the face count.
		if (input == 'f')
		{
			fin.get(input);
			if (input == ' ') { faceCount++; }
		}

		// Otherwise read in the remainder of the line.
		while (input != '\n')
		{
			fin.get(input);
			if (fin.eof())
				break;
		}

		// Start reading the beginning of the next line.
		fin.get(input);
	}
	// Close the file.
	fin.close();

	LoadDataStructures(filename, vertexCount, textureCount, normalCount, faceCount);

	return true;
}

bool ModelClass::LoadDataStructures(const WCHAR* filename, int vertexCount, int textureCount, int normalCount, int faceCount)
{
	XMFLOAT3* vertices, * texcoords, * normals;
	FaceType* faces;
	ifstream fin;
	int vertexIndex, texcoordIndex, normalIndex, faceIndex, vIndex, tIndex, nIndex;
	char input, input2;
	ofstream fout;

	// Initialize the four data structures.
	vertices = new XMFLOAT3[vertexCount];
	if (!vertices)
	{
		return false;
	}

	texcoords = new XMFLOAT3[textureCount];
	if (!texcoords)
	{
		return false;
	}

	normals = new XMFLOAT3[normalCount];
	if (!normals)
	{
		return false;
	}

	faces = new FaceType[faceCount];
	if (!faces)
	{
		return false;
	}
	// Initialize the indexes.
	vertexIndex = 0;
	texcoordIndex = 0;
	normalIndex = 0;
	faceIndex = 0;

	// Open the file.
	fin.open(filename);

	// Check if it was successful in opening the file.
	if (fin.fail() == true)
	{
		return false;
	}

	// Read in the vertices, texture coordinates, and normals into the data structures.
	// Important: Also convert to left hand coordinate system since Maya uses right hand coordinate system.
	fin.get(input);
	while (!fin.eof())
	{
		if (input == 'v')
		{
			fin.get(input);

			// Read in the vertices.
			if (input == ' ')
			{
				fin >> vertices[vertexIndex].x >> vertices[vertexIndex].y >>
					vertices[vertexIndex].z;

				// Invert the Z vertex to change to left hand system.
				vertices[vertexIndex].z = vertices[vertexIndex].z * -1.0f;
				vertexIndex++;
			}

			// Read in the texture uv coordinates.
			if (input == 't')
			{
				fin >> texcoords[texcoordIndex].x >> texcoords[texcoordIndex].y;
				// Invert the V texture coordinates to left hand system.
				texcoords[texcoordIndex].y = 1.0f - texcoords[texcoordIndex].y;
				texcoordIndex++;
			}

			// Read in the normals.
			if (input == 'n')
			{
				fin >> normals[normalIndex].x >> normals[normalIndex].y >>
					normals[normalIndex].z;

				// Invert the Z normal to change to left hand system.
				normals[normalIndex].z = normals[normalIndex].z * -1.0f;
				normalIndex++;
			}
		}

		// Read in the faces.
		if (input == 'f')
		{
			fin.get(input);
			if (input == ' ')
			{
				// Read the face data in backwards to convert it to a left hand system from right hand system.
				fin >> faces[faceIndex].vIndex3 >> input2 >> faces[faceIndex].tIndex3 >>
					input2 >> faces[faceIndex].nIndex3 >> faces[faceIndex].vIndex2 >> input2 >>
					faces[faceIndex].tIndex2 >> input2 >> faces[faceIndex].nIndex2 >>
					faces[faceIndex].vIndex1 >> input2 >> faces[faceIndex].tIndex1 >> input2 >>
					faces[faceIndex].nIndex1;
				faceIndex++;
			}
		}

		// Read in the remainder of the line.
		while (input != '\n')
		{
			fin.get(input);
			if (fin.eof())
				break;
		}

		// Start reading the beginning of the next line.
		fin.get(input);
	}

	//// Close the file.
	//fin.close();
	//// Open the output file.
	//fout.open("model.txt");
	//// Write out the file header that our model format uses.
	//fout << "Vertex Count: " << (faceCount * 3) << endl;
	//fout << endl;
	//fout << "Data:" << endl;
	//fout << endl;

	m_vertexCount = faceCount * 3;

	// Set the number of indices to be the same as the vertex count.
	m_indexCount = m_vertexCount;

	// Create the model using the vertex count that was read in.
	m_model = new ModelType[m_vertexCount];
	if (!m_model)
	{
		return false;
	}

	// Now loop through all the faces and output the three vertices for each face.
	for (int i = 0; i < faceIndex; i++)
	{
		vIndex = faces[i].vIndex1 - 1;
		tIndex = faces[i].tIndex1 - 1;
		nIndex = faces[i].nIndex1 - 1;
		//fout << vertices[vIndex].x << ' ' << vertices[vIndex].y << ' ' << vertices[vIndex].z << ' '
		//	<< texcoords[tIndex].x << ' ' << texcoords[tIndex].y << ' '
		//	<< normals[nIndex].x << ' ' << normals[nIndex].y << ' ' << normals[nIndex].z << endl;

		m_model[i * 3].x = vertices[vIndex].x;
		m_model[i * 3].y = vertices[vIndex].y;
		m_model[i * 3].z = vertices[vIndex].z;

		m_model[i * 3].tu = texcoords[tIndex].x;
		m_model[i * 3].tv = texcoords[tIndex].y;

		m_model[i * 3].nx = normals[nIndex].x;
		m_model[i * 3].ny = normals[nIndex].y;
		m_model[i * 3].nz = normals[nIndex].z;

		vIndex = faces[i].vIndex2 - 1;
		tIndex = faces[i].tIndex2 - 1;
		nIndex = faces[i].nIndex2 - 1;
		//fout << vertices[vIndex].x << ' ' << vertices[vIndex].y << ' ' << vertices[vIndex].z << ' '
		//	<< texcoords[tIndex].x << ' ' << texcoords[tIndex].y << ' '
		//	<< normals[nIndex].x << ' ' << normals[nIndex].y << ' ' << normals[nIndex].z << endl;

		m_model[i * 3 + 1].x = vertices[vIndex].x;
		m_model[i * 3 + 1].y = vertices[vIndex].y;
		m_model[i * 3 + 1].z = vertices[vIndex].z;

		m_model[i * 3 + 1].tu = texcoords[tIndex].x;
		m_model[i * 3 + 1].tv = texcoords[tIndex].y;

		m_model[i * 3 + 1].nx = normals[nIndex].x;
		m_model[i * 3 + 1].ny = normals[nIndex].y;
		m_model[i * 3 + 1].nz = normals[nIndex].z;

		vIndex = faces[i].vIndex3 - 1;
		tIndex = faces[i].tIndex3 - 1;
		nIndex = faces[i].nIndex3 - 1;
		//fout << vertices[vIndex].x << ' ' << vertices[vIndex].y << ' ' << vertices[vIndex].z << ' '
		//	<< texcoords[tIndex].x << ' ' << texcoords[tIndex].y << ' '
		//	<< normals[nIndex].x << ' ' << normals[nIndex].y << ' ' << normals[nIndex].z << endl;

		m_model[i * 3 + 2].x = vertices[vIndex].x;
		m_model[i * 3 + 2].y = vertices[vIndex].y;
		m_model[i * 3 + 2].z = vertices[vIndex].z;

		m_model[i * 3 + 2].tu = texcoords[tIndex].x;
		m_model[i * 3 + 2].tv = texcoords[tIndex].y;

		m_model[i * 3 + 2].nx = normals[nIndex].x;
		m_model[i * 3 + 2].ny = normals[nIndex].y;
		m_model[i * 3 + 2].nz = normals[nIndex].z;
	}

	//// Close the output file.
	//fout.close();

	////////////////////////////////////////////
	for (int i=0; i < vertexCount; i++)
	{
		m_vertPosArray.push_back(vertices[i]);
	}
	////////////////////////////////////////////
	// 
	// Release the four data structures.
	if (vertices)
	{
		delete[] vertices;
		vertices = 0;
	}
	
	if (texcoords)
	{
		delete[] texcoords;
		texcoords = 0;
	}

	if (normals)
	{
		delete[] normals;
		normals = 0;
	}

	if (faces)
	{
		delete[] faces;
		faces = 0;
	}

	return true;
}

void ModelClass::CalculateModelVectors()
{
	TempVertexType vertex1, vertex2, vertex3;
	VectorType tangent, binormal, normal;


	// 모델의 면 수를 계산합니다.
	int faceCount = m_vertexCount / 3;

	// 모델 데이터에 대한 인덱스를 초기화합니다.
	int index = 0;

	// 모든면을 살펴보고 접선, 비공식 및 법선 벡터를 계산합니다.
	for (int i = 0; i < faceCount; i++)
	{
		// 모델에서이면에 대한 세 개의 정점을 가져옵니다.
		vertex1.x = m_model[index].x;
		vertex1.y = m_model[index].y;
		vertex1.z = m_model[index].z;
		vertex1.tu = m_model[index].tu;
		vertex1.tv = m_model[index].tv;
		vertex1.nx = m_model[index].nx;
		vertex1.ny = m_model[index].ny;
		vertex1.nz = m_model[index].nz;
		index++;

		vertex2.x = m_model[index].x;
		vertex2.y = m_model[index].y;
		vertex2.z = m_model[index].z;
		vertex2.tu = m_model[index].tu;
		vertex2.tv = m_model[index].tv;
		vertex2.nx = m_model[index].nx;
		vertex2.ny = m_model[index].ny;
		vertex2.nz = m_model[index].nz;
		index++;

		vertex3.x = m_model[index].x;
		vertex3.y = m_model[index].y;
		vertex3.z = m_model[index].z;
		vertex3.tu = m_model[index].tu;
		vertex3.tv = m_model[index].tv;
		vertex3.nx = m_model[index].nx;
		vertex3.ny = m_model[index].ny;
		vertex3.nz = m_model[index].nz;
		index++;

		// 표면의 탄젠트와 바이 노멀을 계산합니다.
		CalculateTangentBinormal(vertex1, vertex2, vertex3, tangent, binormal);

		// 접선과 binormal을 사용하여 새 법선을 계산합니다.
		CalculateNormal(tangent, binormal, normal);

		// 모델 구조에서 면의 법선, 접선 및 바이 노멀을 저장합니다.
		m_model[index - 1].nx = normal.x;
		m_model[index - 1].ny = normal.y;
		m_model[index - 1].nz = normal.z;
		m_model[index - 1].tx = tangent.x;
		m_model[index - 1].ty = tangent.y;
		m_model[index - 1].tz = tangent.z;
		m_model[index - 1].bx = binormal.x;
		m_model[index - 1].by = binormal.y;
		m_model[index - 1].bz = binormal.z;

		m_model[index - 2].nx = normal.x;
		m_model[index - 2].ny = normal.y;
		m_model[index - 2].nz = normal.z;
		m_model[index - 2].tx = tangent.x;
		m_model[index - 2].ty = tangent.y;
		m_model[index - 2].tz = tangent.z;
		m_model[index - 2].bx = binormal.x;
		m_model[index - 2].by = binormal.y;
		m_model[index - 2].bz = binormal.z;

		m_model[index - 3].nx = normal.x;
		m_model[index - 3].ny = normal.y;
		m_model[index - 3].nz = normal.z;
		m_model[index - 3].tx = tangent.x;
		m_model[index - 3].ty = tangent.y;
		m_model[index - 3].tz = tangent.z;
		m_model[index - 3].bx = binormal.x;
		m_model[index - 3].by = binormal.y;
		m_model[index - 3].bz = binormal.z;
	}
}


void ModelClass::CalculateTangentBinormal(TempVertexType vertex1, TempVertexType vertex2, TempVertexType vertex3,
	VectorType& tangent, VectorType& binormal)
{
	float vector1[3], vector2[3];
	float tuVector[2], tvVector[2];


	// 현재 표면의 두 벡터를 계산합니다.
	vector1[0] = vertex2.x - vertex1.x;
	vector1[1] = vertex2.y - vertex1.y;
	vector1[2] = vertex2.z - vertex1.z;

	vector2[0] = vertex3.x - vertex1.x;
	vector2[1] = vertex3.y - vertex1.y;
	vector2[2] = vertex3.z - vertex1.z;

	// tu 및 tv 텍스처 공간 벡터를 계산합니다.
	tuVector[0] = vertex2.tu - vertex1.tu;
	tvVector[0] = vertex2.tv - vertex1.tv;

	tuVector[1] = vertex3.tu - vertex1.tu;
	tvVector[1] = vertex3.tv - vertex1.tv;

	// 탄젠트 / 바이 노멀 방정식의 분모를 계산합니다.
	float den = 1.0f / (tuVector[0] * tvVector[1] - tuVector[1] * tvVector[0]);

	// 교차 곱을 계산하고 계수로 곱하여 접선과 비 구식을 얻습니다.
	tangent.x = (tvVector[1] * vector1[0] - tvVector[0] * vector2[0]) * den;
	tangent.y = (tvVector[1] * vector1[1] - tvVector[0] * vector2[1]) * den;
	tangent.z = (tvVector[1] * vector1[2] - tvVector[0] * vector2[2]) * den;

	binormal.x = (tuVector[0] * vector2[0] - tuVector[1] * vector1[0]) * den;
	binormal.y = (tuVector[0] * vector2[1] - tuVector[1] * vector1[1]) * den;
	binormal.z = (tuVector[0] * vector2[2] - tuVector[1] * vector1[2]) * den;

	// 이 법선의 길이를 계산합니다.
	float length = sqrt((tangent.x * tangent.x) + (tangent.y * tangent.y) + (tangent.z * tangent.z));

	// 법선을 표준화 한 다음 저장합니다.
	tangent.x = tangent.x / length;
	tangent.y = tangent.y / length;
	tangent.z = tangent.z / length;

	// 이 법선의 길이를 계산합니다.
	length = sqrt((binormal.x * binormal.x) + (binormal.y * binormal.y) + (binormal.z * binormal.z));

	// 법선을 표준화 한 다음 저장합니다.
	binormal.x = binormal.x / length;
	binormal.y = binormal.y / length;
	binormal.z = binormal.z / length;
}


void ModelClass::CalculateNormal(VectorType tangent, VectorType binormal, VectorType& normal)
{
	// 법선 벡터를 줄 수있는 접선과 binormal의 외적을 계산합니다.
	normal.x = (tangent.y * binormal.z) - (tangent.z * binormal.y);
	normal.y = (tangent.z * binormal.x) - (tangent.x * binormal.z);
	normal.z = (tangent.x * binormal.y) - (tangent.y * binormal.x);

	// 법선의 길이를 계산합니다.
	float length = sqrt((normal.x * normal.x) + (normal.y * normal.y) + (normal.z * normal.z));

	// 법선을 표준화합니다.
	normal.x = normal.x / length;
	normal.y = normal.y / length;
	normal.z = normal.z / length;
}

ID3D11ShaderResourceView* ModelClass::GetTexture1()
{
	return m_Texture1->GetTexture();
}


ID3D11ShaderResourceView* ModelClass::GetTexture2()
{
	return m_Texture2->GetTexture();
}


ID3D11ShaderResourceView* ModelClass::GetTexture3()
{
	return m_Texture3->GetTexture();
}
