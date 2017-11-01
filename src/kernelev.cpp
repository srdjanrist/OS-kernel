#include "kernelev.h"

#include "pcb.h"
#include "schedule.h"
#include "ivtentry.h"
#include "lock.h"


KernelEv::KernelEv(ID id, IVTNo ivtNo){
	intFlag = 0;
	this->id = id;
	this->myPCB = PCB::running;
	ivtEntry = IVTEntry::ivTable[ivtNo];
	ivtEntry->setEvent(this);
}

KernelEv::~KernelEv(){
	signal ();
	delete ivtEntry;
}

void KernelEv::wait(){
	if (PCB::running != this->myPCB) return;
	PCB::running->setState(Blocked);
	PCB::numEventWaitingPCB++;
	PCB::numStarted--;
	intFlag = 1;
}

void KernelEv::signal(){
	if (intFlag){
		intFlag = 0;
		this->myPCB->setState(Ready);
		PCB::numEventWaitingPCB--;
		PCB::numStarted++;
		Scheduler::put(this->myPCB);
		PCB::dispatch();
	}
}
