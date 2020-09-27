/*
 * PCB.h
 *
 *  Created on: Aug 22, 2020
 *      Author: OS1
 */

#ifndef PCB_H_
#define PCB_H_

#include "list.h"
#include "Thread.h"

const int SIGNAL_COUNT = 16;

class PCB {
public:
	typedef enum State {
			NEW, READY, BLOCKED, TERMINATED
		};

	friend class Kernel;
	friend class Thread;
	friend class IdleThread;
	friend class KernelSem;
	friend class KernelEv;

private:
	static ID nextID;
	ID pid;

	StackSize stackSize;
	unsigned* stack;
	volatile unsigned sp, ss, bp;

	volatile Time timeRemaining;
	volatile Time timeToWait;
	Time timeSlice;

	volatile State state;

	Thread* myThread;
	PCB* parent;

	List<PCB> waiting;

	struct Signal {
		static int blockedGlobaly[SIGNAL_COUNT];
		int blocked;

		SignalId id;

		List<SignalHandler> handlers;

		Signal(SignalId id) : blocked(0), id(id) {

		}

		~Signal();
	};

	Signal* signals[SIGNAL_COUNT];
	List<Signal> signalsToHandle;

	volatile int handleRequest;

public:

	PCB(Thread* myThread, StackSize stackSize, Time timeSlice);

	void start();
	void waitToComplete();
	ID getId();

	static ID getRunningId();
	static Thread* getThreadById(ID id);

	void signal(SignalId signal);

	void registerHandler(SignalId signal, SignalHandler handler);
	void unregisterAllHandlers(SignalId id);
	void swap(SignalId id, SignalHandler hand1, SignalHandler hand2);

	void blockSignal(SignalId signal);
	static void blockSignalGlobally(SignalId signal);
	void unblockSignal(SignalId signal);
	static void unblockSignalGlobally(SignalId signal);

	void initStack();

	~PCB();
};

#endif /* PCB_H_ */
