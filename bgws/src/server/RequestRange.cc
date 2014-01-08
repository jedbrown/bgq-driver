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


#include "RequestRange.hpp"

#include "common/ContentRange.hpp"

#include <db/include/api/cxxdb/cxxdb.h>

#include <utility/include/Log.h>

#include <boost/lexical_cast.hpp>

#include <string>


using boost::lexical_cast;

using std::string;


LOG_DECLARE_FILE( "bgws" );


namespace bgws {


RequestRange::RequestRange(
        const capena::server::Request &request,
        unsigned default_count,
        unsigned max_count
    )
{
    const string &range_str(request.getRange());

    if ( range_str.empty() ) {
        // No range header in request.
        _header_present = false;
        _range = common::ItemRange::createAbs( 0, default_count-1 );
        return;
    }

    // Range header in request, parse it.
    _header_present = true;
    _range = common::ItemRange::parse( range_str, default_count, max_count );
}


void RequestRange::bindParameters(
        cxxdb::Parameters& parameters,
        const std::string& start_parameter_name,
        const std::string& end_parameter_name
    ) const
{
    parameters[start_parameter_name].cast( _range.getStart() + 1 );
    parameters[end_parameter_name].cast( _range.getEnd() + 1 );
}


void RequestRange::updateResponse(
        capena::server::Response &response,
        unsigned response_count,
        unsigned all_count
    ) const
{
    LOG_DEBUG_MSG( "updateResponse with response_count=" << response_count << " all_count=" << all_count );
    if ( (all_count != 0) && (_header_present || (response_count != all_count)) ) {

        common::ContentRange content_range(common::ContentRange::createCount(
                _range.getStart(),
                response_count,
                all_count
            ));

        response.setPartialContent( lexical_cast<string>( content_range ) );
    }
}


} // namespace bgws
