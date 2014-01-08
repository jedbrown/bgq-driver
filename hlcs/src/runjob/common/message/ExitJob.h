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
#ifndef RUNJOB_COMMON_MESSAGE_EXIT_JOB_H
#define RUNJOB_COMMON_MESSAGE_EXIT_JOB_H

#include "common/Message.h"

#include "common/Coordinates.h"
#include "common/error.h"
#include "common/Uci.h"

#include <boost/archive/binary_oarchive.hpp>

#include <boost/serialization/base_object.hpp>
#include <boost/serialization/map.hpp>
#include <boost/serialization/vector.hpp>

namespace runjob {
namespace message {

/*!
 * \brief Message used to describe how a job has exited.
 */
class ExitJob: public Message
{
public:
    /*!
     * \brief pointer type.
     */
    typedef boost::shared_ptr<ExitJob> Ptr;

    /*!
     * \brief const pointer type.
     */
    typedef boost::shared_ptr<const ExitJob> ConstPtr;

    /*!
     * \brief Node location information.
     */
    class Node
    {
    public:
        Uci _location;
        Coordinates _coordinates;

    private:
        friend class boost::serialization::access;
        template<class Archive>
        void serialize(
                Archive &ar,                //!< [in] archive
                const unsigned int          //!< [in] version number
            )
        {
            ar & _location;
            ar & _coordinates;
        }
    };

    /*!
     * \brief Container of compute node locations.
     */
    typedef std::vector<Node> Nodes;

public:
    /*!
     * \brief ctor
     */
    ExitJob() :
        Message(),
        _status( 0 ),
        _error( error_code::success ),
        _message(),
        _nodes(),
        _rasCount(),
        _rasMessage(),
        _rasSeverity()
    {
        this->setType(Message::ExitJob);
    }

    /*!
     * \brief
     */
    void serialize(std::ostream& os) {
        boost::archive::binary_oarchive ar(os);
        ar & *this;
    }

    int _status;
    error_code::rc _error;
    std::string _message;
    Nodes _nodes;
    std::map<std::string, unsigned> _rasCount;
    std::string _rasMessage;
    std::string _rasSeverity;

private:
    friend class boost::serialization::access;
    template<class Archive>
    void serialize(
            Archive &ar,                //!< [in] archive
            const unsigned int          //!< [in] version number
            )
    {
        ar & boost::serialization::base_object<Message>(*this);
        ar & _status;
        ar & _error;
        ar & _message;
        ar & _nodes;
        ar & _rasCount;
        ar & _rasMessage;
        ar & _rasSeverity;
    }
};

} // message
} // runjob

#endif
