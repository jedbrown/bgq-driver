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

#include "AgentBase.h"

#include "BinaryController.h"

#include <utility/include/Log.h>

#include <boost/foreach.hpp>

LOG_DECLARE_FILE("master");

bool
AgentBase::find_binary(
        const BinaryId& id,
        BinaryControllerPtr& p
        ) const
{
    LOG_TRACE_MSG(__FUNCTION__);
    bool foundit = false;
    BOOST_FOREACH(const BinaryControllerPtr& bincont, _binaries) {
        if (id == bincont->get_binid()) {
            p = bincont;
            foundit = true;
        }
    }
    return foundit;
}

bool
AgentBase::runningAlias(
        const std::string &alias_name
        ) const
{
    LOG_TRACE_MSG(__FUNCTION__);
    BOOST_FOREACH(const BinaryControllerPtr& bptr, _binaries) {
        if (bptr->get_alias_name() == alias_name)
            return true;
    }
    return false;
}

void
AgentBase::addController(
        const BinaryControllerPtr& controller
        )
{
    if ( controller->get_alias_name() == "mmcs_server" ) {
        // mmcs_server goes first
        _binaries.insert( _binaries.begin(), controller );
    } else {
        // insert the other binaries before mc_server
        const Binaries::iterator mc_server = std::find_if(
                _binaries.begin(),
                _binaries.end(),
                boost::bind(
                    &BinaryController::get_alias_name,
                    _1
                    ) == "mc_server"
                );
        _binaries.insert( mc_server, controller );
    }
}

void
AgentBase::removeController(
        const BinaryControllerPtr& controller
        )
{
    _binaries.erase(
            std::remove(
                _binaries.begin(),
                _binaries.end(),
                controller
                ),
            _binaries.end()
            );
}
