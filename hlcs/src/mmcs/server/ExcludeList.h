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

#ifndef MMCS_SERVER_EXCLUDE_LIST_H_
#define MMCS_SERVER_EXCLUDE_LIST_H_

#include <bgq_util/include/pthreadmutex.h>

#include <list>
#include <string>

namespace mmcs {
namespace server {

//!*****************************************************************************
// @Class ExcludeList
// @Brief Used to keep track of which blocks should be excluded from DB polling
//******************************************************************************
class ExcludeList
{
public:
    ExcludeList() : _mutex(PTHREAD_MUTEX_ERRORCHECK_NP) {}
    void add(const std::string& key);       // add a key to the list
    void remove(const std::string& key);    // remove a key from the list
    std::string getSqlListQuoted() const;   // get list in sql form

private:
    mutable PthreadMutex _mutex;     // to serialize access to ExcludeList
    std::list<std::string> _keyList; // list of keys
};

} } // namespace mmcs::server

#endif
