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

#include <hlcs/include/master/BGMasterClientApi.h>

#ifndef MMCS_SUBNET_H
#define MMCS_SUBNET_H

class MMCSSubnet {
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

    MMCSSubnet(bgq::utility::Properties::Ptr _properties, std::string subnet) {
        _name = _properties->getValue(subnet, "Name");
        _hw_to_manage = _properties->getValue(subnet, "HardwareToManage");
        _primary_ip = _properties->getValue(subnet, "PrimaryServer");
        _primary_eth = _properties->getValue(subnet, "PrimaryServerInterface");
        _primary_port = _properties->getValue(subnet, "PrimaryServerPort");
        _primary_bgeth = _properties->getValue(subnet, "PrimaryBGInterface");

        _backup_ip = _properties->getValue(subnet, "BackupServer");
        _backup_eth = _properties->getValue(subnet, "BackupServerInterface");
        _backup_port = _properties->getValue(subnet, "BackupServerPort");
        _backup_bgeth = _properties->getValue(subnet, "BackupBGInterface");
        _home = true;
    }
    
    std::string get_name() { return _name; }

    //! \brief Sets the "home" flag to indicate whether the
    //!  subnet is running on its primary IP.
    //! \param ip An ip address as a string to compare with the primary IP.
    //! \returns True if at home.
    bool set_home(std::string ip) {
        Host prim(_primary_ip);
        if(ip == prim.ip()) _home = true;
        else _home = false;
        return _home;
    }
    std::string getBGEthForIP(std::string ip) {
        Host prim(_primary_ip);
        Host bup(_backup_ip);
        if(ip == prim.ip()) return _primary_bgeth;
        else if(ip == bup.ip()) return _backup_bgeth;
        else {
            std::string ex = "Invalid IP " + ip;
            throw std::runtime_error(ex.c_str());
            return "";
        }
    }
};

inline std::ostream& operator <<(std::ostream& os, const MMCSSubnet& sn) {
    os << "Subnet " << sn._name << std::endl;
    os << "HardwareToManage: " << sn._hw_to_manage << std::endl;
    os << "PrimaryServer: " << sn._primary_ip << std::endl;
    os << "PrimaryServerInterface: " << sn._primary_eth << std::endl;
    os << "PrimaryServerPort: " << sn._primary_port << std::endl;
    os << "PrimaryBGInterface: " << sn._primary_bgeth << std::endl << std::endl;
    os << "BackupServer: " << sn._backup_ip << std::endl;
    os << "BackupServerInterface: " << sn._backup_eth << std::endl;
    os << "BackupServerPort: " << sn._backup_port << std::endl;
    os << "BackupBGInterface: " << sn._backup_bgeth << std::endl;
    return os;
}

#endif
