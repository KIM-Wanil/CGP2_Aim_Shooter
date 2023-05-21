
#include "FontClass.h"
#include "FontShaderClass.h"
#include "TextClass.h"


TextClass::TextClass()
{
}

TextClass::TextClass(const TextClass& other)
{
}

TextClass::~TextClass()
{
}

bool TextClass::Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext, HWND hwnd, int screenWidth, int screenHeight, XMMATRIX baseViewMatrix)
{
	/*SetSize(screenWidth, screenHeight, deviceContext);*/
	// 화면 너비와 높이를 저장합니다.
	m_screenWidth = screenWidth;
	m_screenHeight = screenHeight;
	
	// 기본 뷰 매트릭스를 저장합니다.
	m_baseViewMatrix = baseViewMatrix;

	// 폰트 객체를 생성합니다.
	m_Font = new FontClass;
	if (!m_Font)
	{
		return false;
	}

	// 폰트 객체를 초기화 합니다.
	bool result = m_Font->Initialize(device, L"./data/fontdata.txt",  L"./data/font.dds");
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the font object.", L"Error", MB_OK);
		return false;
	}

	// 폰트 쉐이더 객체를 생성합니다.
	m_FontShader = new FontShaderClass;
	if (!m_FontShader)
	{
		return false;
	}

	// 폰트 쉐이더 객체를 초기화 합니다.
	result = m_FontShader->Initialize(device, hwnd);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the font shader object.", L"Error", MB_OK);
		return false;
	}

	// 첫 번째 문장을 초기화합니다.
	result = InitializeSentence(&m_sentence1, 16, device);
	if (!result)
	{
		return false;
	}

	// 문장 정점 버퍼를 새 문자열 정보로 업데이트합니다.
	result = UpdateSentence(m_sentence1, "Fps: ", 20, 20, 0.0f, 1.0f, 0.0f, deviceContext);
	if (!result)
	{
		return false;
	}

	// 첫 번째 문장을 초기화합니다.
	result = InitializeSentence(&m_sentence2, 16, device);
	if (!result)
	{
		return false;
	}

	// 문장 정점 버퍼를 새 문자열 정보로 업데이트합니다.
	result = UpdateSentence(m_sentence2, "Cpu: ", 20, 40, 0.0f, 1.0f, 0.0f, deviceContext);
	if (!result)
	{
		return false;
	}

	// 첫 번째 문장을 초기화합니다.
	result = InitializeSentence(&m_sentence3, 16, device);
	if (!result)
	{
		return false;
	}

	// 문장 정점 버퍼를 새 문자열 정보로 업데이트합니다.
	result = UpdateSentence(m_sentence3, "Time: ", 760, 800, 0.0f, 1.0f, 0.0f, deviceContext);
	if (!result)
	{
		return false;
	}
	// 첫 번째 문장을 초기화합니다.
	result = InitializeSentence(&m_sentence4, 32, device);
	if (!result)
	{
		return false;
	}

	// 문장 정점 버퍼를 새 문자열 정보로 업데이트합니다.
	result = UpdateSentence(m_sentence4, "Size: ", 20, 60, 0.0f, 1.0f, 0.0f, deviceContext);
	if (!result)
	{
		return false;
	}

	result = InitializeSentence(&m_sentence5, 32, device);
	if (!result)
	{
		return false;
	}
	// 문장 정점 버퍼를 새 문자열 정보로 업데이트합니다.
	result = UpdateSentence(m_sentence5, "Score: ", 20, 80, 0.0f, 1.0f, 0.0f, deviceContext);
	if (!result)
	{
		return false;
	}

	result = InitializeSentence(&m_sentence6, 32, device);
	if (!result)
	{
		return false;
	}

	result = InitializeSentence(&m_sentence7, 128, device);
	if (!result)
	{
		return false;
	}

	result = UpdateSentence(m_sentence7, "numOfObject: ", 20, 100, 0.0f, 1.0f, 0.0f, deviceContext);
	if (!result)
	{
		return false;
	}

	result = InitializeSentence(&m_sentence8, 128, device);
	if (!result)
	{
		return false;
	}

	// //문장 정점 버퍼를 새 문자열 정보로 업데이트합니다.
	result = UpdateSentence(m_sentence8, "numOfPolygon: ", 20, 120, 0.0f, 1.0f, 0.0f, deviceContext);
	if (!result)
	{
		return false;
	}

	return true;
}


