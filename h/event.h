#ifndef _EVENT_H_
#define _EVENT_H_

#include "macro.h"

typedef unsigned char IVTNo;
typedef int ID;

class KernelEv;

class Event{

public:
	Event (IVTNo ivtNo);
	~Event();

	void wait();

protected:
	friend class KernelEv;
	void signal();

private:
	ID id;

};


#endif
