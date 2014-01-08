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

/*!
 * \file api/filtering/BlockFilter.h
 * \brief BlockFilter class definition.
 */

#ifndef BGQDB_FILTERING_BLOCKFILTER_H_
#define BGQDB_FILTERING_BLOCKFILTER_H_

#include <set>
#include <string>

namespace BGQDB {
namespace filtering {

/*!
 * \brief
 */
class BlockFilter
{
public:

    /*!
     * \brief Block status.
     */
    enum Status {
        Allocated = 0,  //!< Block is allocated
        Booting,        //!< Block is booting
        Free,           //!< Block is free
        Initialized,    //!< Block is initialized
        Terminating     //!< Block is terminating
    };

    /*!
     * \brief Container of BlockFilter::Status values.
     */
    typedef std::set<BGQDB::filtering::BlockFilter::Status> Statuses;

    /*!
     * \brief Compute block size.
     */
    struct BlockSize {
        enum Value {
            Large = 0, //!< Compute blocks with size 512 compute nodes or larger
            Small,     //!< Compute blocks with size 32, 64, 128 or 256 compute nodes
            All        //!< All compute block sizes
        };
    };

    /*!
     * \brief Block types.
     */
    struct BlockType {
        enum Value {
            Any, //!< Either compute or I/O blocks.
            Compute, //!< Compute blocks only.
            Io //!< I/O blocks only.
        };
    };


    /*!
     * \brief ctor
     */
    BlockFilter();

    /*!
     * \brief Set compute block owner filter.
     */
    void setOwner(
            const std::string& owner          //!< [in] Compute block owner to filter on
    );

    /*!
     * \brief Set compute block user filter. This is the user that booted the block and only applies
     * to booted blocks.
     */
    void setUser(
            const std::string& user           //!< [in] Compute block user to filter on
    );

    /*!
     * \brief Set compute block status filter.
     */
    void setStatuses(
            const BGQDB::filtering::BlockFilter::Statuses* statusesPtr //!< [in] Pointer to Statuses set to filter on
    );

    /*!
     * \brief Set compute block name filter.
     */
    void setName(
            const std::string& name           //!< [in] Compute block name to filter on
    );

    /*!
     * \brief Set the level of compute block details option.
     *
     * If this filter option is true, then information about midplanes, switches and node boards will be included
     * for compute block objects.
     *
     * If this is filter option is false, then only basic block information will be set for compute block objects.
     */
    void setExtendedInfo(
            bool extendedInfo  //!< [in] true=provide extended block details, false=provide basic block details
    );

    /*!
     * \brief Set the include jobs filter option.
     *
     * If this is filter option is true, then job information will be included for compute block objects.
     *
     * If this is filter option is false, job information will not be set for compute block objects.
     */
    void setIncludeJobs(
            bool includeJobs  //!< [in] true=include job info, false=exclude job info
    );

    /*!
     * \brief Set compute block size.
     */
    void setSize(
            BlockSize::Value blockSize  //!< [in] Compute block size to filter on
    );

    void setBlockType( BlockType::Value blockType )  { _blockType = blockType; }

    /*!
     * \brief Get compute block owner filter.
     *
     * \return Compute block owner filter.
     */
    const std::string& getOwner() const;

    /*!
     * \brief Get compute block user filter. This is the user that booted the block and only applies
     * to booted blocks.
     *
     * \return Compute block user filter.
     */
    const std::string& getUser() const;

    /*!
     * \brief Get compute block status filter.
     *
     * \return Compute block status filter.
     */
    const BGQDB::filtering::BlockFilter::Statuses getStatuses() const;

    /*!
     * \brief Get compute block name filter.
     *
     * \return Compute block name filter.
     */
    const std::string& getName() const;

    /*!
     * \brief Get the level of compute block details option.
     *
     * If this filter option is true, then information about midplanes, switches and node boards will be included
     * for compute block objects.
     *
     * If this is filter option is false, then only basic block information will be set for block objects.
     *
     * \return Level of compute block details option.
     */
    bool getExtendedInfo() const;

    /*!
     * \brief Get the include jobs filter option.
     *
     * If this filter option is true, then job information will be included for compute block objects.
     *
     * If this filter option is false, job information will not be set for compute block objects.
     *
     * \return Include jobs filter option.
     */
    bool getIncludeJobs() const;

    /*!
     * \brief Get the compute block size filter option.
     *
     * \return Compute block size filter option.
     */
    BlockSize::Value getSize() const;

    BlockType::Value getBlockType() const  { return _blockType; }

protected:
    bool _extendedInfo;
    bool _includeJobs;
    std::string _name;
    std::string _owner;
    std::string _user;
    BlockSize::Value _size;
    BGQDB::filtering::BlockFilter::Statuses _status;
    BlockType::Value _blockType;
};

} // filtering
} // BGQDB

#endif
