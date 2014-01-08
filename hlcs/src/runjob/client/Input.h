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
#ifndef RUNJOB_CLIENT_INPUT_H
#define RUNJOB_CLIENT_INPUT_H

#include "common/Uci.h"

#include <boost/asio.hpp>
#include <boost/weak_ptr.hpp>

namespace runjob {
namespace client {

class MuxConnection;

/*!
 * \brief Handle input from stdin.
 */
class Input
{
public:
    /*!
     * \brief Pointer type.
     */
    typedef boost::shared_ptr<Input> Ptr;

public:
    /*!
     * \brief Factory method.
     */
    static Ptr create(
            boost::asio::io_service& io_service,            //!< [in]
            const boost::weak_ptr<MuxConnection>& mux,      //!< [in]
            int fd                                          //!< [in]
            );

public:
    /*!
     * \brief dtor.
     */
    virtual ~Input();

    /*!
     * \brief Start an async_read.
     */
    virtual void read(
            uint32_t rank,          //!< [in] rank the read request came from
            size_t length,          //!< [in] number of bytes to read
            const Uci& location     //!< [in] I/O node location the read request came from
            ) = 0;

    /*!
     * \brief Stop an async_read.
     */
    virtual void stop() = 0;

protected:
    /*!
     * \brief ctor.
     */
    Input(
            const boost::weak_ptr<MuxConnection>& mux     //!< [in]
         );

protected:
    const boost::weak_ptr<MuxConnection> _mux;    //!< connection to runjob_mux
};

} // client
} // runjob

#endif

