////////////////////////////////////////////////////////////////////////////////
// Filename: inputclass.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _INPUTCLASS_H_
#define _INPUTCLASS_H_

#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")


//////////////
// INCLUDES //
//////////////
#include <dinput.h>
////////////////////////////////////////////////////////////////////////////////
// Class name: InputClass
////////////////////////////////////////////////////////////////////////////////
class InputClass
{
public:
	InputClass();
	InputClass(const InputClass&);
	~InputClass();

	bool Initialize(HINSTANCE,HWND, int,int);
	void Shutdown();
	void KeyDown(unsigned int);
	void KeyUp(unsigned int);

	bool IsKeyDown(unsigned int);
	bool IsKeyDownOnce(unsigned int);
	void DetectMouseInput(DIMOUSESTATE*);
	//bool DetectMouseInput(DIMOUSESTATE*);
	DIMOUSESTATE GetMouseLastState();
	void SetMouseLastState(DIMOUSESTATE);
	bool ReadMouse();
	bool IsLeftMouseButtonDown();


private:
	bool m_keys[256];
	IDirectInput8* m_directInput;
	IDirectInputDevice8* DIKeyboard;
	IDirectInputDevice8* DIMouse;
	DIMOUSESTATE m_mouseLastState;
	int m_screenWidth, m_screenHeight;
	int m_mouseX, m_mouseY;

};

#endif