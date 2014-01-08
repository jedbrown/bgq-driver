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
#ifndef RUNJOB_SERVER_MUX_INSERT_JOB_H_
#define RUNJOB_SERVER_MUX_INSERT_JOB_H_

#include "server/block/fwd.h"

#include "server/mux/fwd.h"

#include "server/performance/Counters.h"

#include "server/fwd.h"

#include "common/message/InsertJob.h"

#include <boost/enable_shared_from_this.hpp>
#include <boost/shared_ptr.hpp>

namespace runjob {
namespace server {
namespace mux {

/*!
 * \brief
 */
class InsertJob : public boost::enable_shared_from_this<InsertJob>
{
public:
    /*!
     * \brief pointer type.
     */
    typedef boost::shared_ptr<InsertJob> Ptr;

    /*!
     * \brief Factory.
     */
    static Ptr create();

    /*!
     * \brief
     */
    InsertJob* server(
            const boost::shared_ptr<Server>& server             //!< [in]
            )
    {
        _server = server;
        return this;
    }

    /*!
     * \brief
     */
    InsertJob* mux(
            const boost::shared_ptr<Connection>& mux           //!< [in]
           )
    {
        _mux = mux;
        return this;
    }

    /*!
     * \brief
     */
    InsertJob* message(
            const boost::shared_ptr<runjob::Message>& message  //!< [in]
            )
    {
        _message = boost::static_pointer_cast<message::InsertJob>( message );
        return this;
    }

    /*!
     * \brief
     */
    InsertJob* clients(
            const boost::shared_ptr<ClientContainer>& clients   //!< [in]
            )
    {
        _clients = clients;
        return this;
    }

    /*!
     * \brief
     */
    InsertJob* hostname(
            const std::string& hostname,                         //!< [in]
            const std::string& shortHostname                     //!< [in]
            )
    {
        _hostname = hostname;
        _shortHostname = shortHostname;
        return this;
    }

    /*!
     * \brief
     */
    void start();

private:
    InsertJob();

    void addClientHandler();
    
    void removeClientHandler(
            bool result
            );

    void securityHandler(
            bool result,
            const performance::Counters::JobContainer::Timer::Ptr& timer,
            const boost::shared_ptr<block::Compute>& block
            );

    void findBlockHandler(
            const boost::shared_ptr<block::Compute>& block
            );

    void blockNotFound();

    const std::string& getBlockStatusString(
            BGQDB::BLOCK_STATUS status
            ) const;

private:
    boost::shared_ptr<Server> _server;
    boost::shared_ptr<Connection> _mux;
    boost::shared_ptr<message::InsertJob> _message;
    boost::shared_ptr<ClientContainer> _clients;
    std::string _hostname;
    std::string _shortHostname;
};

} // mux
} // server
} // runjob

#endif
