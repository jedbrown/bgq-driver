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
/* (C) Copyright IBM Corp.  2004, 2011                              */
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

#ifndef BGQ_DB_BGQDBLIB_H
#define BGQ_DB_BGQDBLIB_H

#include "job/types.h"

#include "BlockInfo.h"

#include "cxxdb/fwd.h"

#include <bgq_util/include/Location.h>
#include <bgq_util/include/LocationUtil.h>
#include <utility/include/Properties.h>

#include <boost/array.hpp>
#include <boost/function.hpp>
#include <boost/unordered_map.hpp>

#include <bitset>
#include <deque>
#include <iosfwd>
#include <map>
#include <set>
#include <string>
#include <vector>

#include <stdint.h>


// forward declarations
class RasEvent;

/*!
 * \brief
 */
namespace BGQDB
{

extern const uint32_t  Nodes_Per_Midplane;                   //!< Number of nodes on a midplane
extern const uint32_t  Midplane_A_Dimension;                 //!< Nodes in the A dimension for a midplane
extern const uint32_t  Midplane_B_Dimension;                 //!< Nodes in the B dimension for a midplane
extern const uint32_t  Midplane_C_Dimension;                 //!< Nodes in the C dimension for a midplane
extern const uint32_t  Midplane_D_Dimension;                 //!< Nodes in the D dimension for a midplane
extern const uint32_t  Midplane_E_Dimension;                 //!< Nodes in the E dimension for a midplane
extern const uint32_t  Nodes_Per_Node_Board;                 //!< Number of nodes on a node board

extern const char* BLOCK_FREE;
extern const char* BLOCK_ALLOCATED;
extern const char* BLOCK_BOOTING;
extern const char* BLOCK_BOOTING_NO_CHECK;
extern const char* BLOCK_DEALLOCATING;
extern const char* BLOCK_INITIALIZED;
extern const char* BLOCK_TERMINATING;
extern const char* BLOCK_NO_ACTION;

extern const char* HARDWARE_AVAILABLE;
extern const char* HARDWARE_MISSING;
extern const char* HARDWARE_ERROR;
extern const char* HARDWARE_SERVICE;
extern const char* SOFTWARE_FAILURE;

extern const std::string DEFAULT_MLOADERIMG;
extern const std::string DEFAULT_COMPUTENODECONFIG;
extern const std::string DEFAULT_IONODECONFIG;

typedef std::bitset<30> ColumnsBitmap;

/*!
 * \brief Return value enumeration for all calls offered by this library
 */
enum STATUS {
    OK = 0,
    DB_ERROR,
    FAILED,
    INVALID_ID,
    NOT_FOUND,
    DUPLICATE,
    XML_ERROR,
    CONNECTION_ERROR,
    INVALID_ARG
};

/*!
 * \brief Block states
 */
enum BLOCK_STATUS {
    INVALID_STATE = 0,
    FREE,
    ALLOCATED,
    INITIALIZED,
    BOOTING,
    TERMINATING
};

/*!
 * \brief Possible actions required by a Block
 */
enum BLOCK_ACTION {
    NO_BLOCK_ACTION = 0,
    CONFIGURE_BLOCK,
    DEALLOCATE_BLOCK,
    CONFIGURE_BLOCK_NO_CHECK // For I/O blocks only and indicates to skip I/O nodes in error when booting
};

/*!
 * \brief
 */
enum DIAGS_MODE {
    NO_DIAGS = 0,
    NORMAL_DIAGS,
    SVCACTION_DIAGS
};


/*! \brief Dimensions enum container.
 *
 * When creating blocks, there's essentially only 4 dimensions to worry about, since the E dim is always 2.
 *
 */
struct Dimension {
    enum Value {
        A, B, C, D
    };

    static const unsigned Count = D + 1; //!< The number of dimensions.

