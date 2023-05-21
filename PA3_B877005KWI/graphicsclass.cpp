////////////////////////////////////////////////////////////////////////////////
// Filename: graphicsclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "graphicsclass.h"


GraphicsClass::GraphicsClass()
{
	m_D3D = 0;
	m_Camera = 0;
	modelCount = 5;
	for (int i = 0; i < modelCount; i++)
	{
		m_Model[i] = 0;
	}
	for (int i = 0; i < 3; i++)
	{
		m_Title[i] = 0;
	}

	m_GameOver = 0;
	m_LightShader = 0;
	m_Light = 0;

	m_TextureShader = 0;
	m_SpecMapShader = 0;
	for (int i = 0; i < 10; i++)
	{
		m_EnemySetMatrix[i] = { XMFLOAT3(0.0f, 0.0f, 0.0f),
							XMFLOAT3(0.0f, 0.0f, 0.0f),
							XMFLOAT3(0.0f, 0.0f, 0.0f) };
		m_EnemyWorldMatrix[i] = ConvertWorldMatrix(m_EnemySetMatrix[i]);
		isDie[i] = true;
		isHit[i] = true;

		m_FireSetMatrix[i] = { XMFLOAT3(0.0f, 0.0f, 0.0f),
							XMFLOAT3(0.0f, 0.0f, 0.0f),
							XMFLOAT3(0.0f, 0.0f, 0.0f) };
		m_FireWorldMatrix[i] = ConvertWorldMatrix(m_EnemySetMatrix[i]);
	}
	m_stage = EStage::Title;

}


GraphicsClass::GraphicsClass(const GraphicsClass& other)
{
}


GraphicsClass::~GraphicsClass()
{
}


