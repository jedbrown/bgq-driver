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

#ifndef BGWS_COMMAND_JOBS_SUMMARY_FILTER_HPP_
#define BGWS_COMMAND_JOBS_SUMMARY_FILTER_HPP_


#include "capena-http/http/uri/Query.hpp"

#include <boost/program_options.hpp>

#include <string>
#include <vector>


namespace bgws {
namespace command {


class JobsSummaryFilter
{
public:

    JobsSummaryFilter();

    void addOptionsTo(
            boost::program_options::options_description& desc
        );

    capena::http::uri::Query calcQuery() const;


private:

    std::string _sort;
    std::string _block_id;
    std::string _executable;
    std::string _statuses;
    std::string _user;
    std::string _start_time;
    std::string _end_time;
    std::string _client;

    bool _exit_status_set;
    int _exit_status;


    void _notifyStatusValues( const std::vector<std::string>& status_args );

    void _notifyExitStatus( int exit_status );
};


} } // namespace bgws::command

#endif
