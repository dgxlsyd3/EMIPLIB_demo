/**
* \file feedbackexample.cpp
*/

#include <mipconfig.h>

#include <mipcomponentchain.h>
#include <mipcomponent.h>
#include <miptime.h>
#include <mipaveragetimer.h>
#include <mipwavinput.h>
#include <mipsamplingrateconverter.h>
#include <mipsampleencoder.h>
#include <mipwinmmoutput.h>
#include <mipulawencoder.h>
#include <miprtpulawencoder.h>
#include <miprtpcomponent.h>
#include <miprtpdecoder.h>
#include <miprtpulawdecoder.h>
#include <mipulawdecoder.h>
#include <mipaudiomixer.h>
#include <miprawaudiomessage.h> // Needed for MIPRAWAUDIOMESSAGE_TYPE_S16LE etc
#include <rtpsession.h>
#include <rtpsessionparams.h>
#include <rtpipv4address.h>
#include <rtpudpv4transmitter.h>
#include <rtperrors.h>
#include <stdio.h>
#include <iostream>
#include <string>

#include <mipwinmminput.h>
#include <miprtpspeexencoder.h>
#include <mipspeexencoder.h>

#include <miprtpspeexdecoder.h>
#include <mipspeexdecoder.h>
#include <mipmediabuffer.h>
#include <mipencodedaudiomessage.h>

#pragma comment(lib,"Winmm.lib")
#pragma comment(lib,"Ws2_32.lib")

#ifdef _DEBUG
#pragma comment(lib,"jthread_d.lib")
#pragma comment(lib,"jrtplib_d.lib")
#pragma comment(lib,"emiplib_d.lib")
#else
#pragma comment(lib,"jthread.lib")
#pragma comment(lib,"jrtplib.lib")
#pragma comment(lib,"emiplib.lib")
#endif // _DEBUG



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

