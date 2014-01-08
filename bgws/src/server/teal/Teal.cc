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


#include "Teal.hpp"

#include "TlchalertExecution.hpp"
#include "TlrmalertExecution.hpp"

#include <utility/include/Log.h>

#include <boost/bind.hpp>
#include <boost/lexical_cast.hpp>

#include <string>


using boost::lexical_cast;

using std::string;


LOG_DECLARE_FILE( "bgws" );


namespace bgws {
namespace teal {


const unsigned Teal::LOCATION_DB_COLUMN_SIZE(255);


Teal::Teal(
        boost::asio::io_service& io_service,
        utility::ChildProcesses& child_processes,
        const boost::filesystem::path& tlchalert_executable,
        const boost::filesystem::path& tlrmalert_executable
    ) :
        _io_service(io_service),
        _child_processes(child_processes),
        _strand( io_service ),
        _tlchalert_executable(tlchalert_executable),
        _tlrmalert_executable(tlrmalert_executable)
{
    // Nothing to do.
}


void Teal::setConfiguration(
        const boost::filesystem::path& tlchalert_executable,
        const boost::filesystem::path& tlrmalert_executable
    )
{
    _strand.post( boost::bind( &Teal::_setConfigurationImpl, this,
            tlchalert_executable, // Not boost::ref here so makes a copy.
            tlrmalert_executable  // Not boost::ref here so makes a copy.
        ) );
}


void Teal::closeAlert(
        Id id,
        CloseRemoveCallbackFn cb_fn
    )
{
    _strand.post( boost::bind( &Teal::_closeAlertImpl, this,
            id,
            cb_fn
        ) );
}


void Teal::removeAlert(
        Id id,
        CloseRemoveCallbackFn cb_fn
    )
{
    _strand.post( boost::bind( &Teal::_removeAlertImpl, this,
            id,
            cb_fn
        ) );
}


void Teal::_setConfigurationImpl(
        const boost::filesystem::path& tlchalert_executable,
        const boost::filesystem::path& tlrmalert_executable
    )
{
    LOG_DEBUG_MSG( "Reconfiguring Teal, tlchalert_executable=" << _tlchalert_executable << " -> " << tlchalert_executable << ","
                    " tlrmalert_executable=" << _tlrmalert_executable << " -> " << tlrmalert_executable );

    _tlchalert_executable = tlchalert_executable;
    _tlrmalert_executable = tlrmalert_executable;
}


void Teal::_closeAlertImpl(
        Id id,
        CloseRemoveCallbackFn cb_fn
    )
{
    TlchalertExecution::Ptr ptr(TlchalertExecution::create(
            id,
            cb_fn,
            _tlchalert_executable,
            _child_processes,
            _io_service
        ));

    ptr->start();
}


void Teal::_removeAlertImpl(
        Id id,
        CloseRemoveCallbackFn cb_fn
    )
{
    TlrmalertExecution::Ptr ptr(TlrmalertExecution::create(
            id,
            cb_fn,
            _tlrmalert_executable,
            _child_processes,
            _io_service
        ));

    ptr->start();
}


} } // namespace bgws::teal
