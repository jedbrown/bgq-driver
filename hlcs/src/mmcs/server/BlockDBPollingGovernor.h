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
/* (C) Copyright IBM Corp.  2012, 2012                              */
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

#ifndef MMCS_SERVER_BLOCK_POLLING_DB_GOVERNOR_H_
#define MMCS_SERVER_BLOCK_POLLING_DB_GOVERNOR_H_

#include "DBPollingGovernor.h"

#include "ExcludeList.h"

namespace mmcs {
namespace server {

class BlockDBPollingGovernor : public DBPollingGovernor<std::string, BGQDB::BLOCK_ACTION>
{
public:
    BlockDBPollingGovernor() : dbExcludedBlockList() {}
    ~BlockDBPollingGovernor() { }
    BGQDB::STATUS beginTransaction(std::string& blockName, std::string& userName, BGQDB::BLOCK_ACTION& action);
    void endTransaction(std::string& blockName, BGQDB::BLOCK_ACTION& action, bool exclude = true);
protected:
    const std::string& governorType() { static const std::string thisType("BlockDBPollingGovernor"); return thisType; }
private:
    ExcludeList dbExcludedBlockList; // blocks to be excluded from DB polling
};


} } // namespace mmcs::server

#endif
