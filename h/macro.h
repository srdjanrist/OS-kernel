#ifndef _MACRO_H_
#define _MACRO_H_
#include "ivtentry.h"

#define PREPAREENTRY(ivtNo, old)\
	void interrupt intr##ivtNo(...){\
		IVTEntry::ivTable[##ivtNo]->signal();\
		if (##old) IVTEntry::ivTable[##ivtNo]->callOld();\
	}\
	IVTEntry entry##ivtNo (##ivtNo, intr##ivtNo);


#endif
