
#include "TimerClass.h"


TimerClass::TimerClass()
{
	mSecondsPerCount = 0.0f;
	mDeltaTime= -1.0f;
	mPrevTime = 0;
	mCurrTime = 0;
}


TimerClass::TimerClass(const TimerClass& other)
{
}


TimerClass::~TimerClass()
{
}


bool TimerClass::Initialize()
{
	// �� �ý����� ���� Ÿ�̸Ӹ� �����ϴ��� Ȯ���Ͻʽÿ�.
	QueryPerformanceFrequency((LARGE_INTEGER*)&m_frequency);
	if (m_frequency == 0)
	{
		return false;
	}

	// �� ī���Ͱ� �� �и� �ʸ��� ƽ�ϴ� Ƚ���� Ȯ���մϴ�.
	m_ticksPerMs = (float)(m_frequency / 1000);

	QueryPerformanceCounter((LARGE_INTEGER*)&m_startTime);

	/// ///////////
	__int64 countsPerSec;
	QueryPerformanceFrequency((LARGE_INTEGER*)&countsPerSec);
	mSecondsPerCount = 1.0 / (double)countsPerSec;
	return true;
}


void TimerClass::Frame()
{
	INT64 currentTime = 0;

	QueryPerformanceCounter((LARGE_INTEGER*)&currentTime);
	float timeDifference = (float)(currentTime - m_startTime);

	m_frameTime = timeDifference / m_ticksPerMs;

	m_startTime = currentTime;
}
void TimerClass::Tick()
{
	// Get the time this frame.
	INT64 currTime;
	QueryPerformanceCounter((LARGE_INTEGER*)&currTime);
	mCurrTime = currTime;
	// Time difference between this frame and the previous.
	mDeltaTime = (mCurrTime - mPrevTime) * mSecondsPerCount;
	// Prepare for next frame.
	mPrevTime = mCurrTime;
	// Force nonnegative. The DXSDK��s CDXUTTimer mentions that if the 
	// processor goes into a power save mode or we get shuffled to
	// another processor, then mDeltaTime can be negative.
	if (mDeltaTime < 0.0)
	{
		mDeltaTime = 0.0;
	}
}
float TimerClass::DeltaTime()const
{
	return (float)mDeltaTime;
}

float TimerClass::GetTime()
{
	return m_frameTime;
}