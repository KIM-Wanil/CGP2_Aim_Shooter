#pragma once
#ifndef _TIMERCLASS_H_
#define _TIMERCLASS_H_


//////////////
// INCLUDES //
//////////////
#include <d3d11.h>
#include <directxmath.h>
#include <d3dcompiler.h>
#include <fstream>

using namespace std;
using namespace DirectX;
class TimerClass
{
public:
	TimerClass();
	TimerClass(const TimerClass&);
	~TimerClass();

	bool Initialize();
	void Frame();
	void Tick();
	float DeltaTime()const;
	float GetTime();

private:
	INT64 m_frequency = 0;
	float m_ticksPerMs = 0;
	INT64 m_startTime = 0;
	float m_frameTime = 0;

	double mSecondsPerCount;
	double mDeltaTime;
	INT64 mCurrTime;
	INT64 mPrevTime;
};
#endif