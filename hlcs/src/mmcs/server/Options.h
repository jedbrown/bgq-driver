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

#ifndef MMCS_SERVER_OPTIONS_H
#define MMCS_SERVER_OPTIONS_H

#include <utility/include/LoggingProgramOptions.h>
#include <utility/include/Properties.h>

#include <utility/include/portConfiguration/ServerPortConfiguration.h>

#include <boost/program_options.hpp>
#include <boost/shared_ptr.hpp>

#include <string>
#include <vector>

namespace mmcs {
namespace server {

/*!
 * \brief Parse command line program Options.
 */
class Options
{
public:
    /*!
     * \brief ctor.
     *
     * \throws anything that boost::program_options::command_line_parser::run() throws.
     */
    Options(
            unsigned int argc,      //!< [in]
            char** argv             //!< [in]
           );

    boost::shared_ptr<const bgq::utility::ServerPortConfiguration> getServerConfig() const  { return _serverConfig; }
    const std::vector<std::string>& bringupOptions() const { return _bringupOptions; }

private:
    bgq::utility::Properties::ConstPtr openProperties();

    void validate();

private:
    unsigned int                                             _argc;
    char**                                                   _argv;
    boost::program_options::variables_map                    _vm;
    boost::program_options::options_description              _options;
    bgq::utility::Properties::ProgramOptions                 _propertiesOptions;
    bgq::utility::LoggingProgramOptions                      _loggingOptions;
    boost::shared_ptr<bgq::utility::ServerPortConfiguration> _serverConfig;
    std::vector<std::string>                                 _bringupOptions;  // Hardware bring-up options
};

} } // namespace mmcs::server

#endif
