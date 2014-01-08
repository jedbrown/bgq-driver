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
#ifndef RUNJOB_SERVER_HANDLERS_LOCATE_RANK_EXCEPTION_H
#define RUNJOB_SERVER_HANDLERS_LOCATE_RANK_EXCEPTION_H

#include <hlcs/include/runjob/commands/error.h>

#include <stdexcept>

namespace runjob {
namespace server {
namespace handlers {
namespace locate_rank {

/*!
 * \brief
 */
class Exception : public std::runtime_error
{
public:
    /*!
     * \brief Ctor.
     */
    Exception(
            const std::string& what,        //!< [in]
            runjob::commands::error::rc     //!< [in]
            );

    /*!
     * \brief
     */
    runjob::commands::error::rc error() const { return _error; }

private:
    const runjob::commands::error::rc _error;
};

} // locate_rank
} // handlers
} // server
} // runjob

#endif
