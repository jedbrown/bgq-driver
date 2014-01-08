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

#ifndef BGQDB_GENBLOCK_H_
#define BGQDB_GENBLOCK_H_

#include "BGQDBlib.h"

#include <string>

#include <stdint.h>

namespace BGQDB {

class BlockDatabaseInfo;
class GenBlockParams;

/*! \brief Calculate BlockDatabaseInfo from GenBlockParams.
 *
 *  If do_check is true, this function validates many aspects of the input,
 *  such as
 *  - The midplanes create a valid torus.
 *  - The block isn't too big for the machine.
 *  - A midplane doesn't exist.
 *  - The node boards exist.
 *
 *  This function does NOT require that the ID was set in genblock_params
 *  and does NOT set the blockId in db_info_out.
 *
 *  \throws Exception on error.
 */
void genBlockParamsToBlockDatabaseInfoEx(
        const GenBlockParams& genblock_params,
        BlockDatabaseInfo& db_info_out,
        bool do_check = true
        );

/*! \brief Insert rows for a block.
 *
 *  Inserts rows to:
 *  - BLOCK
 *  - Large: BPBlockMap, SwitchBlockMap, LinkBlockMap
 *  - Small: SmallBlock
 *
 *  \throws Exception if the insert fails or block owner doesn't exist on the system.
 */
void insertBlock(
        const BlockDatabaseInfo& info
        );


/*! \brief Same as genBlockParamsToBlockDatabaseInfoEx but returns a STATUS code rather than exception.
 *
 *  \return
 *  - OK: successfully filled in BlockDatabaseInfo from GenBlockParams.
 *  - other: some kind of error
 */
STATUS genBlockParamsToBlockDatabaseInfo(
        const GenBlockParams& genblock_params,
        BlockDatabaseInfo& db_info_out
        );


/*! \brief Inserts a block to the database given the parameters.
 *
 *  This function requires that the ID is set in params, throws Exception with status=INVALID_ARG if it wasn't set.
 *
 *  Pass do_check = false if you think the params are good and don't have to be checked again.
 *
 *  \throws Exception if an error occurred.
 */
void genBlockEx(
        const GenBlockParams& params,
        bool do_check = true
        );


/*! \brief Just like genBlockEx except returns a STATUS. */
STATUS genBlock(
        const GenBlockParams& params
        );

/*! \brief Creates a block on a single midplane. */
STATUS genBlock(
        const std::string& id,              //!< [in]
        const std::string& midplane,        //!< [in]
        const std::string& owner            //!< [in]
        );

/*! \brief Creates a block for each midplane on the system with the given prefix. */
STATUS genBlocks(
        const std::string& owner,                   //!< [in]
        const std::string& prefix = std::string()   //!< [in]
        );

/*! \brief Creates a block which covers the entire system. */
STATUS genFullBlock(
        const std::string& id,              //!< [in]
        const std::string& owner            //!< [in]
        );

/*! \brief Creates a large block. */
STATUS genMidplaneBlock(
        const std::string& id,                  //!< [in]
        const std::string& corner,              //!< [in]
        unsigned int asize,                     //!< [in]
        unsigned int bsize,                     //!< [in]
        unsigned int csize,                     //!< [in]
        unsigned int dsize,                     //!< [in]
        const std::string& owner,               //!< [in]
        const std::string& apt = std::string(), //!< [in] passthrough
        const std::string& bpt = std::string(), //!< [in] passthrough
        const std::string& cpt = std::string(), //!< [in] passthrough
        const std::string& dpt = std::string()  //!< [in] passthrough
        );

/*! \brief Creates a small block. */
STATUS genSmallBlock(
        const std::string& id,              //!< [in]
        const std::string& midplane,        //!< [in]
        uint32_t cnodes,                    //!< [in]
        const std::string& nodecard,        //!< [in]
        const std::string& owner            //!< [in]
        );

/*! \brief Creates an I/O block. */
STATUS genIOBlock(
        const std::string& id,          //!< [in]
        const std::string& location,    //!< [in]
        unsigned int ionodes,           //!< [in]
        const std::string& owner        //!< [in]
        );

/*! \brief Get torus coordinates for a midplane location.
 *
 *  \throws Exception if the midplane location is not valid or a database error occurs.
 */
void torusCoordinateForMidplane(
        const std::string& midplane_location,        //!< [in] Midplane location to find coordinates for
        MidplaneCoordinate& midplane_coordinate_out  //!< [out] Midplane coordinates for requested midplane location
    );

} // namespace BGQDB


#endif
