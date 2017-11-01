#include "syscall.h"

#include "pcb.h"
#include "kersem.h"
#include "kernelev.h"
#include "schedule.h"
#include "lock.h"
#include "avl.h"
#include "thread.h"
#include "sleep.h"

#include <stdlib.h>
#include <dos.h>


const StackSize defaultSize = 1024;


volatile size_t ttsp, ttss, ttbp;
volatile size_t osp, oss, obp;
volatile size_t param0, param1, param2, param3;
volatile size_t seg, off;

ID ThreadID = 0;
ID SemaphoreID = 0;
ID EventID = 0;

AVL<PCB*>* allPCB = new AVL<PCB*>();
AVL<KernelSem*>* allSem = new AVL<KernelSem*>();
AVL<KernelEv*>* allEv = new AVL<KernelEv*> ();

size_t* stack = new size_t[defaultSize];

int kernelActive = 0;

extern int count;
extern int mutex_glb;

volatile int dispatchFlag = 0;

enum Operation {opCThread, opstart, opwaitToComplete, opsleep, opDThread, opdispatch, opCSemaphore,
				opDSemaphore, opwait, opsignal, opval, opCEvent, opDEvent, opeWait, opeSignal};

volatile Operation op;


void interrupt sys_call(){
	if (!kernelActive){
		asm {
			mov param0, ax
			mov param1, bx
			mov param2, cx
			mov param3, dx

			mov ttsp, sp
			mov ttss, ss
			mov ttbp, bp
		}
		lock
		asm sti
		PCB::running->sp = ttsp;
		PCB::running->ss = ttss;
		PCB::running->bp = ttbp;
	}

	if (!kernelActive && !dispatchFlag){
		op = param0;

		switch (op){

		case opCThread:
			seg = FP_SEG(SysCall::CThread);
			off = FP_OFF(SysCall::CThread);
		break;

		case opstart:
			seg = FP_SEG(SysCall::start);
			off = FP_OFF(SysCall::start);
		break;

		case opwaitToComplete:
			seg = FP_SEG(SysCall::waitToComplete);
			off = FP_OFF(SysCall::waitToComplete);
		break;

		case opsleep:
			seg = FP_SEG(SysCall::sleep);
			off = FP_OFF(SysCall::sleep);
		break;

		case opDThread:
			seg = FP_SEG(SysCall::DThread);
			off = FP_OFF(SysCall::DThread);
		break;

		case opdispatch:
			dispatchFlag = 1;
			kernelActive = 1;
		break;

		case opCSemaphore:
			seg = FP_SEG(SysCall::CSemaphore);
			off = FP_OFF(SysCall::CSemaphore);
		break;

		case opDSemaphore:
			seg = FP_SEG(SysCall::DSemaphore);
			off = FP_OFF(SysCall::DSemaphore);
		break;

		case opwait:
			seg = FP_SEG(SysCall::wait);
			off = FP_OFF(SysCall::wait);
		break;

		case opsignal:
			seg = FP_SEG(SysCall::signal);
			off = FP_OFF(SysCall::signal);
		break;

		case opval:
			seg = FP_SEG(SysCall::val);
			off = FP_OFF(SysCall::val);
		break;

		case opCEvent:
			seg = FP_SEG(SysCall::CEvent);
			off = FP_OFF(SysCall::CEvent);
		break;

		case opDEvent:
			seg = FP_SEG(SysCall::DEvent);
			off = FP_OFF(SysCall::DEvent);
		break;

		case opeWait:
			seg = FP_SEG(SysCall::eWait);
			off = FP_OFF(SysCall::eWait);
		break;

		case opeSignal:
			seg = FP_SEG(SysCall::eSignal);
			off = FP_OFF(SysCall::eSignal);
		break;
		}
	}


	if (!kernelActive && !dispatchFlag){
		oss = ttss;
		osp = ttsp;
		obp = ttbp;
		stack[defaultSize-1] = 0x200;
		stack[defaultSize-2] = seg;
		stack[defaultSize-3] = off;
		ttss = FP_SEG(stack+defaultSize-12);
		ttsp = FP_OFF(stack+defaultSize-12);
		ttbp = FP_OFF(stack+defaultSize-12);
		asm{
			mov sp, ttsp
			mov ss, ttss
			mov bp, ttbp
		}
		kernelActive = 1;
	}
	else{
		dispatchFlag = 0;
		PCB::dispatch();
	}
}



