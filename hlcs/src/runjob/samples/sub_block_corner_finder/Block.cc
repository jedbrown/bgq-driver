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
#include "Block.h"

#include "common/logging.h"
#include "common/SubBlock.h"

#include "Options.h"

#include <control/include/bgqconfig/xml/BGQBlockXML.h>

#include <control/include/bgqconfig/BGQBlockNodeConfig.h>

#include <db/include/api/BGQDBlib.h>

#include <boost/foreach.hpp>
#include <boost/scoped_ptr.hpp>

#include <sstream>

LOG_DECLARE_FILE( runjob::log );

using namespace runjob::samples;

Block::Block(
        const Options& options
        ) :
    _options( options),
    _id( options.getBlock() )
{
    // validate block
    if ( _id.empty() ) {
        throw std::invalid_argument( "Empty block ID" );
    }
    LOG_DEBUG_MSG( "Block " << _id );

    // validate sub-block
    if ( _options.getCorner().empty() ) {
        throw std::invalid_argument( "Empty corner" );
    } else if ( _options.getShape().empty() ) {
        throw std::invalid_argument( "Empty shape" );
    }

    runjob::SubBlock sub_block(
            _options.getCorner(),
            runjob::Shape( _options.getShape() )
            );

    // get block XML
    std::stringstream xml;
    if ( BGQDB::getBlockXML(xml, _id) != BGQDB::OK) {
        throw std::invalid_argument( "Could not get xml definition for block " + _id );
    }

    // get BGQBlockXML
    boost::scoped_ptr<BGQBlockXML> block_xml;
    block_xml.reset(
            BGQBlockXML::create( xml, _options.getMachine().get() )
            );
    if ( !block_xml ) {
        throw std::runtime_error( "Could not create XML object for block " + _id );
    }
    // get BGQBlockNodeConfig
    boost::shared_ptr<BGQBlockNodeConfig> block_config;
    block_config.reset(
            new BGQBlockNodeConfig(
                _options.getMachine().get(),
                block_xml.get()
                )
            );

    // we only support compute blocks
    if ( block_xml->_ioboards.size() ) {
        throw std::invalid_argument( "Block " + _id + " is an I/O block, only compute blocks are supported." );
    }

    // iterate through midplanes
    bool found = false;
    unsigned corner_a, corner_b, corner_c, corner_d, corner_e;
    corner_a = corner_b = corner_c = corner_d = corner_e = 0;
    for (
            BGQBlockNodeConfig::midplane_iterator midplane = block_config->midplaneBegin();
            midplane != block_config->midplaneEnd();
            ++midplane
        )
    {
        LOG_DEBUG_MSG(
                "Midplane " << midplane->posInMachine() << " coordinates relative to block (" <<
                midplane->allMidplaneA() << "," <<
                midplane->allMidplaneB() << "," <<
                midplane->allMidplaneC() << "," <<
                midplane->allMidplaneD() << ")"
                );

        // get origin for partial midplane
        unsigned offset_a, offset_b, offset_c, offset_d, offset_e = 0;
        midplane->partialMidplaneOrigin().getABCDE(
                offset_a,
                offset_b,
                offset_c,
                offset_d,
                offset_e
                );
        LOG_DEBUG_MSG(
                "Origin position within the midplane (" <<
                offset_a << "," <<
                offset_b << "," <<
                offset_c << "," <<
                offset_d << "," <<
                offset_e << ")"
                );

        // iterate through computes
        BOOST_FOREACH( const BGQNodePos& compute, *midplane->computes() ) {
            // create location string
            std::string location;
            location.append( midplane->posInMachine() );
            location.append( "-" );
            location.append( boost::lexical_cast<std::string>( compute ) );

            // get coordinates
            compute.getABCDE(
                        corner_a,
                        corner_b,
                        corner_c,
                        corner_d,
                        corner_e
                        );

            // offset from origin
            corner_a -= offset_a;
            corner_b -= offset_b;
            corner_c -= offset_c;
            corner_d -= offset_d;
            corner_e -= offset_e;

            // log location and coordinates
            LOG_TRACE_MSG(
                    compute << " (" <<
                    corner_a << "," <<
                    corner_b << "," <<
                    corner_c << "," <<
                    corner_d << "," <<
                    corner_e << ")"
                    );

            // find corner coordinates
            std::ostringstream cornerLocation;
            cornerLocation.clear();
            cornerLocation << sub_block.corner();
            if ( location == cornerLocation.str() ) {
                found = true;

                // log corner coordinates
                LOG_DEBUG_MSG(
                        "Corner " << sub_block.corner() << " has coordinates " <<
                        "(" << corner_a << "," << corner_b << "," << corner_c << "," << corner_d << "," << corner_e << ")"
                        );

                // find shape coordinates
                LOG_DEBUG_MSG( "Shape " << std::string(sub_block.shape()) );
                for ( unsigned a = corner_a; a < corner_a + sub_block.shape().a(); ++a ) {
                    for ( unsigned b = corner_b; b < corner_b + sub_block.shape().b(); ++b ) {
                        for ( unsigned c = corner_c; c < corner_c + sub_block.shape().c(); ++c ) {
                            for ( unsigned d = corner_d; d < corner_d + sub_block.shape().d(); ++d ) {
                                for ( unsigned e = corner_e; e < corner_e + sub_block.shape().e(); ++e ) {

                                    // validate coordinates
                                    if (
                                            midplane->fullMidplaneUsed() && (
                                                a >= BGQTopology::MAX_A_NODE ||
                                                b >= BGQTopology::MAX_B_NODE ||
                                                c >= BGQTopology::MAX_C_NODE ||
                                                d >= BGQTopology::MAX_D_NODE ||
                                                e >= BGQTopology::MAX_E_NODE
                                                )
                                       )
                                    {
                                        std::ostringstream msg;
                                        msg << "Shape " << std::string(sub_block.shape()) <<
                                            " extends beyond midplane dimensions (" <<
                                            BGQTopology::MAX_A_NODE << "," <<
                                            BGQTopology::MAX_B_NODE << "," <<
                                            BGQTopology::MAX_C_NODE << "," <<
                                            BGQTopology::MAX_D_NODE << "," <<
                                            BGQTopology::MAX_E_NODE << ")";
                                        LOG_FATAL_MSG( msg.str() );
                                        throw std::invalid_argument( "shape" );
                                    } else if (
                                            !midplane->fullMidplaneUsed() && (
                                                a >= midplane->partialAsize() ||
                                                b >= midplane->partialBsize() ||
                                                c >= midplane->partialCsize() ||
                                                d >= midplane->partialDsize() ||
                                                e >= midplane->partialEsize() )
                                            )
                                    {
                                        std::ostringstream msg;
                                        msg << "Shape " << std::string(sub_block.shape()) <<
                                            " extends beyond nodeboard dimensions (" <<
                                            midplane->partialAsize() << "," <<
                                            midplane->partialBsize() << "," <<
                                            midplane->partialCsize() << "," <<
                                            midplane->partialDsize() << "," <<
                                            midplane->partialEsize() << ")";
                                        LOG_FATAL_MSG( msg.str() );
                                        throw std::invalid_argument( "shape" );
                                    }

                                    BGQNodePos compute( a, b, c, d, e);
                                    LOG_DEBUG_MSG(
                                            midplane->posInMachine() << "-" << compute << " (" <<
                                            a << "," <<
                                            b << "," <<
                                            c << "," <<
                                            d << "," <<
                                            e << ")"
                                            );
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    if ( !found ) {
        std::ostringstream os;
        os << "Could not find corner " << sub_block.corner();
        throw std::invalid_argument( os.str() );
    }
}

