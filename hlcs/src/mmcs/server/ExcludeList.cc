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

#include <sstream>


namespace mmcs {
namespace server {


void
ExcludeList::add(std::string key)
{
    PthreadMutexHolder mutex;
    std::list<std::string>::iterator it;
    int mutex_rc = mutex.Lock(&_excludeListMutex);
    assert(mutex_rc == 0);
    for (it = _keyList.begin(); it != _keyList.end(); ++it)
	if (*it == key)
	    break;
    if (it == _keyList.end())
	_keyList.push_back(key);
    mutex.Unlock();
}

void
ExcludeList::add(unsigned key)
{
    std::ostringstream s;
    s << key;
    add(s.str());
}

void
ExcludeList::remove(std::string key)
{
    PthreadMutexHolder mutex;
    int mutex_rc = mutex.Lock(&_excludeListMutex);
    assert(mutex_rc == 0);
    _keyList.remove(key);
    mutex.Unlock();
}

void
ExcludeList::remove(unsigned key)
{
    std::ostringstream s;
    s << key;
    remove(s.str());
}

std::string
ExcludeList::getSqlListQuoted()
{
    std::string sqlList = "";
    std::list<std::string>::iterator it;
    PthreadMutexHolder mutex;
    int mutex_rc = mutex.Lock(&_excludeListMutex);
    assert(mutex_rc == 0);
    if (!_keyList.empty())
    {
	sqlList.append("(");
	for (it = _keyList.begin(); it != _keyList.end(); ++it)
	{
	    if (it != _keyList.begin())
		sqlList.append(",");
	    sqlList.append("'").append(*it).append("'");
	}
	sqlList.append(")");

    }
    mutex.Unlock();
    return sqlList;
}

std::string
ExcludeList::getSqlListUnquoted()
{
    std::string sqlList = "";
    std::list<std::string>::iterator it;
    PthreadMutexHolder mutex;
    int mutex_rc = mutex.Lock(&_excludeListMutex);
    assert(mutex_rc == 0);
    if (!_keyList.empty())
    {
	sqlList.append("(");
	for (it = _keyList.begin(); it != _keyList.end(); ++it)
	{
	    if (it != _keyList.begin())
		sqlList.append(",");
	    sqlList.append(*it);
	}
	sqlList.append(")");

    }
    mutex.Unlock();
    return sqlList;
}


} } // namespace mmcs::server
