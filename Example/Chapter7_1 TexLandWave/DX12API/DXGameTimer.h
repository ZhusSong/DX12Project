#pragma once

#ifndef CPU_TIMER_H
#define CPU_TIMER_H
class DXGameTimer
{
public:
	DXGameTimer();
	//得到程序已运行的时间
	float GetTotalTime()const;
	//得到每一帧之间的时间差
	float GetDeltaTime()const;

	//重置计时
	void Reset();
	//开始计时
	void Start();
	//停止计时
	void Stop();
	//获取每一帧的时间差
	void GetTick();
	//设置帧数
	bool SetFrame(int frameCount);
	//是否在暂停中
	bool IsStopped()const;

	//wait for specified times (ms)
		//指定された時間（ミリ秒）を待つ
		//等待指定时间(毫秒)
	bool WaitForTime(long long);
private:
	//每一个时钟周期的时间
	double m_SecondssPerCount = 0.0;
	//每一帧的运行时间
	double m_DeltaTime = -1.0;
	//开始运行时的时间
	__int64 m_BaseTime = 0;
	//暂停时的时间
	__int64 m_PausedTime = 0;
	//停止时的时间
	__int64 m_StopTime = 0;
	//上一帧停止时的时间
	__int64 m_PrevTime = 0;
	//当前时间
	__int64 m_CurrTime = 0;
	//是否暂停
	bool m_Stopped = false;
};





#endif