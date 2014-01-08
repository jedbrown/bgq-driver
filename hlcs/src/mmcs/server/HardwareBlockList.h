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

#ifndef MMCS_SERVER_HARDWARE_BLOCK_LIST_H_
#define MMCS_SERVER_HARDWARE_BLOCK_LIST_H_

#include <bgq_util/include/pthreadmutex.h>

#include <boost/utility.hpp>

#include <string>
#include <vector>

namespace mmcs {
namespace server {

class HardwareBlockList : boost::noncopyable
{
    //! Static list of racks not allowed to boot until the controlling subnet_mc returns
    static PthreadMutex _lock;
    //! brief One comma separated list of racks per subnet
    static std::vector<std::string> _hw;
public:
    static void add_to_list(std::string subnet_string);
    static void remove_from_list(std::string subnet_string);
    //! brief Find out if any rack is in the list
    //! \param rack_string rack to search
    //! \returns true if the rack is in the list
    static bool find_in_list(std::string rack_string);
    static int list_size() { return _hw.size(); }
};

} } // namespace mmcs::server

#endif
