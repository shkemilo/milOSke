/*
 * KernelEv.cpp
 *
 *  Created on: Sep 13, 2020
 *      Author: OS1
 */

#include "KernelEv.h"

#include "Kernel.h"
#include "SCHEDULE.H"

KernelEv::KernelEv(IVTNo ivtNo) : blocked(0) {
	lock();
	owner = (PCB*) Kernel::running;
	entry = IVTEntry::getEntry(ivtNo);
	entry->event = this;

	entry->oldRoutine = Kernel::swapIntRoutines(ivtNo, this->entry->oldRoutine);
	unlock();
}

KernelEv::~KernelEv() {
	lock();
	if(blocked) {
		owner->state = PCB::READY;
		Scheduler::put(owner);
	}

	entry->event = 0;
	Kernel::swapIntRoutines(entry->ivtNo, entry->oldRoutine);
	unlock();
}

void KernelEv::signal() {
	lock();
	if(blocked) {
		blocked = 0;
		owner->state = PCB::READY;

		Scheduler::put(owner);
		Kernel::dispatch();
	}
	unlock();
}

void KernelEv::wait() {
	lock();
	if(Kernel::running != owner) {
		unlock();
		return;
	}

	if(!blocked) {
		blocked = 1;
		owner->state = PCB::BLOCKED;
		Kernel::dispatch();
	}

	unlock();
}
