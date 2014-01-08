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
#include <utility/include/performance/Mode.h>

#include <utility/include/Log.h>

#include <utility/include/performance.h>

#include <boost/algorithm/string/case_conv.hpp>

LOG_DECLARE_FILE( "utility" );

namespace bgq {
namespace utility {
namespace performance {

const Mode::Value::Type Mode::DefaultMode = Mode::Value::None;

Mode::Value::Type
Mode::getMode() const
{
    Mode::Value::Type result = DefaultMode;

    // get properties
    Properties::ConstPtr properties = getProperties();

    // get mode
    std::string mode;
    try {
        mode = properties->getValue( "performance", "mode" );
    } catch ( const std::invalid_argument& e ) {
        LOG_WARN_MSG( "missing mode key from [performance] section of properties file " << properties->getFilename() );
        LOG_WARN_MSG( "using default mode of " << this->toString( result ) );

        return result;
    }

    // iterate through each mode
    for ( unsigned i = 0; i < static_cast<unsigned>(Value::NumModes); ++i ) {
        // create string for this mode's type
        std::string compare = this->toString( static_cast<Value::Type>(i) );

        // convert both current mode and mode from properties into lower cast for comparison
        boost::algorithm::to_lower( compare );
        boost::algorithm::to_lower( mode );
        if ( mode == compare ) {
            result = static_cast<Value::Type>(i);
            break;
        }
    }

    return result;
}

} // performance
} // utility
} // bgq
