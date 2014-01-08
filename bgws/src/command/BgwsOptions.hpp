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

#ifndef BGWS_COMMAND_BGWS_OPTIONS_HPP_
#define BGWS_COMMAND_BGWS_OPTIONS_HPP_


#include "capena-http/http/uri/Uri.hpp"

#include <utility/include/Properties.h>

#include <utility/include/portConfiguration/SslConfiguration.h>

#include <boost/program_options.hpp>
#include <boost/shared_ptr.hpp>

#include <string>


namespace bgws {
namespace command {


class BgwsOptions
{
public:

    static const capena::http::uri::Uri DEFAULT_BASE;


    BgwsOptions();

    boost::program_options::options_description& getDesc()  { return _desc; }

    const std::string& getBgPropertiesFileName() const  { return _bg_properties_program_options.getFilename(); }

    const capena::http::uri::Uri& getBase() const  { return *_base_ptr; }

    const bgq::utility::SslConfiguration& getSslConfiguration() const  { return *_ssl_configuration_ptr; }


    void setBgProperties( bgq::utility::Properties::ConstPtr properties_ptr );


private:

    void _setBase( const std::string& base_str );


    bgq::utility::Properties::ProgramOptions _bg_properties_program_options;

    boost::shared_ptr<capena::http::uri::Uri> _base_ptr;

    boost::program_options::options_description _desc;

    boost::shared_ptr<bgq::utility::SslConfiguration> _ssl_configuration_ptr;

};


} } // namespace bgws::command

#endif
