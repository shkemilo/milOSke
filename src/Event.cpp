/*
 * Event.cpp
 *
 *  Created on: Sep 13, 2020
 *      Author: OS1
 */

#include "Event.h"
#include "Kernel.h"
#include "KernelEv.h"

Event::Event(IVTNo ivtNo) {
	lock();
	myImpl = new KernelEv(ivtNo);
	unlock();
}

Event::~Event() {
	lock();
	delete myImpl;
	unlock();
}

void Event::signal() {
	myImpl->signal();
}

void Event::wait() {
	myImpl->wait();
}

