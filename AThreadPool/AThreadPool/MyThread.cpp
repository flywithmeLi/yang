#include "MyThread.h"


MyThread::MyThread(void)
{
	m_bFlagQuit=true;
	m_lCreatThreadnum=0;
	m_lRunThreadnum=0;
	m_lMaxThreadnum=0;
}


MyThread::~MyThread(void)
{
}
//创建一些线程
bool MyThread:: IsCreatThread(long min,long max)
{
	// 校验参数
	if (min<=0||min>max)  return  false ;
	//创建信号量
	m_hsempfore=CreateSemaphore(NULL,0,max,NULL);
	//创建若干个线程
	for(int i=0;i<min;i++)
	{
		HANDLE m_hThread=(HANDLE)_beginthreadex(NULL,NULL,&ThreadProc,this,NULL,NULL);
		if(m_hThread)
			m_lstHandle.push_back(m_hThread);
	}
	m_lCreatThreadnum=min;
	m_lMaxThreadnum=max;
	return true ;
}
//销毁线程池
void  MyThread:: DestoryThreadPool()
{
	m_bFlagQuit=false;
	//销毁线程池
	auto ite=m_lstHandle.begin();
	while(ite!=m_lstHandle.end())
	{
		if(WAIT_TIMEOUT==WaitForSingleObject(&ite,100))
			TerminateThread(*ite,-1);
		CloseHandle(*ite);
		*ite=NULL;
		ite++;
	}
	if(m_hsempfore)
	{
		CloseHandle(m_hsempfore);
		m_hsempfore=NULL;
	}
	ITask * ptask=NULL;
	while(m_quTask.empty()!=NULL)
   {
	   ptask=m_quTask.front();
	   m_quTask.pop();
	   delete ptask;
	   ptask=NULL;
   }
}
//线程函数
unsigned __stdcall MyThread:: ThreadProc( void *lpvoid )
{
	MyThread * pthis=(MyThread*)lpvoid;
	ITask * task;
	while(pthis->m_bFlagQuit)
	{
		if(WAIT_TIMEOUT==WaitForSingleObject(pthis->m_hsempfore,100))  continue;
		pthis->m_lRunThreadnum++;
//一根线程执行完任务,检查任务队列是否为空,如果不为空继续去取任务,来回切换等待浪费时间
//		while(!pthis->m_quTask.empty())
//		while(pthis->m_quTask.empty()!=NULL)
	while(pthis->m_quTask.empty()!=true)
		{
			pthis->locking.lock();
			if(pthis->m_quTask.empty()==true)
			{
			pthis->locking.unlock();
			continue;
			}
			task=pthis->m_quTask.front();
			pthis->m_quTask.pop();
			pthis->locking.unlock();
			task->process();//虚函数进程函数
			 delete task;
			task=NULL;
		}
		pthis->m_lRunThreadnum--;
	}
	return 0;
}
//投递任务
bool MyThread:: PushITask(ITask * p)
{
	// 添加任务到队列
	if(p==NULL)   return false ;
	//添加任务释放信号量,
	//有服务员有地方
	if(m_lCreatThreadnum>m_lRunThreadnum)
	{
		locking.lock();
		m_quTask.push(p);
		locking.unlock();
		ReleaseSemaphore(m_hsempfore,1,NULL);
	}
	else if(m_lCreatThreadnum<m_lMaxThreadnum)
	{
		HANDLE m_hThread=(HANDLE)_beginthreadex(NULL,NULL,&ThreadProc,this,NULL,NULL);
		if(m_hThread)
			m_lstHandle.push_back(m_hThread);
		locking.lock();
		m_quTask.push(p);
		locking.unlock();
		m_lCreatThreadnum++;
		ReleaseSemaphore(m_hsempfore,1,NULL);
	}
	else
	{
		//所有的线程都已经工作
		return false;
	}
	return true;
}