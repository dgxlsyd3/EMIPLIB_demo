#include <mipconfig.h>

#include <mipcomponentchain.h>
#include <mipcomponent.h>
#include <miptime.h>
#include <mipwavoutput.h>
#include <mipwinmminput.h>
#include <miprawaudiomessage.h> // Needed for MIPRAWAUDIOMESSAGE_TYPE_S16LE
#include <mipsampleencoder.h>
#include <iostream>
#include <string>
#include <stdio.h>

#ifdef _DEBUG
	#pragma comment(lib,"emiplib_d.lib")
	#pragma comment(lib,"jthread_d.lib")
#else
	#pragma comment(lib,"emiplib.lib")
	#pragma comment(lib,"jthread.lib")
#endif // _DEBUG

#pragma comment(lib,"Winmm.lib")

int main()
{
	MIPTime interval(0.200);

	//Win32/WinCE 声卡输入组件，他应该用在链的开始位置
	//16 bit signed little endian data
	MIPWinMMInput Input;
	if (!Input.open(/*sampRate*/44100, /*channels*/1, interval, MIPTime(10.0), true)) 
	{
		return -1;
	}

	MIPSampleEncoder SampEnc;
	if (!SampEnc.init(MIPRAWAUDIOMESSAGE_TYPE_U8))
	{
		return -2;
	}

	MIPWAVOutput Output;
	if (!Output.open("sound.wav", /*sampRate*/44100))
	{
		return -3;
	}

	MIPComponentChain chain("录音链");

	chain.setChainStart(&Input);
	chain.addConnection(&Input, &SampEnc);
	chain.addConnection(&SampEnc, &Output);

	if (!chain.start())
	{
		return -4;
	}

	getchar();

	chain.stop();

	return 0;
}