bool GraphicsClass::Initialize(int screenWidth, int screenHeight, HWND hwnd)
{
	// 화면 너비와 높이를 저장합니다.
	m_screenWidth = screenWidth;
	m_screenHeight = screenHeight;
	m_stage = EStage::Title;
	bool result;
	// Create the Direct3D object.
	m_D3D = new D3DClass;
	if(!m_D3D)
	{
		return false;
	}

	// Initialize the Direct3D object.
	result = m_D3D->Initialize(screenWidth, screenHeight, VSYNC_ENABLED, hwnd, FULL_SCREEN, SCREEN_DEPTH, SCREEN_NEAR);
	if(!result)
	{
		MessageBox(hwnd, L"Could not initialize Direct3D.", L"Error", MB_OK);
		return false;
	}

	// Create the camera object.
	m_Camera = new CameraClass;
	if(!m_Camera)
	{
		return false;
	}

	// Set the initial position of the camera.
	
	m_Camera->SetPosition(0.0f, 12.0f,-45.0f);	
	m_Camera->Render();
	XMMATRIX ViewMatrix2D;
	m_Camera->GetViewMatrix(ViewMatrix2D);
	m_ViewMatrix2D = ViewMatrix2D;
	//m_Camera->SetPosition(0.0f, 0.0f, -20.0f);	

	// m_Text 객체 생성
	m_Text = new TextClass;
	if (!m_Text)
	{
		return false;
	}

	// m_Text 객체 초기화
	if (!m_Text->Initialize(m_D3D->GetDevice(), m_D3D->GetDeviceContext(), hwnd, screenWidth, screenHeight,
		ViewMatrix2D))
	{
		MessageBox(hwnd, L"Could not initialize the text object.", L"Error", MB_OK);
		return false;
	}


	// Create the model object.
	for (int i = 0; i < modelCount; i++) 
	{
		m_Model[i] = new ModelClass;
		if (!m_Model[i])
		{
			return false;
		}
	}
	for (int i = 0; i < 3; i++)
	{
		m_Title[i] = new BitmapClass;
		if (!m_Title[i])
		{
			return false;
		}
	}
	m_GameOver = new BitmapClass;


	// Initialize the model object.
	result = m_Model[0]->Initialize(m_D3D->GetDevice(), L"./data/robo.obj", L"./data/robo_diffuse.dds", 0);

	/*result = m_Model[0]->InitializeMulti(m_D3D->GetDevice(), L"./data/robot.obj", L"./data/robot_diffuse.dds",
		L"./data/robot_normal.dds", L"./data/robot_diffuse.dds", 0);*/
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the model object.", L"Error", MB_OK);
		return false;
	}
	CreateBoundingVolumes(m_Model[0]->m_vertPosArray, enemyBoundingBoxVertPosArray, enemyBoundingBoxVertIndexArray);
	for (int i = 0; i < 10; i++)
	{
		m_EnemySetMatrix[i] = { XMFLOAT3(0.0f, 0.0f, 0.0f),
							XMFLOAT3(0.0f, 0.0f, 0.0f),
							XMFLOAT3(0.0f, 0.0f, 0.0f) };
		m_EnemyWorldMatrix[i] = ConvertWorldMatrix(m_EnemySetMatrix[i]);
		isDie[i] = true;
		isHit[i] = true;
		m_FireSetMatrix[i] = { XMFLOAT3(0.0f, 0.0f, 0.0f),
							XMFLOAT3(0.0f, 0.0f, 0.0f),
							XMFLOAT3(0.0f, 0.0f, 0.0f) };
		m_FireWorldMatrix[i] = ConvertWorldMatrix(m_EnemySetMatrix[i]);
	}
	//땅
	/*result = m_Model[1]->Initialize(m_D3D->GetDevice(), L"./data/scifiFloor.obj", L"./data/floor_specular.dds", 1);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the model objectz.", L"Error", MB_OK);
		return false;
	}*/

	/*result = m_Model[1]->InitializeMulti(m_D3D->GetDevice(), L"./data/scifiFloor.obj", L"./data/floor_diffuse.dds",
		L"./data/floor_normal.dds", L"./data/floor_specular.dds", 1);*/
	result = m_Model[1]->InitializeMulti(m_D3D->GetDevice(), L"./data/scifiFloor.obj", L"./data/floor_diffuse.dds",
		L"./data/floor_normal.dds", L"./data/floor_specular.dds", 1);
	/*result = m_Model[1]->InitializeMulti(m_D3D->GetDevice(), L"./data/scifiFloor.obj", L"./data/floor_diffuse.dds",
		L"./data/floor_diffuse.dds", L"./data/floor_diffuse.dds", 1);*/
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the model1 object.", L"Error", MB_OK);
		return false;
	}

	//result = m_Model[2]->Initialize(m_D3D->GetDevice(), L"./data/rifle4.obj", L"./data/floor_diffuse.dds", 2);
	result = m_Model[2]->InitializeMulti(m_D3D->GetDevice(), L"./data/sub.obj", L"./data/sub_diffuse.dds",
		L"./data/sub_normal.dds", L"./data/sub_specular.dds", 2);
	if(!result)
	{
		MessageBox(hwnd, L"Could not initialize the model object.", L"Error", MB_OK);
		return false;
	}


	// Initialize the bitmap object.
	result = m_Model[3]->Initialize2D(m_D3D->GetDevice(), L"./data/aim.dds", 50, 50,3, ViewMatrix2D);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the bitmap object.", L"Error", MB_OK);
		return false;
	}
	
	// 모델 객체 초기화
	if (!m_Model[4]->InitializeEffect(m_D3D->GetDevice(), "./data/square.txt", L"./data/fire01.dds",
		L"./data/noise01.dds", L"./data/alpha01.dds"))
	{
		MessageBox(hwnd, L"Could not initialize the model4 object.", L"Error", MB_OK);
		return false;
	}

	// Initialize the bitmap object.
	result = m_Title[0]->Initialize(m_D3D->GetDevice(),screenWidth,screenHeight, L"./data/title.dds",1600,900);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the title object.", L"Error", MB_OK);
		return false;
	}
	result = m_Title[1]->Initialize(m_D3D->GetDevice(), screenWidth, screenHeight, L"./data/title2.dds", 1600, 900);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the title object.", L"Error", MB_OK);
		return false;
	}
	result = m_Title[2]->Initialize(m_D3D->GetDevice(), screenWidth, screenHeight, L"./data/title3.dds", 1600, 900);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the title object.", L"Error", MB_OK);
		return false;
	}
	result = m_GameOver->Initialize(m_D3D->GetDevice(), screenWidth, screenHeight, L"./data/game_over.dds", 1600, 900);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the gameOver object.", L"Error", MB_OK);
		return false;
	}
	
	m_SpecMapShader = new SpecMapShaderClass;
	if (!m_SpecMapShader)
	{
		return false;
	}

	// specular map shader 객체를 초기화한다.
	if (!m_SpecMapShader->Initialize(m_D3D->GetDevice(), hwnd))
	{
		MessageBox(hwnd, L"Could not initialize the specular map shader object.", L"Error", MB_OK);
		return false;
	}

	// Create the light shader object.
	m_LightShader = new LightShaderClass;
	if (!m_LightShader)
	{
		return false;
	}

	// Initialize the light shader object.
	result = m_LightShader->Initialize(m_D3D->GetDevice(), hwnd);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the light shader object.", L"Error", MB_OK);
		return false;
	}



	// Create the light object.
	m_Light = new LightClass;
	if (!m_Light)
	{
		return false;
	}

	// Initialize the light object.
	m_Light->SetAmbientColor(0.1f, 0.1f, 0.1f, 1.0f);
	m_Light->SetDiffuseColor(0.3f, 0.3f, 0.3f, 1.0f);
	m_Light->SetDirection(1.0f, -1.0f, 1.0f);
	m_Light->SetSpecularColor(0.5f, 0.5f, 0.5f, 1.0f);
	m_Light->SetSpecularPower(16.0f);


	m_Light1 = new LightClass;
	if (!m_Light1)
	{
		return false;
	}
	// Initialize the first light object.

	m_Light1->SetDiffuseColor(0.7f, 0.7f, 0.7f, 1.0f);
	m_Light1->SetPosition(100.0f, 20.0f, -50.0f);
	// Create the second light object.

	m_Light2 = new LightClass;
	if (!m_Light2)
	{
		return false;
	}
	// Initialize the second light object.

	m_Light2->SetDiffuseColor(0.7f, 0.7f, 0.7f, 1.0f);
	m_Light2->SetPosition(-100.0f, 20.0f, -50.0f);


	// Create the third light object.
	m_Light3 = new LightClass;
	if (!m_Light3)
	{
		return false;
	}

	// Initialize the third light object.
	m_Light3->SetDiffuseColor(0.7f, 0.7f, 0.7f, 1.0f);
	m_Light3->SetPosition(-200.0f, 20.0f, -50.0f);

	// Create the fourth light object.
	m_Light4 = new LightClass;
	if (!m_Light4)
	{
		return false;
	}
	// Initialize the fourth light object.
	m_Light4->SetDiffuseColor(0.7f, 0.7f, 0.7f, 1.0f);
	m_Light4->SetPosition(200.0f, 20.0f, -50.0f);	

	m_TextureShader = new TextureShaderClass;
	if (!m_TextureShader)
	{
		return false;
	}
	// Initialize the texture shader object.
	result = m_TextureShader->Initialize(m_D3D->GetDevice(), hwnd);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the texture shader object.", L"Error", MB_OK);
		return false;
	}

	// 스카이 돔 객체를 생성합니다.
	m_SkyDome = new SkyDomeClass;
	if (!m_SkyDome)
	{
		return false;
	}

	// 스카이 돔 객체를 초기화 합니다.
	result = m_SkyDome->Initialize(m_D3D->GetDevice());
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the sky dome object.", L"Error", MB_OK);
		return false;
	}

	// 스카이 돔 쉐이더 객체를 생성합니다.
	m_SkyDomeShader = new SkyDomeShaderClass;
	if (!m_SkyDomeShader)
	{
		return false;
	}

	// 스카이 돔 쉐이더 객체를 초기화 합니다.
	result = m_SkyDomeShader->Initialize(m_D3D->GetDevice(), hwnd);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the sky dome shader object.", L"Error", MB_OK);
		return false;
	}

	// 파이어 쉐이더 객체를 생성한다.
	m_FireShader = new FireShaderClass;
	if (!m_FireShader)
	{
		return false;
	}
	result = m_FireShader->Initialize(m_D3D->GetDevice(), hwnd);
	// 화재 쉐이더 객체를 초기화합니다.
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the fire shader object.", L"Error", MB_OK);
		return false;
	}

	return true;
}


