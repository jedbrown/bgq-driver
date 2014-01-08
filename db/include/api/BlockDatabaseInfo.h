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

#ifndef BGQDB_BLOCK_DATABASE_INFO_H_
#define BGQDB_BLOCK_DATABASE_INFO_H_


#include "BGQDBlib.h"

#include <boost/array.hpp>
#include <boost/shared_ptr.hpp>

#include <deque>
#include <iosfwd>
#include <iostream>
#include <string>

namespace BGQDB {


class BlockDatabaseInfo
{
public:

    /*! \brief Pointer type. */
    typedef boost::shared_ptr<BlockDatabaseInfo> Ptr;

    typedef boost::array<Connectivity::Value,Dimension::Count> ConnectivitySpecs;


    struct MidplaneInfo {
        std::string location;
        MidplaneCoordinate coord;

        MidplaneInfo( const std::string& location_in, const MidplaneCoordinate& coord_in )
            : location(location_in), coord(coord_in)
        { /* Nothing to do */ }

        bool operator==( const MidplaneInfo& other ) const
        { return (location == other.location) && (coord == other.coord); }
    };

    typedef std::deque<MidplaneInfo> Midplanes;


    struct SwitchInfo {
        std::string id;
        SwitchConfig::Value config;
        bool used_for_passthrough; // Not in the database but useful to know.

        SwitchInfo(
                const std::string& id_in,
                SwitchConfig::Value config_in,
                bool used_for_passthrough_in
            ) :
                id(id_in),
                config(config_in),
                used_for_passthrough(used_for_passthrough_in)
        { /* Nothing to do */ }

        bool operator==( const SwitchInfo& other ) const
        { return (id == other.id) && (config == other.config) && (used_for_passthrough == other.used_for_passthrough); }
    };

    typedef std::deque<SwitchInfo> Switches;
    typedef std::deque<std::string> Links;


    BlockDatabaseInfo() { /* Nothing to do */ }

    void setId( const std::string& id )  { _id = id; }
    void setDescription( const std::string& description )  { _description = description; }
    void setOwner( const std::string& owner )  { _owner = owner; }
    void setOptions( const std::string& options )  { _options = options; }
    void setBootOptions( const std::string& boot_options )  { _boot_options = boot_options; }
    void setMloaderImg( const std::string& mloader_img )  { _mloader_img = mloader_img; }
    void setNodeConfig( const std::string& node_config )  { _node_config = node_config; }
    void setSizes( const DimensionSizes& sizes )  { _sizes = sizes; }
    void setConnectivity( const ConnectivitySpecs& connectivity )  { _connectivity = connectivity; }

    /*! \brief For a large block, add a midplane for the bpblockmap. */
    void addMidplane( const MidplaneInfo& info )  { _mps.push_back( info ); }

    /*! \brief For a large block, add a passthrough midplane. */
    void addPassthroughMidplane( const MidplaneInfo& info )  { _passthru_mps.push_back( info ); }

    /*! \brief For a large block, add a switch for the switchBlockMap. */
    void addSwitch( const SwitchInfo& info )  { _switches.push_back( info ); }

    /*! \brief For a large block, add a link for the linkBlockMap. */
    void addLink( const std::string& link_id );

    /*! \brief For a small block, this is the midplane it's on. */
    void setMidplane( const std::string& location )  { _midplane = location; }

    /*! \brief For a small block, the node board positions. */
    void setNodeBoards( const NodeBoardPositions& node_boards )  { _node_boards = node_boards; }


    const std::string& getId() const  { return _id; }
    const std::string& getDescription() const  { return _description; }
    const std::string& getOwner() const  { return _owner; }
    const std::string& getOptions() const  { return _options; }
    const std::string& getBootOptions() const  { return _boot_options; }
    const std::string& getMloaderImg() const  { return _mloader_img; }
    const std::string& getNodeConfig() const  { return _node_config; }
    const DimensionSizes& getSizes() const  { return _sizes; }
    const ConnectivitySpecs& getConnectivitySpecs() const  { return _connectivity; }

    /*! \brief For a large block, the midplanes. */
    const Midplanes& getMidplanes() const  { return _mps; }

    /*! \brief For a large block, any passthrough midplanes. */
    const Midplanes& getPassthroughMidplanes() const  { return _passthru_mps; }

    /*! \brief For a large block, the switches. */
    const Switches& getSwitches() const  { return _switches; }

    /*! \brief For a large block, the links. */
    const Links& getLinks() const  { return _links; }

    /*! \brief For a small block, the midplane. */
    const std::string& getMidplane() const  { return _midplane; }

    /*! \brief For a small block, the node board positions. */
    const NodeBoardPositions& getNodeBoards() const  { return _node_boards; }

    bool isLarge() const {
        if (_node_boards.empty()) {
            return true;
        } else {
            return false;
        }
    }


    bool operator==( const BlockDatabaseInfo& other ) const;


private:

    std::string _id;
    std::string _description;
    std::string _options;
    std::string _boot_options;
    std::string _mloader_img;
    std::string _node_config;
    std::string _owner;
    DimensionSizes _sizes;
    ConnectivitySpecs _connectivity;

    Midplanes _mps;
    Midplanes _passthru_mps;
    Switches _switches;
    Links _links;

    std::string _midplane;
    NodeBoardPositions _node_boards;
};


std::ostream& operator<<( std::ostream& os, const BlockDatabaseInfo& bdi );

} // namespace BGQDB


#endif
