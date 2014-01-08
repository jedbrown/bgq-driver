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
/* (C) Copyright IBM Corp.  2012, 2012                              */
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

#ifndef BGWS_UTILITY_EZ_TIMER_HPP_
#define BGWS_UTILITY_EZ_TIMER_HPP_


#include <boost/date_time.hpp>
#include <boost/scoped_ptr.hpp>

#include <string>


namespace bgws {
namespace utility {


class EzTimer
{
public:

    typedef boost::scoped_ptr<EzTimer> Ptr;


    EzTimer( const std::string& name );

    ~EzTimer();


private:

    std::string _name;
    boost::posix_time::ptime _start_time;

};


} } // namespace bgws::utility


#define EZTIMER_DECL  bgws::utility::EzTimer::Ptr bgws_utility_EzTimer_Ptr_;
#define EZTIMER_START( text )  bgws_utility_EzTimer_Ptr_.reset( new bgws::utility::EzTimer( std::string() + __FUNCTION__ + ": " + text ) );
#define EZTIMER_STOP  bgws_utility_EzTimer_Ptr_.reset();

#endif
