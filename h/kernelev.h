#ifndef _KERNELEV_H_
#define _KERNELEV_H_


class PCB;
class IVTEntry;
class Event;

typedef int ID;
typedef unsigned char IVTNo;


class KernelEv{
public:
	friend class Event;
	friend class IVTEntry;

	friend class SysCall;

	KernelEv (ID, IVTNo);
	~KernelEv();

	int intFlag;

	ID id;
	PCB* myPCB;
	IVTEntry* ivtEntry;

	void wait();
	void signal();
};


#endif
