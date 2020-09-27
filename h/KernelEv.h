/*
 * KernelEv.h
 *
 *  Created on: Sep 13, 2020
 *      Author: OS1
 */

#ifndef KERNELEV_H_
#define KERNELEV_H_
#include "Event.h"
#include "IVTEntry.h"
#include "PCB.H"

class KernelEv {
public:
	KernelEv(IVTNo ivtNo);
	~KernelEv();

	void signal();
	void wait();

private:
	IVTEntry* entry;
	int blocked;
	PCB* owner;

};

#endif /* KERNELEV_H_ */