void GraphicsClass::Shutdown()
{
	// fire shader 객체를 해제한다.
	if (m_FireShader)
	{
		m_FireShader->Shutdown();
		delete m_FireShader;
		m_FireShader = 0;
	}
	// 스카이 돔 쉐이더 객체를 해제합니다.
	if (m_SkyDomeShader)
	{
		m_SkyDomeShader->Shutdown();
		delete m_SkyDomeShader;
		m_SkyDomeShader = 0;
	}

	// 스카이 돔 객체를 해제합니다.
	if (m_SkyDome)
	{
		m_SkyDome->Shutdown();
		delete m_SkyDome;
		m_SkyDome = 0;
	}

	if (m_Text)
	{
		m_Text->Shutdown();
		delete m_Text;
		m_Text = 0;
	}
	// Release the model object.
	if(m_Model[0])
	{
		m_Model[0]->Shutdown();
		delete m_Model[0];
		m_Model[0] = 0;
	}
	if (m_Model[1])
	{
		m_Model[1]->ShutdownMulti();
		delete m_Model[1];
		m_Model[1] = 0;
	}
	if (m_Model[2])
	{
		m_Model[2]->Shutdown();
		delete m_Model[2];
		m_Model[2] = 0;
	}
	if (m_Model[3])
	{
		m_Model[3]->Shutdown();
		delete m_Model[3];
		m_Model[3] = 0;
	}
	if (m_Model[4])
	{
		m_Model[4]->ShutdownEffect();
		delete m_Model[4];
		m_Model[4] = 0;
	}
	for (int i = 0; i < 3; i++)
	{
		if (m_Title[i])
		{
			m_Title[i]->Shutdown();
			delete m_Title[i];
			m_Title[i] = 0;
		}
	}

	// Release the camera object.
	if(m_Camera)
	{
		delete m_Camera;
		m_Camera = 0;
	}

	// Release the D3D object.
	if(m_D3D)
	{
		m_D3D->Shutdown();
		delete m_D3D;
		m_D3D = 0;
	}

	// Release the light object.
	if (m_Light)
	{
		delete m_Light;
		m_Light = 0;
	}
	if (m_Light1)
	{
		delete m_Light1;
		m_Light1 = 0;
	}

	if (m_Light2)
	{
		delete m_Light2;
		m_Light2 = 0;
	}

	if (m_Light3)
	{
		delete m_Light3;
		m_Light3 = 0;
	}

	if (m_Light4)
	{
		delete m_Light4;
		m_Light4 = 0;
	}
	// Release the light shader object.
	if (m_LightShader)
	{
		m_LightShader->Shutdown();
		delete m_LightShader;
		m_LightShader = 0;
	}

	if (m_TextureShader)
	{
		m_TextureShader->Shutdown();
		delete m_TextureShader;
		m_TextureShader = 0;
	}

	if (m_SpecMapShader)
	{
		m_SpecMapShader->Shutdown();
		delete m_SpecMapShader;
		m_SpecMapShader = 0;
	}
	
	return;
}
void GraphicsClass::DieCoolTime(int i)
{

	if (!isHit[i]) return;
		
	if (dieTime[i] > 15.0f)
	{
		isDie[i] = true;
		//enemyCount--;
		isHit[i] = false;
		dieTime[i] = 0.0f;
	}
	else
		dieTime[i] += 0.01f * m_frameTime;

}
void GraphicsClass::SpawnCoolTime()
{
	int balance = score * 0.03f;
	if (spawnTime > 2.0f - balance )
	{
		canSpawn = true;
		spawnTime = 0.0f;
	}
	else
		spawnTime += 0.001f * m_frameTime;
}
void GraphicsClass::ShootCoolTime()
{
	if (shootTime > 0.3f)
	{
		if (numOfBullet > 0)
		{
			canShoot = true;
		}
		shootTime = 0.0f;
	}
	else
		shootTime += 0.001f * m_frameTime;
}
void GraphicsClass::ReloadCoolTime()
{
	if (reloadTime > 2.0f)
	{
		canShoot = true;
		numOfBullet = 10;
		reloadTime = 0.0f;
	}
	else
		reloadTime += 0.001f * m_frameTime;
}
void GraphicsClass::SpawnEnemy()
{
	for (int i = 0; i < 10; ++i)
	{
		if (!isDie[i]) continue;

		//enemyCount++;
		isDie[i] = false;
		isHit[i] = false;
		std::random_device random;
		std::mt19937 gen(random());
		std::uniform_int_distribution<int> range(-150, 150);
		int xPos = range(gen);
		m_EnemySetMatrix[i] = {XMFLOAT3(40.0f, 40.0f, 40.0f),
							   XMFLOAT3(0.0f, 0.0f, 0.0f),
							   XMFLOAT3((float)xPos, 0.0f, 250.0f)};
		canSpawn = false;
		break;
	}
}
bool GraphicsClass::Frame(int fps, int cpu, float frameTime, float test)
{
	switch (m_stage)
	{

	case EStage::Main:
	{
		static float time = 60.0f;
		m_frameTime = frameTime;
		calculateCount();
		// 초당 프레임 수를 설정합니다.
		if (!m_Text->SetFps(fps, m_D3D->GetDeviceContext()))
		{
			return false;
		}
		// cpu 사용을 설정합니다.
		if (!m_Text->SetCpu(cpu, m_D3D->GetDeviceContext()))
		{
			return false;
		}
		if (!m_Text->SetHit(numOfBullet, m_D3D->GetDeviceContext()))
		{
			return false;
		}
		if (!m_Text->SetSize(m_screenWidth, m_screenHeight, m_D3D->GetDeviceContext()))
		{
			return false;
		}
		if (!m_Text->SetScore(score, m_D3D->GetDeviceContext()))
		{
			return false;
		}
		if (!m_Text->SetCount(numOfObject,numOfPolygon, m_D3D->GetDeviceContext()))
		{
			return false;
		}
		if (!canSpawn)
			SpawnCoolTime();
		else
			SpawnEnemy();

		if (!canShoot)
			ShootCoolTime();
		if (numOfBullet <= 0)
			ReloadCoolTime();
		for (int i = 0; i < 10; i++)
		{
			if (isHit[i])
				DieCoolTime(i);
		}
		break;
	}
	default:
		break;
	}
	bool result;

	static float rotation = 0.0f;
	static float trans1 = 0.0f;
	static int dir = 1;
	static float rot1 = -XM_PI * 0.5f;
	// Update the rotation variable each frame.
	rotation += XM_PI * 0.008f;
	if (rotation > 360.0f)
	{
		rotation -= 360.0f;
	}
	trans1 = 0.00002f * frameTime;
	// Render the graphics scene.
	result = Render(rotation,trans1,rot1);
	if(!result)
	{
		return false;
	}

	return true;
}

