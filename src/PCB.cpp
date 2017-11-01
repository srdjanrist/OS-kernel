#include "PCB.h"

#include "thread.h"
#include "schedule.h"
#include "lock.h"
#include "semaphor.h"
#include "kersem.h"
#include <dos.h>

#include "queue.h"
#include "sleep.h"


PCB* PCB::running = 0;
PCB* PCB::mainThread = 0;
PCB* PCB::idleThread = 0;

int PCB::numAllPCB = 0;
int PCB::numStarted = 0;

volatile int PCB::numEventWaitingPCB = 0;


Queue* PCB::all = new Queue();
SleepQueue* PCB::sleeping = new SleepQueue();

extern volatile int requestedContextChange;
extern void interrupt timer();

PCB::PCB(Thread* running, ID id, StackSize stackSize, Time timeSlice){
	this->id = id;
	this->stackSize = stackSize;
	this->timeSlice = timeSlice;
	this->myThread = running;
	this->WTC = new KernelSem(-1, 0);
	finished = 0;
	numAllPCB++;
	this->setState(New);
	all->put(this);
}

PCB::~PCB(){
	all->remove(this);
}

void PCB::initStack(StackSize sz){
	threadStack = new size_t [sz];
	threadStack[sz-1] = FP_SEG(myThread);
	threadStack[sz-2] = FP_OFF(myThread);
	threadStack[sz-3] = FP_SEG(PCB::stop);
	threadStack[sz-4] = FP_OFF(PCB::stop);
	threadStack[sz-5] = 0x200;
	threadStack[sz-6] = FP_SEG (Thread::wrapper);
	threadStack[sz-7] = FP_OFF (Thread::wrapper);
	ss = FP_SEG(threadStack+sz-16);
	sp = FP_OFF(threadStack+sz-16);
	bp = FP_OFF(threadStack+sz-16);
}

void PCB::start(){
	if (this->getState() == New){
		initStack(this->stackSize);
		Scheduler::put((PCB*)this);
		numStarted++;
		this->setState(Ready);
	}
}

void PCB::waitToComplete(){
	if (this == PCB::running) return;
	if (this->getState() != Finished && this->getState() != New){
		this->WTC->wait();
	}
}

void PCB::sleep(Time timeToSleep){
	numStarted--;
	this->timeToSleep = timeToSleep;
	sleeping->put(this, timeToSleep);
	setState(Blocked);
}

void PCB::dispatch(){
	requestedContextChange = 1;
	timer();
}

inline void PCB::setState(ThreadState ts){
	threadState = ts;
}

inline ThreadState PCB::getState() const {
	return threadState;
}

void PCB::stop (){
	lock
	numStarted--;
	numAllPCB--;
	running->finished = 1;
	running->setState(Finished);
	running->wakeWaiting();
	unlock
	PCB::dispatch();
}

void PCB::wakeWaiting(){
	while (running->WTC->val() < 0){
		running->WTC->signal();
	}
}

void PCB::idleFunction(){
	while (sleeping->numSleeping || PCB::numEventWaitingPCB){
	}
	dispatch();
}

void PCB::createIdleThread(){
	StackSize sz = idleStackSize;
	idleThread = new PCB (0, 0, sz, 0);
	idleThread->threadStack = new size_t [sz];
	idleThread->setState(Idle);

	resetIdle(sz);
}

void PCB::resetIdle(StackSize sz){
	idleThread->threadStack[sz-1] = 0x200;
	idleThread->threadStack[sz-2] = FP_SEG (idleFunction);
	idleThread->threadStack[sz-3] = FP_OFF (idleFunction);
	idleThread->ss = FP_SEG(idleThread->threadStack+sz-12);
	idleThread->sp = FP_OFF(idleThread->threadStack+sz-12);
	idleThread->bp = FP_OFF(idleThread->threadStack+sz-12);
}
