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

#include "Behavior.h"

CxxSockets::Host
Behavior::findFailoverTarget(
        const CxxSockets::Host& failed_host
        )
{
    std::map<CxxSockets::Host, CxxSockets::Host>::iterator twoit = _pairs.find(failed_host);
    if (twoit != _pairs.end()) {
        return twoit->second;
    } else {
        CxxSockets::Host bogus;
        return bogus;
    }
}
