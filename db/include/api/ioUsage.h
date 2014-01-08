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
/* (C) Copyright IBM Corp.  2011, 2011                              */
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

#ifndef BGQ_DB_IO_USAGE_H_
#define BGQ_DB_IO_USAGE_H_


#include "Exception.h"

#include <control/include/bgqconfig/BGQMidplaneNodeConfig.h>


namespace BGQDB {
namespace ioUsage {


    /*! This must be called before calling update().
     *  Gets the I/O node usage limit (the Mundy constant) from the BG configuration file, in [cios].max_connected_nodes.
     *  If the property isn't present or is not valid a default is used.
     *
     *  \pre BGQDB::init() was successful.
     */
void init();


    /*! Get the usage limit calculated during init().
     *
     *  \pre init() was successful. */
unsigned getUsageLimit();


    /*! Updates the usage for the I/O nodes.
     *  If the usage for any of the I/O nodes is above the limit, fails with an Exception.
     *  Otherwise, the usage for the I/O nodes for the block is updated.
     *
     *  \throws UsageExceeded if the usage for any of the nodes is above the limit.
     *  \throws Exception Some DB error occurred.
     *
     *  \pre init() was successful. */
void update(
        const BGQMidplaneNodeConfig::ComputeCount& cn_counts,
        const std::string& block_id
    );


class UsageExceeded : public Exception
{
public:

    UsageExceeded(
            const std::string& block_id,
            const std::string& io_node,
            unsigned count
        );

    const std::string& getIoNode() const  { return _io_node; }
    unsigned getCount() const  { return _count; }

    virtual ~UsageExceeded() throw()  { /* Nothing to do */ }


private:
    std::string _io_node;
    unsigned _count;
};


} } // namespace BGQDB::ioUsage

#endif
