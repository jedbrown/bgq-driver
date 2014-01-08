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
/* (C) Copyright IBM Corp.  2011, 2011                              */
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

#ifndef BGWS_JOB_ID_OPTION_HPP_
#define BGWS_JOB_ID_OPTION_HPP_


#include <db/include/api/job/types.h>

#include <boost/optional.hpp>

#include <iosfwd>


namespace bgws {


class JobIdOption
{
public:

    typedef boost::optional<BGQDB::job::Id> JobIdOpt;


    void set( BGQDB::job::Id id )  { _job_id_opt = id; }

    const JobIdOpt& get() const  { return _job_id_opt; }


private:

    JobIdOpt _job_id_opt;

};


std::istream& operator>>( std::istream& is, JobIdOption& job_id_option_out );


} // namespace bgws


#endif
