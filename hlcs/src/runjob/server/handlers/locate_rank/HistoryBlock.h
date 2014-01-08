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
#ifndef RUNJOB_SERVER_HANDLERS_LOCATE_RANK_HISTORY_BLOCK_H
#define RUNJOB_SERVER_HANDLERS_LOCATE_RANK_HISTORY_BLOCK_H

#include "server/handlers/locate_rank/Block.h"
#include "server/handlers/locate_rank/fwd.h"

#include <db/include/api/cxxdb/cxxdb.h>

namespace runjob {
namespace server {
namespace handlers {
namespace locate_rank {

/*!
 * \brief Query a block in the bgqblock_history table.
 *
 * Blocks in the history table are differentiated by their creation ID, which is
 * automatically generated and will be unique for each block.
 */
class HistoryBlock : public Block
{
public:
    /*!
     * \brief ctor.
     */
    HistoryBlock(
            const cxxdb::ConnectionPtr& db,
            const boost::shared_ptr<HistoryJob>& job
            );

private:
    std::string getTableName() const;
    
    std::string getQualifier() const;
};

} // locate_rank
} // handlers
} // server
} // runjob

#endif
