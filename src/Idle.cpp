#include "Idle.h"

#include "Kernel.h"
#include "PCB.h"

IdleThread::IdleThread() : Thread(256, 1) {

}

void IdleThread::run() {
	while(1);
}

void IdleThread::start() {
	lock();
	myPCB->state = PCB::READY;

	myPCB->initStack();

	unlock();
}
