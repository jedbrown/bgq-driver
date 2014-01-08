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
#ifndef RUNJOB_SERVER_MUX_KILL_H
#define RUNJOB_SERVER_MUX_KILL_H

#include "server/mux/fwd.h"

#include "server/fwd.h"

#include <boost/enable_shared_from_this.hpp>
#include <boost/shared_ptr.hpp>

#include <string>

namespace runjob {

class Message;

namespace message {

class KillJob;

} // message

namespace server {
namespace mux {

/*!
 * \brief
 */
class Kill : public boost::enable_shared_from_this<Kill>
{
public:
    /*!
     * \brief pointer type.
     */
    typedef boost::shared_ptr<Kill> Ptr;

    /*!
     * \brief Factory.
     */
    static Ptr create();

    Kill* message(
            const boost::shared_ptr<runjob::Message>& message  //!< [in]
            )
    {
        _message = boost::static_pointer_cast<message::KillJob>( message );
        return this;
    }

    Kill* hostname(
            const std::string& hostname                         //!< [in]
            )
    {
        _hostname = hostname;
        return this;
    }

    Kill* clients(
            const boost::shared_ptr<ClientContainer>& clients   //!< [in]
            )
    {
        _clients = clients;
        return this;
    }

    void start();

private:
    Kill();

    void clientHandler(
            bool result
            );
private:
    boost::shared_ptr<message::KillJob> _message;
    boost::shared_ptr<ClientContainer> _clients;
    std::string _hostname;
};

} // mux
} // server
} // runjob

#endif
