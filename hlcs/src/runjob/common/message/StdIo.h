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
#ifndef RUNJOB_COMMON_MESSAGE_STDIO_H
#define RUNJOB_COMMON_MESSAGE_STDIO_H

#include "common/Message.h"
#include "common/Uci.h"

#include <boost/archive/binary_oarchive.hpp>
#include <boost/serialization/base_object.hpp>

#include <string>

namespace runjob {
namespace message {

/*!
 * \brief A single line or block of output from stdin, stdout, or stderr
 */
class StdIo : public Message
{
public:
    /*!
     * \brief pointer type.
     */
    typedef boost::shared_ptr<StdIo> Ptr;

    /*!
     * \brief const pointer type.
     */
    typedef boost::shared_ptr<const StdIo> ConstPtr;

public:
    /*!
     * \brief ctor.
     */
    StdIo() : 
        Message(),
        _data(),
        _length( 0 ),
        _rank( 0 ),
        _location()
    {
    
    }

    /*!
     * \brief
     */
    void serialize(std::ostream& os) {
        boost::archive::binary_oarchive ar(os);
        ar & *this;
    }

    /*!
     * \brief Set data and length.
     */
    void setData(
            const char* data,   //!< [in]
            size_t length       //!< [in]
            )
    {
        _data = std::string(data, length);
        this->setLength( length );
    }

    /*!
     * \brief Set rank.
     */
    void setRank(
            const uint32_t rank //!< [in]
            )
    {
        _rank = rank;
    }

    /*!
     * \brief Set data length.
     */
    void setLength(
            size_t length   //!< [in]
            )
    {
        _length = length;
    }

    /*!
     * \brief Set I/O node location.
     */
    void setLocation(
            const Uci& location    //!< [in]
            )
    {
        _location = location;
    }

    const std::string& getData() const { return _data; }    //!< Get data.
    size_t getLength() const { return _length; }    //!< Get data length.
    uint32_t getRank() const { return _rank; }    //!< Get rank.
    const Uci& getLocation() const { return _location; }    //!< Get I/O node location.

private:
    friend class boost::serialization::access;
    template<class Archive>
    void serialize(
            Archive &ar,                //!< [in] archive
            const unsigned int          //!< [in] version number
            )
    {
        // save/load base class information
        ar & boost::serialization::base_object<Message>(*this);
        ar & _data;
        ar & _length;
        ar & _rank;
        ar & _location;
    }

private:
    std::string _data;
    size_t _length;
    uint32_t _rank;
    Uci _location;
};

} // message
} // runjob

#endif
