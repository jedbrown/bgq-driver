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

#include "Ids.h"

#include "../lib/exceptions.h"

#include <utility/include/cxxsockets/exception.h>

#include <sstream>

#include <stdlib.h>

Id::Id()
{
    // Nothing to do
}

Id::Id(
        const Id& id
        ) :
    _id(id._id)

{

}

Id::Id(
        const std::string& id_string
        ) :
    _id(id_string)
{
}

Id&
Id::operator=(
        const Id& id
        )
{
    _id = id._id;
    return *this;
}

bool
Id::operator<(
        const Id& id
        ) const
{
    if (_id < id._id) {
        return true;
    }  else {
        return false;
    }
}

bool
Id::operator==(
        const Id& id
        ) const
{
    if (_id == id._id) {
        return true;
    } else {
        return false;
    }
}

CxxSockets::Host
Id::get_host() const
{
    size_t newindex = 0;
    newindex = _id.find_last_of(':');
    try {
        size_t index = 0;
        CxxSockets::Host h(_id.substr(index, newindex - index));
        return h;
    } catch ( CxxSockets::Error& e ) {
        throw exceptions::APIUserError(exceptions::INFO, e.what());
    }
}

bool
Id::Comp::operator() (
        const Id& lhs,
        const Id& rhs
        )
{
    if (lhs._id < rhs._id) {
        return true;
    } else {
        return false;
    }
}

BGAgentId::BGAgentId(
        const std::string& id_string
        )
{
    _id = id_string;
}

BGAgentId::BGAgentId(
        const BGAgentId& id
        ) :
    Id()
{
    _id = id._id;
}

BGAgentId::BGAgentId(
        const int port,
        const std::string& host
        )
{
    std::ostringstream p;
    p << port;
    _id = host + ":" + p.str();
}

BGAgentId::BGAgentId(
        const std::string& port,
        const std::string& host
        )
{
    _id = host + ":" + port;
}

// client ids

ClientId::ClientId(
        const std::string& id_string
        )
{
    _id = id_string;
}

ClientId::ClientId(
        const ClientId& id
        ) :
    Id()
{
    _id = id._id;
}

ClientId::ClientId(
        const int port,
        const std::string& host
        )
{
    std::ostringstream p;
    p << port;
    _id = host + ":" + p.str();
}

ClientId::ClientId(
        const std::string& port,
        const std::string& host
        )
{
    _id = host + ":" + port;
}

BinaryId::BinaryId(
        const std::string& id_string
        )
{
    _id = id_string;
}

BinaryId::BinaryId(
        const BinaryId& id
        ) :
    Id()
{
    _id = id._id;
}

BinaryId::BinaryId(
        const int pid,
        const std::string& host
        )
{
    std::ostringstream p;
    p << pid;
    _id = host + ":" + p.str();
}

BinaryId::BinaryId(
        const std::string& pid,
        const std::string& host
        )
{
    _id = host + ":" + pid;
}

int
BinaryId::get_pid()
{
    size_t newindex = 0;
    newindex = _id.find_last_of(':');
    std::string pidstr = _id.substr(newindex + 1, _id.length());
    return atoi(pidstr.c_str());
}