bool GraphicsClass::Render(float rotation, float trans1, float rot1)
{

	XMMATRIX worldMatrix, viewMatrix, projectionMatrix, orthoMatrix;
	bool result;
	
	// Clear the buffers to begin the scene.
	m_D3D->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);


	switch (m_stage)
	{

	case EStage::Title:
	{

		
		m_D3D->GetWorldMatrix(worldMatrix);
		m_D3D->GetOrthoMatrix(orthoMatrix);
		m_D3D->TurnOnAlphaBlending();
		m_D3D->TurnZBufferOff();

		result = m_Title[titleNum]->Render(m_D3D->GetDeviceContext(), 0, 0);
		if (!result) return false;

		result = m_TextureShader->Render(m_D3D->GetDeviceContext(), m_Title[titleNum]->GetIndexCount(), worldMatrix, m_ViewMatrix2D, orthoMatrix, m_Title[titleNum]->GetTexture(), 1.0f);
		if (!result) { return false; }

		m_D3D->TurnOffAlphaBlending();
		m_D3D->TurnZBufferOn();

		break;
	}
	case EStage::Over:
	{

		
		m_D3D->GetWorldMatrix(worldMatrix);
		m_D3D->GetOrthoMatrix(orthoMatrix);
		m_D3D->TurnOnAlphaBlending();
		m_D3D->TurnZBufferOff();
		result = m_GameOver->Render(m_D3D->GetDeviceContext(), 0, 0);
		if (!result) return false;

		result = m_TextureShader->Render(m_D3D->GetDeviceContext(), m_GameOver->GetIndexCount(), worldMatrix, m_ViewMatrix2D, orthoMatrix, m_GameOver->GetTexture(), 1.0f);
		if (!result) { return false; }


		result = m_Text->RenderScore(m_D3D->GetDeviceContext(), worldMatrix, orthoMatrix);
		if (!result)
		{
			return false;
		}
		m_D3D->TurnOffAlphaBlending();
		m_D3D->TurnZBufferOn();

		


		break;
	}
	case EStage::Main:
	{
		///////////////////////////////////////////////////////////////////////////////
		static float time = 0.0f;
		// 프레임 시간 카운터를 증가시킵니다.
		time += 0.01f;
		if (time > 1000.0f)
		{
			time = 0.0f;
		}
		float blendAmount;
		blendAmount = 0.5f;
		// 세 가지 다른 노이즈 텍스처에 대해 세 가지 스크롤 속도를 설정합니다.
		XMFLOAT3 scrollSpeeds = XMFLOAT3(1.3f, 2.1f, 2.3f);

		// 세 개의 서로 다른 노이즈 옥타브 텍스처를 만드는 데 사용할 세 개의 스케일을 설정합니다.
		XMFLOAT3 scales = XMFLOAT3(1.0f, 2.0f, 3.0f);

		// 세 가지 다른 노이즈 텍스처에 대해 세 가지 다른 x 및 y 왜곡 인수를 설정합니다.
		XMFLOAT2 distortion1 = XMFLOAT2(0.1f, 0.2f);
		XMFLOAT2 distortion2 = XMFLOAT2(0.1f, 0.3f);
		XMFLOAT2 distortion3 = XMFLOAT2(0.1f, 0.1f);

		// 텍스처 좌표 샘플링 섭동의 스케일과 바이어스.
		float distortionScale = 0.8f;
		float distortionBias = 0.5f;


		/////////////////////////////////////////////////////////////////////////



		// Generate the view matrix based on the camera's position.
		//m_Camera->SetPosition(0.0f, 10.0f, -45.0f);
		m_Camera->Render();
		// Get the world, view, and projection matrices from the camera and d3d objects.
		m_Camera->GetViewMatrix(viewMatrix);
		m_D3D->GetWorldMatrix(worldMatrix);
		m_D3D->GetProjectionMatrix(projectionMatrix);

		m_D3D->GetOrthoMatrix(orthoMatrix);//평행투영 (2D는 원근투영x)
		// Rotate the world matrix by the rotation value so that the triangle will spin.
		//worldMatrix = XMMatrixRotationY(rotation);
		m_SetMatrix[0] = { XMFLOAT3(10.0f, 15.0f, 10.0f),
						   XMFLOAT3(0.0f, 0.0f, 0.0f),
						   XMFLOAT3(0.0f, 8.0f, 0.0f) };

		m_SetMatrix[1] = { XMFLOAT3(1.0f, 1.0f, 1.0f),
						  XMFLOAT3(0.0f, 0.0f, 0.0f),
						   XMFLOAT3(0.0f, 0.0f, -5.0f) };

		m_SetMatrix[2] = { XMFLOAT3(20.0f, 20.0f, 20.0f),
							XMFLOAT3(0.0f, 0.0f , 0.0f),
			//XMFLOAT3(m_Camera->GetCamPitch(),m_Camera->GetCamYaw() /* - XM_PI / 2 */ , 0.0f),
			 m_Camera->GetPosition() };
		int balance = score * 0.025f;
		for (int i = 0; i < 10; i++)
		{
			if (!isHit[i])
			{
				m_EnemySetMatrix[i].trans.z -= (0.07 * m_frameTime) + balance;
				if (m_EnemySetMatrix[i].trans.z <= -40.0f && !isDie[i])
				{
					Reset();
					m_stage = EStage::Over;
					//score++;
				}
			}
		}


		m_SetMatrix[2].trans.x += XMVectorGetX(m_Camera->GetCamForward()) * 40.0f + XMVectorGetX(m_Camera->GetCamUp()) * (-90.0f) + XMVectorGetX(m_Camera->GetCamRight()) * (5.0f);
		m_SetMatrix[2].trans.y += XMVectorGetY(m_Camera->GetCamForward()) * 40.0f + XMVectorGetY(m_Camera->GetCamUp()) * (-90.0f) + XMVectorGetY(m_Camera->GetCamRight()) * (5.0f);
		m_SetMatrix[2].trans.z += XMVectorGetZ(m_Camera->GetCamForward()) * 40.0f + XMVectorGetZ(m_Camera->GetCamUp()) * (-90.0f) + XMVectorGetZ(m_Camera->GetCamRight()) * (5.0f);
		double angle = atan2(m_SetMatrix[2].trans.x - m_Camera->GetPosition().x, m_SetMatrix[2].trans.z - m_Camera->GetPosition().z) * (180.0 / XM_PI);
		float rotation2 = (float)angle * 0.0174532925f;

		m_SetMatrix[2].rotate.y = rotation2 + XM_PI / 2.3;
		//m_SetMatrix[2].rotate.z += (m_Camera->GetCamPitch()) - (XM_PI / 18);
		m_SetMatrix[2].rotate.z = (m_Camera->GetCamPitch()) - (XM_PI / 36);
		//if(m_Camera->GetCamPitch()>0)
		//	m_SetMatrix[2].rotate.z = (m_Camera->GetCamPitch())  - (XM_PI / 36);
		//else //if (m_Camera->GetCamPitch() > 0)
		//	m_SetMatrix[2].rotate.z = (m_Camera->GetCamPitch()) + (XM_PI / 36);


		XMFLOAT4 pointDiffusecolor[4];
		XMFLOAT4 lightPosition[4];

		/*pointDiffusecolor[0] = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
		pointDiffusecolor[1] = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
		pointDiffusecolor[2] = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
		pointDiffusecolor[3] = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);

		lightPosition[0] = XMFLOAT4(0.0f, 0.1f, 0.0f, 1.0f);
		lightPosition[1] = XMFLOAT4(-0.1f, 0.01f, 0.1f, 1.0f);
		lightPosition[2] = XMFLOAT4(1.0f, 1.0f, -1.0f, 1.0f);
		lightPosition[3] = XMFLOAT4(-1.0f, 1.0f, -1.0f, 1.0f);*/
		pointDiffusecolor[0] = m_Light1->GetDiffuseColor();
		pointDiffusecolor[1] = m_Light2->GetDiffuseColor();
		pointDiffusecolor[2] = m_Light3->GetDiffuseColor();
		pointDiffusecolor[3] = m_Light4->GetDiffuseColor();

		lightPosition[0] = m_Light1->GetPosition();
		lightPosition[1] = m_Light2->GetPosition();
		lightPosition[2] = m_Light3->GetPosition();
		lightPosition[3] = m_Light4->GetPosition();


		m_D3D->TurnZBufferOff();
		// 표면 컬링을 끕니다.
		m_D3D->TurnOffCulling();
		m_D3D->TurnOnAlphaBlending();
		 //스카이 돔 셰이더를 사용하여 하늘 돔을 렌더링합니다.
		XMFLOAT3 cameraPosition;
		// 카메라 위치를 얻는다.
		cameraPosition = m_Camera->GetPosition();
		// 스카이 돔을 카메라 위치를 중심으로 변환합니다.
		worldMatrix = XMMatrixScaling(1.0f, 1.0f, 1.0f);
		worldMatrix *= XMMatrixTranslation(cameraPosition.x, cameraPosition.y-0.5f, cameraPosition.z);
		m_SkyDome->Render(m_D3D->GetDeviceContext());
		m_SkyDomeShader->Render(m_D3D->GetDeviceContext(), m_SkyDome->GetIndexCount(), worldMatrix, viewMatrix,
			projectionMatrix, m_SkyDome->GetApexColor(), m_SkyDome->GetCenterColor());
		// 다시 표면 컬링을 되돌립니다.
		m_D3D->TurnOffAlphaBlending();
		m_D3D->TurnZBufferOn();
		m_D3D->TurnOnCulling();


		//model0:로봇
		for (int i = 0; i < 10; i++)
		{
			m_EnemyWorldMatrix[i] = ConvertWorldMatrix(m_EnemySetMatrix[i]);
			if (isDie[i] == false)
			{
				m_Model[0]->Render(m_D3D->GetDeviceContext());
				//// Render the model using the light shader.
				result = m_LightShader->Render(m_D3D->GetDeviceContext(), m_Model[0]->GetIndexCount(),
					m_Model[0]->GetInstanceCount(0),
					m_EnemyWorldMatrix[i], viewMatrix, projectionMatrix,
					m_Model[0]->GetTexture(),
					m_Light->GetDirection(), m_Light->GetAmbientColor(), m_Light->GetDiffuseColor(),
					m_Camera->GetPosition(), m_Light->GetSpecularColor(), m_Light->GetSpecularPower(), pointDiffusecolor, lightPosition);
				if (!result)
				{
					return false;
				}
			}
		}
		/*m_Model[0]->RenderMulti(m_D3D->GetDeviceContext());
		result = m_SpecMapShader->Render(m_D3D->GetDeviceContext(), m_Model[0]->GetIndexCount(), m_Model[0]->GetInstanceCount(0), worldMatrix, viewMatrix, projectionMatrix,
			m_Model[0]->GetTextureArray(), m_Light->GetDirection(), m_Light->GetAmbientColor(), m_Light->GetDiffuseColor(),
			m_Camera->GetPosition(), m_Light->GetSpecularColor(), m_Light->GetSpecularPower(), pointDiffusecolor, lightPosition);
		*/

		//model1:땅
		worldMatrix = ConvertWorldMatrix(m_SetMatrix[1]);
		m_Model[1]->RenderMulti(m_D3D->GetDeviceContext());
		result = m_SpecMapShader->Render(m_D3D->GetDeviceContext(), m_Model[1]->GetIndexCount(), m_Model[1]->GetInstanceCount(1), worldMatrix, viewMatrix, projectionMatrix,
			m_Model[1]->GetTextureArray(), m_Light->GetDirection(), m_Light->GetAmbientColor(), m_Light->GetDiffuseColor(),
			m_Camera->GetPosition(), m_Light->GetSpecularColor(), m_Light->GetSpecularPower(), pointDiffusecolor, lightPosition);
		//m_Model[1]->Render(m_D3D->GetDeviceContext());

		// Render the model using the light shader.
		// 
		/*result = m_LightShader->Render(m_D3D->GetDeviceContext(), m_Model[1]->GetIndexCount(),
			m_Model[1]->GetInstanceCount(1),
			worldMatrix, viewMatrix, projectionMatrix,
			m_Model[1]->GetTexture(),
			m_Light->GetDirection(), m_Light->GetAmbientColor(), m_Light->GetDiffuseColor(),
			m_Camera->GetPosition(), m_Light->GetSpecularColor(), m_Light->GetSpecularPower(), pointDiffusecolor, lightPosition);*/



		if (!result)
		{
			return false;
		}
		//model2:총
		m_Model[2]->Render(m_D3D->GetDeviceContext());
		//worldMatrix = viewMatrix;
		worldMatrix = ConvertWorldMatrix(m_SetMatrix[2]);
		// Render the model using the light shader.
		result = m_SpecMapShader->Render(m_D3D->GetDeviceContext(), m_Model[2]->GetIndexCount(), m_Model[2]->GetInstanceCount(2), worldMatrix, viewMatrix, projectionMatrix,
			m_Model[2]->GetTextureArray(), m_Light->GetDirection(), m_Light->GetAmbientColor(), m_Light->GetDiffuseColor(),
			m_Camera->GetPosition(), m_Light->GetSpecularColor(), m_Light->GetSpecularPower(), pointDiffusecolor, lightPosition);
		//result = m_LightShader->Render(m_D3D->GetDeviceContext(), m_Model[2]->GetIndexCount(),
		//	m_Model[2]->GetInstanceCount(2),
		//	worldMatrix, viewMatrix, projectionMatrix,
		//	m_Model[2]->GetTexture(),
		//	m_Light->GetDirection(), m_Light->GetAmbientColor(), m_Light->GetDiffuseColor(),
		//	m_Camera->GetPosition(), m_Light->GetSpecularColor(), m_Light->GetSpecularPower(), pointDiffusecolor, lightPosition);
		if (!result)
		{
			return false;
		}

		//////////////2D렌더링
		// 카메라 위치를 얻는다.
		//cameraPosition = m_Camera->GetPosition();
		m_D3D->TurnOnAlphaBlending();
		for (int i = 0; i < 10; i++)
		{
			//m_D3D->GetWorldMatrix(worldMatrix);

			if (isHit[i] == true)
			{
				double angle = atan2(m_FireSetMatrix[i].trans.x - cameraPosition.x, m_FireSetMatrix[i].trans.z - cameraPosition.z) * (180.0 / XM_PI);
				// 회전을 라디안으로 변환합니다.
				float rotation = (float)angle * 0.0174532925f;
				m_FireSetMatrix[i].rotate.y = rotation;
				m_FireWorldMatrix[i] = ConvertWorldMatrix(m_FireSetMatrix[i]);
				m_Model[4]->RenderEffect(m_D3D->GetDeviceContext());
				// 화재 쉐이더를 사용하여 사각형 모델을 렌더링합니다.
				if (!m_FireShader->Render(m_D3D->GetDeviceContext(), m_Model[4]->GetIndexCount(), m_FireWorldMatrix[i], viewMatrix,
					projectionMatrix, m_Model[4]->GetTexture1(), m_Model[4]->GetTexture2(), m_Model[4]->GetTexture3(),
					time, scrollSpeeds, scales, distortion1, distortion2, distortion3, distortionScale,
					distortionBias))
				{
					return false;
				}
			}
		}


		m_D3D->TurnZBufferOff();
		//m_D3D->TurnOnAlphaBlending();

		m_D3D->GetWorldMatrix(worldMatrix);




		m_D3D->GetWorldMatrix(worldMatrix);
		worldMatrix = XMMatrixRotationY(-XM_PI / 72);
		// Put the model vertex and index buffers on the graphics pipeline to prepare them for drawing.
		result = m_Model[3]->Render2D(m_D3D->GetDeviceContext(), 800, 450);//100 100 : X,Y좌표
		if (!result)
		{
			return false;
		}
		// Render the bitmap with the texture shader.
		result = m_TextureShader->Render(m_D3D->GetDeviceContext(), m_Model[3]->GetIndexCount(), worldMatrix, m_Model[3]->GetViewMatrix2D(), orthoMatrix, m_Model[3]->GetTexture(), blendAmount);
		if (!result)
		{
			return false;

		}

		result = m_Text->Render(m_D3D->GetDeviceContext(), worldMatrix, orthoMatrix);
		if (!result)
		{
			return false;
		}
		// 정사각형 모델의 정점과 인덱스 버퍼를 그래픽 파이프 라인에 배치하여 그리기를 준비합니다.
		m_D3D->TurnZBufferOn();
		m_D3D->TurnOffAlphaBlending();
		//////////////
		break;
	}
	}
	// Present the rendered scene to the screen.
	m_D3D->EndScene();

	return true;
	
}
XMMATRIX GraphicsClass::ConvertWorldMatrix(SetMatrix temp_matrix)
{
	XMMATRIX matTrans, matRotation, matScale, matFinal;

	matScale = XMMatrixScaling(temp_matrix.scale.x, temp_matrix.scale.y, temp_matrix.scale.z);
	matRotation = XMMatrixRotationRollPitchYaw(temp_matrix.rotate.x, temp_matrix.rotate.y, temp_matrix.rotate.z);
	matTrans = XMMatrixTranslation(temp_matrix.trans.x, temp_matrix.trans.y, temp_matrix.trans.z);
	matFinal = matScale * matRotation * matTrans;
	return matFinal;
}


