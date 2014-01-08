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
#ifndef BGSCHED_REALTIME_ABSTRACT_MESSAGE_H_
#define BGSCHED_REALTIME_ABSTRACT_MESSAGE_H_

#include <boost/shared_ptr.hpp>

#include <boost/serialization/access.hpp>
#include <boost/serialization/assume_abstract.hpp>

#include <string>

namespace bgsched {
namespace realtime {

class AbstractMessage
{
public:

    typedef boost::shared_ptr<AbstractMessage> Ptr;
    typedef boost::shared_ptr<const AbstractMessage> ConstPtr;


    static std::string toString( ConstPtr ptr );
    static Ptr fromString( const std::string& s );

    virtual ~AbstractMessage();

private:

    static void _escape( std::string& msg_in_out );
    static void _unescape( std::string& msg_in_out );


    friend class boost::serialization::access;

    template<class Archive>
    void serialize( Archive & /*ar*/, const unsigned int /*version*/ )
    { /* Nothing to do */ }
};

BOOST_SERIALIZATION_ASSUME_ABSTRACT(AbstractMessage)

} } // namespace bgsched::realtime


#endif
