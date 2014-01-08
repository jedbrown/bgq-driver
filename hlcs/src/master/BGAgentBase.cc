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

#include <utility/include/Log.h>
#include "BinaryController.h"
#include "BGAgentBase.h"
#include "BinaryController.h"
#include "Host.h"
#include "Alias.h"

LOG_DECLARE_FILE("master");

bool BGAgentBase::find_binary(BinaryId& id, BinaryControllerPtr& p) {
    LOG_INFO_MSG(__FUNCTION__);
    bool foundit = false;
    BOOST_FOREACH(BinaryControllerPtr& bincont, _binaries) {
        if(id == bincont->get_binid()) {
            p = bincont;
            foundit = true;
        }
    }
    return foundit;
}

bool BGAgentBase::runningAlias(AliasPtr& al) {
    LOG_INFO_MSG(__FUNCTION__);
    BOOST_FOREACH(BinaryControllerPtr& bptr, _binaries) {
        if(bptr->get_alias_name() == al->get_name())
            return true;
    }
    return false;
}
