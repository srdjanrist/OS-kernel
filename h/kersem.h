#ifndef _KERNELSEM_H_
#define _KERNELSEM_H_


typedef int ID;

class Semaphore;
class Queue;
class PCB;

class KernelSem{

private:
	friend class SysCall;
	friend class PCB;
	friend class Semaphore;

	KernelSem(ID, int);
	~KernelSem();

	void wait();
	void signal();

	int val() const;
	int value;

	ID id;

	Queue* waitQueue;

};

#endif
