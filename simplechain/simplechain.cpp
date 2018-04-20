/**
* \file simplechain.cpp
*/

#include <mipcomponentchain.h>
#include <mipcomponent.h>
#include <miptime.h>
#include <mipaveragetimer.h>
#include <mipmessagedumper.h>
#include <iostream>
#include <string>
// #include <unistd.h>
#include <cstdlib>
#ifdef _DEBUG
#pragma comment(lib,"emiplib_d.lib")
#pragma comment(lib,"jthread_d.lib")
#else
#pragma comment(lib,"emiplib.lib")
#pragma comment(lib,"jthread.lib")
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
	//0.5��
	MIPTime time(0.5);

	//��ʱ�����
	MIPAverageTimer timer(time);

	//��ϢDump���
	MIPMessageDumper msgDump;

	//������Ҳ��һ�����
	//����һϵ�е�MIPComponent�������������������MIPMessage��Ϣ
	MyChain chain("��ʾ��");

	//�������ӵ���
	//�����������
	bool returnValue = chain.setChainStart(&timer);
	checkError(returnValue, chain);

	//addConnection(MIPComponent *pPullComponent, MIPComponent *pPushCompontent,...
	//pPullComponent ->  pPushCompontent 
	returnValue = chain.addConnection(&timer, &msgDump);
	checkError(returnValue, chain);

	//���������
	returnValue = chain.start();
	checkError(returnValue, chain);

	//�ȴ�5��
	MIPTime t(5.0);
	MIPTime::wait(t);

	//ֹͣ��̨�߳�
	returnValue = chain.stop();
	checkError(returnValue, chain);

	return 0;
}

