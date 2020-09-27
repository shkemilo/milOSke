/*
 * Event.h
 *
 *  Created on: Sep 13, 2020
 *      Author: OS1
 */

#ifndef EVENT_H_
#define EVENT_H_

#include <IVTEntry.h>

typedef unsigned char IVTNo;
class KernelEv;

class Event {
public:
	Event(IVTNo ivtNo);
	~Event();

	void wait();

protected:
	friend class KernelEv;
	void signal();

private:
	KernelEv* myImpl;
};

#define PREPAREENTRY(numEntry, callOld)\
void interrupt inter##numEntry(...);\
IVTEntry newEntry##numEntry(numEntry, inter##numEntry);\
void interrupt inter##numEntry(...) {\
	newEntry##numEntry.signal();\
	if (callOld)\
		newEntry##numEntry.callOldRoutine();\
}

#endif /* EVENT_H_ */
