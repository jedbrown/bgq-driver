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

#include <bgsched/bgsched.h>
#include <bgsched/Block.h>
#include <bgsched/Coordinates.h>

#include <bgsched/core/core.h>

#include <boost/bind.hpp>

#include <iostream>
#include <iterator>
#include <string>

using namespace bgsched;

int
main()
{
    using std::cout;
    using std::endl;

    bgsched::init( std::string() );

    Block::Ptrs blocks;
    BlockFilter filter;

    // get all blocks
    blocks = core::getBlocks(filter);
    cout << "Got " << blocks.size() << " blocks" << endl;

    // output each block name to stdout
    std::transform(
            blocks.begin(),
            blocks.end(),
            std::ostream_iterator<std::string>(std::cout,"\n"),
            boost::bind(&Block::getName, _1));

    // get all hardware
    ComputeHardware::ConstPtr bgq = core::getComputeHardware();
    for (uint32_t  a = 0; a < bgq->getMachineSize(Dimension::A); ++a) {
        for (uint32_t  b = 0; b < bgq->getMachineSize(Dimension::B); ++b) {
            for (uint32_t  c = 0; c < bgq->getMachineSize(Dimension::C); ++c) {
                for (uint32_t  d = 0; d < bgq->getMachineSize(Dimension::D); ++d) {
                    Coordinates coords(a, b, c, d);
                    Midplane::ConstPtr midplane = bgq->getMidplane(coords);

                    cout << midplane->toString() << endl;
                }
            }
        }
    }
}
