#include "thread.h"

#include "PCB.h"
#include "timer.h"
#include "semaphor.h"
#include <dos.h>


int ret;
int _argc;
char** _argv;

int userMain(int argc, char* argv[]);

class UserMain : public Thread{
public:
	UserMain();
	void run();
};

UserMain::UserMain() : Thread(4096, 0) {}

void UserMain::run(){
	ret = userMain(_argc, _argv);
}

void extern initIVT();

int main (int argc, char* argv[]){

	initIVT();
	inic();

	PCB::mainThread = PCB::running = new PCB(0, 0, 2048, 0);
	PCB::running->setState(Main);
	PCB::numStarted++;

	_argc = argc;
	_argv = argv;

	UserMain* userThread = new UserMain();

	userThread->start();

	delete userThread;

	restore();
	return ret;
}
