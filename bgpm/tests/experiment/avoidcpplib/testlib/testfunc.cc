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


extern "C" int TestFunc()
{
    printf("In testfunc\n");

    std::vector<int> vec;
    for (int i=0; i<=6; ++i) {
        vec.push_back(i);
    }

    char * strg = new char[1024];
    strcpy(strg, "testfunc hi there");
    printf("strg = %s\n", strg);

    for (int i=0; i<(int)vec.size(); ++i) {
        printf("vec[%d]=%d\n", i, vec[i]);
    }

    return(0);
}
