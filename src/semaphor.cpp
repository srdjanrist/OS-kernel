#include "semaphor.h"

#include "kersem.h"
#include "lock.h"
#include "dos.h"
#include <stdlib.h>

Semaphore::Semaphore(int init){
	size_t temp;
	asm{
		mov ax, 6
		mov bx, init
		int 60h
		mov temp, ax
	}
	this->id = temp;
}

Semaphore::~Semaphore(){
	ID id = this->id;
	asm{
		mov ax, 7
		mov bx, id
		int 60h
	}
}

void Semaphore::wait(){
	ID id = this->id;
	asm{
		mov ax, 8
		mov bx, id
		int 60h
	}
}

void Semaphore::signal(){
	ID id = this->id;
	asm{
		mov ax, 9
		mov bx, id
		int 60h
	}
}

int Semaphore::val() const {
	int temp;
	ID id = this->id;
	asm{
		mov ax, 10
		mov bx, id

		int 60h
		mov temp, ax
	}
	return temp;
}
