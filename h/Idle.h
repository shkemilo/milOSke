/*
 * IdleThread.h
 *
 *  Created on: Aug 24, 2020
 *      Author: OS1
 */

#ifndef IDLE_H_
#define IDLE_H_

#include "Thread.h"

class IdleThread : public Thread {
public:
	IdleThread();

	virtual void run();

	void start();

	friend class PCB;
};

#endif /* IDLE_H_ */
