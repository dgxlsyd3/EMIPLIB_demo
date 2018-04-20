// ExactTimer.cpp: implementation of the CExactTimer class.

#include "stdafx.h"
#include "ExactTimer.h"
#include "Utilities.h"

/**
* ���캯��������ȡCPU����Ƶ����ȷ��MHz
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

//CPU��Ƶ��
double CExactTimer::CpuFrequency()
{
	//On a multiprocessor machine, it should not matter which processor is called. 
	//However, you can get different results on different processors due to bugs in 
	//the BIOS or the HAL. To specify processor affinity for a thread, use the SetThreadAffinityMask function. 
	HANDLE hThread=GetCurrentThread();
	SetThreadAffinityMask(hThread,0x1);   

	//�����ϸ߾��ȶ�ʱ���ľ���Ƶ��
	//�����ʱ��Ӧ�þ���һƬ8253����8254
	//��intel ich7�м�����8254
	LARGE_INTEGER lFrequency;
	QueryPerformanceFrequency(&lFrequency);
	//printf("�߾��ȶ�ʱ���ľ���Ƶ�ʣ�%1.0fHz.\n",(double)lFrequency.QuadPart);

	//�����ʱ��ÿ����һ��ʱ�����ڣ����������+1
	LARGE_INTEGER lPerformanceCount_Start;
	QueryPerformanceCounter(&lPerformanceCount_Start);

	//RDTSCָ��:��ȡCPU������ʱ��������
	__int64 _i64StartCpuCounter=CExactTimer::RDTSC();

	//��ʱ��һ��,����Сһ��
	// 	int nTemp=100000;
	// 	while (--nTemp);
	Sleep(100);

	LARGE_INTEGER lPerformanceCount_End;
	QueryPerformanceCounter(&lPerformanceCount_End);

	__int64 _i64EndCpuCounter=CExactTimer::RDTSC();

	//f=1/T => f=��������/(��������*T)
	//����ġ���������*T������ʱ���
	double fTime=((double)lPerformanceCount_End.QuadPart-(double)lPerformanceCount_Start.QuadPart)
		/(double)lFrequency.QuadPart;

	return (_i64EndCpuCounter-_i64StartCpuCounter)/fTime;
}

unsigned __int64 CExactTimer::RDTSC()
{
	//RDTSC-Read Time-Stamp Counter
	//�Կ�������CPU������ʱ��������
	__asm
	{
		_emit 0x0F;
		_emit 0x31;
	}
}

/**
* ֹͣ��ʱ,��ȡ���ŵ�ʱ��,us
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
* ��ʾ���ŵ�ʱ��
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
* ��ʼ��ʱ
* @return void
* @see
*/
void CExactTimer::Start()
{
	m_i64StartCpuCounter = RDTSC();
}
