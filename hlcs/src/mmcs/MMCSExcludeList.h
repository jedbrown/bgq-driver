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
/* (C) Copyright IBM Corp.  2007, 2011                              */
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

#ifndef __MMCSEXCLUDELIST_H
#define __MMCSEXCLUDELIST_H

#include <string>
#include <list>
#include "bgq_util/include/pthreadmutex.h"

//!*****************************************************************************
// @Class ExcludeList
// @Brief Used to keep track of which blocks or jobs should be excluded from DB polling
//******************************************************************************
class ExcludeList
{
public:
    ExcludeList() : _excludeListMutex(PTHREAD_MUTEX_ERRORCHECK_NP) {}
    void add(std::string key);		// add a key to the list
    void add(unsigned key);		// add a key to the list
    void remove(std::string key);	        // remove a key from the list
    void remove(unsigned key);		// remove a key from the list
    std::string getSqlListQuoted();		// get list in sql form
    std::string getSqlListUnquoted();		// get list in sql form
private:
    PthreadMutex    _excludeListMutex;  // to serialize access to ExcludeList
    std::list<std::string> _keyList;		// list of keys
};


#endif
