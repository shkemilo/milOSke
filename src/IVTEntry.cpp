/*
 * IVTEntry.cpp
 *
 *  Created on: Sep 13, 2020
 *      Author: OS1
 */

#include "IVTEntry.h"

#include "Kernel.h"
#include "KernelEv.h"

IVTEntry* IVTEntry::ivtEntires[] = { 0 };

IVTEntry* IVTEntry::getEntry(IVTNo ivtNo) {
	return ivtEntires[ivtNo];
}

IVTEntry::IVTEntry(IVTNo ivtNo, InterruptRoutine newRoutine) : ivtNo(ivtNo), event(0), oldRoutine(newRoutine) {
	ivtEntires[ivtNo] = this;
}

IVTEntry::~IVTEntry() {
	ivtEntires[ivtNo] = 0;
}

void IVTEntry::signal() {
	if(event) {
		event->signal();
	}
}

void IVTEntry::callOldRoutine() {
	if(oldRoutine) {
		lock();
		oldRoutine();
		unlock();
	}
}

