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
#include <utility/include/performance.h>

#include <utility/include/Log.h>

LOG_DECLARE_FILE( "utility" );

// anonymous namespace for properties storage
namespace {
    bgq::utility::Properties::ConstPtr properties;
}

namespace bgq {
namespace utility {
namespace performance {

Properties::ConstPtr
getProperties()
{
    if ( !properties ) {
        // warn user and create properties
        LOG_WARN_MSG( "call bgq::utility::performance::init to initialize API" );
        properties = bgq::utility::Properties::create();
    }
    return properties; 
}

void
init(
        Properties::ConstPtr p
    )
{
    if ( p ) {
        LOG_DEBUG_MSG( "initialized with '" << p->getFilename() << "'" );
    }
    properties = p;
}
    
} // performance
} // utility
} // bgq
