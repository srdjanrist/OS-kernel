#ifndef _SYSCALL_H_
#define _SYSCALL_H_

typedef unsigned long StackSize;
typedef unsigned int Time;
typedef int ID;

void initIVT();
void interrupt IR();

class SysCall{
	friend void interrupt IR();

public:
	//Thread
	static void CThread();
	static void start();
	static void waitToComplete();
	static void sleep();
	static void DThread();

	///Semaphore
	static void CSemaphore();
	static void DSemaphore();
	static void wait();
	static void signal();
	static void val();


	///Event
	static void CEvent();
	static void DEvent();
	static void eWait();
	static void eSignal();


public:
	static void dispatch();

};


#endif
