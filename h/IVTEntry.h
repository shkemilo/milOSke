/*
 * IVTEntry.h
 *
 *  Created on: Sep 13, 2020
 *      Author: OS1
 */

#ifndef IVTENTRY_H_
#define IVTENTRY_H_

typedef void interrupt (*InterruptRoutine)(...);
typedef unsigned char IVTNo;

class KernelEv;

const int IVT_SIZE = 256;

class IVTEntry {
public:
	IVTEntry(IVTNo ivtNo, InterruptRoutine newRoutine);
	~IVTEntry();

	static IVTEntry* getEntry(IVTNo ivtNo);

	void signal();
	void callOldRoutine();

private:
	static IVTEntry* ivtEntires[IVT_SIZE];

	IVTNo ivtNo;
	KernelEv* event;
	InterruptRoutine oldRoutine;

	friend class KernelEv;
};

#endif /* IVTENTRY_H_ */
