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
#ifndef RUNJOB_SERVER_JOB_END_OF_FILE_H
#define RUNJOB_SERVER_JOB_END_OF_FILE_H

#include "common/Uci.h"

#include "server/mux/fwd.h"

#include "server/fwd.h"

#include <hlcs/include/runjob/commands/error.h>

#include <boost/shared_ptr.hpp>

namespace runjob {
namespace message {

class StdIo;

} // message

namespace server {
namespace job {

/*!
 * \brief 
 */
class EndOfFile
{
public:
    /*!
     * \brief Pointer type.
     */
    typedef boost::shared_ptr<EndOfFile> Ptr;

public:
    /*!
     * \brief Factory.
     */
    static void create(
            const boost::shared_ptr<Job>& job,  //!< [in]
            const Uci& location                 //!< [in]
            );

private:
    EndOfFile(
            const boost::shared_ptr<Job>& job
         );

    void impl(
            const Uci& location
            );
    
private:
    boost::shared_ptr<Job> _job;
};

} // job
} // server
} // runjob

#endif
