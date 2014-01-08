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
#include "server/job/ClassRoute.h"

#include "server/job/class_route/Mapping.h"
#include "server/job/class_route/Np.h"
#include "server/job/class_route/Parameters.h"

#include "common/logging.h"

#include <spi/include/mu/Classroute_inlines.h>

LOG_DECLARE_FILE( runjob::server::log );

namespace runjob {
namespace server {
namespace job  {

ClassRoute::ClassRoute(
        class_route::Parameters& parameters
        ) :
    _allNodes(),
    _npNodes(),
    _coordinates( parameters._coordinates )
{
    // comm root is the job leader
    CR_COORD_T& commRoot = parameters._jobLeader->impl();

    // Get this node's coordinate
    CR_COORD_T& me = parameters._coordinates.impl();

    // this class route excludes the nodes outside of the --np
    // circumscribing rectangle
    ::ClassRoute np_nodes_cr;
    MUSPI_BuildNodeClassrouteSparse(
            &parameters._world->impl(),
            &commRoot,
            &me,
            &parameters._np->rectangle().impl(),
            parameters._np->container().get(),
            static_cast<int>( parameters._np->size() ),
            parameters._mapping->impl(),
            parameters._np->primaryDimension(),
            &np_nodes_cr 
            );

    _npNodes.input = np_nodes_cr.input | BGQ_CLASS_INPUT_VC_SYSTEM;
    _npNodes.output = np_nodes_cr.output;

    // This class route does not include the nodes excluded by --np since
    // the non-participating nodes need to use the primordial global interrupt
    // barrier.
    ::ClassRoute all_nodes_cr;
    MUSPI_BuildNodeClassrouteSparse(
            &parameters._world->impl(),
            &commRoot,
            &me,
            &parameters._world->impl(),
            NULL, // no nodes to exclude
            0,    //
            parameters._mapping->impl(),
            parameters._np->primaryDimension(),
            &all_nodes_cr 
            );

    _allNodes.input = all_nodes_cr.input | BGQ_CLASS_INPUT_VC_SYSTEM;
    _allNodes.output = all_nodes_cr.output;
}

// print the direction of each dimension of a given link. 
// Output will look like:
// "A+ B+ C+ D+ E+"
// "A- B- C- D- E-"
void
printLink(
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

std::ostream&
operator<<(
        std::ostream& os,
        const ClassRoute& cr
        )
{
    os << cr.coordinates();

    os << " col up: ";
    printLink( os, cr.npNodes().output );

    os << " col dn: ";
    printLink( os, cr.npNodes().input );

    if ( cr.npNodes().input & BGQ_CLASS_INPUT_LINK_LOCAL) {
        os << " L";
    }

    os << " gi up: ";
    printLink( os, cr.allNodes().output );

    os << " gi dn: ";
    printLink( os, cr.allNodes().input );

    if ( cr.allNodes().input & BGQ_CLASS_INPUT_LINK_LOCAL) {
        os << " L";
    }

    return os;
}

} // job
} // server
} // runjob