void GraphicsClass::OnOffAmbientLight() { m_LightShader->OnOffAmbientLight(); };
void GraphicsClass::OnOffDiffuseLight() { m_LightShader->OnOffDiffuseLight(); };
void GraphicsClass::OnOffSpecularLight() { m_LightShader->OnOffSpecularLight(); };
//void GraphicsClass::OnOffSpecularColor() { m_Light->OnOffSpecularColor(); };

float GraphicsClass::GetCamYaw()
{
	return m_Camera->GetCamYaw();
}

float GraphicsClass::GetCamPitch()
{
	return m_Camera->GetCamPitch();
}

void GraphicsClass::SetMoveLeftRight(float speed)
{
	m_Camera->SetMoveLeftRight(speed);
}

void GraphicsClass::SetMoveBackForward(float speed)
{
	m_Camera->SetMoveBackForward(speed);
}
void GraphicsClass::SetMoveUpDown(float speed)
{
	m_Camera->SetMoveUpDown(speed);
}
void GraphicsClass::SetYawPitch(float yaw, float pitch)
{
	m_Camera->SetYawPitch(yaw, pitch);
}
void GraphicsClass::AddTitleNum()
{
	titleNum += 1;
	if (titleNum == 3)
	{
		m_stage = EStage::Main;
	}
}

