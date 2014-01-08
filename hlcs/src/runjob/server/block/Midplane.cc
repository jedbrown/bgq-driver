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
#include "server/block/Midplane.h"

#include "common/Coordinates.h"
#include "common/Corner.h"
#include "common/error.h"
#include "common/Exception.h"
#include "common/logging.h"
#include "common/SubBlock.h"

#include "server/block/ComputeNode.h"
#include "server/block/IoNode.h"

#include "server/job/Create.h"

#include "server/Job.h"

#include <bgq_util/include/Location.h>

#include <control/include/bgqconfig/BGQMidplaneNodeConfig.h>
#include <control/include/bgqconfig/BGQTopology.h>

#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>

#include <algorithm>

LOG_DECLARE_FILE( runjob::server::log );

namespace runjob {
namespace server {
namespace block {

Midplane::Midplane(
        const std::string& block,
        const std::string& location,
        BGQMidplaneNodeConfig& config,
        const Io::Links& links
        ) :
    _block( block ),
    _location( location ),
    _smallBlock( !config.fullMidplaneUsed() ),
    _coordinates(),
    _offset(),
    _nodes(),
    _nodeboards(),
    _corners(),
    _io(),
    _job()
{
    LOGGING_DECLARE_LOCATION_MDC( _location );

    if ( BG_UCI_GET_COMPONENT(_location.get()) != BG_UCI_Component_Midplane ) {
        LOG_RUNJOB_EXCEPTION( error_code::block_invalid, location << " is not a valid midplane location" );
    }

    this->getCoordinates( config );
    this->resize( config );

    // iterate through computes
    BOOST_FOREACH( const BGQNodePos& compute, *config.computes() ) {
        // get and offset coordinates
        unsigned a, b, c, d, e;
        compute.getABCDE(a, b, c, d, e);
        a -= _offset.a();
        b -= _offset.b();
        c -= _offset.c();
        d -= _offset.d();
        e -= _offset.e();

        // get connected I/O node location from bgqconfig
        const std::string nodeboard = BGQTopology::nodeCardNameFromPos( compute.nodeCard() );
        const std::string position = BGQTopology::processorCardNameFromJtagPort( compute.jtagPort() );
        const std::string connectedIo = config.connectedIONode( nodeboard, position );
        LOG_TRACE_MSG( 
                compute << " (" << a << "," << b << "," << c << "," << d << "," << e << ")" <<
                " <--> " <<
                "'" << connectedIo << "'"
                );

        const Uci io_uci( connectedIo );
        Compute::IoRatio::iterator ratio = _io.find( io_uci );
        if ( ratio == _io.end() ) {
            (void)_io.insert( Compute::IoRatio::value_type(io_uci, 1) );
        } else {
            ratio->second += 1;
        }

        const ComputeNode::Ptr node(
                new ComputeNode(
                    _location,
                    compute,
                    io_uci,
                    links
                    )
                );

        // add to container
        _nodes[a][b][c][d][e] = node;

        // add to corner map
        _corners.insert(
                CornerMap::value_type(
                    node->getLocation(),
                    Coordinates( a, b, c, d, e )
                    )
                );

        // add to node board container
        if ( std::find(_nodeboards.begin(), _nodeboards.end(), nodeboard) == _nodeboards.end() ) {
            _nodeboards.push_back( nodeboard );
        }
    }

    // log ratio of computes for each I/O link
    for ( Compute::IoRatio::const_iterator i = _io.begin(); i != _io.end(); ++i ) {
        LOG_TRACE_MSG( i->first << ": " << i->second << " compute nodes" );
    }
}

void
Midplane::getCoordinates(
        const BGQMidplaneNodeConfig& config
        )
{
    _coordinates = Coordinates(
            config.a(),
            config.b(),
            config.c(),
            config.d(),
            config.e()
            );

    unsigned a, b, c, d, e;
    config.partialMidplaneOrigin().getABCDE( a, b, c, d, e );
    _offset = Coordinates( a, b, c, d, e );
    LOG_DEBUG_MSG( "coordinates " << std::string(_coordinates) << " offset " << std::string(_offset) );
}

void
Midplane::resize(
        BGQMidplaneNodeConfig& config
        )
{
    LOG_DEBUG_MSG( "creating " << config.computes()->size() << " nodes" );

    if ( config.fullMidplaneUsed() ) {
        _nodes.resize(
                boost::extents
                [BGQTopology::MAX_A_NODE]
                [BGQTopology::MAX_B_NODE]
                [BGQTopology::MAX_C_NODE]
                [BGQTopology::MAX_D_NODE]
                [BGQTopology::MAX_E_NODE]
                );
    } else {
        _nodes.resize(
                boost::extents
                [config.partialAsize()]
                [config.partialBsize()]
                [config.partialCsize()]
                [config.partialDsize()]
                [config.partialEsize()]
                );
    }

    LOG_DEBUG_MSG(
            "dimensions: (" <<
            _nodes.shape()[0] << "," <<
            _nodes.shape()[1] << "," <<
            _nodes.shape()[2] << "," <<
            _nodes.shape()[3] << "," <<
            _nodes.shape()[4] << ")"
            );
}

Midplane::~Midplane()
{

}

bool
Midplane::arbitrate(
        const SubBlock& resource,
        const job::Create::Ptr& job
        )
{
    LOGGING_DECLARE_LOCATION_MDC( _location );

    if ( !this->available(job) ) return false;
    if ( !this->validateCorner(job) ) return false;
    if ( !this->validateSubBlockNp(job) ) return false;
    if ( !this->validateShape(job) ) return false;
    if ( !this->validateStdinRank(job) ) return false;
    if ( !this->validateStraceRank(job) ) return false;

    // getting here means the corner and shape are valid
    // time to do the arbitration.
    if ( resource.corner().isCoreSet() ) {
        return this->addSingleCoreJob( job );
    } else {
        return this->addMultiNodeJob( job );
    }
}

bool
Midplane::validateStdinRank(
        const job::Create::Ptr& job
        )
{
    const JobInfo& info = job->_job_info;
    const size_t stdinRank = info.getStdinRank();
    const SubBlock& subBlock = info.getSubBlock();
    const Shape& shape = subBlock.shape();
    const size_t ranks = info.getRanksPerNode();
    const size_t jobSize = shape.size() * ranks;

    if ( stdinRank >= jobSize ) {
        job->error(
                "stdin rank value of " + boost::lexical_cast<std::string>(stdinRank) +
                " with ranks per node value of " + boost::lexical_cast<std::string>(ranks) +
                " is greater than sub-block shape " + std::string(shape) +
                " size of " + boost::lexical_cast<std::string>( shape.size() ),
                error_code::job_np_invalid 
                );

        return false;
    }

    return true;
}

bool
Midplane::validateStraceRank(
        const job::Create::Ptr& job
        )
{
    const JobInfo& info = job->_job_info;
    const Strace& strace = info.getStrace();
    if ( strace.getScope() == Strace::None ) return true;

    const SubBlock& subBlock = info.getSubBlock();
    const Shape& shape = subBlock.shape();
    const size_t ranks = info.getRanksPerNode();
    const size_t jobSize = shape.size() * ranks;

    if ( strace.getRank() >= jobSize ) {
        job->error(
                "strace rank value of " + boost::lexical_cast<std::string>(strace.getRank()) +
                " with ranks per node value of " + boost::lexical_cast<std::string>(ranks) +
                " is greater than sub-block shape " + std::string(shape) +
                " size of " + boost::lexical_cast<std::string>( shape.size() ),
                error_code::job_np_invalid 
                );

        return false;
    }

    return true;
}

bool
Midplane::addSingleCoreJob(
        const job::Create::Ptr& job
        )
{
    JobInfo& info = job->_job_info;
    SubBlock& subBlock = info.getSubBlock();
    const Corner& corner = subBlock.corner();
    const Coordinates& coordinates = corner.getMidplaneCoordinates();
    const ComputeNode::Ptr cn = _nodes[coordinates.a()][coordinates.b()][coordinates.c()][coordinates.d()][coordinates.e()];

    // remember this job is using this compute node
    Shape& shape = subBlock.shape();
    shape.addNode( cn->getLocation() );
    
    return cn->arbitrate(subBlock, job);
}

bool
Midplane::addMultiNodeJob(
        const job::Create::Ptr& job
        )
{
    const JobInfo& info = job->_job_info;
    const SubBlock& subBlock = info.getSubBlock();
    const Shape& shape = subBlock.shape();
    const Corner& corner = subBlock.corner();
    const Coordinates& coordinates = corner.getMidplaneCoordinates();

    std::vector<ComputeNode::Ptr> requested_nodes;

    // iterate through our array starting from the corner using the entire shape provided
    for ( unsigned a = coordinates.a(); a < shape.a() + coordinates.a(); ++a ) {
        for ( unsigned b = coordinates.b(); b < shape.b() + coordinates.b(); ++b ) {
            for ( unsigned c = coordinates.c(); c < shape.c() + coordinates.c(); ++c ) {
                for ( unsigned d = coordinates.d(); d < shape.d() + coordinates.d(); ++d ) {
                    for ( unsigned e = coordinates.e(); e < shape.e() + coordinates.e(); ++e ) {
                        const ComputeNode::Ptr compute = _nodes[a][b][c][d][e];
                        if ( !compute->available(job) ) {
                            return false;
                        }

                        requested_nodes.push_back( compute );
                        LOG_DEBUG_MSG(
                                "added " << compute->getLocation() << " at (" <<
                                a << "," << b << "," << c << "," << d << "," << e << ")"
                                );
                    }
                }
            }
        }
    }

    // build map of compute to I/O
    typedef std::map<IoNode::Ptr, unsigned> IoMap;
    IoMap io_map;

    // getting here means all nodes requested are available
    BOOST_FOREACH( const ComputeNode::Ptr& compute, requested_nodes ) {
        // insert into I/O map so we know how many compute nodes each I/O node has to manage
        const IoMap::iterator i =  io_map.find( compute->getConnectedIo() );
        if ( i == io_map.end() ) {
            io_map.insert( IoMap::value_type(compute->getConnectedIo(), 1) );
            LOG_TRACE_MSG( 
                    "inserting I/O " << compute->getConnectedIo()->getLocation() << 
                    " for compute " << compute->getLocation()
                    );
        } else {
            // already have this entry, increment the number of compute nodes managed by one
            i->second += 1;
            LOG_TRACE_MSG( 
                    "incrementing I/O " << i->first->getLocation() << " to " << i->second << 
                    " for compute " << compute->getLocation()
                    );
        }

        // add each compute node location
        JobInfo& info = job->_job_info;
        SubBlock& subBlock = info.getSubBlock();
        Shape& shape = subBlock.shape();
        shape.addNode( compute->getLocation() );
    }

    // iterate through I/O map and add each node to the job
    BOOST_FOREACH( IoMap::value_type io, io_map ) {
        job->io( io.first, io.second );
    }

    // mark these compute nodes as in use
    BOOST_FOREACH( const ComputeNode::Ptr& compute, requested_nodes ) {
        compute->setJob( job->create() );
    }

    return true;
}

bool
Midplane::available(
        const job::Create::Ptr& job
        )
{
    if ( !_job.expired() ) {
        job->error(
                ( _smallBlock ? "block " + _block : "midplane " + boost::lexical_cast<std::string>(_location) ) +
                " has a job running",
                error_code::block_busy
                );
        return false;
    }

    return true;
}

bool
Midplane::validateCorner(
        const job::Create::Ptr& job
        )
{
    const JobInfo& info = job->_job_info;
    const SubBlock& subBlock = info.getSubBlock();
    const Corner& corner = subBlock.corner();

    bool result = false;
    if ( corner.getBlockCoordinates().valid() ) {
        result = this->validateCornerCoordinates( job );
    } else {
        result = this->validateCornerLocation( job );
    }

    if ( !result ) return result;

    LOG_INFO_MSG( 
            "corner " << corner << 
            " within midplane " << corner.getMidplaneCoordinates() <<
            " within block " << corner.getBlockCoordinates()
            );

    return true;
}

bool
Midplane::validateCornerLocation(
        const job::Create::Ptr& job
        )
{
    JobInfo& info = job->_job_info;
    SubBlock& subBlock = info.getSubBlock();
    Corner& corner = subBlock.corner();

    // convert corner location string into 5 dimensional coordinates
    const Coordinates* coordinates = this->getCorner( corner );
    if ( !coordinates ) {
        job->error(
                "corner " + boost::lexical_cast<std::string>(corner) + " does not exist in " +
                ( _smallBlock ? "block " + _block : "midplane " + boost::lexical_cast<std::string>(_location) ),
                error_code::corner_invalid
                );
        return false;
    }

    // CNK needs to know the coordinates of the sub-block corner within the block itself. The
    // coordinates obtained from BGQTopology are within the midplane, so we need to offset them
    // using this midplane's coordinates within its block.
    corner.setMidplaneCoordinates(
            *coordinates
            );
    corner.setBlockCoordinates( 
            Coordinates(
                coordinates->a() + _coordinates.a(),
                coordinates->b() + _coordinates.b(),
                coordinates->c() + _coordinates.c(),
                coordinates->d() + _coordinates.d(),
                coordinates->e() + _coordinates.e()
                )
            );

    return true;
}

bool
Midplane::validateCornerCoordinates(
        const job::Create::Ptr& job
        )
{
    JobInfo& info = job->_job_info;
    SubBlock& subBlock = info.getSubBlock();
    Corner& corner = subBlock.corner();
    const Coordinates& coordinates = corner.getBlockCoordinates();

    const Coordinates midplane_coordinates(
            coordinates.a() - _coordinates.a(),
            coordinates.b() - _coordinates.b(),
            coordinates.c() - _coordinates.c(),
            coordinates.d() - _coordinates.d(),
            coordinates.e() - _coordinates.e()
            );
    LOG_TRACE_MSG( midplane_coordinates );

    for ( unsigned int dimension=0; dimension < NumDimensions; ++dimension ) {
        // maximum size in this dimension
        const size_t max = _nodes.shape()[dimension];

        unsigned corner = 0;
        switch ( dimension ) {
            case BGQTopology::A: corner = midplane_coordinates.a(); break;
            case BGQTopology::B: corner = midplane_coordinates.b(); break;
            case BGQTopology::C: corner = midplane_coordinates.c(); break;
            case BGQTopology::D: corner = midplane_coordinates.d(); break;
            case BGQTopology::E: corner = midplane_coordinates.e(); break;
            default: BOOST_ASSERT( !"unhandled dimension" );
        }

        // validate corner exists within the midplane
        if ( corner >= max ) {
            job->error(
                    "corner " +
                    boost::lexical_cast<std::string>( coordinates ) +
                    " does not exist within " +
                    ( _smallBlock ? "block " + _block : "midplane " + boost::lexical_cast<std::string>(_location) ),
                    error_code::corner_invalid
                    );

            return false;
        }
    }

    const ComputeNode::Ptr& node = _nodes
        [ midplane_coordinates.a() ]
        [ midplane_coordinates.b() ]
        [ midplane_coordinates.c() ]
        [ midplane_coordinates.d() ]
        [ midplane_coordinates.e() ]
        ;
    corner.setLocation( node->getLocation() );

    // block coordinates are already set, but we need the coordinates of this
    // corner within the midplane
    corner.setMidplaneCoordinates( 
            midplane_coordinates
            );

    return true;
}

bool
Midplane::validateShape(
        const job::Create::Ptr& job
        )
{
    const JobInfo& info = job->_job_info;
    const SubBlock& subBlock = info.getSubBlock();

    // convert location string into 5 dimensional coordinates
    const Corner& corner = subBlock.corner();
    const Coordinates& coordinates = corner.getMidplaneCoordinates();
    const Shape& shape = subBlock.shape();
    for ( unsigned int dimension=0; dimension < NumDimensions; ++dimension ) {
        // maximum size in this dimension
        const size_t max = _nodes.shape()[dimension];

        // starting location is based off our corner coordinates
        // stride comes from our shape
        size_t start = 0;
        size_t stride = 0;
        switch ( dimension ) {
            case BGQTopology::A: start = coordinates.a(); stride = shape.a(); break;
            case BGQTopology::B: start = coordinates.b(); stride = shape.b(); break;
            case BGQTopology::C: start = coordinates.c(); stride = shape.c(); break;
            case BGQTopology::D: start = coordinates.d(); stride = shape.d(); break;
            case BGQTopology::E: start = coordinates.e(); stride = shape.e(); break;
            default: BOOST_ASSERT( !"unhandled dimension" );
        }

        // validate shape does not extend past maximum size in this dimension
        if ( start + stride > max ) {
            std::ostringstream msg;
            msg <<
                "corner " << corner << 
                " with coordinates " << corner.getMidplaneCoordinates() <<
                " using shape " << shape <<
                " extends beyond "
                ;
            switch ( dimension ) {
                case 0: msg << "A"; break;
                case 1: msg << "B"; break;
                case 2: msg << "C"; break;
                case 3: msg << "D"; break;
                case 4: msg << "E"; break;
                default: BOOST_ASSERT( !"unhandled dimension" );
            }
            msg << " dimension of midplane " << corner.getMidplane();
            job->error(
                    msg.str(),
                    error_code::shape_invalid
                    );
            return false;
        }
    }

    if ( corner.isCoreSet() ) {
        LOG_INFO_MSG( 
                "sub-node shape: " << 
                shape << 
                "x" <<
                static_cast<unsigned>( corner.getCore() )
                );
    } else {
        LOG_INFO_MSG( "sub-block shape: " << shape );
    }

    return true;
}

bool
Midplane::validateSubBlockNp(
        const job::Create::Ptr& job
        )
{
    JobInfo& info = job->_job_info;
    const SubBlock& subBlock = info.getSubBlock();
    const Shape& shape = subBlock.shape();
    const size_t np = info.getNp();
    const size_t ranks = info.getRanksPerNode();
    const size_t jobSize = shape.size() * ranks;
    if ( np > jobSize ) {
        job->error(
                "np value of " + boost::lexical_cast<std::string>(np) +
                " with ranks per node value of " + boost::lexical_cast<std::string>(ranks) +
                " is greater than sub-block shape " + std::string(shape) +
                " size of " + boost::lexical_cast<std::string>( shape.size()),
                error_code::job_np_invalid 
                );
        return false;
    }

    // set --np to job size if it has not been set
    if ( !np ) {
        info.setNp( static_cast<unsigned>(jobSize) );
        LOG_TRACE_MSG( "set np to " << jobSize );
    }

    return true;
}

bool
Midplane::arbitrate(
        const job::Create::Ptr& job
        )
{
    LOGGING_DECLARE_LOCATION_MDC( _location );

    if ( !this->available(job) ) return false;

    // check if we have any sub-block jobs running
    for ( unsigned a = 0; a < _nodes.shape()[0]; ++a ) {
        for ( unsigned b = 0; b < _nodes.shape()[1]; ++b ) {
            for ( unsigned c = 0; c < _nodes.shape()[2]; ++c ) {
                for ( unsigned d = 0; d < _nodes.shape()[3]; ++d ) {
                    for ( unsigned e = 0; e < _nodes.shape()[4]; ++e ) {
                        const ComputeNode::Ptr compute = _nodes[a][b][c][d][e];

                        // ensure node is available
                        if ( !compute->available(job) ) {
                            return false;
                        }

                        // add I/O link
                        if ( const IoNode::Ptr& io = compute->getIo() ) {
                            const unsigned computes = 0;
                            job->io( io, computes );
                        }
                    }
                }
            }
        }
    }

    return true;
}

void
Midplane::unavailable(
        const Job::Ptr& job
        )
{
    LOGGING_DECLARE_LOCATION_MDC( _location );
    LOG_TRACE_MSG( "unavailable" );

    for ( unsigned a = 0; a < _nodes.shape()[0]; ++a ) {
        for ( unsigned b = 0; b < _nodes.shape()[1]; ++b ) {
            for ( unsigned c = 0; c < _nodes.shape()[2]; ++c ) {
                for ( unsigned d = 0; d < _nodes.shape()[3]; ++d ) {
                    for ( unsigned e = 0; e < _nodes.shape()[4]; ++e ) {
                        const ComputeNode::Ptr compute = _nodes[a][b][c][d][e];

                        if ( job == _job.lock() ) {
                            // full block job
                            compute->unavailable();
                        } else {
                            // sub-block job
                            compute->unavailable( job );
                        }
                    }
                }
            }
        }
    }
}

const ComputeNode::Ptr&
Midplane::node(
        const Coordinates& coordinates
        ) const
{
    if ( coordinates.a() >= _nodes.shape()[0] ) {
        BOOST_THROW_EXCEPTION( 
                std::invalid_argument( 
                    "A dimension: " + 
                    boost::lexical_cast<std::string>( coordinates.a() ) +
                    " is greater than size " +
                    boost::lexical_cast<std::string>( _nodes.shape()[0] )
                    )
                );
    }

    if ( coordinates.b() >= _nodes.shape()[1] ) {
        BOOST_THROW_EXCEPTION( 
                std::invalid_argument( 
                    "B dimension: " + 
                    boost::lexical_cast<std::string>( coordinates.b() ) +
                    " is greater than size " +
                    boost::lexical_cast<std::string>( _nodes.shape()[1] )
                    )
                );
    }

    if ( coordinates.c() >= _nodes.shape()[2] ) {
        BOOST_THROW_EXCEPTION( 
                std::invalid_argument( 
                    "C dimension: " + 
                    boost::lexical_cast<std::string>( coordinates.c() ) +
                    " is greater than size " +
                    boost::lexical_cast<std::string>( _nodes.shape()[2] )
                    )
                );
    }
    
    if ( coordinates.d() >= _nodes.shape()[3] ) {
        BOOST_THROW_EXCEPTION( 
                std::invalid_argument( 
                    "D dimension: " + 
                    boost::lexical_cast<std::string>( coordinates.d() ) +
                    " is greater than size " +
                    boost::lexical_cast<std::string>( _nodes.shape()[3] )
                    )
                );
    }

    if ( coordinates.e() >= _nodes.shape()[4] ) {
        BOOST_THROW_EXCEPTION( 
                std::invalid_argument( 
                    "E dimension: " + 
                    boost::lexical_cast<std::string>( coordinates.e() ) +
                    " is greater than size " +
                    boost::lexical_cast<std::string>( _nodes.shape()[4] )
                    )
                );
    }

    return _nodes[ coordinates.a() ][ coordinates.b() ][ coordinates.c() ][ coordinates.d() ][ coordinates.e() ];
}

const Coordinates*
Midplane::getCorner(
        const Corner& corner
        )
{
    CornerMap::const_iterator result = _corners.end();

    // special case for sub-node
    if ( corner.isCoreSet() ) {
        // extract the row, column, midplane, node board, and compute card from
        // the corner UCI but omit the processor core
        BG_UniversalComponentIdentifier uci( 0 );
        uci |= BG_UCI_SET_ROW( BG_UCI_GET_ROW(corner.getUci().get()) );
        uci |= BG_UCI_SET_COLUMN( BG_UCI_GET_COLUMN(corner.getUci().get()) );
        uci |= BG_UCI_SET_MIDPLANE( BG_UCI_GET_MIDPLANE(corner.getUci().get()) );
        uci |= BG_UCI_SET_NODE_BOARD( BG_UCI_GET_NODE_BOARD(corner.getUci().get()) );
        uci |= BG_UCI_SET_COMPUTE_CARD( BG_UCI_GET_COMPUTE_CARD(corner.getUci().get()) );
        uci |= BG_UCI_SET_COMPONENT( BG_UCI_Component_ComputeCardOnNodeBoard );
        LOG_TRACE_MSG( "searching for corner node: " << Uci(uci) << " from core " << corner.getUci() );
        result = _corners.find( uci );
    } else {
        result = _corners.find( corner.getUci() );
    }

    if ( result == _corners.end() ) {
        return 0;
    } else {
        return &result->second;
    }
}

} // block
} // server
} // runjob
