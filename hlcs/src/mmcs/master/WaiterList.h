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

#ifndef MASTER_WAITER_LIST_H
#define MASTER_WAITER_LIST_H

#include <bgq_util/include/pthreadmutex.h>

#include <algorithm>
#include <map>
#include <string>
#include <vector>

#include <boost/utility.hpp>

namespace mmcs {
namespace master {

class WaiterList : private boost::noncopyable {
public:
    enum Status { STOPPED, WAIT_FOR_START, RUNNING, ERROR };
    void init();

    Status getStat(const std::string& alias);
    
    void setStat(const std::string& alias, Status stat);

    std::string getStopped();

    std::string getErrored();

    void addBinId(const std::string& binid);

    void removeBinId(const std::string& binid);

    bool findBinId(const std::string& binid);

private:
    PthreadMutex _list_lock;
    std::map<std::string, Status> _list;
    unsigned _used_binids_cap;
    std::vector<std::string> _used_binids;
};

} // master
} // mmcs

#endif

