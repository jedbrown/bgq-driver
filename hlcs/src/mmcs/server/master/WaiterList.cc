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

WaiterList::WaiterList() :
    _list_lock(),
    _list(),
    _used_binids_lock(),
    _used_binids()
{

}

void
WaiterList::init()
{
    // circular buffer capacity is the number of configured subnets plus
    // an arbitrary number
    std::string dont_care = "not_init";
    _used_binids.resize(
            common::Properties::getSubnets(dont_care) + 10
            );
    LOG_DEBUG_MSG( "Setting used binary ID capacity to " << _used_binids.capacity() );
}

void
WaiterList::setStat(
        const std::string& alias,
        Status stat
        )
{
    boost::mutex::scoped_lock lock( _list_lock );
    LOG_DEBUG_MSG(alias << " status set to " << stat);
    _list[alias] = stat;
}

WaiterList::Status
WaiterList::getStat(
        const std::string& alias
        ) const
{
    boost::mutex::scoped_lock lock( _list_lock );
    const std::map<std::string, Status>::const_iterator result = _list.find( alias );
    if ( result == _list.end() ) {
        throw std::invalid_argument( alias + " not found" );
    }

    return result->second;
}

std::string
WaiterList::getStopped() const
{
    boost::mutex::scoped_lock lock( _list_lock );
    for (std::map<std::string, Status>::const_iterator i = _list.begin(); i != _list.end(); ++i) {
        if (i->second == STOPPED) {
            return i->first;
        }
    }

    return std::string();
}

std::string
WaiterList::getErrored() const
{
    boost::mutex::scoped_lock lock( _list_lock );
    for (std::map<std::string, Status>::const_iterator i = _list.begin(); i != _list.end(); ++i) {
        if (i->second == ERROR) {
            return i->first;
        }
    }

    return std::string();
}

void
WaiterList::addBinId(
        const std::string& binid
        )
{
    boost::mutex::scoped_lock lock( _used_binids_lock );
    LOG_DEBUG_MSG( "Adding used binary ID " << binid );

    _used_binids.push_back(binid);
}

bool
WaiterList::findBinId(
        const std::string& binid
        ) const
{
    boost::mutex::scoped_lock lock( _used_binids_lock );
    LOG_DEBUG_MSG( "Finding used binary ID " << binid );

    return std::find(
            _used_binids.begin(),
            _used_binids.end(),
            binid
            ) != _used_binids.end();
}

} } } // namespace mmcs::server::master
