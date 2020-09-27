/*
 * PCB.cpp
 *
 *  Created on: Aug 22, 2020
 *      Author: OS1
 */

#include "PCB.h"

#include "Kernel.h"
#include "SCHEDULE.H"
#include "DOS.H"

ID PCB::nextID = 0;
int PCB::Signal::blockedGlobaly[] = { 0 };

PCB::PCB(Thread* myThread, StackSize stackSize, Time timeSlice) {
	lock();
	this->myThread = myThread;
	this->stackSize = stackSize / sizeof(unsigned);
	this->timeSlice = timeSlice;
	timeRemaining = timeSlice;
	timeToWait = 0;

	pid = nextID++;
	state = NEW;
	stack = 0;
	ss = sp = bp = 0;

	Kernel::allThreads.push_back(this);

	for(int i = 0; i < SIGNAL_COUNT; ++i) {
		signals[i] = new Signal(i);
		if(i == 0) {
			registerHandler(0, Kernel::killThread);
		}
	}
	handleRequest = 0;

	parent = (PCB*)Kernel::running;
	unlock();
}

void PCB::initStack() {
	lock();
	stack = new unsigned[stackSize];
	stack[stackSize - 1] = 0x200;
	stack[stackSize - 2] = FP_SEG(&Kernel::runWrapper);
	stack[stackSize - 3] = FP_OFF(&Kernel::runWrapper);
	ss = FP_SEG(stack + stackSize - 12);
	sp = bp = FP_OFF(stack + stackSize - 12);
	unlock();

}

void PCB::start() {
	lock();
	state = PCB::READY;
	initStack();

	Scheduler::put(this);
	unlock();
}

void PCB::waitToComplete() {
	lock();
	if(state == PCB::TERMINATED || state == PCB::NEW ||
			this == Kernel::running || this == Kernel::starting->myPCB ||
			this->myThread == Kernel::idle) {
		unlock();
		return;
	}

	Kernel::running->state = PCB::BLOCKED;
	waiting.push_back((PCB*)Kernel::running);
	Kernel::dispatch();
	unlock();
}

ID PCB::getId() {
	return pid;
}

ID PCB::getRunningId() {
	return Kernel::running->pid;
}

Thread* PCB::getThreadById(ID id) {
	for(List<PCB>::Iterator it = Kernel::allThreads.begin(); it != Kernel::allThreads.end(); ++it) {
		if((*it).pid == id)
			return (*it).myThread;
	}

	return 0;
}

void PCB::signal(SignalId signal) {
	lock();
	if(signal < SIGNAL_COUNT) {
		signalsToHandle.push_back(signals[signal]);
		handleRequest = 1;
	}
	unlock();
}

void PCB::registerHandler(SignalId signal, SignalHandler handler) {
	lock();
	if(signal < SIGNAL_COUNT) {
		signals[signal]->handlers.push_back(new SignalHandler(handler));
	}
	unlock();
}

void PCB::unregisterAllHandlers(SignalId signal) {
	lock();
	if(signal < SIGNAL_COUNT) {
		while(!signals[signal]->handlers.empty()) {
			signals[signal]->handlers.pop_back();
		}
	}
	unlock();
}

void PCB::swap(SignalId id, SignalHandler hand1, SignalHandler hand2) {
	lock();
	if(id < SIGNAL_COUNT) {
		List<SignalHandler>& handlers = signals[id]->handlers;
		List<SignalHandler>::Iterator it1, it2, empty;
		for(List<SignalHandler>::Iterator it = handlers.begin(); it != handlers.end(); ++it) {
			if(*it == hand1) {
				it1 = it;
			}

			if(*it == hand2) {
				it2 = it;
			}

			if(it1 != empty && it2 != empty)
				break;
		}

		if(it1 == empty || it2 == empty) {
			unlock();
			return;
		}

		handlers.insert(it1, &hand2);
		handlers.insert(it2, &hand1);
		handlers.erase(it1);
		handlers.erase(it2);
	}
	unlock();
}

void PCB::blockSignal(SignalId signal) {
	lock();
	if(signal < SIGNAL_COUNT) {
		signals[signal]->blocked = 1;
	}
	unlock();
}

void PCB::blockSignalGlobally(SignalId signal) {
	lock();
	if(signal < SIGNAL_COUNT) {
		Signal::blockedGlobaly[signal] = 1;
	}
	unlock();
}

void PCB::unblockSignal(SignalId signal) {
	lock();
	if(signal < SIGNAL_COUNT) {
			signals[signal]->blocked = 0;
	}
	unlock();
}

void PCB::unblockSignalGlobally(SignalId signal) {
	lock();
	if(signal < SIGNAL_COUNT) {
			Signal::blockedGlobaly[signal] = 0;
	}
	unlock();
}

PCB::Signal::~Signal() {
	lock();
	while(!handlers.empty())
		delete handlers.pop_front();
	unlock();
}

PCB::~PCB() {
	lock();
	state = PCB::TERMINATED;
	for(int i = 0; i < SIGNAL_COUNT; ++i)
		if(signals[i]) {
			delete signals[i];
			signals[i] = 0;
		}

	for(List<PCB>::Iterator it = Kernel::allThreads.begin(); it != Kernel::allThreads.end(); ++it)
			if(it.getData() == this) {
				Kernel::allThreads.erase(it);
				break;
			}

	if(stack)
		delete stack;
	unlock();
}
