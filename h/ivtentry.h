#ifndef _IVTENTRY_H_
#define _IVTENTRY_H_

typedef unsigned char IVTNo;
typedef void interrupt (*pInterrupt) (...);

class Event;

class IVTEntry{
	friend class KernelEv;

	KernelEv* myEvent;

	IVTNo ivtNo;
	pInterrupt oldRoutine;

	void setEvent(KernelEv*);

public:
	IVTEntry (IVTNo, pInterrupt);
	~IVTEntry();
	static IVTEntry* ivTable[];
	void  callOld();
	void signal();

};


#endif
