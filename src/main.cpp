/*
 * hello.cpp
 *
 *  Created on: Apr 1, 2015
 *      Author: OS1
 */

#include "Kernel.h"

extern int userMain(int argc, char* argv[]);

int main(int argc, char* argv[])
{
	Kernel::load();

	int ret = userMain(argc, argv);

	Kernel::finish();

	return ret;
}

