#pragma once

/////////////
// LINKING //
/////////////
#pragma comment(lib, "pdh.lib")
#include <pdh.h>
#include <d3d11.h>
#include <directxmath.h>
#include <d3dcompiler.h>
#include <fstream>

using namespace std;
using namespace DirectX;

class CpuClass
{
public:
	CpuClass();
	CpuClass(const CpuClass&);
	~CpuClass();

	void Initialize();
	void Shutdown();
	void Frame();
	int GetCpuPercentage();

private:
	bool m_canReadCpu = true;
	HQUERY m_queryHandle;
	HCOUNTER m_counterHandle;
	unsigned long m_lastSampleTime = 0;
	long m_cpuUsage = 0;
};