void GraphicsClass::Reset()
{
	canSpawn = true;
	spawnTime = 0.0f;
	titleNum = 0;
	//m_stage = EStage::Main;
	score =0.0f;
	isShoot = true;
	for (int i = 0; i < 10; ++i)
	{
		isHit[i] = false;
		isDie[i] = true;
	}
	//enemyCount = 0;
	numOfBullet = 10;
	m_Camera->SetPosition(0.0f, 12.0f, -45.0f);
	m_Camera->SetYawPitch(0.0f, 0.0f);
}

void GraphicsClass::calculateCount()
{
	for (int i = 0; i < 10; i++)
	{
		if (!isDie[i])
		{
			enemyCount++;
		}
		if (isHit[i])
		{
			fireCount++;
		}
	}
	floorCount = m_Model[1]->GetInstanceCount(1);
	numOfObject = enemyCount  + floorCount + m_Model[2]->GetInstanceCount(2) + m_Model[3]->GetInstanceCount(3) + fireCount;
	numOfPolygon = (enemyCount*m_Model[0]->GetIndexCount()/3) + (floorCount * m_Model[1]->GetIndexCount() / 3) + (m_Model[2]->GetInstanceCount(2) * m_Model[2]->GetIndexCount() / 3) + (m_Model[3]->GetInstanceCount(3) * m_Model[3]->GetIndexCount() / 3) + (fireCount * m_Model[4]->GetIndexCount() / 3);
	enemyCount = 0;
	fireCount = 0;
}


