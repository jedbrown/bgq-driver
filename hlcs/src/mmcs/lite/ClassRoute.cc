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

#include "ClassRoute.h"

#include "JobInfo.h"

#include "server/CNBlockController.h"
#include "server/IOBlockController.h"

#include <utility/include/Log.h>

#include <iostream>

LOG_DECLARE_FILE( "mmcs.lite" );


namespace mmcs {
namespace lite {


ClassRoute::ClassRoute(
        const JobInfo& info,
        const server::BlockPtr& block
        ) :
    _np( info, block ),
    _block( block ),
    _leader(),
    _container()
{
    this->pickJobLeader();

    unsigned aSize = 0;
    unsigned bSize = 0;
    unsigned cSize = 0;
    unsigned dSize = 0;
    unsigned eSize = 0;
    if ( boost::dynamic_pointer_cast<server::CNBlockController>(block) ) {
        aSize = _block->getBlockNodeConfig()->aNodeSize();
        bSize = _block->getBlockNodeConfig()->bNodeSize();
        cSize = _block->getBlockNodeConfig()->cNodeSize();
        dSize = _block->getBlockNodeConfig()->dNodeSize();
        eSize = _block->getBlockNodeConfig()->eNodeSize();
    } else if ( boost::dynamic_pointer_cast<server::IOBlockController>(_block) ) {
        aSize = _block->getBlockNodeConfig()->aIONodeSize();
        bSize = _block->getBlockNodeConfig()->bIONodeSize();
        cSize = _block->getBlockNodeConfig()->cIONodeSize();
        dSize = _block->getBlockNodeConfig()->dIONodeSize();
        eSize = 1;
    }


    for ( unsigned a = 0; a < aSize; ++a ) {
        for ( unsigned b = 0; b < bSize; ++b ) {
            for ( unsigned c = 0; c < cSize; ++c ) {
                for ( unsigned d = 0; d < dSize; ++d ) {
                    for ( unsigned e = 0; e < eSize; ++e ) {
                        CR_COORD_T me;
                        me.coords[0] = a;
                        me.coords[1] = b;
                        me.coords[2] = c;
                        me.coords[3] = d;
                        me.coords[4] = e;
                        this->generate( me );
                    }
                }
            }
        }
    }
}

void
ClassRoute::pickJobLeader()
{
    // Find a root node for comm-world using the rectangle calculated previously
    int primaryDimension;
    MUSPI_PickWorldRoot( &_np.rectangle(), _np.mapping(), &_leader, &primaryDimension );
    LOG_TRACE_MSG( "primary dimension " << CR_DIM_NAMES[primaryDimension] );

    LOG_TRACE_MSG(
            "job leader (" <<
            _leader.coords[0] << "," <<
            _leader.coords[1] << "," <<
            _leader.coords[2] << "," <<
            _leader.coords[3] << "," <<
            _leader.coords[4] <<
            ")"
            );
}

void
ClassRoute::generate(
        CR_COORD_T& me
        )
{
    bgcios::jobctl::NodeClassRoute cr;
    cr.coords.aCoord = me.coords[0];
    cr.coords.bCoord = me.coords[1];
    cr.coords.cCoord = me.coords[2];
    cr.coords.dCoord = me.coords[3];
    cr.coords.eCoord = me.coords[4];

    // collective class route only includes nodes within the circumscribing rectangle
    // previously generated
    MUSPI_BuildNodeClassrouteSparse(
            &_np.world(),
            &_leader,
            &me,
            &_np.rectangle(),
            _np.container().get(),
            _np.size(),
            _np.mapping(),
            _np.primaryDimension(),
            &cr.npClassRoute
            );

    cr.npClassRoute.input |= BGQ_CLASS_INPUT_VC_SYSTEM;

    // Build this node's global interrupt classroute for comm-world.
    // This class route does not include the nodes excluded by --np since
    // the non-participating nodes need to use the primordial global interrupt
    // barrier.
    MUSPI_BuildNodeClassrouteSparse(
            &_np.world(),
            &_leader,
            &me,
            &_np.world(),
            NULL, // no nodes to exclude
            0,    //
            _np.mapping(),
            _np.primaryDimension(),
            &cr.subBlockClassRoute
            );

    cr.subBlockClassRoute.input |= BGQ_CLASS_INPUT_VC_SYSTEM;

    // remember this node's class route so it can be added to the setup job
    // message later
    _container.push_back( cr );

    // log the class route information for debug purposes
    std::ostringstream os;
    this->print( os, cr );
    LOG_TRACE_MSG(
            "(" <<
            me.coords[0] << "," <<
            me.coords[1] << "," <<
            me.coords[2] << "," <<
            me.coords[3] << "," <<
            me.coords[4] <<
            ") " <<
            os.str()
            );
}

void
ClassRoute::printLink(
        std::ostream& os,
        uint32_t link
        )
{
    // count number of characters for formatting purposes
    unsigned dimension_count = 0;

    // iterate through all dimensions
    for ( int x = 0; x < CR_NUM_DIMS; ++x ) {
        if ( link & CR_LINK(x, CR_SIGN_POS) ) {
            if ( dimension_count ) {
                // if we've printed a dimension previously, add a space
                os << " ";
            }
            os << CR_DIM_NAMES[x] << CR_DIR_NAMES[CR_SIGN_POS];
            ++dimension_count;
        }
        if ( link & CR_LINK(x, CR_SIGN_NEG) ) {
            if ( dimension_count ) {
                // if we've printed a dimension previously, add a space
                os << " ";
            }
            os << CR_DIM_NAMES[x] << CR_DIR_NAMES[CR_SIGN_NEG];
            ++dimension_count;
        }
    }

    if ( !dimension_count ) {
        // if we didn't print any dimensions, add two spaces
        os << "  ";
    }
}

void
ClassRoute::print(
        std::ostream& os,
        const bgcios::jobctl::NodeClassRoute& cr
        )
{
    os << " col up: ";
    printLink( os, cr.npClassRoute.output );

    os << " col dn: ";
    printLink( os, cr.npClassRoute.input );

    if ( cr.npClassRoute.input & BGQ_CLASS_INPUT_LINK_LOCAL) {
        os << " L";
    }

    os << " gi up: ";
    printLink( os, cr.subBlockClassRoute.output );

    os << " gi dn: ";
    printLink( os, cr.subBlockClassRoute.input );

    if ( cr.subBlockClassRoute.input & BGQ_CLASS_INPUT_LINK_LOCAL) {
        os << " L";
    }
}

} } // namespace mmcs::lite