void TextClass::Shutdown()
{
	// 첫번째 문장을 반환합니다.
	ReleaseSentence(&m_sentence1);

	// 두번째 문장을 반환합니다.
	ReleaseSentence(&m_sentence2);

	// 두번째 문장을 반환합니다.
	ReleaseSentence(&m_sentence3);
	// 두번째 문장을 반환합니다.
	ReleaseSentence(&m_sentence4);
	// 두번째 문장을 반환합니다.
	ReleaseSentence(&m_sentence5);
	ReleaseSentence(&m_sentence6);
	ReleaseSentence(&m_sentence7);
	ReleaseSentence(&m_sentence8);

	// 폰트 쉐이더 객체를 반환합니다.
	if (m_FontShader)
	{
		m_FontShader->Shutdown();
		delete m_FontShader;
		m_FontShader = 0;
	}

	// 폰트 객체를 반환합니다.
	if (m_Font)
	{
		m_Font->Shutdown();
		delete m_Font;
		m_Font = 0;
	}
}


bool TextClass::Render(ID3D11DeviceContext* deviceContext, XMMATRIX worldMatrix, XMMATRIX orthoMatrix)
{
	// 첫번째 문장을 그립니다.
	if (!RenderSentence(deviceContext, m_sentence1, worldMatrix, orthoMatrix))
	{
		return false;
	}

	if (!RenderSentence(deviceContext, m_sentence2, worldMatrix, orthoMatrix))
	{
		return false;
	}
	if (!RenderSentence(deviceContext, m_sentence3, worldMatrix, orthoMatrix))
	{
		return false;
	}
	if (!RenderSentence(deviceContext, m_sentence4, worldMatrix, orthoMatrix))
	{
		return false;
	}
	if (!RenderSentence(deviceContext, m_sentence5, worldMatrix, orthoMatrix))
	{
		return false;
	}
	if (!RenderSentence(deviceContext, m_sentence7, worldMatrix, orthoMatrix))
	{
		return false;
	}
	if (!RenderSentence(deviceContext, m_sentence8, worldMatrix, orthoMatrix))
	{
		return false;
	}
	return true;
}
bool TextClass::RenderScore(ID3D11DeviceContext* deviceContext, XMMATRIX worldMatrix, XMMATRIX orthoMatrix)
{
	char tempString[16] = { 0, };
	//_itoa_s(time, tempString, 10);
	_itoa_s(m_score, tempString, 10);
	// cpu 문자열을 설정합니다.
	char scoreString[16] = { };
	strcat_s(scoreString, tempString);
	UpdateSentence(m_sentence6, scoreString, 830, 570, 1.0f, 1.0f, 1.0f, deviceContext);
	if (!RenderSentence(deviceContext, m_sentence6, worldMatrix, orthoMatrix))
	{
		return false;
	}
	return true;
}