void SysCall::dispatch(){
	dispatchFlag = 1;
	sys_call();
}
void SysCall::CThread(){
	struct newThreadRequest{
		StackSize stackSize;
		Time timeSlice;
		Thread* running;
	};
	newThreadRequest* pointer = (newThreadRequest*)MK_FP(param2, param1);
	StackSize stackSize = pointer->stackSize;
	Time timeSlice = pointer->timeSlice;
	Thread* running = pointer->running;

	ID id = ++ThreadID;
	size_t temp = osp+0x10;
	asm{
		push es

		mov ax, oss
		mov es, ax
		mov ax, id
		mov bx, temp

		mov word ptr es:bx, ax

		pop es
	}

	PCB* pcb = new PCB(running, id, stackSize, timeSlice);
	allPCB->insert(pcb, id);
	dispatch();
}

void SysCall::start(){
	PCB* pcb = allPCB->findByID(param1);
	pcb->start();
	dispatch();
}

void SysCall::waitToComplete(){
	PCB* pcb = allPCB->findByID(param2);
	pcb->waitToComplete();
	dispatch();
}

void SysCall::sleep(){
	PCB::running->sleep(param2);
	dispatch();
}

void SysCall::DThread(){
	PCB* pcb = allPCB->findByID(param2);
	allPCB->remove(param2);
	delete pcb;
	dispatch();
}


void SysCall::CSemaphore(){
	int id = ++SemaphoreID;
	size_t temp = osp+0x10;
	asm{
		push es

		mov ax, oss
		mov es, ax
		mov ax, id
		mov bx, temp

		mov word ptr es:bx, ax

		pop es
	}
	KernelSem* sem = new KernelSem (id, param1);
	allSem->insert(sem, id);
	dispatch();
}

void SysCall::DSemaphore(){
	KernelSem* sem = allSem->findAndRemoveByID(param1);
	delete sem;
	dispatch();
}

void SysCall::wait(){
	KernelSem* sem = allSem->findByID(param1);
	sem->wait();
	dispatch();
}

void SysCall::signal(){
	KernelSem* sem = allSem->findByID(param1);
	sem->signal();
	dispatch();
}

void SysCall::val(){
	KernelSem* sem = allSem->findByID(param1);
	int rtn;
	rtn = sem->val();
	size_t temp = osp+0x10;
	asm{
		push es

		mov ax, oss
		mov es, ax
		mov ax, rtn
		mov bx, temp

		mov word ptr es:bx, ax

		pop es
	}
	dispatch();
}


void SysCall::CEvent(){
	ID id = ++EventID;
	size_t temp = osp+0x10;
	asm{
		push es

		mov ax, oss
		mov es, ax
		mov ax, id
		mov bx, temp

		mov word ptr es:bx, ax

		pop es
	}
	KernelEv* ev = new KernelEv (id, param1);
	allEv->insert(ev, id);
	dispatch();
}

void SysCall::DEvent(){
	KernelEv* ev = allEv->findAndRemoveByID(param1);
	delete ev;
	dispatch();
}

void SysCall::eWait(){
	KernelEv* ev = allEv->findByID(param1);
	ev->wait();
	if (PCB::running->getState() == Blocked) dispatch();
	sys_call();
}

void SysCall::eSignal(){
	KernelEv* ev = allEv->findByID(param1);
	ev->signal();
	sys_call();
}

void initIVT(){
	asm{
		cli

		push es
		push ax

		mov ax, 0
		mov es, ax

		mov word ptr es:0182h, seg sys_call
		mov word ptr es:0180h, offset sys_call

		pop ax
		pop es

		sti
	}
}
