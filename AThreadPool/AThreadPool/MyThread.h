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
	virtual~MyThread(void);//所有的父类几乎都是虚析构
public:
	//创建一些线程
	bool IsCreatThread(long min,long max);
	//销毁线程池
	void  DestoryThreadPool();
    //线程函数
	static unsigned __stdcall ThreadProc( void * );
	//投递任务
	bool PushITask(ITask * p);
public :
	queue<ITask *>  m_quTask;
	list<HANDLE > m_lstHandle;
	//信号量
    HANDLE  m_hsempfore;
	bool m_bFlagQuit;
	long m_lCreatThreadnum;
	volatile long m_lRunThreadnum;   //原子锁,线程函数里处理的变量,应避免多跟线程同时处理同一个变量
	long m_lMaxThreadnum;
    CLock  locking;//定义锁
};

