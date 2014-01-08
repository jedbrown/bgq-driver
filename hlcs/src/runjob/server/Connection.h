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
#ifndef RUNJOB_SERVER_CONNECTION_H
#define RUNJOB_SERVER_CONNECTION_H

#include "common/Connection.h"

#include "server/fwd.h"

#include <utility/include/portConfiguration/Acceptor.h>

#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>

namespace runjob {
namespace server {

/*!
 * \brief Base class for connections.
 */
class Connection : public runjob::Connection
{
public:
    /*!
     * \brief ctor.
     *
     * \pre socket != NULL
     * \pre server != NULL
     */
    Connection(
            const bgq::utility::portConfig::SocketPtr& socket,  //!< [in]
            const boost::shared_ptr<Server>& server             //!< [in]
            );

    /*!
     * \brief dtor.
     */
    virtual ~Connection();

private:
    /*!
     * \brief
     */
    void resolveComplete();

    virtual void addComplete();

protected:
    const Options& _options;                            //!< Program options
    boost::weak_ptr<Server> _server;                    //!< Server object
};

} // server
} // runjob

#endif