bool TextClass::InitializeSentence(SentenceType** sentence, int maxLength, ID3D11Device* device)
{
	// 새로운 문장 개체를 만듭니다.
	*sentence = new SentenceType;
	if (!*sentence)
	{
		return false;
	}

	// 문장 버퍼를 null로 초기화합니다.
	(*sentence)->vertexBuffer = 0;
	(*sentence)->indexBuffer = 0;

	// 문장의 최대 길이를 설정합니다.
	(*sentence)->maxLength = maxLength;

	// 정점 배열의 정점 수를 설정합니다.
	(*sentence)->vertexCount = 6 * maxLength;

	// 인덱스 배열의 인덱스 수를 설정합니다.
	(*sentence)->indexCount = (*sentence)->vertexCount;

	// 정점 배열을 만듭니다.
	VertexType* vertices = new VertexType[(*sentence)->vertexCount];
	if (!vertices)
	{
		return false;
	}

	// 인덱스 배열을 만듭니다.
	unsigned long* indices = new unsigned long[(*sentence)->indexCount];
	if (!indices)
	{
		return false;
	}

	// 처음에는 정점 배열을 0으로 초기화합니다.
	memset(vertices, 0, (sizeof(VertexType) * (*sentence)->vertexCount));

	// 인덱스 배열을 초기화합니다.
	for (int i = 0; i<(*sentence)->indexCount; i++)
	{
		indices[i] = i;
	}

	// 동적 인 정점 버퍼의 설명을 설정한다.
	D3D11_BUFFER_DESC vertexBufferDesc;
	vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	vertexBufferDesc.ByteWidth = sizeof(VertexType) * (*sentence)->vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	// subresource 구조에 정점 데이터에 대한 포인터를 제공합니다.
	D3D11_SUBRESOURCE_DATA vertexData;
	vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	// 정점 버퍼를 만든다.
	if (FAILED(device->CreateBuffer(&vertexBufferDesc, &vertexData, &(*sentence)->vertexBuffer)))
	{
		return false;
	}

	// 정적 인덱스 버퍼의 설명을 설정합니다.
	D3D11_BUFFER_DESC indexBufferDesc;
	
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * (*sentence)->indexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	// 하위 리소스 구조에 인덱스 데이터에 대한 포인터를 제공합니다.
	D3D11_SUBRESOURCE_DATA indexData;
	indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	// 인덱스 버퍼를 만듭니다.
	if (FAILED(device->CreateBuffer(&indexBufferDesc, &indexData, &(*sentence)->indexBuffer)))
	{
		return false;
	}

	// 더 이상 필요하지 않은 정점 배열을 해제합니다.
	delete[] vertices;
	vertices = 0;

	// 더 이상 필요하지 않은 인덱스 배열을 해제합니다.
	delete[] indices;
	indices = 0;

	return true;
}


bool TextClass::UpdateSentence(SentenceType* sentence, const char* text, int positionX, int positionY, float red, float green, float blue, ID3D11DeviceContext* deviceContext)
{
	// 문장의 색을 저장한다.
	sentence->red = red;
	sentence->green = green;
	sentence->blue = blue;

	// 가능한 버퍼 오버 플로우를 확인합니다.
	if ((int)strlen(text) > sentence->maxLength)
	{
		return false;
	}

	// 정점 배열을 만듭니다.
	VertexType* vertices = new VertexType[sentence->vertexCount];
	if (!vertices)
	{
		return false;
	}

	// 처음에는 정점 배열을 0으로 초기화합니다.
	memset(vertices, 0, (sizeof(VertexType) * sentence->vertexCount));

	// 그리기를 시작할 화면에서 X 및 Y 픽셀 위치를 계산합니다.
	float drawX = (float)(((m_screenWidth / 2) * -1) + positionX);
	float drawY = (float)((m_screenHeight / 2) - positionY);

	// 폰트 클래스를 사용하여 문장 텍스트와 문장 그리기 위치에서 정점 배열을 만듭니다.
	m_Font->BuildVertexArray((void*)vertices, text, drawX + 50.0f, drawY);

	// 버텍스 버퍼를 쓸 수 있도록 잠급니다.
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	if (FAILED(deviceContext->Map(sentence->vertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource)))
	{
		return false;
	}

	// 정점 버퍼의 데이터를 가리키는 포인터를 얻는다.
	VertexType* verticesPtr = (VertexType*)mappedResource.pData;

	// 데이터를 정점 버퍼에 복사합니다.
	memcpy(verticesPtr, (void*)vertices, (sizeof(VertexType) * sentence->vertexCount));

	// 정점 버퍼의 잠금을 해제합니다.
	deviceContext->Unmap(sentence->vertexBuffer, 0);

	// 더 이상 필요하지 않은 정점 배열을 해제합니다.
	delete[] vertices;
	vertices = 0;

	return true;
}


