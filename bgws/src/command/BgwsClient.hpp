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

#ifndef BGWS_COMMAND_BGWS_CLIENT_HPP_
#define BGWS_COMMAND_BGWS_CLIENT_HPP_


#include "BgwsOptions.hpp"
#include "JobsSummaryFilter.hpp"

#include "common/common.hpp"
#include "common/ItemRange.hpp"
#include "common/RefreshBgwsServerConfiguration.hpp"

#include "chiron-json/json.hpp"

#include <db/include/api/GenBlockParams.h>

#include <db/include/api/job/types.h>

#include <boost/optional.hpp>

#include <map>
#include <string>

#include <stdint.h>


namespace bgws {
namespace command {


class BgwsClient
{
public:

    typedef std::map<std::string,std::string> LoggingSettings;


    struct GenBlockParamsType {
        enum Value {
            LARGE,
            SMALL,
            MIDPLANES,
            NODE_BOARDS
        };
    };


    BgwsClient();


    boost::program_options::options_description& getDesc()  { return _options.getDesc(); }

    void notifyOptionsSet();
    bgq::utility::Properties::ConstPtr getBgProperties() const  { return _bg_properties_ptr; }
    void notifyLoggingIsInitialized();


    void createBlock(
            const BGQDB::GenBlockParams& gen_block_params,
            const std::string& description,
            GenBlockParamsType::Value gen_block_params_type
        );

    json::ArrayValuePtr getBlocksSummary(
            const std::string& status,
            common::blocks_query::Type block_type
        );

    json::ObjectValuePtr getBlockDetails( const std::string& block_id );

    void deleteBlock( const std::string& block_id );


    void getJobsSummary(
            const JobsSummaryFilter& filter,
            const boost::optional<bgws::common::ItemRange>& range_opt,
            json::ArrayValuePtr* value_ptr_out,
            uint64_t* total_jobs_out,
            uint64_t* content_range_end_out
        );

    json::ObjectValuePtr getJobDetails( BGQDB::job::Id job_id );


    json::ObjectValuePtr getBgwsServerStatus();


    json::ObjectValuePtr getLogging();
    void setLogging( const LoggingSettings& logging_settings );


    void refreshBgwsServerConfiguration(
            const ::bgws::common::RefreshBgwsServerConfiguration& refresh_bgws_server_configuration
        );


    json::ArrayValuePtr getRas(
            const std::string& block,
            BGQDB::job::Id job_id
        );


    ~BgwsClient();


private:


    static json::ValuePtr _genBlockParamsToJson(
            const BGQDB::GenBlockParams& gen_block_params,
            const std::string& description,
            GenBlockParamsType::Value gen_block_params_type
        );


    capena::http::uri::Uri _calcReqUri(
            const capena::http::uri::Path& path,
            const capena::http::uri::Query& query = capena::http::uri::Query()
        );


    BgwsOptions _options;
    bgq::utility::Properties::Ptr _bg_properties_ptr;

};


} } // namespace bgws::command


#endif
