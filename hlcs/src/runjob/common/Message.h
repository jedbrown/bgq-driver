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
#ifndef RUNJOB_COMMON_MESSAGE_H
#define RUNJOB_COMMON_MESSAGE_H

#include <db/include/api/job/types.h>

#include <boost/serialization/utility.hpp>
#include <boost/serialization/version.hpp>

#include <boost/shared_ptr.hpp>

#include <sstream>
#include <string>

#include <stdint.h>

namespace runjob {

/*!
 * \brief base class for all messages to derive from.
 */
class Message
{
public:
    /*!
     * \brief possible message types
     */
    enum Type
    {
        Invalid,        //!<
        ExitJob,        //!<
        InsertJob,      //!<
        InsertRas,      //!<
        Handshake,      //!<
        KillJob,        //!<
        PerfCounters,   //!<
        Proctable,      //!<
        Result,         //!<
        StartJob,       //!<
        StartTool,      //!<
        StdError,       //!<
        StdIn,          //!<
        StdOut,         //!<
        NumTypes        //!<
    };

    /*!
     * \brief pointer type.
     */
    typedef boost::shared_ptr<Message> Ptr;

    /*!
     * \brief const pointer type.
     */
    typedef boost::shared_ptr<const Message> ConstPtr;

public:
    /*!
     * \brief convert a message type into a string.
     */
    static const char* toString(
            Type t      //!< [in]
            ) {
        static const char* messages[] = {
            "INVALID",
            "EXIT JOB",
            "INSERT JOB",
            "INSERT RAS",
            "HANDSHAKE",
            "KILL JOB",
            "PERFORMANCE COUNTERS",
            "PROCTABLE",
            "RESULT",
            "START JOB",
            "START TOOL",
            "STANDARD ERROR",
            "STANDARD INPUT",
            "STANDARD OUTPUT"
        };

        if (t >= Invalid && t < NumTypes) {
            return messages[t];
        } else {
            return messages[0];
        }
    }

    /*!
     * \brief Convert to arbitrary type.
     */
    template <typename T>
    static boost::shared_ptr<const T> get(
            const ConstPtr& msg //!< [in]
            ) 
    {
        return boost::static_pointer_cast<const T>( msg );
    }

public:
    /*!
     * \brief ctor
     */
    Message() :
        _type(),
        _jobid( 0 ),
        _clientid( 0 )
    {
    
    }

    /*!
     * \brief dtor.
     */
    virtual ~Message() { }

    /*!
     * \brief serialize the message into a string suitable for transmission across a socket.
     */
    virtual void serialize(
            std::ostream& os
            ) = 0;

    // getters
    Type getType() const { return _type; }  //!< Get Type.
    BGQDB::job::Id getJobId() const { return _jobid; }    //!< Get job ID.
    uint64_t getClientId() const { return _clientid; }  //!< Get client ID.

    // setters
    void setType(Type t) { _type = t; } //!< Set Type.
    void setJobId(BGQDB::job::Id id) { _jobid = id; } //!< Set job ID.
    void setClientId(uint64_t id) { _clientid = id; } //!< Set client ID.

protected:
    Type _type;             //!< message type
    BGQDB::job::Id _jobid;  //!< job ID
    uint64_t _clientid;     //!< runjob client ID

private:
    friend class boost::serialization::access;
    template<class Archive>
    void __attribute__ ((visibility("hidden"))) serialize(Archive & ar, const unsigned int /* file_version */)
    {
        ar & _jobid;
        ar & _clientid;
    }
};

} // runjob

#endif
