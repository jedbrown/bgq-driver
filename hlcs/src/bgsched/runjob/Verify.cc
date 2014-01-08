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
#include <bgsched/runjob/Verify.h>

#include "VerifyImpl.h"

namespace bgsched {
namespace runjob {

Verify::Verify(
        const Pimpl& impl
        ) :
    _impl( impl )
{

}

Verify&
Verify::deny_job(
        DenyJob::Value d
        )
{
    _impl->_denyJob = d;
    _impl->_message.clear();
    return *this;
}

Verify&
Verify::deny_job(
        const std::string& message
        )
{
    _impl->_denyJob = DenyJob::Yes;
    _impl->_message = message;
    return *this;
}

Verify&
Verify::exe(
        const std::string& exe
        )
{
    _impl->_exe = exe;
    return *this;
}

Verify&
Verify::args(
        const Arguments& args
        )
{
    _impl->_args = args;
    return *this;
}

Verify&
Verify::envs(
        const Environments& envs
        )
{
    _impl->_envs = envs;
    return *this;
}

Verify&
Verify::block(
        const std::string& block
        )
{
    _impl->_block = block;
    return *this;
}

Verify&
Verify::corner(
        const Corner& corner
        )
{
    _impl->_corner = corner;
    return *this;
}

Verify&
Verify::shape(
        const Shape& shape
        )
{
    _impl->_shape = shape;
    return *this;
}

Verify&
Verify::scheduler_data(
        const std::string& data
        )
{
    _impl->_schedulerData = data;
    return *this;
}

Verify::DenyJob::Value
Verify::deny_job() const
{
    return _impl->_denyJob;
}

pid_t
Verify::pid() const
{
    return _impl->_pid;
}

const std::string&
Verify::exe() const
{
    return _impl->_exe;
}

const Verify::Arguments&
Verify::args() const
{
    return _impl->_args;
}

const Verify::Environments&
Verify::envs() const
{
    return _impl->_envs;
}

const std::string&
Verify::block() const
{
    return _impl->_block;
}

const Corner&
Verify::corner() const
{
    return _impl->_corner;
}

const Shape&
Verify::shape() const
{
    return _impl->_shape;
}

const std::string&
Verify::scheduler_data() const
{
    return _impl->_schedulerData;
}

const std::string&
Verify::message() const
{
    return _impl->_message;
}

const UserId&
Verify::user() const
{
    return _impl->_user;
}

} // runjob
} // bgsched