void TextClass::ReleaseSentence(SentenceType** sentence)
{
	if (*sentence)
	{
		// 문장 버텍스 버퍼를 해제합니다.
		if ((*sentence)->vertexBuffer)
		{
			(*sentence)->vertexBuffer->Release();
			(*sentence)->vertexBuffer = 0;
		}

		// 문장 인덱스 버퍼를 해제합니다.
		if ((*sentence)->indexBuffer)
		{
			(*sentence)->indexBuffer->Release();
			(*sentence)->indexBuffer = 0;
		}

		// 문장을 해제합니다.
		delete *sentence;
		*sentence = 0;
	}
}


bool TextClass::RenderSentence(ID3D11DeviceContext* deviceContext, SentenceType* sentence, XMMATRIX worldMatrix, XMMATRIX orthoMatrix)
{
	// 정점 버퍼 간격 및 오프셋을 설정합니다.
	unsigned int stride = sizeof(VertexType);
	unsigned int offset = 0;

	// 렌더링 할 수 있도록 입력 어셈블러에서 정점 버퍼를 활성으로 설정합니다.
	deviceContext->IASetVertexBuffers(0, 1, &sentence->vertexBuffer, &stride, &offset);

	// 렌더링 할 수 있도록 입력 어셈블러에서 인덱스 버퍼를 활성으로 설정합니다.
	deviceContext->IASetIndexBuffer(sentence->indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	// 이 정점 버퍼에서 렌더링 되어야 하는 프리미티브 유형을 설정합니다.이 경우에는 삼각형입니다.
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// 입력 된 문장 색상으로 픽셀 색상 벡터를 만듭니다.
	XMFLOAT4 pixelColor = XMFLOAT4(sentence->red, sentence->green, sentence->blue, 1.0f);

	// 폰트 셰이더를 사용하여 텍스트를 렌더링합니다.
	if (!m_FontShader->Render(deviceContext, sentence->indexCount, worldMatrix, m_baseViewMatrix, orthoMatrix, m_Font->GetTexture(), pixelColor))
	{
		false;
	}

	return true;
}

bool TextClass::SetFps(int fps, ID3D11DeviceContext* deviceContext)
{
	// fps를 10,000 이하로 자릅니다.
	if(fps > 9999)
	{
		fps = 9999;
	}

	// fps 정수를 문자열 형식으로 변환합니다.
	char tempString[16] = { 0, };
	_itoa_s(fps, tempString, 10);

	// fps 문자열을 설정합니다.
	char fpsString[16] = { 0, };
	strcpy_s(fpsString, "Fps: ");
	strcat_s(fpsString, tempString);

	float red = 0;
	float green = 0;
	float blue = 0;

	// fps가 60 이상이면 fps 색상을 녹색으로 설정합니다.
	if(fps >= 60)
	{
		red = 0.0f;
		green = 1.0f;
		blue = 0.0f;
	}

	// fps가 60보다 작은 경우 fps 색상을 노란색으로 설정합니다.
	if(fps < 60)
	{
		red = 1.0f;
		green = 1.0f;
		blue = 0.0f;
	}

	// fps가 30 미만이면 fps 색상을 빨간색으로 설정합니다.
	if(fps < 30)
	{
		red = 1.0f;
		green = 0.0f;
		blue = 0.0f;
	}

	// 문장 정점 버퍼를 새 문자열 정보로 업데이트합니다.
	return UpdateSentence(m_sentence1, fpsString, 20, 20, red, green, blue, deviceContext);
}


bool TextClass::SetCpu(int cpu, ID3D11DeviceContext* deviceContext)
{
	// cpu 정수를 문자열 형식으로 변환합니다.
	char tempString[16] = { 0, };
	_itoa_s(cpu, tempString, 10);

	// cpu 문자열을 설정합니다.
	char cpuString[16] = { 0, };
	strcpy_s(cpuString, "Cpu: ");
	strcat_s(cpuString, tempString);
	strcat_s(cpuString, "%");

	// 문장 정점 버퍼를 새 문자열 정보로 업데이트합니다.
	return UpdateSentence(m_sentence2, cpuString, 20, 40, 0.0f, 1.0f, 0.0f, deviceContext);
}

bool TextClass::SetHit(int numOfBullet, ID3D11DeviceContext* deviceContext)
{
	// cpu 정수를 문자열 형식으로 변환합니다.
	//string tempString = to_string(time);	
	char tempString[16];
	//strcpy_s(tempString2, tempString.c_str());
	_itoa_s(numOfBullet, tempString, 10);

	// cpu 문자열을 설정합니다.
	char string[16] = { 0, };

	float red = 0;
	float green = 0;
	float blue = 0;

	if (numOfBullet <= 0)
	{
		strcpy_s(string, "RELOAD ..");
		red = 1.0f;
		green = 0.0f;
		blue = 0.0f;
	}
	else
	{
		strcpy_s(string, "Bullet:");
		strcat_s(string, tempString);
		strcat_s(string, "/10");
		red = 0.0f;
		green = 1.0f;
		blue = 0.0f;
	}
	//strcat_s(timeString, "sec");

	// 문장 정점 버퍼를 새 문자열 정보로 업데이트합니다.
	return UpdateSentence(m_sentence3, string, 760, 800, red, green, blue, deviceContext);
}
bool TextClass::SetSize(int sizeX,int sizeY, ID3D11DeviceContext* deviceContext)
{
	// cpu 정수를 문자열 형식으로 변환합니다.
	char tempString[16] = { 0, };
	char tempString2[16] = { 0, };
	_itoa_s(sizeX, tempString, 10);
	_itoa_s(sizeY, tempString2, 10);

	// cpu 문자열을 설정합니다.
	char sizeString[32] = { 0, };
	strcpy_s(sizeString, "size: ");
	strcat_s(sizeString, tempString);
	strcat_s(sizeString, "X");
	strcat_s(sizeString, tempString2);

	// 문장 정점 버퍼를 새 문자열 정보로 업데이트합니다.
	return UpdateSentence(m_sentence4, sizeString, 20, 60, 0.0f, 1.0f, 0.0f, deviceContext);
}

bool TextClass::SetScore(int score, ID3D11DeviceContext* deviceContext)
{
	m_score = score;
	// cpu 정수를 문자열 형식으로 변환합니다.
	char tempString[16] = { 0, };
	//_itoa_s(time, tempString, 10);
	_itoa_s(score, tempString, 10);
	// cpu 문자열을 설정합니다.
	char scoreString[16] = { 0, };
	strcpy_s(scoreString, "score: ");
	strcat_s(scoreString, tempString);


	// 문장 정점 버퍼를 새 문자열 정보로 업데이트합니다.
	return UpdateSentence(m_sentence5, scoreString, 1450, 40, 1.0f, 1.0f, 0.0f, deviceContext);
}

bool TextClass::SetCount(int numOfObject, int numOfPolygon, ID3D11DeviceContext* deviceContext)
{
	// cpu 정수를 문자열 형식으로 변환합니다.
	char tempString[32] = { 0, };
	//_itoa_s(time, tempString, 10);
	_itoa_s(numOfObject, tempString, 10);
	// cpu 문자열을 설정합니다.
	char objectString[32] = { 0, };
	strcpy_s(objectString, "numOfO: ");
	strcat_s(objectString, tempString);

	bool result;
	// 문장 정점 버퍼를 새 문자열 정보로 업데이트합니다.
	result = UpdateSentence(m_sentence7, objectString, 20, 80, 0.0f, 1.0f, 0.0f, deviceContext);
	if(!result)
	{
		return false;
	}

	char tempString2[64] = { 0, };
	//_itoa_s(time, tempString, 10);
	_itoa_s(numOfPolygon, tempString2, 10);
	// cpu 문자열을 설정합니다.
	char polygonString[64] = { 0, };
	strcpy_s(polygonString, "numOfP: ");
	strcat_s(polygonString, tempString2);

	// 문장 정점 버퍼를 새 문자열 정보로 업데이트합니다.
	result = UpdateSentence(m_sentence8, polygonString, 20, 100, 0.0f, 1.0f, 0.0f, deviceContext);
	if (!result)
	{
		return false;
	}

	return true;
}