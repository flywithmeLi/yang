#include <iostream>
#include"MyThread.h"
class AA: public ITask
{
public:
  AA(int a,int b)
  {
	  m_a=a;
	  m_b=b;
  }
  virtual ~AA()
  {
  }
public:
  int m_a;
  int m_b;
public:
void process()
{
	cout<<m_a<<"+"<<m_b<<"="<<m_a+m_b<<endl;
}
};
int  main()
{
	MyThread mythread;
	mythread.IsCreatThread(1,1);
	for(int i=0;i<10;i++)
	{
		ITask *p=new AA(i,i+1);
		 mythread.PushITask(p);
	}
	
	system("pause");
    return 0;
}