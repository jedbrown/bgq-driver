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
#ifndef RUNJOB_CLIENT_OUTPUT_H
#define RUNJOB_CLIENT_OUTPUT_H

#include "common/message/StdIo.h"

#include <boost/asio.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/scoped_ptr.hpp>

#include <deque>

namespace runjob {
namespace client {

namespace options {

class Parser;

} // options

/*!
 * \brief Handle output for stdout or stderr.
 */
class Output : public boost::enable_shared_from_this<Output>
{
public:
    /*!
     * \brief Pointer type.
     */
    typedef boost::shared_ptr<Output> Ptr;

public:
    /*!
     * \brief Factory method.
     */
    static Ptr create(
            const boost::shared_ptr<const options::Parser>& options,    //!< [in]
            boost::asio::io_service& io_service,                        //!< [in]
            int fd,                                                     //!< [in]
            const char* label                                           //!< [in]
            );

public:
    /*!
     * \brief dtor.
     */
    virtual ~Output();

    /*!
     * \brief Start an async_write.
     */
    void write(
            const message::StdIo::ConstPtr& msg //!< [in]
            );

protected:
    /*!
     * \brief ctor.
     */
    Output(
            const boost::shared_ptr<const options::Parser>& options,
            const char* label
         );

    /*!
     * \brief Handle a write.
     */
    void writeHandler(
            const boost::system::error_code& error, //!< [in]
            size_t length                           //!< [in]
            );

private:
    virtual void write() = 0;

protected:
    std::deque<message::StdIo::ConstPtr> _outbox;           //!< outgoing message queue
    boost::shared_ptr<const options::Parser> _options;      //!< program options
    const char* _label;                                     //!< label for stdout/stderr
    const char* _longPrefix;                                //!<
    const char* _longSuffix;                                //!<
    const char* _shortSuffix;                               //!<
    std::string _rank;                                      //!< rank of message in label
    bool _addLabel;                                         //!<
};

} // client
} // runjob

#endif

