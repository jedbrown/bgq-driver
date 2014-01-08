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

#ifndef MMCS_LITE_OPTIONS_H
#define MMCS_LITE_OPTIONS_H

#include <utility/include/LoggingProgramOptions.h>
#include <utility/include/Properties.h>

#include <boost/program_options.hpp>

#include <string>


namespace mmcs {
namespace lite {


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

    /*!
     * \brief Get the default listener flag.
     */
    bool getDefaultListener( ) const { return !_variables_map["no-default-listener"].as<bool>(); }

    /*!
     * \brief Get the bringup flag.
     */
    bool doBringup( ) const { return !_variables_map["no-bringup"].as<bool>(); }

    std::string Host( ) const { return _host_port; }
private:
    /*!
     * \brief Open properties file.
     */
    void openProperties();

private:
    unsigned int _argc;                                             //!<
    char** _argv;                                                   //!<
    boost::program_options::variables_map _variables_map;                      //!<
    boost::program_options::options_description _options;           //!<
    bgq::utility::Properties::ProgramOptions _propertiesOptions;    //!<
    bgq::utility::LoggingProgramOptions _loggingOptions;            //!<
    bool _defaultListener;                                          //!<
    bool _doBringup;                                                //!<
    std::string _host_port;
};

} } // namespace mmcs::lite

#endif
