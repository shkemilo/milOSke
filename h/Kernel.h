/*
 * Kernel.h
 *
 *  Created on: Aug 24, 2020
 *      Author: OS1
 */

#ifndef KERNEL_H_
#define KERNEL_H_

#include "KernelSe.h"
#include "list.h"
#include "PCB.h"

class KernelSem;
class PCB;

#define lock() { asm{pushf; cli;}}
#define unlock() {asm{popf;}}

typedef void interrupt (*InterruptRoutine)(...);
typedef unsigned char IVTNo;

class Kernel {
public:
	static void load();
	static void finish();

	static void dispatch();
	static void interrupt newTimer(...);
	static InterruptRoutine swapIntRoutines(IVTNo entryNumber, InterruptRoutine newIntRoutine);

	static volatile PCB* running;
	static volatile int contextSwitchEnabled;
	static volatile int forcedDispatch;
	static List<PCB> allThreads;
	static List<PCB> blockedThreads;
	static List<KernelSem> allSemaphores;
	static InterruptRoutine oldTimer;
	static Thread* starting;
	static IdleThread* idle;

private:
	friend class PCB;
	static void runWrapper();
	static void updateTime();
	static void handleSignals();
	static void killThread();

	static void terminateRunning();
};

#endif /* KERNEL_H_ */
