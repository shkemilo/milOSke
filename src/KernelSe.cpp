/*
 * KernelSem.cpp
 *
 *  Created on: Aug 27, 2020
 *      Author: OS1
 */

#include "KernelSe.h"

#include "Kernel.h"
#include "SCHEDULE.H"

KernelSem::KernelSem(int init) {
	lock();
	Kernel::allSemaphores.push_back(this);
	value = init;
	unlock();
}

KernelSem::~KernelSem() {
	lock();
	while(!waiting.empty()) {
		unblock();
	}

	for(List<KernelSem>::Iterator it = Kernel::allSemaphores.begin(); it != Kernel::allSemaphores.end(); ++it)
				if(it.getData() == this) {
					Kernel::allSemaphores.erase(it);
					break;
				}
	unlock();
}

int KernelSem::val() const {
	return value;
}

int KernelSem::wait(Time maxTimeToWait) {
	lock();
	--value;
	if(value < 0) {
		Kernel::running->state = PCB::BLOCKED;
		Kernel::running->timeToWait = maxTimeToWait;
		waiting.push_front((PCB*)Kernel::running);
		Kernel::dispatch();
		if(maxTimeToWait != 0 && Kernel::running->timeToWait == 0) {
			unlock();
			return 0;
		}
	}

	Kernel::running->timeToWait = 0;
	unlock();
	return 1;
}

int KernelSem::signal(int n) {
	lock();
	if(n < 0) {
		unlock();
		return n;
	}

	if(n == 0) {
		++value;
		if(value<=0)
			unblock();

		unlock();
		return 0;
	}

	value += n;

	int deblockedCount = n;
	while(!waiting.empty() &&  deblockedCount != 0) {
		unblock();
		--deblockedCount;
	}

	unlock();
	return n - deblockedCount;
}

void KernelSem::tick() {
	lock();
	List<PCB>::Iterator i = waiting.begin();
	while(i != waiting.end()) {
		PCB* temp = i.getData();
		if(temp->timeToWait > 0) {
			--(temp->timeToWait);
			if(temp->timeToWait == 0) {
				temp->state = PCB::READY;
				Scheduler::put(temp);
				i = waiting.erase(i);
				continue;
			}
		}
		++i;
	}
	lock();
}

void KernelSem::block() {

}

void KernelSem::unblock() {
	lock();
	if(waiting.empty())
		return;

	PCB* temp = waiting.pop_back();
	temp->state = PCB::READY;
	Scheduler::put(temp);
	unlock();
}
