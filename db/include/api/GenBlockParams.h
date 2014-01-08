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

#ifndef BGQDB_GEN_BLOCK_PARAMS_H_
#define BGQDB_GEN_BLOCK_PARAMS_H_

#include "BGQDBlib.h"
#include "BlockDatabaseInfo.h"

#include <boost/array.hpp>

#include <iosfwd>
#include <string>
#include <vector>

#include <stdint.h>

namespace BGQDB {

/*! \brief Specification for a block in a dimension.
 *
 *  In each dimension, a large block has
 *  - The midplanes that are included or passed through
 *  - Connectivity, either Torus or Mesh
 *
 *  This class represents the midplanes that are included or passed through
 *  as a vector<bool>,
 *  where the value is true if the midplane is included
 *  and false if the midplane is passed through.
 *
 *  The number of 1s is the size of the block in the dimension.
 *
 */
class DimensionSpec
{
public:

    typedef std::vector<bool> IncludedMidplanes; //!< Type for indicators of whether the midplane is included or passed through.


    static const DimensionSpec Midplane; //!< A single midplane.
    static const DimensionSpec Full; //!< Use the full size of the machine in the dimension.


    /*! \brief Parse a string of 0s and 1s into an IncludedMidplanes. */
    static IncludedMidplanes strToMidplanes( const std::string& s );

    /*! \brief Generate a string of 1s and 0s given the midplanes. */
    static std::string midplanesToStr( const IncludedMidplanes& mps );


    /*! \brief Construct with the given size and connectivity.
     *
     *  All the midplanes are included.
     *
     *  If the size is 1 the connectivity must be Torus.
     *
     *  \throw invalid_argument if the arguments are not valid.
     */
    DimensionSpec(
            unsigned size = 1,
            Connectivity::Value connectivity = Connectivity::Torus
        );

    /*! \brief Construct with the included midplanes and connectivity.
     *
     *  true indicates to include the midplane and false indicates to pass through.
     *
     *  If the size is 1, the connectivity must be Torus
     *  and must not have any passthrough.
     *  If the connectivity is Mesh then must include first and last midplane.
     *
     *  \throw invalid_argument is the arguments are not valid.
     */
    DimensionSpec(
            const IncludedMidplanes& included_midplanes,
            Connectivity::Value connectivity = Connectivity::Torus
        );

    /*! \brief Create from a string.
     *
     *  The format of str is like [&lt;size&gt;][T|M][:&lt;mp_specs&gt;]
     *
     *  Size is the size. If left off, size is specified my mp_specs.
     *
     *  T|M specifies the connectivity (Torus or Mesh).
     *  The default is Torus.
     *
     *  mp_specs is a string where 1 indicates include and 0 indicates passthrough.
     *  The default is to include all given the size.
     *
     *  If both size and mp_specs are specified,
     *  then the number of 1s in mp_specs must be the same as the size.
     *
     *  If neither size nor mp_specs are specified,
     *  then indicates use the full machine.
     *
     *  If the size is 1, the connectivity must be Torus
     *  and must not have any passthrough.
     *  If the connectivity is Mesh then must include first and last midplane.
     *
     *  \throw invalid_argument is the arguments are not valid.
     */
    explicit DimensionSpec(
            const std::string& str
        );

    /*! \brief Create from a size and string.
     *
     *  The format of the string here is like [T|M][&lt;mp_specs&gt;]
     *
     *  size can be 0, indicating calculate from mp_specs.
     *
     *  T|M specifies the connectivity (Torus or Mesh).
     *  The default is Torus.
     *
     *  If size is 0 and mp_specs is not specified then
     *  the full machine will be used.
     *
     *  If the size is 1, the connectivity must be Torus
     *  and must not have any passthrough.
     *  If the connectivity is Mesh then must include first and last midplane.
     *
     *  \throw invalid_argument is the arguments are not valid.
     */
    DimensionSpec(
            unsigned size,
            const std::string& str
        );


    /*! \brief For small blocks and large blocks with midplanes, just set the connectivity. */
    explicit DimensionSpec(
            Connectivity::Value connectivity
        );


    /*! \brief Get the connectivity for the dimension. */
    Connectivity::Value getConnectivity() const  { return _connectivity; }

