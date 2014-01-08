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
#ifndef RUNJOB_COMMON_MESSAGE_PROCTABLE_H
#define RUNJOB_COMMON_MESSAGE_PROCTABLE_H

#include "common/tool/Proctable.h"

#include "common/Message.h"
#include "common/Uci.h"

#include <boost/archive/binary_oarchive.hpp>

#include <boost/serialization/base_object.hpp>
#include <boost/serialization/map.hpp>
#include <boost/serialization/vector.hpp>

#include <map>
#include <vector>

namespace runjob {
namespace message {

/*!
 * \brief 
 */
class Proctable : public Message
{
public:
    /*!
     * \brief pointer type
     */
    typedef boost::shared_ptr<Proctable> Ptr;

    /*!
     * \brief pointer type
     */
    typedef boost::shared_ptr<const Proctable> ConstPtr;

    /*!
     * \brief I/O uci to IP address mapping.
     */
    typedef std::map<Uci, std::string> Io;

public:
    /*!
     * \brief ctor.
     */
    Proctable() : 
        Message(),
        _id( 0 ),
        _io(),
        _proctable()
    {
        this->setType(Message::Proctable);
    }

    /*!
     * \brief
     */
    void serialize(std::ostream& os) {
        boost::archive::binary_oarchive ar(os);
        ar & *this;
    }

private:
    friend class boost::serialization::access;
    template<class Archive>
    void serialize(
            Archive &ar,                //!< [in] archive
            const unsigned int          //!< [in] version number
            )
    {
        ar & boost::serialization::base_object<Message>(*this);
        ar & _id;
        ar & _io;
        ar & _proctable;
    }

public:
    unsigned _id;   //!< tool ID
    Io _io;
    tool::Proctable _proctable;
};

} // message
} // runjob

#endif
