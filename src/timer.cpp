#include "timer.h"

#include <stdlib.h>
#include "PCB.h"
#include "schedule.h"
#include "lock.h"

#include "sleep.h"

volatile size_t tsp, tss, tbp;
volatile int requestedContextChange = 0;
volatile int count;

size_t oldTimerOFF, oldTimerSEG;

extern void tick();
extern int kernelActive;


void interrupt timer(){
	if (!requestedContextChange){
		tick();
		asm int 61h
		PCB::sleeping->updateSleepTime();
		count--;
	}
	if (!count || requestedContextChange){
		if (lockFlag || (kernelActive && !lockFlag)){
			requestedContextChange = 0;
			if (!kernelActive){

				asm{
					mov tsp, sp
					mov tss, ss
					mov tbp, bp
				}
				PCB::running->sp = tsp;
				PCB::running->ss = tss;
				PCB::running->bp = tbp;
			}


			if ((!PCB::running->finished && PCB::running->getState() == Running) || PCB::running->getState() == Main){
				Scheduler::put(PCB::running);
				if (PCB::running->getState() == Running)
					PCB::running->setState(Ready);
			}

			PCB::running = Scheduler::get();
			if (PCB::running->getState() == Ready)
				PCB::running->setState(Running);

			if (PCB::running == 0){
				if (PCB::sleeping->numSleeping || PCB::numEventWaitingPCB){
					if (PCB::idleThread == 0)
						PCB::createIdleThread();
					PCB::running = PCB::idleThread;
					PCB::running->setState(Idle);
				}
			}
			if (PCB::running == PCB::mainThread) PCB::running->setState(Main);

			tsp = PCB::running->sp;
			tss = PCB::running->ss;
			tbp = PCB::running->bp;

			count = PCB::running->timeSlice;
			asm{
				mov sp, tsp
				mov ss, tss
				mov bp, tbp
			}

			if (kernelActive) kernelActive = 0;
			lockFlag = 1;
		}else{
			requestedContextChange = 1;
		}
	}
}




void inic(){
	asm{
		cli

		push es
		push ax

		mov ax, 0
		mov es, ax

		mov ax, word ptr es:0022h
		mov word ptr oldTimerSEG, ax
		mov ax, word ptr es:0020h
		mov word ptr oldTimerOFF, ax

		mov word ptr es:0022h, seg timer
		mov word ptr es:0020h, offset timer

		mov ax, oldTimerSEG
		mov word ptr es:0186h, ax
		mov ax, oldTimerOFF
		mov word ptr es:0184h, ax

		pop ax
		pop es

		sti
	}
}

void restore(){
	asm{
		cli

		push es
		push ax

		mov ax, 0
		mov es, ax

		mov ax, word ptr oldTimerSEG
		mov word ptr es:0022h, ax
		mov ax, word ptr oldTimerOFF
		mov word ptr es:0020h, ax

		pop ax
		pop es

		sti
	}
}
