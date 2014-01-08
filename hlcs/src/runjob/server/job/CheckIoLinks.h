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
#ifndef RUNJOB_SERVER_JOB_CHECK_IO_LINKS_H
#define RUNJOB_SERVER_JOB_CHECK_IO_LINKS_H

#include "common/Uci.h"

#include "server/cios/Connection.h"
#include "server/cios/fwd.h"

#include "server/fwd.h"

#include <boost/enable_shared_from_this.hpp>
#include <boost/shared_ptr.hpp>

#include <map>
#include <string>

namespace runjob {
namespace server {
namespace job {

class CheckIoLinks : public boost::enable_shared_from_this<CheckIoLinks>
{
    /*!
     * \brief Pointer type.
     */
    typedef boost::shared_ptr<CheckIoLinks> Ptr;

public:
    /*!
     * \brief Factory.
     */
    static void create(
            const boost::shared_ptr<Job>& job  //!< [in]
            );

private:
    CheckIoLinks(
            const boost::shared_ptr<Job>& job
        );

    void execute( 
            const cios::Connection::SocketPtr& socket
            );
    
    void executeImpl( 
            const cios::Connection::SocketPtr& socket
            );

    void updateClient(
            bool result
            );
    
    void updateClientImpl(
            bool result
            );

private:
    typedef std::multimap<Uci, cios::Connection::Ptr> IoLinks;

private:
    const boost::shared_ptr<Job> _job;
    IoLinks _links;
    IoLinks::iterator _link;
};

} // job
} // server
} // runjob

#endif
