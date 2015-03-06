/* begin_generated_IBM_copyright_prolog                             */
/*                                                                  */
/* This is an automatically generated copyright prolog.             */
/* After initializing,  DO NOT MODIFY OR MOVE                       */
/*  --------------------------------------------------------------- */
/* Licensed Materials - Property of IBM                             */
/* Blue Gene/Q 5765-PER 5765-PRP                                    */
/*                                                                  */
/* (C) Copyright IBM Corp. 2011, 2012 All Rights Reserved           */
/* US Government Users Restricted Rights -                          */
/* Use, duplication, or disclosure restricted                       */
/* by GSA ADP Schedule Contract with IBM Corp.                      */
/*                                                                  */
/*  --------------------------------------------------------------- */
/*                                                                  */
/* end_generated_IBM_copyright_prolog                               */
/**
 * \file test/internals/pwq/noncontig.cc
 * \brief ???
 */
#include "PipeWorkQueue.h"

#define SIZEOF_STRUCT_ELEM(struct,elem)	sizeof(((struct *)0)->elem)

typedef struct {
	double a;
	char _pad1[64];
	double b;
	char _pad2[64];
	double c;
} test1_t;

test1_t array1[64];

int main(int argc, char **argv) {
	int x;

	PAMI::Type::TypeCode *type1 = new PAMI::Type::TypeCode();
	type1->AcquireReference();
	type1->AddSimple(SIZEOF_STRUCT_ELEM(test1_t,a), SIZEOF_STRUCT_ELEM(test1_t,a), 1);
	type1->AddShift(SIZEOF_STRUCT_ELEM(test1_t,_pad1));
	type1->AddSimple(SIZEOF_STRUCT_ELEM(test1_t,b), SIZEOF_STRUCT_ELEM(test1_t,b), 1);
	type1->AddShift(SIZEOF_STRUCT_ELEM(test1_t,_pad2));
	type1->AddSimple(SIZEOF_STRUCT_ELEM(test1_t,c), SIZEOF_STRUCT_ELEM(test1_t,b), 1);
	type1->Complete();
	type1->SetAtomSize(sizeof(double));

	memset(array1, -1, sizeof(array1));
	for (x = 0; x < 64; ++x) {
		array1[x].a = x + 0.1;
		array1[x].b = x + 0.2;
		array1[x].c = x + 0.3;
	}

	PAMI::PipeWorkQueue pwq1;
	pwq1.configure((char *)array1, sizeof(array1), sizeof(array1), NULL, type1);

	size_t bytes;

	size_t bytes_left = 64 * type1->GetDataSize();
	while ((bytes = pwq1.bytesAvailableToConsume()) > 0) {
		char *buf = pwq1.bufferToConsume();
		printf("Test 1: %zu bytes at %p (%g)\n", bytes, buf, *((double *)buf));
		pwq1.consumeBytes(bytes);
		if (bytes > bytes_left) {
			pwq1.dump("Test #1 overflow?:");
		}
		bytes_left -= bytes;
	}
	pwq1.dump("Test #1 Final:");

	type1->ReleaseReference();
	return 0;
}
