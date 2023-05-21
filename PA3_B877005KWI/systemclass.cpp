////////////////////////////////////////////////////////////////////////////////
// Filename: systemclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "systemclass.h"


SystemClass::SystemClass()
{
	m_Input = 0;
	m_Graphics = 0;

}


SystemClass::SystemClass(const SystemClass& other)
{
}


SystemClass::~SystemClass()
{
}


bool SystemClass::Initialize()
{
	int screenWidth, screenHeight;
	bool result;


	// Initialize the width and height of the screen to zero before sending the variables into the function.
	screenWidth = 0;
	screenHeight = 0;

	// Initialize the windows api.
	InitializeWindows(screenWidth, screenHeight);

	// Create the input object.  This object will be used to handle reading the keyboard input from the user.
	m_Input = new InputClass;
	if(!m_Input)
	{
		return false;
	}

	// Initialize the input object.
	m_Input->Initialize(m_hinstance,m_hwnd,screenWidth, screenHeight);

	// Create the graphics object.  This object will handle rendering all the graphics for this application.
	m_Graphics = new GraphicsClass;
	if(!m_Graphics)
	{
		return false;
	}

	// Initialize the graphics object.
	result = m_Graphics->Initialize(screenWidth, screenHeight, m_hwnd);
	if(!result)
	{
		return false;
	}
	// fps ��ü�� ����ϴ�.
	m_Fps = new FpsClass;
	if (!m_Fps)
	{
		return false;
	}

	// fps ��ü�� �ʱ�ȭ�մϴ�.
	m_Fps->Initialize();

	// cpu ��ü�� ����ϴ�.
	m_Cpu = new CpuClass;
	if (!m_Cpu)
	{
		return false;
	}

	// cpu ��ü�� �ʱ�ȭ�մϴ�.
	m_Cpu->Initialize();

	// Ÿ�̸� ��ü�� ����ϴ�.
	m_Timer = new TimerClass;
	if (!m_Timer)
	{
		return false;
	}

	// Ÿ�̸� ��ü�� �ʱ�ȭ�մϴ�.
	if (!m_Timer->Initialize())
	{
		MessageBox(m_hwnd, L"Could not initialize the Timer object.", L"Error", MB_OK);
		return false;
	}


	
	return true;
}


void SystemClass::Shutdown()
{
	if (m_Timer)
	{
		delete m_Timer;
		m_Timer = 0;
	}

	// cpu ��ü�� �����մϴ�.
	if (m_Cpu)
	{
		m_Cpu->Shutdown();
		delete m_Cpu;
		m_Cpu = 0;
	}

	// fps ��ü�� �����մϴ�.
	if (m_Fps)
	{
		delete m_Fps;
		m_Fps = 0;
	}
	// Release the graphics object.
	if(m_Graphics)
	{
		m_Graphics->Shutdown();
		delete m_Graphics;
		m_Graphics = 0;
	}

	// Release the input object.
	if(m_Input)
	{
		delete m_Input;
		m_Input = 0;
	}

	// Shutdown the window.
	ShutdownWindows();
	
	return;
}


