#include "kersem.h"

#include "queue.h"
#include "pcb.h"
#include "schedule.h"
#include "lock.h"


KernelSem::KernelSem(ID id, int init){
	this->id = id;
	value = init;
	waitQueue = new Queue();
}

KernelSem::~KernelSem(){
	delete waitQueue;
}

void KernelSem::wait(){
	if (--value < 0){
		PCB::running->setState(Blocked);
		waitQueue->put(PCB::running);
		PCB::numStarted--;
	}
}

void KernelSem::signal(){
	if (value++ < 0){
		PCB* temp = waitQueue->take();
		waitQueue->remove(temp);
		temp->setState(Ready);
		PCB::numStarted++;
		Scheduler::put(temp);
	}
}

int KernelSem::val() const {
	return value;
}