    /*! \brief Get the midplane indicators for the dimension.
     *
     *  If is empty, indicates use the full machine.
     */
    const IncludedMidplanes& getIncludedMidplanes() const  { return _included_midplanes; }

    /*! \brief Get the number of included midplanes in the dimension.
     *
     *  May be 0, indicating use the full machine.
     */
    uint8_t size() const;

    /*! \brief Convert to a string that can be used on the string constructor. */
    std::string toString() const;


private:

    Connectivity::Value _connectivity;
    IncludedMidplanes _included_midplanes;


    void _validate();
};


typedef boost::array<DimensionSpec,Dimension::Count> DimensionSpecs; //!< Type for dimension spec in each dimension.

extern const DimensionSpecs MidplaneDimensionSpecs; //!< Dimension specs for a single-midplane block.
extern const DimensionSpecs FullDimensionSpecs; //!< Dimension specs to use the full machine.


/*! \brief Parameters to genBlock. */
class GenBlockParams
{
public:

    typedef std::vector<std::string> NodeBoardLocations;

    typedef boost::array<std::string,Dimension::Count> DimensionStrings; //!< String in each dimension.

    typedef std::vector<std::string> MidplaneLocations;


    static const MidplaneLocations EmptyPassthroughLocations;


    /*! \brief Constructor.
     *
     *  The GenBlockParams starts out with Large / No Midplanes and a full midplane.
     *  The block ID is not set.
     *  The description is not set.
     */
    GenBlockParams();

    /*! \brief Set the block ID.
     *
     * The block ID must be valid
     * (cannot be empty,
     *  too long,
     *  or contain invalid characters).
     *
     * \throw std::invalid_argument if the block ID is not valid.
     */
    void setBlockId( const std::string& block_id );

    /*! \brief Set the description.
     *
     *  The default description is an empty string.
     */
    void setDescription( const std::string& description )  { _description = description; }

    /*! \brief Set the Options.
     *
     *  The default Options is an empty string.
     */
    void setOptions( const std::string& options )  { _options = options; }

    /*! \brief Set the Boot Options.
     *
     *  The default Boot Options is an empty string.
     */
    void setBootOptions( const std::string& boot_options )  { _boot_options = boot_options; }

    /*! \brief Set the Microloader Image filename.
     *
     *  The default Microloader Image is DEFAULT_MLOADERIMG.
     */
    void setMicroloaderImage( const std::string& microloader_image )  { _microloader_image = microloader_image; }

    /*! \brief Set the Node Configuration name.
     *
     *  The default Node Configuration is DEFAULT_COMPUTENODECONFIG.
     */
    void setNodeConfiguration( const std::string& node_configuration )  { _node_configuration = node_configuration; }


    /*! \brief Set the Owner.
     *
     *  The default Owner is an empty string.
     */
    void setOwner( const std::string& owner )  { _owner = owner; }

    /*! \brief Set the midplane.
     *
     *  For a large block, this is the corner midplane.
     *  For a small block, this is the midplane.
     *
     *  If was Large / Midplanes is now Large / No Midplanes.
     *
     *  The midplane must be valid (cannot be empty or too long).
     *
     *  \throw std::invalid_argument if the midplane is not valid.
     */
    void setMidplane( const std::string& midplane_location );

    /*! \brief Set the dimension specs.
     *
     *  Sets the type to Large.
     */
    void setDimensionSpecs(
            const DimensionSpecs& dim_specs
        );

    /*! \brief Set the midplanes and passthrough midplanes.
     *
     *   Sets the type to Large / Midplanes.
     *
     *   The dimension specs are reset to torus in all dimensions
     *   (with empty included midplanes).
     *   Use setDimensionSpecs to set the connectivity in each dimension.
     *
     *   If the dimension connectivity is a mesh,
     *   the mesh will start at the first midplane location given.
     *
     *   \throws std::invalid_argument if midplanes is empty.
     */
    void setMidplanes(
            const MidplaneLocations& midplanes,
            const MidplaneLocations& passthrough_locations = EmptyPassthroughLocations
        );

