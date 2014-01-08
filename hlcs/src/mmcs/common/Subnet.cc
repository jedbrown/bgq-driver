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

#include "Subnet.h"

#include <utility/include/Log.h>

#include <utility/include/cxxsockets/Host.h>

LOG_DECLARE_FILE( "mmcs.common" );

namespace mmcs {
namespace common {

Subnet::Subnet(
        const bgq::utility::Properties::Ptr& _properties,
        const std::string& subnet
        ) :
    _name( _properties->getValue(subnet, "Name") ),
    _hw_to_manage( _properties->getValue(subnet, "HardwareToManage") ),
    _primary_ip( _properties->getValue(subnet, "PrimaryServer") ),
    _primary_eth( _properties->getValue(subnet, "PrimaryServerInterface") ),
    _primary_port( _properties->getValue(subnet, "PrimaryServerPort") ),
    _primary_bgeth( _properties->getValue(subnet, "PrimaryBGInterface") ),
    _backup_ip( _properties->getValue(subnet, "BackupServer") ),
    _backup_eth( _properties->getValue(subnet, "BackupServerInterface") ),
    _backup_port( _properties->getValue(subnet, "BackupServerPort") ),
    _backup_bgeth( _properties->getValue(subnet, "BackupBGInterface") ),
    _home( true )
{

}

bool
Subnet::set_home(
        const std::string& ip
        )
{
    CxxSockets::Host prim(_primary_ip);
    if (ip == prim.ip())
        _home = true;
    else
        _home = false;
    return _home;
}

const std::string&
Subnet::getBGEthForIP(
        const std::string& ip
        ) const
{
    CxxSockets::Host prim(_primary_ip);
    CxxSockets::Host bup(_backup_ip);
    if (ip == prim.ip())
        return _primary_bgeth;
    else if (ip == bup.ip())
            return _backup_bgeth;
    else {
        std::string ex = "Invalid IP " + ip;
        throw std::runtime_error(ex.c_str());
    }
}

void
Subnet::output() const
{
    LOG_DEBUG_MSG( "Subnet " << _name );
    LOG_DEBUG_MSG( "HardwareToManage: " << _hw_to_manage );
    LOG_DEBUG_MSG( "PrimaryServer: " << _primary_ip );
    LOG_DEBUG_MSG( "PrimaryServerInterface: " << _primary_eth );
    LOG_DEBUG_MSG( "PrimaryServerPort: " << _primary_port );
    LOG_DEBUG_MSG( "PrimaryBGInterface: " << _primary_bgeth );
    LOG_DEBUG_MSG( "BackupServer: " << _backup_ip );
    LOG_DEBUG_MSG( "BackupServerInterface: " << _backup_eth );
}

} } // namespace mmcs::common
