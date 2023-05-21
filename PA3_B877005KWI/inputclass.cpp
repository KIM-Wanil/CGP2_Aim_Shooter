////////////////////////////////////////////////////////////////////////////////
// Filename: inputclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "inputclass.h"


InputClass::InputClass()
{
	m_directInput = 0;
	DIKeyboard = 0;
	DIMouse = 0;
}


InputClass::InputClass(const InputClass& other)
{
}


InputClass::~InputClass()
{
}



bool InputClass::Initialize(HINSTANCE hInstance, HWND hwnd, int screenWidth,int screenHeight)
{
	HRESULT result;
	m_screenWidth = screenWidth;
	m_screenHeight = screenHeight;
	m_mouseX = 0;
	m_mouseY = 0;

	for (int i = 0; i < 256; i++)
	{
		m_keys[i] = false;
	}

	result = DirectInput8Create(hInstance,DIRECTINPUT_VERSION,IID_IDirectInput8,
		(void**)&m_directInput,NULL);
	if (FAILED(result))
	{
		return false;
	}

	result = m_directInput->CreateDevice(GUID_SysKeyboard,&DIKeyboard,NULL);
	if (FAILED(result))
	{
		return false;
	}
	
	result = DIKeyboard->SetDataFormat(&c_dfDIKeyboard);
	if (FAILED(result))
	{
		return false;
	}
	result = DIKeyboard->SetCooperativeLevel(hwnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
	if (FAILED(result))
	{
		return false;
	}
	result = DIKeyboard->Acquire();
	if (FAILED(result))
	{
		return false;
	}

	result = m_directInput->CreateDevice(GUID_SysMouse, &DIMouse, NULL);
	if (FAILED(result))
	{
		return false;
	}
	result = DIMouse->SetDataFormat(&c_dfDIMouse);
	if (FAILED(result))
	{
		return false;
	}
	result = DIMouse->SetCooperativeLevel(hwnd, DISCL_EXCLUSIVE | DISCL_NOWINKEY | DISCL_FOREGROUND);
	if (FAILED(result))
	{
		return false;
	}
	result = DIMouse->Acquire();
	if (FAILED(result))
	{
		return false;
	}
	return true;
}
void InputClass::Shutdown()
{
	if (DIMouse)
	{
		DIMouse->Unacquire();
		DIMouse->Release();
		DIMouse = 0;
	}
	if (DIKeyboard)
	{
		DIKeyboard->Unacquire();
		DIKeyboard->Release();
		DIKeyboard = 0;
	}
	if (m_directInput)
	{
		m_directInput->Release();
		m_directInput = 0;
	}

	return;
}

void InputClass::KeyDown(unsigned int input)
{
	// If a key is pressed then save that state in the key array.
	m_keys[input] = true;
	return;
}


void InputClass::KeyUp(unsigned int input)
{
	// If a key is released then clear that state in the key array.
	m_keys[input] = false;
	return;
}


bool InputClass::IsKeyDown(unsigned int key)
{
	//return (GetAsyncKeyState(key) & 0X8000);
	// Return what state the key is in (pressed/not pressed).
	return  m_keys[key];
}

bool InputClass::IsKeyDownOnce(unsigned int key)
{
	//return (GetAsyncKeyState(key) & 0X8000);
	// Return what state the key is in (pressed/not pressed).
	if (m_keys[key] == false)
		return true;
	else
		return false;
	//return  m_keys[key];
	//return  m_keys[key];
}

bool InputClass::ReadMouse()
{
	HRESULT result;


	// Read the mouse device.
	result = DIMouse->GetDeviceState(sizeof(DIMOUSESTATE), (LPVOID)&m_mouseLastState);

	if (FAILED(result))
	{
		// If the mouse lost focus or was not acquired then try to get control back.
		if ((result == DIERR_INPUTLOST) || (result == DIERR_NOTACQUIRED))
		{
			DIMouse->Acquire();
		}
		else
		{
			return false;
		}
	}

	return true;
}

DIMOUSESTATE InputClass::GetMouseLastState()
{
	return m_mouseLastState;
}

void InputClass::SetMouseLastState(DIMOUSESTATE mouseCurrState)
{
	m_mouseLastState = mouseCurrState;
}

void InputClass::DetectMouseInput(DIMOUSESTATE* mouseCurrState)
{

	DIMouse->Acquire();
	DIMouse->GetDeviceState(sizeof(DIMOUSESTATE), &mouseCurrState);

}

bool InputClass::IsLeftMouseButtonDown()
{
	// 마우스 왼쪽 버튼이 현재 눌러져 있는지 확인합니다.
	if (m_mouseLastState.rgbButtons[0] & 0x80)
	{
		return true;
	}

	return false;
}
// 
//bool InputClass::DetectMouseInput(DIMOUSESTATE* mouseCurrState)
//{
//	DIMouse->Acquire();
//	DIMouse->GetDeviceState(sizeof(DIMOUSESTATE), &mouseCurrState);
//	if ((mouseCurrState->lX != m_mouseLastState.lX) || (mouseCurrState->lY != m_mouseLastState.lY))
//		return true;
//	else
//		return false;
//}
//void InputClass::DetectInput(HWND hwnd, double time)
//{
//	DIMOUSESTATE mouseCurrState;
//
//	BYTE keyboardState[256];
//
//	DIKeyboard->Acquire();
//	DIMouse->Acquire();
//
//	DIMouse->GetDeviceState(sizeof(DIMOUSESTATE), &mouseCurrState);
//
//	DIKeyboard->GetDeviceState(sizeof(keyboardState), (LPVOID)&keyboardState);
//
//	if (keyboardState[DIK_ESCAPE] & 0x80)
//		PostMessage(hwnd, WM_DESTROY, 0, 0);
//
//	float speed = 15.0f * (float)time;
//
//	if (keyboardState[DIK_A] & 0x80)
//	{
//		moveLeftRight -= speed;
//	}
//	if (keyboardState[DIK_D] & 0x80)
//	{
//		moveLeftRight += speed;
//	}
//	if (keyboardState[DIK_W] & 0x80)
//	{
//		moveBackForward += speed;
//	}
//	if (keyboardState[DIK_S] & 0x80)
//	{
//		moveBackForward -= speed;
//	}
//	if ((mouseCurrState.lX != mouseLastState.lX) || (mouseCurrState.lY != mouseLastState.lY))
//	{
//		camYaw += mouseLastState.lX * 0.001f;
//
//		camPitch += mouseCurrState.lY * 0.001f;
//
//		mouseLastState = mouseCurrState;
//	}
//
//	UpdateCamera();
//
//	return;
//}