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

#include <bgsched/InternalException.h>

#include "bgsched/NodeImpl.h"
#include "bgsched/utility.h"

#include <utility/include/Log.h>
#include <utility/include/XMLEntity.h>

#include <boost/lexical_cast.hpp>

using namespace bgsched;
using namespace std;

LOG_DECLARE_FILE("bgsched");

namespace bgsched {

Node::Impl::Impl(
        const string& location,
        const XMLEntity* XMLEntityPtr
        ) :
    Hardware::Impl(location)
{
    _state = convertDatabaseState(XMLEntityPtr->attrByName("status"));
    try {
        _sequenceId = boost::lexical_cast<SequenceId>(XMLEntityPtr->attrByName("statusSeqID"));
    } catch (const boost::bad_lexical_cast& e) {
        LOG_INFO_MSG(e.what());
        THROW_EXCEPTION(
                bgsched::InternalException,
                bgsched::InternalErrors::XMLParseError,
                "Unexpected error parsing XML."
        );
    }
}

Node::Impl::Impl(
        const std::string& location,
        const Hardware::State state,
        const SequenceId sequenceId
        ) :
    Hardware::Impl(location)
{
    _state = state;
    _sequenceId = sequenceId;
}

} // namespace bgsched