// We'll be using an RTPSession instance from the JRTPLIB library. The following
// function checks the JRTPLIB error code.
void checkError(int status)
{
	if (status >= 0)
		return;

	std::cerr << "An error occured in the RTP component: " << std::endl;
	std::cerr << "Error description: " << RTPGetErrorString(status) << std::endl;

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
	WSADATA dat;
	WSAStartup(MAKEWORD(2,2),&dat);

	MyChain chain("TestChain");	

	/*
	MIPTime interval(0.020);
	MIPAverageTimer timer(interval);
	returnValue = chain.setChainStart(&timer);
	checkError(returnValue, chain);

	//打开一个wav文件
	MIPWAVInput mipWAVInput;
	bool returnValue = mipWAVInput.open("C:\\WINDOWS\\Media\\Windows XP 注销音.wav", interval);
	checkError(returnValue, mipWAVInput);
	returnValue = chain.addConnection(&timer, &mipWAVInput);
	checkError(returnValue, chain);

	//采样频率转换器
	int samplingRate = 8000;
	int numChannels = 1;
	MIPSamplingRateConverter mipSamplingRateConverter;
	returnValue = mipSamplingRateConverter.init(samplingRate, numChannels);
	checkError(returnValue, mipSamplingRateConverter);
	returnValue = chain.addConnection(&mipWAVInput, &mipSamplingRateConverter);
	checkError(returnValue, chain);

	//重采样编码器
	MIPSampleEncoder mipSampleEncoder;
	returnValue = mipSampleEncoder.init(MIPRAWAUDIOMESSAGE_TYPE_S16);
	checkError(returnValue, mipSampleEncoder);

	returnValue = chain.addConnection(&mipSamplingRateConverter, &mipSampleEncoder);
	checkError(returnValue, chain);
	*/

	MIPTime interval(0.020);
	MIPWinMMInput Input;

	int samplingRate = 8000;
	int numChannels = 1;
	bool returnValue=Input.open(44100, numChannels, interval, MIPTime(10.0), true);
	checkError(returnValue, chain);
	returnValue = chain.setChainStart(&Input);
	checkError(returnValue, chain);

	MIPSampleEncoder mipSampleEncoder;//重采样编码器
	returnValue = mipSampleEncoder.init(MIPRAWAUDIOMESSAGE_TYPE_FLOAT);
	checkError(returnValue, mipSampleEncoder);
	returnValue = chain.addConnection(&Input, &mipSampleEncoder);
	checkError(returnValue, chain);

	MIPSamplingRateConverter mipSamplingRateConverterSend;
	returnValue = mipSamplingRateConverterSend.init(8000, numChannels);
	checkError(returnValue, mipSamplingRateConverterSend);
	returnValue = chain.addConnection(&mipSampleEncoder, &mipSamplingRateConverterSend, true);
	checkError(returnValue, chain);

	MIPSampleEncoder mipSampleEncoder2;//重采样编码器
	returnValue = mipSampleEncoder2.init(MIPRAWAUDIOMESSAGE_TYPE_S16);
	checkError(returnValue, mipSampleEncoder2);
	returnValue = chain.addConnection(&mipSamplingRateConverterSend, &mipSampleEncoder2);
	checkError(returnValue, chain);
	
	MIPULawEncoder mipULawEncoder;//U-law编码器
	returnValue = mipULawEncoder.init();
	checkError(returnValue, mipULawEncoder);
	returnValue = chain.addConnection(&mipSampleEncoder2, &mipULawEncoder);
	checkError(returnValue, chain);

	MIPRTPULawEncoder mipRTPULawEncoder;//U-law编码的RTP音频数据包编码器
	returnValue = mipRTPULawEncoder.init();
	checkError(returnValue, mipRTPULawEncoder);
	returnValue = chain.addConnection(&mipULawEncoder, &mipRTPULawEncoder);
	checkError(returnValue, chain);

	int portBase = 60000;
	RTPSessionParams sessionParams;//UDP会话参数
	sessionParams.SetOwnTimestampUnit(1.0/((double)samplingRate));
	sessionParams.SetMaximumPacketSize(64000);
	sessionParams.SetAcceptOwnPackets(true);

	RTPUDPv4TransmissionParams transmissionParams;//UDP传输参数
	transmissionParams.SetPortbase(portBase);
	RTPSession rtpSession;
	int status = rtpSession.Create(sessionParams,&transmissionParams);
	checkError(status);
	RTPIPv4Address ipv4Addr(ntohl(inet_addr("127.0.0.1")),transmissionParams.GetPortbase());//发送UDP数据包到127.0.0.1
	status = rtpSession.AddDestination(ipv4Addr);
	checkError(status);
	
	MIPRTPComponent mipRTPComponent;//发送与接收RTP消息的组件
	returnValue = mipRTPComponent.init(&rtpSession);
	checkError(returnValue, mipRTPComponent);
	returnValue = chain.addConnection(&mipRTPULawEncoder, &mipRTPComponent);
	checkError(returnValue, chain);

	MIPRTPDecoder mipRTPDecoder;//RTP音频解码器
	returnValue = mipRTPDecoder.init(true, 0, &rtpSession);
	checkError(returnValue, mipRTPDecoder);
	MIPRTPULawDecoder mipRTPULawDecoder;//将RTP消息中的U-law数据解码出来
	returnValue = mipRTPDecoder.setPacketDecoder(0,&mipRTPULawDecoder);
	checkError(returnValue, mipRTPDecoder);
	returnValue = chain.addConnection(&mipRTPComponent, &mipRTPDecoder);
	checkError(returnValue, chain);

	MIPULawDecoder mipULawDecoder;//u-law解码器
	returnValue = mipULawDecoder.init();
	checkError(returnValue, mipULawDecoder);
	returnValue = chain.addConnection(&mipRTPDecoder, &mipULawDecoder, true);
	checkError(returnValue, chain);

	MIPSampleEncoder  mipSampleEncoderRecv;//将接收到的音频数据转换为浮点格式
	returnValue = mipSampleEncoderRecv.init(MIPRAWAUDIOMESSAGE_TYPE_FLOAT);
	checkError(returnValue, mipSampleEncoderRecv);
	returnValue = chain.addConnection(&mipULawDecoder, &mipSampleEncoderRecv, true);
	checkError(returnValue, chain);

	MIPSamplingRateConverter mipSamplingRateConverterRecv;
	returnValue = mipSamplingRateConverterRecv.init(samplingRate, numChannels);
	checkError(returnValue, mipSamplingRateConverterRecv);
	returnValue = chain.addConnection(&mipSampleEncoderRecv, &mipSamplingRateConverterRecv, true);
	checkError(returnValue, chain);

	MIPAudioMixer mipAudioMixer;//初始化混频器
	returnValue = mipAudioMixer.init(samplingRate, numChannels, interval);
	checkError(returnValue, mipAudioMixer);
	returnValue = chain.addConnection(&mipSamplingRateConverterRecv, &mipAudioMixer, true);
	checkError(returnValue, chain);

	MIPSampleEncoder  mipSampleEncoderOut;
	returnValue = mipSampleEncoderOut.init(MIPRAWAUDIOMESSAGE_TYPE_S16LE);
	checkError(returnValue, mipSampleEncoderOut);
	returnValue = chain.addConnection(&mipAudioMixer, &mipSampleEncoderOut);
	checkError(returnValue, chain);

	MIPWinMMOutput mipWinMMOutput;//声卡
	returnValue = mipWinMMOutput.open(samplingRate, numChannels, interval);
	checkError(returnValue, mipWinMMOutput);
	returnValue = chain.addConnection(&mipSampleEncoderOut, &mipWinMMOutput);
	checkError(returnValue, chain);

	returnValue = chain.start();
	checkError(returnValue, chain);

	getc(stdin);

	returnValue = chain.stop();
	checkError(returnValue, chain);

	rtpSession.Destroy();

	// We'll let the destructors of the other components take care
	// of their de-initialization.
	WSACleanup();

	return 0;
}

