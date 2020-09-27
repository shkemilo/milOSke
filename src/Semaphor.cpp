/*
 * Semaphor.cpp
 *
 *  Created on: Aug 27, 2020
 *      Author: OS1
 */

#include "Semaphor.h"

#include "Kernel.h"
#include "KernelSe.h"

Semaphore::Semaphore(int init) {
	lock();
	myImpl = new KernelSem(init);
	unlock();
}

int Semaphore::wait(Time maxTimeToWait) {
	return myImpl->wait(maxTimeToWait);
}

int Semaphore::signal(int n) {
	return myImpl->signal(n);
}

int Semaphore::val() const {
	return myImpl->val();
}

Semaphore::~Semaphore() {
	lock();
	delete myImpl;
	unlock();
}

