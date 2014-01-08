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
#ifndef RUNJOB_SERVER_CIOS_MESSAGE_H
#define RUNJOB_SERVER_CIOS_MESSAGE_H

#include <db/include/api/job/types.h>

#include <ramdisk/include/services/MessageHeader.h>

#include <boost/shared_array.hpp>
#include <boost/shared_ptr.hpp>

#include <boost/asio/buffer.hpp>

namespace runjob {
namespace server {
namespace cios {

/*!
 * \brief messages sent to the CIOS components.
 *
 * \note type safety is retained when using shared_ptr<void> due to the custom deleter
 * object that is implicitly created with each shared_ptr. That is
 *
 * \code
 * shared_ptr<void> foo( new Bar() )
 * \endcode
 *
 * will correctly invoke Bar::~Bar() when foo goes out of scope.
 *
 * see http://www.boost.org/doc/libs/1_44_0/libs/smart_ptr/sp_techniques.html#pvoid
 */
class Message
{
public:
    /*!
     * \brief Pointer type.
     */
    typedef boost::shared_ptr<Message> Ptr;

    /*!
     * \brief Factory.
     */
    static Ptr create(
            uint16_t type,      //!< [in] message type
            BGQDB::job::Id job, //!< [in] job ID
            uint32_t rank = 0   //!< [in] Rank of compute node, note this is rarely used when coming from runjob_server
            );

    /*!
     * \brief
     */
    static Ptr create();

public:
    /*!
     * \brief ctor.
     */
    Message();

    /*!
     * \brief get buffer.
     */
    boost::asio::const_buffers_1 buffer() const;

    /*!
     * \brief cast message.
     */
    template <typename T>
    boost::shared_ptr<T> as() {
        return boost::static_pointer_cast<T>( _message );
    }

    /*!
     * \brief Get type.
     */
    uint16_t type() const { return _type; }

    /*!
     * \brief Get header.
     */
    const bgcios::MessageHeader* header() const { 
        return reinterpret_cast<bgcios::MessageHeader*>( _message.get() );
    }

    /*!
     * \brief Get header.
     */
    bgcios::MessageHeader* header() { 
        return reinterpret_cast<bgcios::MessageHeader*>( _message.get() );
    }

    /*!
     * \brief Prepare for reading.
     */
    boost::asio::mutable_buffer prepare(
            const bgcios::MessageHeader& header //!< [in]
            );

    /*!
     * \brief
     */
    bool unhandled() const { return _unhandled; }

private:
    boost::shared_ptr<void> _message;
    boost::shared_array<char> _unhandled;
    uint16_t _type;
};

} // cios
} // server
} // runjob

#endif
