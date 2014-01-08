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
#ifndef RUNJOB_COMMON_LOG_SIGNAL_INFO_H
#define RUNJOB_COMMON_LOG_SIGNAL_INFO_H

#include <csignal>

namespace runjob {

/*!
 * \brief Log information about what uid sent a signal.
 */
class LogSignalInfo
{
public:
    /*!
     * \brief ctor.
     */
    LogSignalInfo(
            const siginfo_t& info   //!< [in]
            );
};

} // runjob

#endif
