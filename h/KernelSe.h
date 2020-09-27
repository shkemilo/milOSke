/*
 * KernelSem.h
 *
 *  Created on: Aug 27, 2020
 *      Author: OS1
 */

#ifndef KERNELSE_H_
#define KERNELSE_H_

#include "list.h"
#include "PCB.h"

typedef unsigned int Time;

class KernelSem {
public:
	friend class Semaphore;
	friend class Kernel;

	KernelSem(int init = 1);
	~KernelSem();

	int wait(Time maxTimeToWait);
	int signal(int n = 0);

	int val() const;

private:
	void unblock();
	void block();
	void tick();

	int value;
	List<PCB> waiting;

};

#endif /* KERNELSE_H_ */
