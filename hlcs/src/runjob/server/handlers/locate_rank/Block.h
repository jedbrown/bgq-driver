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
#ifndef RUNJOB_SERVER_HANDLERS_LOCATE_RANK_BLOCK_H
#define RUNJOB_SERVER_HANDLERS_LOCATE_RANK_BLOCK_H

#include "common/Uci.h"

#include "server/handlers/locate_rank/fwd.h"

#include <db/include/api/cxxdb/cxxdb.h>

#include <spi/include/kernel/location.h>

#include <boost/shared_ptr.hpp>

namespace runjob {
namespace server {
namespace handlers {
namespace locate_rank {

/*!
 * \brief Abstract base for active and history blocks.
 *
 * Small blocks query the bgqsmallblock or bgqsmallblock_history table to get a list of node boards
 * and the containing midplane. Using this list, the offset of the small block within the midplane 
 * is calculated.
 *
 * Large blocks query the bgqbpblockmap or bgqbpblockmap_history table to get the midplane location
 * of the rank's coordinates.
 *
 * The result of both of these operations is a set coordinates within a midplane, which can be 
 * converted into a node board and compute card location. Combined with the midplane location, this
 * results in a complete location string.
 */
class Block
{
public:
    /*!
     * \brief pointer type.
     */
    typedef boost::shared_ptr<Block> Ptr;

public:
    /*!
     * \brief Factory.
     */
    static Ptr create(
            const cxxdb::ConnectionPtr& db,     //!< [in]
            const boost::shared_ptr<Job>& job   //!< [in]
            );

    /*!
     * \brief dtor.
     */
    virtual ~Block() = 0;

    /*!
     * \brief
     */
    Uci findNodeLocation(
            const BG_CoordinateMapping_t& coordinates   //!< [in]
            );

    /*!
     * \brief
     */
    int creationId() const { return _creationId; }

protected:
    Block(
            const boost::shared_ptr<Job>& job
         );

private:
    virtual std::string getTableName() const = 0;
    
    virtual std::string getQualifier() const = 0;

    void querySmallBlock(
            const cxxdb::ConnectionPtr& db
            );
   
    void queryLargeBlock(
            const cxxdb::ConnectionPtr& db
            );

    void executeLargeBlock(
            const BG_CoordinateMapping_t& coordaintes
            );

    void findCorner(
            const std::string& location
            );

protected:
    cxxdb::QueryStatementPtr _statement;
    const std::string _id;
    bool _small;
    unsigned _offset[5];
    unsigned _corner[5];
    std::string _midplane;
    int _creationId;
};

} // locate_rank
} // handlers
} // server
} // runjob

#endif
