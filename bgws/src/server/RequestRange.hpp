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

#ifndef BGWS_REQUEST_RANGE_HPP_
#define BGWS_REQUEST_RANGE_HPP_


#include "common/ItemRange.hpp"

#include "capena-http/server/Request.hpp"
#include "capena-http/server/Response.hpp"

#include <db/include/api/cxxdb/fwd.h>


namespace bgws {


class RequestRange
{
public:

    RequestRange(
            const capena::server::Request &request,
            unsigned default_count = 50,
            unsigned max_count = 100
        );


    const common::ItemRange& getRange() const  { return _range; }
    bool isHeaderPresent() const  { return _header_present; }


    void bindParameters(
            cxxdb::Parameters& parameters,
            const std::string& start_parameter_name,
            const std::string& end_parameter_name
        ) const;

    /*! Update the response.
     *
     *  HTTP says
     *  - if request had Range header then response must have ContentRange.
     *  - if didn't respond with all items then must have ContentRange.
     *  - otherwise don't put ContentRange.
     */
    void updateResponse(
            capena::server::Response &response,
            unsigned response_count,
            unsigned all_count
        ) const;


private:

    common::ItemRange _range;

    bool _header_present;

};

} // namespace bgws


#endif
