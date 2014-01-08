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

#ifndef BGWS_OPTIONS_HPP_
#define BGWS_OPTIONS_HPP_


#include <utility/include/Properties.h>

#include <boost/program_options.hpp>

#include <string>


namespace bgws_clients {


class BgwsOptions
{
public:


    static const std::string DEFAULT_BASE;


    BgwsOptions();

    boost::program_options::options_description& getDesc()  { return _desc; }

    const std::string& getBgPropertiesFileName() const  { return _bg_properties_program_options.getFilename(); }
    bool useSessionFile() const  { return _use_session_file; }
    const std::string& getSessionFilename() const  { return _session_filename; }
    const std::string& getSessionId() const  { return _session_id; }

    const std::string& getBase() const  { return _base; }

    void setBgProperties( bgq::utility::Properties& properties );

    const std::string& getCaCertFilename() const  { return _ca_cert_filename; }


private:

    bgq::utility::Properties::ProgramOptions _bg_properties_program_options;

    bool _use_session_file;
    std::string _session_filename;

    std::string _base;

    std::string _ca_cert_filename;

    std::string _session_id;

    boost::program_options::options_description _desc;


    void _notifySessionFile( const std::string& s );
};


} // namespace bgws_clients

#endif
