// ExactTimer.cpp: implementation of the CExactTimer class.

#include "stdafx.h"
#include "ExactTimer.h"
#include "Utilities.h"

/**
* 构造函数：将获取CPU的主频，精确到MHz
* @return
* @see
*/
CExactTimer::CExactTimer()
{
	DWORD dwFrequency=0;

	if (CUtilities::GetRegValue(
		HKEY_LOCAL_MACHINE,
		"HARDWARE\\DESCRIPTION\\System\\CentralProcessor\\0",
		"~MHz",dwFrequency))
	{
		m_fFrequencyMHz = dwFrequency*1.0;	
	}
	else
	{
		m_fFrequencyMHz=CpuFrequency();
	}
}

CExactTimer::~CExactTimer()
{

}

//CPU的频率
double CExactTimer::CpuFrequency()
{
	//On a multiprocessor machine, it should not matter which processor is called. 
	//However, you can get different results on different processors due to bugs in 
	//the BIOS or the HAL. To specify processor affinity for a thread, use the SetThreadAffinityMask function. 
	HANDLE hThread=GetCurrentThread();
	SetThreadAffinityMask(hThread,0x1);   

	//主板上高精度定时器的晶振频率
	//这个定时器应该就是一片8253或者8254
	//在intel ich7中集成了8254
	LARGE_INTEGER lFrequency;
	QueryPerformanceFrequency(&lFrequency);
	//printf("高精度定时器的晶振频率：%1.0fHz.\n",(double)lFrequency.QuadPart);

	//这个定时器每经过一个时钟周期，其计数器会+1
	LARGE_INTEGER lPerformanceCount_Start;
	QueryPerformanceCounter(&lPerformanceCount_Start);

	//RDTSC指令:获取CPU经历的时钟周期数
	__int64 _i64StartCpuCounter=CExactTimer::RDTSC();

	//延时长一点,误差会小一点
	// 	int nTemp=100000;
	// 	while (--nTemp);
	Sleep(100);

	LARGE_INTEGER lPerformanceCount_End;
	QueryPerformanceCounter(&lPerformanceCount_End);

	__int64 _i64EndCpuCounter=CExactTimer::RDTSC();

	//f=1/T => f=计数次数/(计数次数*T)
	//这里的“计数次数*T”就是时间差
	double fTime=((double)lPerformanceCount_End.QuadPart-(double)lPerformanceCount_Start.QuadPart)
		/(double)lFrequency.QuadPart;

	return (_i64EndCpuCounter-_i64StartCpuCounter)/fTime;
}

unsigned __int64 CExactTimer::RDTSC()
{
	//RDTSC-Read Time-Stamp Counter
	//自开机以来CPU经历的时钟周期数
	__asm
	{
		_emit 0x0F;
		_emit 0x31;
	}
}

/**
* 停止计时,获取流逝的时间,us
* f=1/T=n/(n*T) => (n*T)=n/f
* 
* @return double
* @see 
*/
double CExactTimer::GetTimePassage(UINT unit)
{
	m_i64EndCpuCounter = RDTSC();
	double fTime=(m_i64EndCpuCounter -m_i64StartCpuCounter) / m_fFrequencyMHz;
	switch (unit)
	{
	case unitMicroseconds:
		return fTime;//us
	case unitMilliseconds:
		return (fTime / 1000.0);//ms
	case unitSeconds:
		return (fTime / (1000 * 1000.0));//s
	default:
		return fTime;//us
	}
}

/**
* 显示流逝的时间
* @return double
* @see
*/
void CExactTimer::ShowTimePassage(UINT unit)
{
	double fValue = 0.0;
	char szUnit[5];
	switch (unit)
	{
	case unitMicroseconds:
		{
			strcpy_s(szUnit, "us");
			break;
		}
	case unitMilliseconds:
		{
			strcpy_s(szUnit, "ms");
			break;
		}
	case unitSeconds:
		{
			strcpy_s(szUnit, "s");
			break;
		}
	default:
		strcpy_s(szUnit, "us");
		break;
	}

	TCHAR szText[250];
	_stprintf_s(szText,"TimePassage:%1.6f %s\n", GetTimePassage(unit), szUnit);
	OutputDebugString(szText);
}

/**
* 开始计时
* @return void
* @see
*/
void CExactTimer::Start()
{
	m_i64StartCpuCounter = RDTSC();
}