    /*! \brief Set the starting node board and number of node boards in the small block.
     *
     *  Positions are N00 - N15.
     *
     *  The valid node board and count combinations are:
     *  all positions -- 1
     *  N00, N02, N04, N06, N08, N10, N12, N14 -- 2
     *  N00, N04, N08, N12 -- 4
     *  N00, N08 -- 8
     *
     *  Sets the size to Small.
     *
     *  The dimension specs are set so that the connectivity is
     *  correct for the size of the small block,
     *  but the included midplanes are empty.
     *
     *  \throw std::invalid_argument if the start node board position or node board count are not valid.
     */
    void setNodeBoardAndCount(
            const std::string& start_node_board_position,
            uint8_t node_board_count
        );

    /*! \brief Set the node boards in the small block.
     *
     *  Node board locations are like R00-M0-N00.
     *
     *  Sets the size to Small.
     *
     *  The dimension specs are set so that the connectivity is
     *  correct for the size of the small block,
     *  but the included midplanes are empty.
     *
     *  The collection of node boards must form a valid small block.
     *  The node boards must be contiguous and be a power of 2 and start at the right position.
     *
     *  The midplane must be the same for all node board locations (throws std::invalid_argument).
     *
     *  \throw std::invalid_argument if the node board locations are not valid.
     */
    void setNodeBoardLocations(
            const NodeBoardLocations& node_board_locations
        );

    /*! \brief Gets the block ID.
     *
     *  The block ID must have been set.
     *
     *  \throw std::logic_error if the block ID hasn't been set.
     */
    const std::string& getBlockId() const;

    /*! \brief Get the description.
     *
     *  If the description wasn't set then an empty string is returned.
     */
    const std::string& getDescription() const  { return _description; }

    const std::string& getOptions() const  { return _options; }

    const std::string& getBootOptions() const  { return _boot_options; }

    const std::string& getMicroloaderImage() const  { return _microloader_image; }

    const std::string& getNodeConfiguration() const  { return _node_configuration; }

    const std::string& getOwner() const  { return _owner; }

    /*! \brief Get the midplane.
     *
     *  The midplane must have been set.
     *
     *  \throw std::logic_error if the midplane hasn't been set.
     */
    const std::string& getMidplane() const;

    /*! \brief Get whether the block is Large or Small. */
    bool isLarge() const;

    /*! \brief Get whether is Large / Midplanes.
     *
     *   If it's Large and not Large / Midplanes then it's Large / No Midplanes.
     */
    bool isLargeWithMidplanes() const;


    /*! \brief Gets the dimension specs. */
    const DimensionSpecs& getDimensionSpecs() const  { return _dimension_specs; }


    /*! \brief Get the midplanes.
     *
     *  \throw std::logic_error if the block is not Large / Midplanes.
     */
    const MidplaneLocations& getMidplanes() const;

    /*! \brief Gets the pasthrough locations.
     *
     *  \throw std::logic_error if the block is not Large / Midplanes.
     */
    const MidplaneLocations& getPassthroughLocations() const;


    /*! \brief Get the configured node board positions.
     *
     *  \return vector of N00, N01, etc.
     *
     *  \throw std::logic_error if the size is Large.
     */
    const NodeBoardPositions& getNodeBoardPositions() const;

    /*! \brief Gets the starting node board position.
     *
     *  The node board and count must have been set.
     *
     *  \throw std::logic_error if the size is Large.
     */
    const std::string& getNodeBoard() const;

    /*! \brief Gets the number of node boards.
     *
     *  The node board and count must have been set.
     *
     *  \throw std::logic_error if the size is Large.
     */
    uint8_t getNodeBoardCount() const;

    /*! \brief Calculate the included midplanes as a string for each dimension.
     *
     *  The strings can be used with the DimensionSpec( const string& ) constructor.
     *
     *  \throw std::logic_error if the size is Small.
     */
    DimensionStrings calcDimensionSpecStrings() const;


    friend std::ostream& operator<<( std::ostream& os, const GenBlockParams& gen_block_params );


private:

    std::string _block_id;
    std::string _description;
    std::string _options;
    std::string _boot_options;
    std::string _microloader_image;
    std::string _node_configuration;
    std::string _owner;

    std::string _midplane_location;
    MidplaneLocations _midplanes, _passthrough_locations;
    NodeBoardPositions _node_board_positions;

    DimensionSpecs _dimension_specs;
};


std::ostream& operator<<( std::ostream& os, const GenBlockParams& gen_block_params );


} // namespace BGQDB

#endif
