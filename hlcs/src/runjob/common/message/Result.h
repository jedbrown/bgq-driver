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
#ifndef RUNJOB_COMMON_MESSAGE_RESULT_H
#define RUNJOB_COMMON_MESSAGE_RESULT_H

#include "common/Message.h"
#include "common/error.h"

#include <boost/archive/binary_oarchive.hpp>
#include <boost/serialization/base_object.hpp>

namespace runjob {
namespace message {

/*!
 * \brief Result message used for every request.
 */
class Result : public Message
{
public:
    /*!
     * \brief pointer type
     */
    typedef boost::shared_ptr<Result> Ptr;

    /*!
     * \brief pointer type
     */
    typedef boost::shared_ptr<const Result> ConstPtr;

public:
    /*!
     * \brief ctor.
     */
    Result() : 
        Message(),
        _error(error_code::success),
        _message()
    {
        this->setType(Message::Result);
    }

    // setters
    void setError(error_code::rc error) { _error = error; } //!< Set error_code.
    void setMessage(const std::string& m) { _message = m; } //!< Set error message.

    // getters
    error_code::rc getError() const { return _error; }  //!< Get error_code.
    const std::string& getMessage() const { return _message; }  //!< Get error message.

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
        ar & _error;
        ar & _message;
    }

private:
    error_code::rc _error;
    std::string _message;
};

} // message
} // runjob

#endif
