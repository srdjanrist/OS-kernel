#include "ivtentry.h"

#include "kernelev.h"
#include <stdlib.h>
#include <dos.h>


IVTEntry* IVTEntry::ivTable[256];

IVTEntry::IVTEntry(IVTNo ivtNo, pInterrupt newRoutine){
	size_t newSEG = FP_SEG(newRoutine);
	size_t newOFF = FP_OFF(newRoutine);
	size_t oldSEG, oldOFF;

	ivTable[ivtNo] = this;
	this->ivtNo = ivtNo;
	size_t tmp = ivtNo * 4;

	asm{
		cli

		push es
		push ax
		push bx

		mov ax, 0
		mov es, ax
		mov bx, word ptr tmp

		mov ax, word ptr es:bx
		mov word ptr oldOFF, ax
		mov ax, word ptr es:bx+2
		mov word ptr oldSEG, ax

		mov ax, word ptr newOFF
		mov word ptr es:bx, ax
		mov ax, word ptr newSEG
		mov word ptr es:bx+2, ax

		pop bx
		pop ax
		pop es

		sti
	}

	oldRoutine = (pInterrupt)MK_FP(oldSEG, oldOFF);
}


IVTEntry::~IVTEntry(){
	size_t oldSEG = FP_SEG(oldRoutine);
	size_t oldOFF = FP_OFF(oldRoutine);
	size_t tmp = this->ivtNo * 4;

	asm{
		cli

		push es
		push ax
		push bx

		mov ax, 0
		mov es, ax
		mov bx, word ptr tmp

		mov ax, word ptr oldOFF
		mov word ptr es:bx, ax
		mov ax, word ptr oldSEG
		mov word ptr es:bx+2, ax

		pop bx
		pop ax
		pop es

		sti
	}

	ivTable[ivtNo] = 0;
}

void IVTEntry::callOld(){
	(*oldRoutine)();
}

void IVTEntry::setEvent(KernelEv* ev){
	this->myEvent = ev;
}

void IVTEntry::signal(){
	this->myEvent->signal();
}

