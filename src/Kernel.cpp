/*
 * Kernel.cpp
 *
 *  Created on: Aug 24, 2020
 *      Author: OS1
 */

#include "Kernel.h"

#include "Idle.h"
#include "SCHEDULE.H"
#include "Thread.h"
#include "STDIO.H"
#include "DOS.H"

extern void tick();

InterruptRoutine Kernel::oldTimer = 0;
volatile PCB* Kernel::running = 0;
List<PCB> Kernel::allThreads;
List<PCB> Kernel::blockedThreads;
List<KernelSem> Kernel::allSemaphores;
volatile int Kernel::forcedDispatch = 0;
volatile int Kernel::contextSwitchEnabled = 1;
Thread* Kernel::starting = 0;
IdleThread* Kernel::idle = 0;

void Kernel::load() {
	lock();
	printf("Mateja Milosevic 2018/0177 presents: \n");
	printf("===============================================================================\n");
	printf("               	          _   _    ___    ___   _\n");
	printf("               	  _ __   (_) | |  / _ \\  / __| | |__  ___\n");
	printf("               	 | \'  \\  | | | | | (_) | \\__ \\ | / / / -_)\n");
	printf("               	 |_|_|_| |_| |_|  \\___/  |___/ |_\\_\\ \\___|\n");
	printf("===============================================================================\n");
	oldTimer = swapIntRoutines(0x08, newTimer);

	starting = new Thread(0x10000, 1);
	starting->myPCB->state = PCB::READY;
	running = (volatile PCB*)starting->myPCB;

	idle = new IdleThread();
	idle->start();
	unlock();
}

void Kernel::finish() {
	if(running != starting->myPCB)
		return;

	lock();
	swapIntRoutines(0x08, oldTimer);
	delete idle;
	delete starting;
	printf("Shutting down...\n");
	unlock();
}

InterruptRoutine Kernel::swapIntRoutines(IVTNo entryNumber, InterruptRoutine newIntRoutine) {
	InterruptRoutine oldIR = 0;
	lock();
	oldIR = getvect(entryNumber);
	setvect(entryNumber, newIntRoutine);
	unlock();

	return oldIR;
}

void Kernel::dispatch() {
	lock();
	forcedDispatch = 1;

	asm {
		int 0x08
	}

	forcedDispatch = 0;
	unlock();
}

void interrupt Kernel::newTimer(...) {
	//if(forcedDispatch) syncPrintf("Called from dispatch!\n");

	//syncPrintf("Current thread is: %d.\n", Thread::getRunningId());
	static volatile unsigned tempSS, tempSP, tempBP;
	if(contextSwitchEnabled && (forcedDispatch || running->timeSlice != 0))
		if(forcedDispatch || ((running->timeRemaining == 0 ? 0 : --(running->timeRemaining)) == 0)) {
			//syncPrintf("Saving context!\n");
#ifndef BCC_BLOCK_IGNORE
			asm {
					mov tempSS, ss
					mov tempSP, sp
					mov tempBP, bp
				}
#endif
				running->ss = tempSS;
				running->sp = tempSP;
				running->bp = tempBP;

				//syncPrintf("Getting new running!\n");

				if (running->state == PCB::READY && running != idle->myPCB)
					Scheduler::put((PCB*)running);

				running = Scheduler::get();

				if (!running) {
					running = idle->myPCB;
					//cout << "next is idle" << endl;
				}

				//syncPrintf("Restoring time slice!\n");
				if(running->timeRemaining == 0)
					running->timeRemaining = running->timeSlice;


				//syncPrintf("Restoring context!\n");
				tempSS = running->ss;
				tempSP = running->sp;
				tempBP = running->bp;
#ifndef BCC_BLOCK_IGNORE
				asm {
					mov ss, tempSS
					mov sp, tempSP
					mov bp, tempBP
				}
#endif

				//syncPrintf("Context restored!\n");
		}

	if(!forcedDispatch) {
			(*oldTimer)();
			tick();
			Kernel::updateTime();
		} else {
			forcedDispatch = 0;
		}

	if(running->handleRequest) {
		handleSignals();
	}
	//syncPrintf("Current thread after is: %d.\n", Thread::getRunningId());
}

void Kernel::runWrapper() {
	running->myThread->run();

	lock();
	if(running->parent)
		running->parent->signal(1);
	running->signal(2);

	terminateRunning();
	dispatch();
}

void Kernel::updateTime() {
	lock();
	for(List<KernelSem>::Iterator it = allSemaphores.begin(); it != allSemaphores.end(); ++it) {
		(*it).tick();
	}
	unlock();
}

void Kernel::handleSignals() {
#ifndef BCC_BLOCK_IGNORE
	asm { sti }
#endif
	contextSwitchEnabled = 0;

	running->handleRequest = 0;

	while(!((PCB*)running)->signalsToHandle.empty()) {
		PCB::Signal* signal = ((PCB*)running)->signalsToHandle.pop_front();
		if(!signal->blocked && !PCB::Signal::blockedGlobaly[signal->id])
			for(List<SignalHandler>::Iterator it = signal->handlers.begin(); it != signal->handlers.end(); ++it)
				(*it)();
	}

	contextSwitchEnabled = 1;
}

void Kernel::killThread() {
	if(running == starting->myPCB)
		return;

	if(running->stack)
		delete running->stack;
	running->stack = 0;

	terminateRunning();

	contextSwitchEnabled = 1;
	Kernel::dispatch();
}

void Kernel::terminateRunning() {
	running->state = PCB::TERMINATED;

	while( !((PCB*)running)->waiting.empty() ) {
			PCB* temp = running->waiting.pop_back();
			temp->state = PCB::READY;
			Scheduler::put(temp);
		}

}
