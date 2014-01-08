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
/* (C) Copyright IBM Corp.  2010, 2011                              */
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

// Exercise dimension object
#include <bgsched/Dimension.h>

#include <iostream>

using namespace std;
using namespace bgsched;

int
main(int /*argc*/, const char** /*argv*/)
{
   cout << "Test postfix dimension increment operator" << endl;
   for (Dimension dim = Dimension::A; dim != Dimension();) {
       cout << string(dim++) << endl;
   }

   cout << "Test prefix dimension increment operator" << endl;
   for (Dimension dim = Dimension::A; dim != Dimension();) {
       cout << string(++dim) << endl;
   }

   cout << "Test postfix dimension decrement operator" << endl;
   for (Dimension dim = Dimension::E; dim != Dimension();) {
       cout << string(dim--) << endl;
   }

   cout << "Test prefix dimension decrement operator" << endl;
   for (Dimension dim = Dimension::E; dim != Dimension();) {
       cout << string(--dim) << endl;
   }
}