    /*! \brief Convert a Dimension::Value to a string. */
    static const std::string& toString( Value dim );
};


/*
 * Dynamic wire sparing information used by the postProcessRAS method to process
 * the BQL_SPARE event generated for a specific RX link chip.
 */
struct SparingInfo {
    std::string txLoc;     // TX link chip location (Rxx-Mx-Nxx-Lxx, Rxx-Ix-Uxx, or Qxx-Ix-Uxx)
    std::string txReg;     // TX link chip register (D01 or D23)
    std::string rxLoc;     // RX link chip location (Rxx-Mx-Nxx-Lxx, Rxx-Ix-Uxx, or Qxx-Ix-Uxx)
    std::string rxReg;     // RX link chip register (C01 or C23)
    int         wireMask;  // Aggregated bad wire mask for BGQCable table (48 bits)
    uint16_t    txMask;    // Bad fiber mask for the TX register (12 bits)
    uint16_t    rxMask;    // Bad fiber mask for the RX register (12 bits)
};

typedef boost::array<uint32_t, Dimension::Count> DimensionSizes; //!< Size in each dimension
typedef std::vector<std::string> NodeBoardPositions; //!< Vector of node board positions, like "N00".
typedef boost::array<uint32_t, Dimension::Count> MidplaneCoordinate; //!< The coordinate of a midplane (A,B,C,D), either the offset in the machine torus or the offset in the block.


bool operator==( const MidplaneCoordinate& lhs, const MidplaneCoordinate& rhs );

/*! \brief Allows MidplaneCoordinate in a boost::unordered_hash or _set. */
struct MidplaneCoordinateHashFn : std::unary_function<MidplaneCoordinate, size_t>
{
    size_t operator()( const MidplaneCoordinate& coord ) const;
};


/*! \brief Each dimension can be connected as either a Torus or a Mesh. */
struct Connectivity {
    enum Value {
        Torus,
        Mesh
    };
};


typedef boost::array<Connectivity::Value,Dimension::Count> DimensionConnectivity; //!< Connectivity in each dimension.


/*! \brief A switch can be set to either Wrap, Include-both in and out, Include-out only, or Include-in only. */
struct SwitchConfig {
    enum Value {
        Wrap,
        Include,
        Out,
        In
    };
};

typedef std::vector<std::string> ConnectedIONodes;

class MidplaneIOInfo
{
public:
    MidplaneIOInfo() :
        IOLinkCount( 0 ),
        IONodes()
    {

    }

