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

/*!
 * \file realtime/types.h
 */

#ifndef BGSCHED_REALTIME_TYPES_H_
#define BGSCHED_REALTIME_TYPES_H_

#include <stdint.h>

#include <string>

namespace bgsched {
namespace realtime {

/*! \brief RAS severities. */
struct RasSeverity {
    /*! \brief Values for RAS severities. */
    enum Value {
        INFO, /*!< Informational */
        WARN, /*!< Warning */
        FATAL, /*!< Fatal */
        UNKNOWN /*!< Unknown */
    };
};

typedef uint64_t RasRecordId; //!< RAS event IDs.

} // namespace bgsched::realtime
} // namespace bgsched

#endif