void GraphicsClass::PickBoundBox(DIMOUSESTATE mouseCurrState, HWND hwnd)
{
	///////////////**************new**************////////////////////
		//score++;
		//m_Model[0]->isHit = true;
		POINT mousePos;
		GetCursorPos(&mousePos);
		ScreenToClient(hwnd, &mousePos);
		int mousex = mousePos.x;
		m_mouseX = mousex;			
		int mousey = mousePos.y;
		m_mouseY = mousey;
		float tempDist;
		float closestDist = FLT_MAX;
		int hitIndex;
		XMVECTOR prwsPos, prwsDir;
		pickRayVector(mousex, mousey, prwsPos, prwsDir);

		for (int i = -0; i < 10; i++)
			//double pickOpStartTime = timeGetTime();        // Get the time before we start our picking operation
		{
			if (isHit[i] == false) // No need to check bottles already hit
			{
				tempDist = FLT_MAX;
				// Bounding Box picking test
				tempDist = pick(prwsPos, prwsDir, enemyBoundingBoxVertPosArray, enemyBoundingBoxVertIndexArray, m_EnemyWorldMatrix[i]);
				if (tempDist < closestDist)
				{
					closestDist = tempDist;
					hitIndex = i;
				}
			}
		}

		if (closestDist < FLT_MAX)
		{
			isHit[hitIndex] = true;
			score++;
			m_FireSetMatrix[hitIndex] = { XMFLOAT3(20.0f, 20.0f, 20.0f),
										  XMFLOAT3(0.0f, 0.0f, 0.0f),
										  m_EnemySetMatrix[hitIndex].trans };
			m_FireSetMatrix[hitIndex].trans.y += 20.0f;
		};
		
		m_Camera->SetPitchPlus(-0.03f);
		numOfBullet -= 1;
		canShoot = false;
}

void GraphicsClass::CreateBoundingVolumes(std::vector<XMFLOAT3>& vertPosArray,
	std::vector<XMFLOAT3>& boundingBoxVerts,
	std::vector<DWORD>& boundingBoxIndex)
{
	XMFLOAT3 minVertex = XMFLOAT3(FLT_MAX, FLT_MAX, FLT_MAX);
	XMFLOAT3 maxVertex = XMFLOAT3(-FLT_MAX, -FLT_MAX, -FLT_MAX);

	for (UINT i = 0; i < vertPosArray.size(); i++)
	{
		// The minVertex and maxVertex will most likely not be actual vertices in the model, but vertices
		// that use the smallest and largest x, y, and z values from the model to be sure ALL vertices are
		// covered by the bounding volume

		//Get the smallest vertex 
		minVertex.x = min(minVertex.x, vertPosArray[i].x);    // Find smallest x value in model
		minVertex.y = min(minVertex.y, vertPosArray[i].y);    // Find smallest y value in model
		minVertex.z = min(minVertex.z, vertPosArray[i].z);    // Find smallest z value in model

		//Get the largest vertex 
		maxVertex.x = max(maxVertex.x, vertPosArray[i].x);    // Find largest x value in model
		maxVertex.y = max(maxVertex.y, vertPosArray[i].y);    // Find largest y value in model
		maxVertex.z = max(maxVertex.z, vertPosArray[i].z);    // Find largest z value in model
	}

	// Compute distance between maxVertex and minVertex
	float distX = (maxVertex.x - minVertex.x) / 2.0f;
	float distY = (maxVertex.y - minVertex.y) / 2.0f;
	float distZ = (maxVertex.z - minVertex.z) / 2.0f;

	// Create bounding box    
	// Front Vertices
	boundingBoxVerts.push_back(XMFLOAT3(minVertex.x, minVertex.y, minVertex.z));
	boundingBoxVerts.push_back(XMFLOAT3(minVertex.x, maxVertex.y, minVertex.z));
	boundingBoxVerts.push_back(XMFLOAT3(maxVertex.x, maxVertex.y, minVertex.z));
	boundingBoxVerts.push_back(XMFLOAT3(maxVertex.x, minVertex.y, minVertex.z));

	// Back Vertices
	boundingBoxVerts.push_back(XMFLOAT3(minVertex.x, minVertex.y, maxVertex.z));
	boundingBoxVerts.push_back(XMFLOAT3(maxVertex.x, minVertex.y, maxVertex.z));
	boundingBoxVerts.push_back(XMFLOAT3(maxVertex.x, maxVertex.y, maxVertex.z));
	boundingBoxVerts.push_back(XMFLOAT3(minVertex.x, maxVertex.y, maxVertex.z));

	DWORD* i = new DWORD[36];

	// Front Face
	i[0] = 0; i[1] = 1; i[2] = 2;
	i[3] = 0; i[4] = 2; i[5] = 3;

	// Back Face
	i[6] = 4; i[7] = 5; i[8] = 6;
	i[9] = 4; i[10] = 6; i[11] = 7;

	// Top Face
	i[12] = 1; i[13] = 7; i[14] = 6;
	i[15] = 1; i[16] = 6; i[17] = 2;

	// Bottom Face
	i[18] = 0; i[19] = 4; i[20] = 5;
	i[21] = 0; i[22] = 5; i[23] = 3;

	// Left Face
	i[24] = 4; i[25] = 7; i[26] = 1;
	i[27] = 4; i[28] = 1; i[29] = 0;

	// Right Face
	i[30] = 3; i[31] = 2; i[32] = 6;
	i[33] = 3; i[34] = 6; i[35] = 5;

	for (int j = 0; j < 36; j++)
		boundingBoxIndex.push_back(i[j]);
}

void GraphicsClass::pickRayVector(float mouseX, float mouseY, XMVECTOR& pickRayInWorldSpacePos, XMVECTOR& pickRayInWorldSpaceDir)
{
	XMVECTOR pickRayInViewSpaceDir = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
	XMVECTOR pickRayInViewSpacePos = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
	XMMATRIX projectionMatrix;

	m_D3D->GetProjectionMatrix(projectionMatrix);
	XMFLOAT3X3 projectionMatrix4;
	XMStoreFloat3x3(&projectionMatrix4, projectionMatrix);

	float PRVecX, PRVecY, PRVecZ;

	//Transform 2D pick position on screen space to 3D ray in View space
	//PRVecX = (((2.0f * mouseX) / ClientWidth) - 1) / camProjection(0, 0);
	//PRVecY = -(((2.0f * mouseY) / ClientHeight) - 1) / camProjection(1, 1);
	PRVecX = 0.0f;//(((2.0f * mouseX) / m_screenWidth) - 1) / projectionMatrix4._11;
	PRVecY = 0.0f;//-(((2.0f * mouseY) / m_screenWidth) - 1) / projectionMatrix4._22;

	///////////화면중앙으로바꾸기
	/*PRVecX = (((m_screenWidth / 2))) / projectionMatrix4._11;
	PRVecY = (((-m_screenHeight / 2))) / projectionMatrix4._22;*/
	//PRVecX = 0.5f;//(((m_screenWidth / 2)));
	//PRVecY = 0.5f; //(((m_screenHeight / 2)));
	//m_mouseX = int(PRVecX);
	//m_mouseY = int(PRVecY);
	//////////
	PRVecZ = 1.0f;    //View space's Z direction ranges from 0 to 1, so we set 1 since the ray goes "into" the screen

	pickRayInViewSpaceDir = XMVectorSet(PRVecX, PRVecY, PRVecZ, 0.0f);

	pickRayInViewSpaceDir = XMVector3Normalize(pickRayInViewSpaceDir);

	//Uncomment this line if you want to use the center of the screen (client area)
	//to be the point that creates the picking ray (eg. first person shooter)
	//pickRayInViewSpaceDir = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);

	// Transform 3D Ray from View space to 3D ray in World space
	XMMATRIX pickRayToWorldSpaceMatrix;
	XMVECTOR matInvDeter;    //We don't use this, but the xna matrix inverse function requires the first parameter to not be null
	XMMATRIX viewMatrix;
	m_Camera->GetViewMatrix(viewMatrix);
	pickRayToWorldSpaceMatrix = XMMatrixInverse(&matInvDeter, viewMatrix);    //Inverse of View Space matrix is World space matrix

	pickRayInWorldSpacePos = XMVector3TransformCoord(pickRayInViewSpacePos, pickRayToWorldSpaceMatrix);
	pickRayInWorldSpaceDir = XMVector3TransformNormal(pickRayInViewSpaceDir, pickRayToWorldSpaceMatrix);
	//pickRayInWorldSpaceDir = XMVector3Normalize(pickRayInWorldSpaceDir);
}

