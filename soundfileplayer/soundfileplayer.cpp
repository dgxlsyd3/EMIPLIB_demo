/**
 * \file soundfileplayer.cpp
 */

#include <mipconfig.h>
#include <mipcomponentchain.h>
#include <mipcomponent.h>
#include <miptime.h>
#include <mipaveragetimer.h>
#include <mipwavinput.h>
#include <mipsampleencoder.h>
#include <mipossinputoutput.h>
#include <mipwinmmoutput.h> 
#include <miprawaudiomessage.h> // Needed for MIPRAWAUDIOMESSAGE_TYPE_S16LE
#include <iostream>
#include <string>
#include <stdio.h>
#include <cstdlib>

using namespace std;

#ifdef _DEBUG
#pragma comment(lib,"emiplib_d.lib")
#pragma comment(lib,"jthread_d.lib")
#else
#pragma comment(lib,"emiplib.lib")
#pragma comment(lib,"jthread.lib")
#endif // _DEBUG
#pragma comment(lib,"Winmm.lib")


void checkError(bool returnValue, const MIPComponent &component)
{
	if (returnValue == true)
		return;

	std::cerr << "An error occured in component: " << component.getComponentName() << std::endl;
	std::cerr << "Error description: " << component.getErrorString() << std::endl;

	exit(-1);
}

void checkError(bool returnValue, const MIPComponentChain &chain)
{
	if (returnValue == true)
		return;

	std::cerr << "An error occured in chain: " << chain.getName() << std::endl;
	std::cerr << "Error description: " << chain.getErrorString() << std::endl;

	exit(-1);
}

class MyChain : public MIPComponentChain
{
public:
	MyChain(const std::string &chainName) : MIPComponentChain(chainName)
	{
	}
private:
	void onThreadExit(bool error, const std::string &errorComponent, const std::string &errorDescription)
	{
		if (!error)
			return;

		std::cerr << "An error occured in the background thread." << std::endl;
		std::cerr << "    Component: " << errorComponent << std::endl;
		std::cerr << "    Error description: " << errorDescription << std::endl;
	}	
};

int main(void)
{
	//�൱��CTimeSpan
	MIPTime interval(0.020);

	//��ʱ�����
	MIPAverageTimer timer(interval);
	
	MyChain chain("wav�ļ�������");

	//wav�ļ��������
	MIPWAVInput sndFileInput;
	bool returnValue = sndFileInput.open("E:\\MyDoc\\ͼƬ�ղ�\\2009��04��24�ջؼҽ��\\�������\\����01(�����ɴ���)\\����\\Paris.wav", interval);
	checkError(returnValue, sndFileInput);
	
	int samplingRate = sndFileInput.getSamplingRate();
	int numChannels = sndFileInput.getNumberOfChannels();
	cout<<"����Ƶ��="<<samplingRate<<endl;
	cout<<"ͨ������="<<numChannels<<endl;
	cout<<"  ��֡��="<<sndFileInput.getNumberOfFrames()<<endl;

	//С��16λ�����������
	//accepts all raw audio messages and produces similar raw audio messages
	MIPSampleEncoder sampEnc;
	returnValue = sampEnc.init(MIPRAWAUDIOMESSAGE_TYPE_S16LE);
	checkError(returnValue, sampEnc);

	//Win32/WinCE������������
	//accept raw audio messages using signed 16 bit little endian encoding.
	MIPWinMMOutput sndCardOutput;
	returnValue = sndCardOutput.open(samplingRate, numChannels, interval);
	checkError(returnValue, sndCardOutput);

	//������
	//����˳��Ϊ��timer -> sndFileInput -> sampEnc -> sndCardOutput
	returnValue = chain.setChainStart(&timer);
	checkError(returnValue, chain);

	returnValue = chain.addConnection(&timer, &sndFileInput);
	checkError(returnValue, chain);

	returnValue = chain.addConnection(&sndFileInput, &sampEnc);
	checkError(returnValue, chain);

	returnValue = chain.addConnection(&sampEnc, &sndCardOutput);
	checkError(returnValue, chain);

	returnValue = chain.start();
	checkError(returnValue, chain);

	getc(stdin);

	returnValue = chain.stop();
	checkError(returnValue, chain);

	return 0;
}

