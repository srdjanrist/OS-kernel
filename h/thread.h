#ifndef _THREAD_H_
#define _THREAD_H_

typedef unsigned long StackSize;
const StackSize defaultStackSize = 4096;
typedef unsigned int Time;
const Time defaultTimeSlice = 2;
typedef int ID;

class PCB;

class Thread {

public:
	void start();
	void waitToComplete();
	virtual ~Thread();

	static void sleep(Time timeToSleep);

protected:
	friend class PCB;
	Thread (StackSize stackSize = defaultStackSize, Time timeSlice = defaultTimeSlice);
	virtual void run();

private:
	ID id;
	static void wrapper (Thread*);

};

void dispatch();

#endif
