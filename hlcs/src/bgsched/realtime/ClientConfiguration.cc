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
#include <bgsched/realtime/ClientConfiguration.h>

#include "ClientConfigurationImpl.h"

using namespace std;

namespace bgsched {
namespace realtime {

const ClientConfiguration::Hosts ClientConfiguration::DEFAULT_HOST;


const ClientConfiguration& ClientConfiguration::getDefault()
{
    return ClientConfigurationImpl::INSTANCE;
}


ClientConfiguration::~ClientConfiguration()
{
    // Nothing to do.
}


} // namespace bgsched::realtime

} // namespace bgsched
