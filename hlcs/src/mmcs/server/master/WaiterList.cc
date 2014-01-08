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

#include "WaiterList.h"

#include "common/Properties.h"

#include <utility/include/Log.h>

LOG_DECLARE_FILE("mmcs.server");

namespace mmcs {
namespace server {
namespace master {

void
WaiterList::init()
{
    std::string dont_care = "not_init";
    _used_binids_cap = common::Properties::getSubnets(dont_care) + 10;
}

void
WaiterList::setStat(
        const std::string& alias,
        Status stat
        )
{
    PthreadMutexHolder _holder(&_list_lock);
    LOG_DEBUG_MSG(alias << " status set to " << stat);
    _list[alias] = stat;
}

WaiterList::Status
WaiterList::getStat(
        const std::string& alias)
{
    PthreadMutexHolder _holder(&_list_lock);
    return _list[alias];
}

std::string
WaiterList::getStopped()
{
    PthreadMutexHolder _holder(&_list_lock);
    for(std::map<std::string, Status>::const_iterator li = _list.begin();
            li != _list.end(); ++li) {
        if(li->second == STOPPED) {
            return li->first;
        }
    }

    return std::string();
}

std::string
WaiterList::getErrored()
{
    PthreadMutexHolder _holder(&_list_lock);
    for(std::map<std::string, Status>::const_iterator li = _list.begin();
            li != _list.end(); ++li) {
        if(li->second == ERROR) {
            return li->first;
        }
    }

    return std::string();
}

void
WaiterList::addBinId(
        const std::string& binid
        )
{
    _used_binids.push_back(binid);
    if(_used_binids.size() > _used_binids_cap) {
        _used_binids.pop_back();
    }
}

void
WaiterList::removeBinId(
        const std::string& binid
        )
{
    _used_binids.erase(std::remove(_used_binids.begin(), _used_binids.end(), binid));
}

bool
WaiterList::findBinId(
        const std::string& binid
        )
{
    if(std::find(_used_binids.begin(), _used_binids.end(), binid) != _used_binids.end())
        return true;
    else return false;
}

} } } // namespace mmcs::server::master
