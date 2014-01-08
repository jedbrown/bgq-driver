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

#include "bgsched/HardwareConfig.h"
#include "bgsched/utility.h"

#include <utility/include/Log.h>
#include <utility/include/Properties.h>

#include <boost/lexical_cast.hpp>

#include <stdexcept>
#include <string>

LOG_DECLARE_FILE( "bgsched" );

using namespace bgsched;
using namespace std;

namespace {
const string BGSchedSection = "bgsched.hardware";
const string MCSection = "machinecontroller";
}

namespace bgsched {

HardwareConfig::HardwareConfig() :
    _noDb(true),
    _rows(0),
    _columns(0)
{
    const bgq::utility::Properties& properties = bgsched::getProperties();
    LOG_DEBUG_MSG( "Getting config from file '" << properties.getFilename() << "'." );
    string db_str;
    try {
        db_str = properties.getValue( BGSchedSection, "db" );
    } catch (const std::invalid_argument& e) {
        LOG_WARN_MSG( e.what() );
    }

    bool db(false);

    if (db_str == string()) {
        LOG_INFO_MSG(
                "The properties file '" << properties.getFilename() << "' doesn't contain a db setting in the hardware section."
                " The default value of false will be used."
        );
    } else if (db_str == "yes" || db_str == "true") {
        db = true;
    } else if (db_str == "no" || db_str == "false") {
        db = false;
    } else {
        try {
            db = boost::lexical_cast<bool>(db_str);
        } catch ( const boost::bad_lexical_cast& e ) {
            LOG_WARN_MSG(
                    "The properties file '" << properties.getFilename() << "' contains an invalid db setting value in the hardware section."
                    " The value must be a boolean, but is '" << db_str << "'."
                    " The default value of false will be used."
            );
        }
    }

    if(db) {
        _noDb = false;
        // Note: Row and column count must be set at some point before object is used
    }

    if(_noDb) {
        try {
            _rows = boost::lexical_cast<uint32_t>(properties.getValue( MCSection, "rackRows" ));
            _columns = boost::lexical_cast<uint32_t>(properties.getValue( MCSection, "rackColumns" ));
        } catch (const std::invalid_argument& e) {
            LOG_WARN_MSG( e.what() );
            // Use sensible defaults
            _rows = 1;
            _columns = 1;
            LOG_INFO_MSG( "Defaulting to " << _rows << " rows and " << _columns << " columns" );
        } catch (const boost::bad_lexical_cast& e) {
            LOG_WARN_MSG( e.what() );
            // Use sensible defaults
            _rows = 1;
            _columns = 1;
            LOG_INFO_MSG( "Defaulting to " << _rows << " rows and " << _columns << " columns" );
        }
    }
}

bool
HardwareConfig::getNoDatabase() const
{
    return _noDb;
}

uint32_t
HardwareConfig::getRows() const
{
    return _rows;
}

uint32_t
HardwareConfig::getColumns() const
{
    return _columns;
}

void
HardwareConfig::setRows(
        const uint32_t rows
        )
{
    _rows = rows;
}

void
HardwareConfig::setColumns(
        const uint32_t columns
        )
{
    _columns = columns;
}

} // namespace bgsched
