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
//����һЩ�߳�
bool MyThread:: IsCreatThread(long min,long max)
{
	// У�����
	if (min<=0||min>max)  return  false ;
	//�����ź���
	m_hsempfore=CreateSemaphore(NULL,0,max,NULL);
	//�������ɸ��߳�
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
//�����̳߳�
void  MyThread:: DestoryThreadPool()
{
	m_bFlagQuit=false;
	//�����̳߳�
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
//�̺߳���
unsigned __stdcall MyThread:: ThreadProc( void *lpvoid )
{
	MyThread * pthis=(MyThread*)lpvoid;
	ITask * task;
	while(pthis->m_bFlagQuit)
	{
		if(WAIT_TIMEOUT==WaitForSingleObject(pthis->m_hsempfore,100))  continue;
		pthis->m_lRunThreadnum++;
//һ���߳�ִ��������,�����������Ƿ�Ϊ��,�����Ϊ�ռ���ȥȡ����,�����л��ȴ��˷�ʱ��
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
			task->process();//�麯�����̺���
			 delete task;
			task=NULL;
		}
		pthis->m_lRunThreadnum--;
	}
	return 0;
}
//Ͷ������
bool MyThread:: PushITask(ITask * p)
{
	// ������񵽶���
	if(p==NULL)   return false ;
	//��������ͷ��ź���,
	//�з���Ա�еط�
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
		//���е��̶߳��Ѿ�����
		return false;
	}
	return true;
}