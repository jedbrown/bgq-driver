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

#include "ExcludeList.h"

#include <algorithm>

namespace mmcs {
namespace server {

void
ExcludeList::add(
        const std::string& key
        )
{
    PthreadMutexHolder mutex;
    const int mutex_rc = mutex.Lock(&_mutex);
    assert(mutex_rc == 0);
    const std::list<std::string>::const_iterator it = std::find( _keyList.begin(), _keyList.end(), key );
    if ( it == _keyList.end() ) {
        _keyList.push_back(key);
    }
}

void
ExcludeList::remove(
        const std::string& key
        )
{
    PthreadMutexHolder mutex;
    const int mutex_rc = mutex.Lock(&_mutex);
    assert(mutex_rc == 0);
    _keyList.remove(key);
}

std::string
ExcludeList::getSqlListQuoted() const
{
    std::string result;
    PthreadMutexHolder mutex;
    const int mutex_rc = mutex.Lock(&_mutex);
    assert(mutex_rc == 0);
    if (_keyList.empty()) return result;

    result.append("(");
    for ( std::list<std::string>::const_iterator it = _keyList.begin(); it != _keyList.end(); ++it) {
        if (it != _keyList.begin()) {
            result.append(",");
        }
        result.append("'").append(*it).append("'");
    }
    result.append(")");

    return result;
}

} } // namespace mmcs::server
