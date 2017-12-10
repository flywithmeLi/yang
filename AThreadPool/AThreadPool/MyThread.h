#pragma once
#include <process.h>
#include <windows.h>
#include<list>
#include<queue>
using namespace std;
class CLock
{
public:
	CLock()
	{
		InitializeCriticalSection(&m_cs);
	}
	~CLock()
	{
		DeleteCriticalSection(&m_cs);
	}
public:
	void  lock()
	{
		EnterCriticalSection(&m_cs);
	}
	void unlock()
	{
		LeaveCriticalSection(&m_cs);
	}
private:
	CRITICAL_SECTION m_cs;

};
class ITask
{
public :
	ITask()
	{}
	virtual~ITask()
	{}
public:
	virtual  void process()=0;

};
class MyThread
{
public:
	MyThread(void);
	virtual~MyThread(void);//���еĸ��༸������������
public:
	//����һЩ�߳�
	bool IsCreatThread(long min,long max);
	//�����̳߳�
	void  DestoryThreadPool();
    //�̺߳���
	static unsigned __stdcall ThreadProc( void * );
	//Ͷ������
	bool PushITask(ITask * p);
public :
	queue<ITask *>  m_quTask;
	list<HANDLE > m_lstHandle;
	//�ź���
    HANDLE  m_hsempfore;
	bool m_bFlagQuit;
	long m_lCreatThreadnum;
	volatile long m_lRunThreadnum;   //ԭ����,�̺߳����ﴦ��ı���,Ӧ�������߳�ͬʱ����ͬһ������
	long m_lMaxThreadnum;
    CLock  locking;//������
};

