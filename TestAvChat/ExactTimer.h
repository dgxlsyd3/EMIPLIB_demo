// ExactTimer.h
#pragma once

#pragma warning(disable:4035)

class CExactTimer  
{
public:
	enum TimerUint
	{
		unitMicroseconds	= 0,//us
		unitMilliseconds	= 1,//ms
		unitSeconds			= 2//s
	};

private:
	__int64 m_i64StartCpuCounter;
	__int64 m_i64EndCpuCounter;
	double  m_fFrequencyMHz;

public:
	CExactTimer();
	virtual ~CExactTimer();

	static double CpuFrequency();
	static unsigned __int64 RDTSC();

public:
	void Start();
	void ShowTimePassage(UINT unit = unitMilliseconds);
	double GetTimePassage(UINT unit = unitMilliseconds);
};
