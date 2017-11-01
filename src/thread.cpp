#include "thread.h"

#include "PCB.h"
#include "lock.h"
#include <dos.h>
#include <stdlib.h>


struct newThreadRequest{
	StackSize stackSize;
	Time timeSlice;
	Thread* running;
	newThreadRequest(StackSize sz, Time ts, Thread* t) : stackSize(sz), timeSlice(ts), running(t) {}
};

Thread::Thread(StackSize stackSize, Time timeSlice){
	lock
	newThreadRequest* param = new newThreadRequest(stackSize, timeSlice, this);
	unlock
	size_t off = FP_OFF(param);
	size_t segm = FP_SEG(param);
	size_t temp;
	asm{
		mov ax, 0
		mov bx, off
		mov cx, segm
		int 60h
		mov temp, ax
	}
	this->id = temp;
}

void Thread::start(){
	ID id = this->id;
	asm{
		mov ax, 1
		mov bx, id
		int 60h
	}
}

void Thread::waitToComplete(){
	ID id = this->id;
	asm{
		mov ax, 2
		mov cx, id
		int 60h
	}
}

void Thread::sleep(Time timeToSleep){
	asm{
		mov ax, 3
		mov cx, timeToSleep
		int 60h
	}
}

Thread::~Thread(){
	this->waitToComplete();
	ID id = this->id;
	asm{
		mov ax, 4
		mov cx, id
		int 60h
	}
}

void Thread::run(){  }

void Thread::wrapper(Thread* running){
	running->run();
}
void dispatch(){
	asm{
		mov ax, 5
		int 60h
	}
}
