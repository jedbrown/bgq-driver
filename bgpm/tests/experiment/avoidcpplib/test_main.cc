/* begin_generated_IBM_copyright_prolog                             */
/*                                                                  */
/* This is an automatically generated copyright prolog.             */
/* After initializing,  DO NOT MODIFY OR MOVE                       */
/* ================================================================ */
/*                                                                  */
/* Licensed Materials - Property of IBM                             */
/*                                                                  */
/* Blue Gene/Q                                                      */
/*                                                                  */
/* (C) Copyright IBM Corp.  2010, 2012                              */
/*                                                                  */
/* US Government Users Restricted Rights -                          */
/* Use, duplication or disclosure restricted                        */
/* by GSA ADP Schedule Contract with IBM Corp.                      */
/*                                                                  */
/* This software is available to you under the                      */
/* Eclipse Public License (EPL).                                    */
/*                                                                  */
/* ================================================================ */
/*                                                                  */
/* end_generated_IBM_copyright_prolog                               */

#include <stdio.h>
#include <stdlib.h>
#include "spi/include/kernel/location.h"
#include <vector>
#include <algorithm>
//#include "alloc.h"

extern "C" int TestFunc();

using namespace std;


int main(int argc, char *argv[])
{
    printf("Quicky C++ test to see what it takes to avoid the need for the c++ std lib\n");

    vector<int> coll;

    char * strg = new char[1024];
    strcpy(strg, "hi there");
    printf("strg = %s\n", strg);

   // vector<int>::iterator pos = coll.begin();
   // reverse (++pos, coll.end());

    for (int i=0; i<=6; ++i) {
        coll.push_back(i);
    }


    for (int i=(int)coll.size()-1; i>=-1; --i) {
        printf("coll[%d]=%d\n", i, coll[i]);
    }

    for (int i=0; i<32; ++i) {
        char *fail = new char[1024*1024*1024];
    }

    TestFunc();

    return(0);
}
