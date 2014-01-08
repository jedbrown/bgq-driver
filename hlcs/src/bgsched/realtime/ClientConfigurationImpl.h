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
#ifndef CLIENT_CONFIGURATION_IMPL_H_
#define CLIENT_CONFIGURATION_IMPL_H_

#include <bgsched/realtime/ClientConfiguration.h>

namespace bgsched {
namespace realtime {

class ClientConfigurationImpl : public ClientConfiguration
{
public:
    ClientConfigurationImpl();

    static const ClientConfigurationImpl INSTANCE;

    const Hosts& getHost() const;
};

} // namespace bgsched::realtime
} // namespace realtime

#endif
