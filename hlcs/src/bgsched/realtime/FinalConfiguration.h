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
#ifndef FINALCONFIGURATION_H_
#define FINALCONFIGURATION_H_

#include <bgsched/realtime/ClientConfiguration.h>

#include <utility/include/portConfiguration/ClientPortConfiguration.h>

#include <memory>
#include <string>

namespace bgsched {
namespace realtime {

class FinalConfiguration
{
public:
    static const std::string DEFAULT_SERVICE_NAME;

    static const std::string CONFIG_FILE_REALTIME_SECTION_NAME;


    FinalConfiguration( const ClientConfiguration& client_configuration );

    const bgq::utility::ClientPortConfiguration& getPortConfiguration() const  { return _port_config; }

private:

    bgq::utility::ClientPortConfiguration _port_config;
};

} // namespace bgsched::realtime
} // namespace bgsched

#endif