void SystemClass::Run()
{
	MSG msg;
	bool done, result;


	// Initialize the message structure.
	ZeroMemory(&msg, sizeof(MSG));
	
	// Loop until there is a quit message from the window or the user.
	done = false;
	while(!done)
	{
		// Handle the windows messages.
		if(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		// If windows signals to end the application then exit out.
		if(msg.message == WM_QUIT)
		{
			done = true;
		}
		else
		{
			// Otherwise do the frame processing.
			result = Frame();
			if(!result)
			{
				done = true;
			}
		}

	}

	return;
}



bool SystemClass::Frame()
{
	
	m_Timer->Frame();
	m_Timer->Tick();
	m_Fps->Frame();
	m_Cpu->Frame();
	/*if (m_Graphics->isGameOver)
	{
		m_Graphics->SetStage(GraphicsClass::EStage::Title);
	}*/
	stage = m_Graphics->GetStage();
	bool result;
	result = m_Input->ReadMouse();
	if (!result)
	{
		return false;
	}
	if (m_Input->IsKeyDown(VK_ESCAPE))
	{
		return false;
	}
	switch (stage)
	{
	case GraphicsClass::EStage::Title:
		if (m_Input->IsKeyDown(VK_SPACE))
		{
			if (!spaceKeyDown)
			{
				m_Graphics->AddTitleNum();
				spaceKeyDown = true;
			}
		}
		else
		{
			if (spaceKeyDown)
				spaceKeyDown = false;
		}

		break;
	case GraphicsClass::EStage::Over:
		if (m_Input->IsKeyDown(VK_SPACE))
		{
			if (!spaceKeyDown)
			{
				m_Graphics->Reset();
				m_Graphics->SetStage(GraphicsClass::EStage::Main);
				spaceKeyDown = true;
			}
		}
		else
		{
			if (spaceKeyDown)
				spaceKeyDown = false;
		}
		break;

	case GraphicsClass::EStage::Main:
		DIMOUSESTATE mouseCurrState;
		m_Input->DetectMouseInput(&mouseCurrState);
		static float test = 100;
		//test += 1;
		canShoot = m_Graphics->canShoot;
		if (m_Input->IsLeftMouseButtonDown())
		{
			if (canShoot && !isShoot)
			{
				//m_Graphics->score += 1;
				m_Graphics->PickBoundBox(mouseCurrState, m_hwnd);
			}
			isShoot = true;
		}
		if (!m_Input->IsLeftMouseButtonDown())
		{
			isShoot = false;
		}
		// Check if the user pressed escape and wants to exit the application.
		

		if (m_Input->IsKeyDownOnce(0X36) && GetAsyncKeyState(0X36))//input '6'
		{
			m_Graphics->OnOffAmbientLight();
		}
		if (m_Input->IsKeyDownOnce(0X37) && GetAsyncKeyState(0X37))//input '7'
		{
			m_Graphics->OnOffDiffuseLight();
		}
		if (m_Input->IsKeyDownOnce(0X38) && GetAsyncKeyState(0X38))//input '8'
		{
			m_Graphics->OnOffSpecularLight();
		}
		/////
		//if (m_Input->IsKeyDown(0X57) || m_Input->IsKeyDown(0X77))//input 'w' or 'W'
		//{
		//	m_Graphics->SetMoveBackForward(1.0f);
		//}
		if (m_Input->IsKeyDown(0X41) || m_Input->IsKeyDown(0X61))//input 'a' or 'A'
		{
			m_Graphics->SetMoveLeftRight(-0.5f);
		}
		//if (m_Input->IsKeyDown(0X53) || m_Input->IsKeyDown(0X73))//input 's' or 'S'
		//{
		//	m_Graphics->SetMoveBackForward(-1.0f);
		//}
		if (m_Input->IsKeyDown(0X44) || m_Input->IsKeyDown(0X64))//input 'd' or 'D'
		{
			m_Graphics->SetMoveLeftRight(0.5f);
		}

		//if (m_Input->IsKeyDown(0X51) || m_Input->IsKeyDown(0X71))//input 'q' or 'Q'
		//{
		//	m_Graphics->SetMoveUpDown(1.0f);
		//}
		//if (m_Input->IsKeyDown(0X45) || m_Input->IsKeyDown(0X65))//input 'e' or 'E'
		//{
		//	m_Graphics->SetMoveUpDown(-1.0f);
		//}
		if (m_Input->IsKeyDown(0X52) || m_Input->IsKeyDown(0X72))//input 'r' or 'R'
		{
			if (m_Graphics->reloadTime == 0 && m_Graphics->numOfBullet != 10)
			{
				m_Graphics->numOfBullet = 0;
			}
		}

		if (m_Input->IsKeyDown(0X5A) || m_Input->IsKeyDown(0X7A))//input 'z' or 'Z'
		{
			m_Graphics->SetYawPitch(0.0f, XM_PI / 90);
		}


		if ((mouseCurrState.lX != m_Input->GetMouseLastState().lX) || (mouseCurrState.lY != m_Input->GetMouseLastState().lY))
		{
			m_Graphics->SetYawPitch(m_Graphics->GetCamYaw() + m_Input->GetMouseLastState().lX * 0.0005f, m_Graphics->GetCamPitch() + m_Input->GetMouseLastState().lY * 0.0005f);
			m_Input->SetMouseLastState(mouseCurrState);
		}
		break;
	}
	// Do the frame processing for the graphics object.
	result = m_Graphics->Frame(m_Fps->GetFps(), m_Cpu->GetCpuPercentage(), m_Timer->GetTime(), 100);
	if (!result)
	{
		return false;
	}

	return true;
}

LRESULT CALLBACK SystemClass::MessageHandler(HWND hwnd, UINT umsg, WPARAM wparam, LPARAM lparam)
{
	switch(umsg)
	{
		// Check if a key has been pressed on the keyboard.
		case WM_KEYDOWN:
		{
			// If a key is pressed send it to the input object so it can record that state.
			m_Input->KeyDown((unsigned int)wparam);
			return 0;
		}

		// Check if a key has been released on the keyboard.
		case WM_KEYUP:
		{
			// If a key is released then send it to the input object so it can unset the state for that key.
			m_Input->KeyUp((unsigned int)wparam)
;
			return 0;
		}

		// Any other messages send to the default message handler as our application won't make use of them.
		default:
		{
			return DefWindowProc(hwnd, umsg, wparam, lparam);
		}
	}
}


void SystemClass::InitializeWindows(int& screenWidth, int& screenHeight)
{
	WNDCLASSEX wc;
	DEVMODE dmScreenSettings;
	int posX, posY;


	// Get an external pointer to this object.	
	ApplicationHandle = this;

	// Get the instance of this application.
	m_hinstance = GetModuleHandle(NULL);

	// Give the application a name.
	m_applicationName = L"Engine";

	// Setup the windows class with default settings.
	wc.style         = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wc.lpfnWndProc   = WndProc;
	wc.cbClsExtra    = 0;
	wc.cbWndExtra    = 0;
	wc.hInstance     = m_hinstance;
	wc.hIcon		 = LoadIcon(NULL, IDI_WINLOGO);
	wc.hIconSm       = wc.hIcon;
	wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wc.lpszMenuName  = NULL;
	wc.lpszClassName = m_applicationName;
	wc.cbSize        = sizeof(WNDCLASSEX);
	
	// Register the window class.
	RegisterClassEx(&wc);

	// Determine the resolution of the clients desktop screen.
	screenWidth  = GetSystemMetrics(SM_CXSCREEN);
	screenHeight = GetSystemMetrics(SM_CYSCREEN);

	// Setup the screen settings depending on whether it is running in full screen or in windowed mode.
	if(FULL_SCREEN)
	{
		// If full screen set the screen to maximum size of the users desktop and 32bit.
		memset(&dmScreenSettings, 0, sizeof(dmScreenSettings));
		dmScreenSettings.dmSize       = sizeof(dmScreenSettings);
		dmScreenSettings.dmPelsWidth  = (unsigned long)screenWidth;
		dmScreenSettings.dmPelsHeight = (unsigned long)screenHeight;
		dmScreenSettings.dmBitsPerPel = 32;			
		dmScreenSettings.dmFields     = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

		// Change the display settings to full screen.
		ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN);

		// Set the position of the window to the top left corner.
		posX = posY = 0;
	}
	else
	{
		// If windowed then set it to 800x600 resolution.
		screenWidth  = 1600;
		screenHeight = 900;

		// Place the window in the middle of the screen.
		posX = (GetSystemMetrics(SM_CXSCREEN) - screenWidth)  / 2;
		posY = (GetSystemMetrics(SM_CYSCREEN) - screenHeight) / 2;
	}

	// Create the window with the screen settings and get the handle to it.
	m_hwnd = CreateWindowEx(WS_EX_APPWINDOW, m_applicationName, m_applicationName, 
						    WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_POPUP,
						    posX, posY, screenWidth, screenHeight, NULL, NULL, m_hinstance, NULL);

	// Bring the window up on the screen and set it as main focus.
	ShowWindow(m_hwnd, SW_SHOW);
	SetForegroundWindow(m_hwnd);
	SetFocus(m_hwnd);

	// Hide the mouse cursor.
	ShowCursor(false);

	return;
}


void SystemClass::ShutdownWindows()
{
	// Show the mouse cursor.
	ShowCursor(true);

	// Fix the display settings if leaving full screen mode.
	if(FULL_SCREEN)
	{
		ChangeDisplaySettings(NULL, 0);
	}

	// Remove the window.
	DestroyWindow(m_hwnd);
	m_hwnd = NULL;

	// Remove the application instance.
	UnregisterClass(m_applicationName, m_hinstance);
	m_hinstance = NULL;

	// Release the pointer to this class.
	ApplicationHandle = NULL;

	return;
}


LRESULT CALLBACK WndProc(HWND hwnd, UINT umessage, WPARAM wparam, LPARAM lparam)
{
	switch(umessage)
	{
		// Check if the window is being destroyed.
		case WM_DESTROY:
		{
			PostQuitMessage(0);
			return 0;
		}

		// Check if the window is being closed.
		case WM_CLOSE:
		{
			PostQuitMessage(0);		
			return 0;
		}

		// All other messages pass to the message handler in the system class.
		default:
		{
			return ApplicationHandle->MessageHandler(hwnd, umessage, wparam, lparam);
		}
	}
}