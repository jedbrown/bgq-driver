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

#include "HardwareBlockList.h"

#include <algorithm>

namespace mmcs {
namespace server {

PthreadMutex HardwareBlockList::_lock;
std::vector<std::string> HardwareBlockList::_hw;

void HardwareBlockList::add_to_list(std::string subnet_string) {
    PthreadMutexHolder holder(true /*assert*/);
    holder.Lock(&_lock);
    // Don't add it if it is already there.
    if (std::find(_hw.begin(), _hw.end(), subnet_string) == _hw.end())
        _hw.push_back(subnet_string);
}

void HardwareBlockList::remove_from_list(std::string subnet_string) {
    PthreadMutexHolder holder(true /*assert*/);
    holder.Lock(&_lock);
    _hw.erase(std::remove(_hw.begin(), _hw.end(), subnet_string), _hw.end());
}

bool HardwareBlockList::find_in_list(std::string rack_string) {
    for (std::vector<std::string>::iterator it = _hw.begin(); it != _hw.end(); ++it) {
        if (it->find(rack_string) != std::string::npos) {
            return true;
        }
    }
    return false;
}

} } // namespace mmcs::server
