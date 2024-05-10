#include <thread>
#include <chrono>
#include <windows.h>
#include "DXGameTimer.h"
//#include "WinMin.h"
using namespace std;

DXGameTimer::DXGameTimer()
{
	__int64 countsPerSec{};
	//QueryPerformanceFrequency：得到每个时钟周期的频率
	QueryPerformanceFrequency((LARGE_INTEGER*)&countsPerSec);
	m_SecondssPerCount = 1.0 / (double)countsPerSec;
}

//                     |<-- Paused Time -->|
// ----*---------------*-------------------*----------*------------*------> time
//  m_BaseTime       m_StopTime        startTime     m_StopTime    m_CurrTime

float DXGameTimer::GetTotalTime()const
{
	if (m_Stopped)
	{
		return (float)(((m_StopTime - m_PausedTime) - m_BaseTime) * m_SecondssPerCount);
	}
	else
	{
		return((float)((m_CurrTime - m_PausedTime) - m_BaseTime) * m_SecondssPerCount);
	}
}
float DXGameTimer::GetDeltaTime()const
{
	return (float)m_DeltaTime;
}
void DXGameTimer::Reset()
{
	__int64 currTime{};
	QueryPerformanceCounter((LARGE_INTEGER*)&currTime);

	m_BaseTime = currTime;
	m_PrevTime = currTime;
	m_StopTime = 0;
	m_PausedTime = 0;
	m_Stopped = false;
}

//                     |<---PausedTime-->|
// ----*---------------*-----------------*------------> time
//  m_BaseTime       m_StopTime        startTime     
void DXGameTimer::Start()
{
	__int64 startTime{};
	QueryPerformanceCounter((LARGE_INTEGER*)&startTime);

	if (m_Stopped)
	{
		m_PausedTime += (startTime - m_StopTime);
		m_PrevTime = startTime;
		m_StopTime = 0;
		m_Stopped = false;
	}
}

void DXGameTimer::Stop()
{
	if (!m_Stopped)
	{
		__int64 currTime{};

		QueryPerformanceCounter((LARGE_INTEGER*)&currTime);
		m_StopTime = currTime;
		m_Stopped = true;
	}
}

void DXGameTimer::GetTick()
{
	if (m_Stopped)
	{
		m_DeltaTime = 0.0;
		return;
	}
	__int64 currTime{};
	QueryPerformanceCounter((LARGE_INTEGER*)&currTime);
	m_CurrTime = currTime;
	m_DeltaTime = (m_CurrTime - m_PrevTime) * m_SecondssPerCount;
	m_PrevTime = m_CurrTime;
	if (m_DeltaTime < 0)
	{
		m_DeltaTime = 0;
	}
}

bool DXGameTimer::SetFrame(int frameCount)
{
	float frameTime = 1.0f / frameCount;
	if (m_Stopped)
	{
		m_DeltaTime = 0.0;
		return false;
	}
	__int64 currTime{};
	QueryPerformanceCounter((LARGE_INTEGER*)&currTime);
	m_CurrTime = currTime;
	m_DeltaTime = (m_CurrTime - m_PrevTime) * m_SecondssPerCount;
	if (m_DeltaTime <= frameTime)
	{
		return false;
	}
	m_PrevTime = m_CurrTime;
	if (m_DeltaTime < 0)
	{
		m_DeltaTime = 0;
	}
	return true;
}

bool DXGameTimer::IsStopped()const
{
	return m_Stopped;
}
bool  DXGameTimer::WaitForTime(long long time)
{
	this_thread::sleep_for(chrono::milliseconds(time));
	return true;
}