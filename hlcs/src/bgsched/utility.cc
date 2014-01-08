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

#include "bgsched/utility.h"

#include <utility/include/Log.h>

#include <boost/shared_ptr.hpp>

using bgq::utility::Properties;
using namespace std;

LOG_DECLARE_FILE("bgsched");

namespace boost {

void
assertion_failed(char const * expr, char const * function, char const * file, long line)
{
    LOG_FATAL_MSG("expression " << expr << " failed at " << file << ":" << line << " in " << function);
    abort();
}

} // naespace boost

namespace
{
    Properties::Ptr properties;
} // anonymous namespace

namespace bgsched {

void
setProperties(
        const bgq::utility::Properties::Ptr prop
        )
{
    properties = prop;
}


bgq::utility::Properties::Ptr
getProperties()
{
    return properties;
}


void
logXML(
        const stringstream& os,
        const bool force
        )
{
    if (force) {
    // Fall through since we should always dump XML if we're forced to
    } else {
        // Environmental to dump XML, note value does not matter, as long as it's not NULL
        if (getenv("BG_DUMP_XML") == NULL) {
            return;
        }
    }
    LOG_ERROR_MSG("*** Start of XML output ***\n" << os.str());
    LOG_ERROR_MSG("*** End of XML output ***");
}

} // namespace bgsched
