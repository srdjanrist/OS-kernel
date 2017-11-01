#ifndef _PCB_H_
#define _PCB_H_
#include <stdlib.h>


class Thread;
class Queue;


typedef unsigned long StackSize;
typedef unsigned int Time;
typedef int ID;

const StackSize idleStackSize = 128;

enum ThreadState{New, Ready, Blocked, Running, Finished, Main, Idle};

class PCB {

protected:
	friend void dispatch();
	friend class Thread;
	friend void interrupt timer();
	friend int main(int, char**);
	friend class KernelSem;
	friend class KernelEv;
	friend class Event;
	friend class Semaphore;
	friend class SleepQueue;

	friend class SysCall;
	friend void interrupt sys_call();

private:
	Thread* myThread;
	ID id;
	size_t* threadStack;
	StackSize stackSize;
	Time timeSlice;
	size_t ss, sp, bp;

	ThreadState threadState;

	Time timeToSleep;
	int finished;
	KernelSem* WTC;


	static int numAllPCB, numStarted;
	volatile static int numEventWaitingPCB;
	static Queue* all;
	static SleepQueue* sleeping;


public:
	PCB(Thread*, ID, StackSize, Time);
	static PCB* running;
	static PCB* mainThread;
	static PCB* idleThread;

private:
	~PCB();
	void initStack(StackSize);
	void start();
	static void stop();
	void waitToComplete();
	void sleep(Time);
	static void dispatch();
	inline void setState (ThreadState);
	inline ThreadState getState() const;

	void wakeWaiting();

	static void idleFunction();
	static void createIdleThread();
	static void resetIdle(StackSize);
};

#endif
