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
/* (C) Copyright IBM Corp.  2012 2012                              */
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

#include "bgsched/IODrawerImpl.h"
#include "bgsched/IONodeImpl.h"

#include <db/include/api/cxxdb/cxxdb.h>

#include <db/include/api/tableapi/gensrc/bgqtableapi.h>

#include <vector>

using namespace bgsched;
using namespace std;

namespace bgsched {

IODrawer::Pimpl
IODrawer::Impl::createFromDatabase(
        const cxxdb::Columns& IODrawer_cols
)
{
    IODrawer::Pimpl ret(new IODrawer::Impl(IODrawer_cols));
    return ret;
}

IODrawer::Impl::Impl(
        const cxxdb::Columns& IODrawer_cols
) :
    Hardware::Impl(IODrawer_cols[BGQDB::DBTIodrawer::LOCATION_COL].getString()),
    _availableIONodeCount(0),
    _IONodes()
{
    _state = Hardware::Impl::convertDatabaseState(IODrawer_cols[BGQDB::DBTIodrawer::STATUS_COL].getString().c_str());
    _sequenceId= IODrawer_cols[BGQDB::DBTIodrawer::SEQID_COL].as<SequenceId>();
}

IODrawer::Impl::Impl(
        const IODrawer::Pimpl fromIODrawer
) :
    Hardware::Impl(fromIODrawer->getLocationString()),
    _availableIONodeCount(fromIODrawer->getAvailableIONodeCount()),
    _IONodes()
{
    _state = fromIODrawer->getState();           // Set I/O drawer hardware state
    _sequenceId = fromIODrawer->getSequenceId(); // Set I/O drawer sequence ID

    // Set I/O nodes
    vector<IONode::Pimpl> IONodePimpls = fromIODrawer->getIONodes();
    for (vector<IONode::Pimpl>::const_iterator iter = IONodePimpls.begin(); iter != IONodePimpls.end(); ++iter) {
        _IONodes.push_back(IONode::Pimpl(new IONode::Impl(*iter)));
    }
}

uint32_t
IODrawer::Impl::getAvailableIONodeCount() const
{
    return _availableIONodeCount;
}

void
IODrawer::Impl::addIONode(
        const IONode::Pimpl ionode
        )
{
    _IONodes.push_back(ionode);
    if (ionode->getState() == Hardware::Available) {
        _availableIONodeCount++;
    }

}

const vector<IONode::Pimpl>&
IODrawer::Impl::getIONodes()
{
    return _IONodes;
}

} // namespace bgsched
