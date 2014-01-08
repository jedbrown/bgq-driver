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

#ifndef MMCS_COMMON_SUBNET_H_
#define MMCS_COMMON_SUBNET_H_

#include <utility/include/Properties.h>

#include <string>

namespace mmcs {
namespace common {

class Subnet
{
public:
    std::string _name;
    std::string _hw_to_manage;

    std::string _primary_ip;
    std::string _primary_eth;
    std::string _primary_port;
    std::string _primary_bgeth;

    std::string _backup_ip;
    std::string _backup_eth;
    std::string _backup_port;
    std::string _backup_bgeth;
    bool _home;

    Subnet(
            const bgq::utility::Properties::Ptr& _properties,
            const std::string& subnet
            );

    const std::string& get_name() const { return _name; }

    //! \brief Sets the "home" flag to indicate whether the subnet is running on its primary IP.
    //! \param ip An IP address as a string to compare with the primary IP.
    //! \returns True if at home.
    bool set_home(const std::string& ip);

    const std::string& getBGEthForIP(const std::string& ip) const;

    void output() const;
};

} } // namespace mmcs::common

#endif