    int IOLinkCount;
    ConnectedIONodes IONodes;
};


/*! \brief Values in the Replacement_history table type column. See create_trigger_bgq.sql. */
namespace replacement_history_types {
    extern const std::string Midplane;
    extern const std::string NodeCard;
    extern const std::string NodeBoardDCA;
    extern const std::string Node;
    extern const std::string IoNode;
    extern const std::string IoRack;
    extern const std::string IoDrawer;
    extern const std::string ServiceCard;
    extern const std::string ClockCard;
    extern const std::string LinkChip;
    extern const std::string IoLinkChip;
    extern const std::string BulkPower;
}

/*!
 * \brief initialize the API.
 *
 * The minimum and maximum pool connection values are obtained from the
 * min_pool_connections and max_pool_connections keys in the requested
 * section of the properties file. If those values are outside the range
 * (see DBConnectionPool) they will be set to the default values.
 *
 * If init() has already been called, does nothing.
 *
 * \throws std::invalid_argument if properties == NULL
 */
void init(
        const bgq::utility::Properties::ConstPtr properties,    //!< [in]
        const std::string& section_name                         //!< [in]
        );

/*!
 * \brief initialize the API.
 *
 * The minimum and maximum pool connection values come from the parameters.
 *
 * If init() has already been called, does nothing.
 *
 * \throws std::logic_error if the API has already been initialized.
 * \throws std::invalid_argument if connection_pool_size is too large.
 * \throws std::invalid_argument if properties == NULL
 */
void init(
        const bgq::utility::Properties::ConstPtr properties,    //!< [in]
        unsigned connection_pool_size                           //!< [in]
        );

bool checkIdentifierValidCharacters(
        const std::string& id
        );

bool isBlockIdValid(
        const std::string& blockid,
        const char *function_name
        );

/*!
 *  \brief Get machine data in XML format.
 *
 *  \return
 *  - DB_ERROR if database error occurs retrieving data.
 *  - OK if data was retrieved successfully.
 */
STATUS getMachineXML(
        std::ostream& xml,                      //!< [out]
        std::vector<std::string>* memory = NULL //!< [out] invalid memory locations
        );

/*!
 *  \brief Get midplanes in XML format.
 *
 *  \return
 *  - DB_ERROR if database error occurs retrieving data.
 *  - CONNECTION_ERROR if database connection error occurs retrieving data.
 *  - NOT_FOUND if no midplanes found in database.
 *  - OK if data was retrieved successfully.
 */
STATUS getBPs(
        std::ostream& xml      //!< [out]
        );

/*!
 *  \brief Get node boards for midplane in XML format.
 *
 *  \return
 *  - DB_ERROR if database error occurs retrieving data.
 *  - CONNECTION_ERROR if database connection error occurs retrieving data.
 *  - OK if data was retrieved successfully.
 */
STATUS getBPNodeCards(
        std::ostream& xml,              //!< [out]
        const std::string& midplane     //!< [in] midplane ID
        );

/*!
 *  \brief Get switches in XML format.
 *
 *  \return
 *  - DB_ERROR if database error occurs retrieving data.
 *  - CONNECTION_ERROR if database connection error occurs retrieving data.
 *  - NOT_FOUND if no switches found in database.
 *  - OK if data was retrieved successfully.
 */
STATUS getSwitches(
        std::ostream& xml      //!< [out]
        );

/*!
 *  \brief Get midplane cables in XML format.
 *
 *  \return
 *  - DB_ERROR if database error occurs retrieving data.
 *  - CONNECTION_ERROR if database connection error occurs retrieving data.
 *  - NOT_FOUND if no midplane cables found in database.
 *  - OK if data was retrieved successfully.
 */
STATUS getBPWireList(
        std::ostream& xml       //!< [out]
        );

/*!
 *  \brief Get midplane I/O links in XML format.
 *
 *  \return
 *  - DB_ERROR if database error occurs retrieving data.
 *  - CONNECTION_ERROR if database connection error occurs retrieving data.
 *  - NOT_FOUND if no midplane I/O links found in database.
 *  - OK if data was retrieved successfully.
 */
STATUS getIOWireList(
        std::ostream& xml,              //!< [out]
        const std::string& midplane     //!< [in] midplane ID
        );

/*!
 *  \brief Get I/O links for block in XML format.
 *
 *  \return
 *  - DB_ERROR if database error occurs retrieving data.
 *  - CONNECTION_ERROR if database connection error occurs retrieving data.
 *  - NOT_FOUND if no I/O links found in database.
 *  - OK if data was retrieved successfully.
 */
STATUS checkIOLinks(
        std::ostream& xml,              //!< [out]
        const std::string& block    //!< [in] block ID
        );

/*!
 *  \brief Get machine midplane dimension sizes.
 *
 *  \return
 *  - DB_ERROR if database error occurs retrieving data.
 *  - CONNECTION_ERROR if database connection error occurs retrieving data.
 *  - NOT_FOUND if R00-M0 midplane not found in database.
 *  - OK if data was retrieved successfully.
 */
STATUS getMachineBPSize(
        DimensionSizes& midplane_sizes_out
        );

/*!
 *  \brief Get machine midplane dimension sizes.
 *
 *  \return
 *  - DB_ERROR if database error occurs retrieving data.
 *  - CONNECTION_ERROR if database connection error occurs retrieving data.
 *  - NOT_FOUND if R00-M0 midplane not found in database.
 *  - OK if data was retrieved successfully.
 */
STATUS getMachineBPSize(
        uint32_t& a,        //!< [out]
        uint32_t& b,        //!< [out]
        uint32_t& c,        //!< [out]
        uint32_t& d         //!< [out]
        );


/*!
 *  \brief Get block in XML format.
 *
 *  \return
 *  - DB_ERROR if database error occurs retrieving data.
 *  - CONNECTION_ERROR if database connection error occurs retrieving data.
 *  - NOT_FOUND if block not found in database.
 *  - OK if data was retrieved successfully.
 */
STATUS getBlockXML(
        std::ostream& xml,          //!< [out]
        const std::string& block,   //!< [in] block ID
        bool diags = false          //!< [in]
        );

/*!
 *  \brief Get all block names.
 *
 *  \return
 *  - DB_ERROR if database error occurs retrieving data.
 *  - CONNECTION_ERROR if database connection error occurs retrieving data.
 *  - OK if data was retrieved successfully.
 */
STATUS getBlockIds(
        const std::string& whereClause,         //!< [in]
        std::vector<std::string>& outBlockList  //!< [out]
        );

/*!
 *  \brief Removes the block from the database.
 *
 *  A block can only be deleted if its state is Free.
 *
 *  \return
 *  - DB_ERROR if database error occurs retrieving data.
 *  - CONNECTION_ERROR if database connection error occurs retrieving data.
 *  - INVALID_ID if the block name is too long.
 *  - NOT_FOUND if the block doesn't exist in the database.
 *  - FAILED if the block is not Free.
 *  - OK if the block was deleted.
 */
STATUS deleteBlock(
        const std::string& block        //!< [in] block ID
        );

/*!
 *  \brief Insert RAS event into database.
 *
 *  \return
 *  - DB_ERROR if database error occurs retrieving data.
 *  - CONNECTION_ERROR if database connection error occurs retrieving data.
 *  - OK if RAS event was inserted to database successfully.
 */
STATUS putRAS(
        const RasEvent& rasEvent,                   //!< [in]
        const std::string& block = std::string(),   //!< [in]
        job::Id job = 0,                            //!< [in] job ID
        const uint32_t qualifier = 0,               //!< [in] boot cookie
        std::vector<job::Id> *jobsToKill=NULL,      //!< [out] list of job IDs to kill due to END_JOB control action
        uint32_t* recid=NULL                        //!< [out] record ID of inserted RAS event
        );

/*!
 *  \brief Augment RAS event with missing info
 *
 *  \return
 *  - DB_ERROR if database error occurs retrieving data.
 *  - CONNECTION_ERROR if database connection error occurs retrieving data.
 *  - OK if RAS event was augmented, or no data was missing
 */
STATUS augmentRAS(
        RasEvent& rasEvent              //!< [in]
        );

/*!
 *  \brief Insert RAS event into database.
 *
 *  \return
 *  - DB_ERROR if database error occurs retrieving data.
 *  - CONNECTION_ERROR if database connection error occurs retrieving data.
 *  - OK if RAS event was inserted to database successfully.
 */
STATUS putRAS(
        const std::string& block,                           //!< [in] block ID
        const std::map<std::string, std::string>& rasmap,   //!< [in]
        const timeval& rastime,                             //!< [in]
        const job::Id job = 0,                              //!< [in] job ID
        const bool diags = false                            //!< [in]
        );

/*!
 *  \brief Process a RAS event after its been inserted into database.
 *
 *  \return
 *  - DB_ERROR if database error occurs retrieving data.
 *  - CONNECTION_ERROR if database connection error occurs retrieving data.
 *  - OK if RAS event was processed successfully.
 */
STATUS postProcessRAS(
                      uint32_t recid,                         //!< [in]  record id
                      std::vector<job::Id>& jobsToKill,       //!< [out] job ID(s) to be killed due to control action
                      SparingInfo& info                       //!< [out] wire sparing info for RAS event
                      );

/*!
 *  \brief Query for missing hardware for block.
 *
 *  \return
 *  - DB_ERROR if database error occurs retrieving data or block not found.
 *  - CONNECTION_ERROR if database connection error occurs retrieving data.
 *  - NOT_FOUND if the block doesn't exist in the database.
 *  - OK if data was retrieved successfully.
 */
STATUS queryMissing(
        const std::string& block,                   //!< [in] block ID
        std::vector<std::string>& missing,          //!< [out]
        BGQDB::DIAGS_MODE diags = BGQDB::NO_DIAGS   //!< [in]
        );

/*!
 *  \brief Query for hardware in Error state for a block.
 *
 *  \return
 *  - DB_ERROR if database error occurs retrieving data or block not found.
 *  - CONNECTION_ERROR if database connection error occurs retrieving data.
 *  - OK if data was retrieved successfully.
 */
STATUS queryError(
        const std::string& block,                   //!< [in] block ID
        std::vector<std::string>& error             //!< [out]
        );

/*!
 *  \brief Create new block based on existing block.
 *
 *  \return
 *  - DB_ERROR if database error occurs retrieving data.
 *  - CONNECTION_ERROR if database connection error occurs retrieving data.
 *  - INVALID_ID if block name is incorrect.
 *  - OK if block was created successfully.
 */
STATUS copyBlock(
        const std::string& from,    //!< existing block ID
        const std::string& to,      //!< new block ID
        const std::string& owner    //!< owner
        );

/*!
 *  \brief Get the block state.
 *
 *  \return
 *  - DB_ERROR if database error occurs retrieving data.
 *  - CONNECTION_ERROR if database connection error occurs retrieving data.
 *  - INVALID_ID if the block name is too long.
 *  - NOT_FOUND if the block doesn't exist in the database.
 *  - FAILED if block is in unknown state.
 *  - OK if data was retrieved successful.
 */
STATUS getBlockStatus(
                     const std::string& block,            //!< [in] block ID
                     BGQDB::BLOCK_STATUS& currentState    //!< [out]
                     );

/*!
 *  \brief Get the JTAG id for a list of node board or IO board locations
 *
 *  \return
 *
 *  - DB_ERROR if database error occurs retrieving data.
 *  - CONNECTION_ERROR if database connection error occurs retrieving data.
 *  - INVALID_ID if the block name is too long.
 *  - NOT_FOUND if the block doesn't exist in the database.
 *  - FAILED if block is in unknown state.
 *  - OK if data was retrieved successful.
 */
STATUS getJtagID(
            const std::vector<std::string>& locations,
            std::map<std::string,uint32_t>&   jtag
            );

/*!
 *  \brief Get the node customization.
 *
 *  \return
 *  - DB_ERROR if database error occurs retrieving data.
 *  - CONNECTION_ERROR if database connection error occurs retrieving data.
 *  - INVALID_ID if the block name is too long.
 *  - OK if data was retrieved successful.
 */

STATUS getCustomization(
                        const std::string& id,                //!< [in] block ID
                        std::map<std::string,std::string>& nodeCust    //!< [out]
                        );

/*!
 *  \brief Set the block state.
 *
 *  \return
 *  - DB_ERROR if database error occurs retrieving data.
 *  - CONNECTION_ERROR if database connection error occurs retrieving data.
 *  - INVALID_ID if the block name is too long.
 *  - NOT_FOUND if the block doesn't exist in the database.
 *  - FAILED if block is in unknown state.
 *  - OK if setting block state was successful.
 */
STATUS setBlockStatus(
        const std::string& block,                                           //!< [in] block ID
        BGQDB::BLOCK_STATUS targetState,                                    //!< [in]
        const std::deque<std::string>& option = std::deque<std::string>()   //!< [in] owner=username when targetState == ALLOCATED
        );

/*!
 *  \brief Get block information.
 *
 *  \return
 *  - DB_ERROR if database error occurs retrieving data.
 *  - CONNECTION_ERROR if database connection error occurs retrieving data.
 *  - INVALID_ID if the block name is too long.
 *  - NOT_FOUND if the block doesn't exist in the database.
 *  - FAILED if receiving buffer is not large enough.
 *  - OK if data was retrieved successful.
 */
STATUS getBlockInfo(
        const std::string& id,              //!< [in] block id
        BlockInfo& info                     //!< [out]
        );

/*!
 *  \brief Set block information.
 *
 *  \return
 *  - DB_ERROR if database error occurs retrieving data.
 *  - CONNECTION_ERROR if database connection error occurs retrieving data.
 *  - INVALID_ID if the block name is too long.
 *  - INVALID_ARG if an invalid argument was detected.
 *  - NOT_FOUND if the block doesn't exist in the database.
 *  - OK if block info was set successful.
 */
STATUS setBlockInfo(
        const std::string& id,              //!< [in] block ID
        const BlockInfo& info               //!< [in]
        );

/*!
 *  \brief Get block owner.
 *
 *  \return
 *  - DB_ERROR if database error occurs retrieving data.
 *  - CONNECTION_ERROR if database connection error occurs retrieving data.
 *  - INVALID_ID if the block name is too long.
 *  - NOT_FOUND if the block doesn't exist in the database.
 *  - OK if data was retrieved successful.
 */
STATUS getBlockOwner(
        const std::string& id,              //!< [in] block ID
        std::string& owner                  //!< [out]
        );

/*!
 *  \brief Get block user and qualifier
 *
 *  \return
 *  - DB_ERROR if database error occurs retrieving data.
 *  - CONNECTION_ERROR if database connection error occurs retrieving data.
 *  - INVALID_ID if the block name is too long.
 *  - NOT_FOUND if the block doesn't exist in the database.
 *  - OK if data was retrieved successful.
 */
STATUS getBlockUser(
        const std::string& id,              //!< [in] block ID
        std::string& user,                  //!< [out]
        int& qualifier                      //!< [out]
        );


/*!
 *  \brief Set block boot options.
 *
 *  \return
 *  - DB_ERROR if database error occurs retrieving data.
 *  - CONNECTION_ERROR if database connection error occurs retrieving data.
 *  - INVALID_ARG if options argument is too long.
 *  - NOT_FOUND if the block doesn't exist in the database.
 *  - OK if boot options were set successful.
 */
STATUS setBootOptions(
        const std::string& id,              //!< [in] block ID
        const std::string& options          //!< [in] boot options
        );

/*!
 *  \brief Add block boot option.
 *
 *  \return
 *  - DB_ERROR if database error occurs retrieving data or block not found.
 *  - CONNECTION_ERROR if database connection error occurs retrieving data.
 *  - OK if boot option was added successful.
 */
STATUS addBootOption(
        const std::string& id,              //!< [in] block ID
        const std::string& option           //!< [in] boot option
        );

/*!
 *  \brief Get block security key.
 *
 *  \return
 *  - DB_ERROR if database error occurs retrieving data.
 *  - CONNECTION_ERROR if database connection error occurs retrieving data.
 *  - INVALID_ID if the block name is too long.
 *  - NOT_FOUND if the block doesn't exist in the database.
 *  - OK if data was retrieved successful.
 */
STATUS getBlockSecurityKey(
        const std::string& id,              //!< [in] block ID
        unsigned char *key,                 //!< [out]
        size_t keyBufferSize                //!< [in]
        );

/*!
 *  \brief Set block security key.
 *
 *  \return
 *  - DB_ERROR if database error occurs retrieving data.
 *  - CONNECTION_ERROR if database connection error occurs retrieving data.
 *  - INVALID_ID if the block name is too long.
 *  - INVALID_ARG if security key is too long.
 *  - NOT_FOUND if the block doesn't exist in the database.
 *  - OK if block security key was set successful.
 */
STATUS setBlockSecurityKey(
        const std::string& id,              //!< [in] block ID
        unsigned char *key,                 //!< [in]
        size_t keyLen                       //!< [in]
        );

/*!
 *  \brief Get block error text.
 *
 *  \return
 *  - DB_ERROR if database error occurs retrieving data.
 *  - CONNECTION_ERROR if database connection error occurs retrieving data.
 *  - INVALID_ID if the block name is too long.
 *  - NOT_FOUND if the block doesn't exist in the database.
 *  - OK if data was retrieved successful.
 */
STATUS getBlockErrorText(
        const std::string& id,              //!< [in] block ID
        std::string& text                   //!< [out]
        );

/*!
 *  \brief Set block description.
 *
 *  \return
 *  - DB_ERROR if database error occurs retrieving data.
 *  - CONNECTION_ERROR if database connection error occurs retrieving data.
 *  - INVALID_ID if the block name is too long.
 *  - INVALID_ARG if block description text is too long.
 *  - NOT_FOUND if the block doesn't exist in the database.
 *  - OK if block description was set successful.
 */
STATUS setBlockDesc(
        const std::string& id,              //!< [in] block ID
        const std::string& description      //!< [in]
        );

/*!
 *  \brief Sets the block action.
 *
 *  \return
 *  - DB_ERROR if database error occurs retrieving data.
 *  - CONNECTION_ERROR if database connection error occurs retrieving data.
 *  - INVALID_ID if the block name is too long.
 *  - NOT_FOUND if the block doesn't exist in the database.
 *  - FAILED if not a valid action request or block is in unknown state.
 *  - DUPLICATE if block has pending action.
 *  - OK if setting the block action was successful.
 */
STATUS setBlockAction(
        const std::string& block,                                           //!< [in] block ID
        BGQDB::BLOCK_ACTION action,                                         //!< [in]
        const std::deque<std::string>& option = std::deque<std::string>()   //!< [in] owner=username when action == CONFIGURE_BLOCK
        );

/*!
 *  \brief Get the block action.
 *
 *  \return
 *  - DB_ERROR if database error occurs retrieving data or block not found.
 *  - CONNECTION_ERROR if database connection error occurs retrieving data.
 *  - OK if operation was successful.
 */
STATUS getBlockAction(
        std::string& id,                            //!< [out] block ID
        BGQDB::BLOCK_ACTION& action,                //!< [out]
        const std::string& exclude = std::string()  //!< [in]
        );


/*!
 *  \brief Kill jobs and free blocks impacted by a service action
 *
 *  \return
 *  - CONNECTION_ERROR if database connection error occurs retrieving data.
 *  - DB_ERROR if database error occurs retrieving data.
 *  - INVALID_ID if the location of hardware provided was invalid
 *  - FAILED if the blocks or jobs are in an unknown state.
 *  - OK if operation was successful.
 */
STATUS killMidplaneJobs(
        const std::string& location,                   //!< [in] location
        std::vector<job::Id> *jobs_list_out=NULL,      //!< [in] list of jobs
        bool listOnly = false                          //!< [in] dont kill or free anything
        );

/*!
 *  \brief Clear the block action.
 *
 *  \return
 *  - DB_ERROR if database error occurs retrieving data or block not found.
 *  - CONNECTION_ERROR if database connection error occurs retrieving data.
 *  - OK if operation was successful.
 */
STATUS clearBlockAction(
        const std::string& id                       //!< [in] block ID
        );

/*!
 *  \brief Check a list of IO nodes to see if an attached compute block is still booted
 *
 *  \return
 *  - DB_ERROR if database error occurs retrieving data or block not found.
 *  - CONNECTION_ERROR if database connection error occurs retrieving data.
 *  - OK if operation was successful.
 */
STATUS checkIONodeConnection(
        const std::vector<std::string>& locations,   //!< [in] IO nodes
        std::vector<std::string>& connected          //!< [out] connected CN blocks
        );

/*!
 *  \brief Check an IO block to see if an attached compute block is still booted
 *
 *  \return
 *  - DB_ERROR if database error occurs retrieving data or block not found.
 *  - CONNECTION_ERROR if database connection error occurs retrieving data.
 *  - OK if operation was successful.
 */
STATUS checkIOBlockConnection(
        const std::string& id,                   //!< [in] block ID
        std::vector<std::string>* connected    //!< [out]
        );

/*!
 *  \brief Check a compute block to see if all of its connected (and available) IO nodes are booted
 *
 *  \return
 *  - DB_ERROR if database error occurs retrieving data or block not found.
 *  - CONNECTION_ERROR if database connection error occurs retrieving data.
 *  - INVALID_ID if the block name is too long.
 *  - NOT_FOUND if the block name was not found in the database.
 *  - OK if operation was successful.
 */
STATUS checkBlockConnection(
        const std::string& id,                   //!< [in] block ID
        std::vector<std::string>* unconnected    //!< [out] Unconnected I/O node locations
        );

/*!
 *  \brief Check a compute block to see if enough I/O is booted to allow the compute block to boot.
 *  This method will verify I/O nodes are connected and that enough I/O links exists for every midplane.
 *
 *  \return
 *  - DB_ERROR if database error occurs retrieving data.
 *  - CONNECTION_ERROR if database connection error occurs retrieving data.
 *  - INVALID_ID if the block name is too long.
 *  - NOT_FOUND if the block name was not found in the database.
 *  - OK if operation was successful.
 */
STATUS checkBlockIO(
        const std::string& block,                               //!< [in] block ID
        std::vector<std::string>* unconnectedIONodes,           //!< [out] All unconnected I/O node locations
        std::vector<std::string>* midplanesFailingIORules,      //!< [out] Midplanes failing I/O requirements
        std::vector<std::string>* unconnectedAvailableIONodes   //!< [out] Unconnected but 'Available' I/O node locations
       );

/*!
 *  \brief Check if rack exists in db
 *
 *  \return
 *  - DB_ERROR if database error occurs retrieving data.
 *  - CONNECTION_ERROR if database connection error occurs retrieving data.
 *  - INVALID_ID if the rack name is too long.
 *  - NOT_FOUND if the rack doesn't exist in the database.
 *  - OK if rack was found
 */
STATUS checkRack(
        const std::vector<std::string>& locations,   //!< [in] hw list, each element can be comma-separated
        std::vector<std::string>& invalid            //!< [out]
        );

/*! \brief Given a block action returns the block status as a string. */
std::string blockActionToString( BLOCK_ACTION action );

/*! \brief Given a block status returns the block status as a string. */
std::string blockStatusToString( BLOCK_STATUS status );

/*! \brief Given a block code returns the block code as a string. */
std::string blockCodeToString( const char* code );

/*! \brief Given a block status returns the block code. */
const char* blockStatusToCode( BLOCK_STATUS state );

/*! \brief Given a block code returns the block status. */
BLOCK_STATUS blockCodeToStatus( const char* code );

/*! \brief Returns true if the value exists in the table. */
bool checkValueExists(
        const std::string& table_name,
        const std::string& column_name,
        const std::string& value,
        cxxdb::ConnectionPtr conn_ptr = cxxdb::ConnectionPtr() // may be null and will check out a new connection.
    );

/*! \brief Returns true if hardware at the location exists. */
bool checkLocationExists(
        const bgq::util::Location& location,
        cxxdb::ConnectionPtr conn_ptr = cxxdb::ConnectionPtr() // may be null and will check out a new connection.
    );


} // namespace BGQDB


namespace std {

std::ostream& operator<<( std::ostream& os, const BGQDB::MidplaneCoordinate& coord );

} // namespace std

#endif
