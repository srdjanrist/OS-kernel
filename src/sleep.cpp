#include "sleep.h"

#include "pcb.h"
#include "schedule.h"
#include "lock.h"


extern volatile int requestedContextChange;

SleepQueue::SleepQueue(){
	globalTime = 0;
	goalTime = 0;
	numSleeping = 0;
}

SleepQueue::~SleepQueue(){
	Elem* prev;
	for (Elem* tek = head; tek != 0;){
		prev = tek;
		tek = tek->next;
		delete prev;
	}
}

void SleepQueue::put (PCB* pcb, Time timeToSleep) {
	if (!numSleeping){
		head = tail = new Elem(pcb);
	}else{
		Elem* tek = head;
		Elem* prev = 0;
		while (tek && ((tek->pcb->timeToSleep-=globalTime) < timeToSleep)){
			prev = tek;
			tek = tek->next;
		}
		if (!prev){
			head = new Elem(pcb, head);
		}else{
			prev->next = new Elem (pcb, prev->next);
		}
		if (tek) tek = tek->next;
		while (tek){
			tek->pcb->timeToSleep -= globalTime;
			tek = tek->next;
		}
	}
	numSleeping++;
	goalTime = head->pcb->timeToSleep;
	globalTime = 0;
}

void SleepQueue::updateSleepTime(){
	if (!numSleeping) return;
	if (++globalTime < goalTime) return;

	Elem* tek = head;
	while (tek){
		PCB* pcb = tek->pcb;
		if (pcb->timeToSleep == globalTime){
			Scheduler::put(pcb);
			pcb->setState(Ready);
			PCB::numStarted++;
			numSleeping--;
			tek = tek->next;
			take();
		}
		else{
			if (globalTime < maxGlobalTime){
				goalTime = pcb->timeToSleep;
			}
			else{
				while (tek){
					tek->pcb->timeToSleep -= globalTime;
					if (tek == head) goalTime = tek->pcb->timeToSleep;
					tek = tek->next;
				}
				globalTime = 0;
			}
			break;
		}
	}
	if (PCB::running == PCB::idleThread) requestedContextChange = 1;
}

