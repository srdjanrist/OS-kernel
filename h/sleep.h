#ifndef _SLEEP_H_
#define _SLEEP_H_

#include "queue.h"


typedef unsigned int Time;

const Time maxGlobalTime = 5000;


class SleepQueue : public Queue{

public:
	SleepQueue();
	~SleepQueue();

	virtual void put (PCB*, Time);
	void updateSleepTime();

	volatile Time globalTime;
	volatile Time goalTime;
	volatile unsigned numSleeping;

};

void updateSleepTime();

#endif
