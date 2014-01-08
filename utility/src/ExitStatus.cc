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


#include "ExitStatus.h"

#include <boost/lexical_cast.hpp>

#include <iostream>


using boost::lexical_cast;

using std::string;


namespace bgq {
namespace utility {


const ExitStatus ExitStatus::Normal(0);


std::ostream& operator<<( std::ostream& os, const ExitStatus& exit_status )
{
    os << (exit_status.exited() ? (string() + "status " + lexical_cast<string>(exit_status.getExitStatus())) :
           exit_status.signaled() ? (string() + "signal " + lexical_cast<string>(exit_status.getSignal())) :
           (string() + "unknown " + lexical_cast<string>(exit_status.get())));

    return os;
}

} } // namespace bgq::utility
