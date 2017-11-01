#include "event.h"

#include "kernelev.h"
#include "lock.h"
#include <stdlib.h>

Event::Event(IVTNo ivtNo){
	size_t temp;
	int tmp = ivtNo;
	asm{
		mov ax, 11
		mov bx, tmp
		int 60h
		mov temp, ax
	}
	this->id = temp;
}

Event::~Event(){
	int id = this->id;
	asm{
		mov ax, 12
		mov bx, id
		int 60h
	}
}

void Event::wait(){
	int id = this->id;
	asm{
		mov ax, 13
		mov bx, id
		int 60h
	}
}

void Event::signal(){
	int id = this->id;
	asm{
		mov ax, 14
		mov bx, id
		int 60h
	}
}