float GraphicsClass::pick(XMVECTOR pickRayInWorldSpacePos,
	XMVECTOR pickRayInWorldSpaceDir,
	std::vector<XMFLOAT3>& vertPosArray,
	std::vector<DWORD>& indexPosArray,
	XMMATRIX& worldSpace)
{
	//Loop through each triangle in the object
	for (int i = 0; i < indexPosArray.size() / 3; i++)
	{
		//Triangle's vertices V1, V2, V3
		XMVECTOR tri1V1 = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
		XMVECTOR tri1V2 = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
		XMVECTOR tri1V3 = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);

		//Temporary 3d floats for each vertex
		XMFLOAT3 tV1, tV2, tV3;

		//Get triangle 
		tV1 = vertPosArray[indexPosArray[(i * 3) + 0]];
		tV2 = vertPosArray[indexPosArray[(i * 3) + 1]];
		tV3 = vertPosArray[indexPosArray[(i * 3) + 2]];

		tri1V1 = XMVectorSet(tV1.x, tV1.y, tV1.z, 0.0f);
		tri1V2 = XMVectorSet(tV2.x, tV2.y, tV2.z, 0.0f);
		tri1V3 = XMVectorSet(tV3.x, tV3.y, tV3.z, 0.0f);

		//Transform the vertices to world space
		tri1V1 = XMVector3TransformCoord(tri1V1, worldSpace);
		tri1V2 = XMVector3TransformCoord(tri1V2, worldSpace);
		tri1V3 = XMVector3TransformCoord(tri1V3, worldSpace);

		//Find the normal using U, V coordinates (two edges)
		XMVECTOR U = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
		XMVECTOR V = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
		XMVECTOR faceNormal = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);

		U = tri1V2 - tri1V1;
		V = tri1V3 - tri1V1;

		//Compute face normal by crossing U, V
		faceNormal = XMVector3Cross(U, V);

		faceNormal = XMVector3Normalize(faceNormal);

		//Calculate a point on the triangle for the plane equation
		XMVECTOR triPoint = tri1V1;

		//Get plane equation ("Ax + By + Cz + D = 0") Variables
		float tri1A = XMVectorGetX(faceNormal);
		float tri1B = XMVectorGetY(faceNormal);
		float tri1C = XMVectorGetZ(faceNormal);
		float tri1D = (-tri1A * XMVectorGetX(triPoint) - tri1B * XMVectorGetY(triPoint) - tri1C * XMVectorGetZ(triPoint));

		//Now we find where (on the ray) the ray intersects with the triangles plane
		float ep1, ep2, t = 0.0f;
		float planeIntersectX, planeIntersectY, planeIntersectZ = 0.0f;
		XMVECTOR pointInPlane = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);

		ep1 = (XMVectorGetX(pickRayInWorldSpacePos) * tri1A) + (XMVectorGetY(pickRayInWorldSpacePos) * tri1B) + (XMVectorGetZ(pickRayInWorldSpacePos) * tri1C);
		ep2 = (XMVectorGetX(pickRayInWorldSpaceDir) * tri1A) + (XMVectorGetY(pickRayInWorldSpaceDir) * tri1B) + (XMVectorGetZ(pickRayInWorldSpaceDir) * tri1C);

		//Make sure there are no divide-by-zeros
		if (ep2 != 0.0f)
			t = -(ep1 + tri1D) / (ep2);

		if (t > 0.0f)    //Make sure you don't pick objects behind the camera
		{
			//Get the point on the plane
			planeIntersectX = XMVectorGetX(pickRayInWorldSpacePos) + XMVectorGetX(pickRayInWorldSpaceDir) * t;
			planeIntersectY = XMVectorGetY(pickRayInWorldSpacePos) + XMVectorGetY(pickRayInWorldSpaceDir) * t;
			planeIntersectZ = XMVectorGetZ(pickRayInWorldSpacePos) + XMVectorGetZ(pickRayInWorldSpaceDir) * t;

			pointInPlane = XMVectorSet(planeIntersectX, planeIntersectY, planeIntersectZ, 0.0f);

			//Call function to check if point is in the triangle
			if (PointInTriangle(tri1V1, tri1V2, tri1V3, pointInPlane))
			{
				//Return the distance to the hit, so you can check all the other pickable objects in your scene
				//and choose whichever object is closest to the camera
				return t / 2.0f;
			}
		}
	}
	//return the max float value (near infinity) if an object was not picked
	return FLT_MAX;
}


bool GraphicsClass::PointInTriangle(XMVECTOR& triV1, XMVECTOR& triV2, XMVECTOR& triV3, XMVECTOR& point)
{
	//To find out if the point is inside the triangle, we will check to see if the point
	//is on the correct side of each of the triangles edges.

	XMVECTOR cp1 = XMVector3Cross((triV3 - triV2), (point - triV2));
	XMVECTOR cp2 = XMVector3Cross((triV3 - triV2), (triV1 - triV2));
	if (XMVectorGetX(XMVector3Dot(cp1, cp2)) >= 0)
	{
		cp1 = XMVector3Cross((triV3 - triV1), (point - triV1));
		cp2 = XMVector3Cross((triV3 - triV1), (triV2 - triV1));
		if (XMVectorGetX(XMVector3Dot(cp1, cp2)) >= 0)
		{
			cp1 = XMVector3Cross((triV2 - triV1), (point - triV1));
			cp2 = XMVector3Cross((triV2 - triV1), (triV3 - triV1));
			if (XMVectorGetX(XMVector3Dot(cp1, cp2)) >= 0)
			{
				return true;
			}
			else
				return false;
		}
		else
			return false;
	}
	return false;
}
///////////////**************new**************////////////////////