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


#ifndef BGWS_TEAL_TEAL_HPP_
#define BGWS_TEAL_TEAL_HPP_


#include "fwd.hpp"

#include "../utility/ChildProcesses.hpp"

#include <boost/asio.hpp>
#include <boost/filesystem.hpp>
#include <boost/utility.hpp>


namespace bgws {
namespace teal {


class Teal : public boost::noncopyable
{
public:

    // The size of the x_tealalertlog."event_loc" column.
    static const unsigned LOCATION_DB_COLUMN_SIZE;


    Teal(
            boost::asio::io_service& io_service,
            utility::ChildProcesses& child_processes,
            const boost::filesystem::path& tlchalert_executable,
            const boost::filesystem::path& tlrmalert_executable
        );


    void setConfiguration(
            const boost::filesystem::path& tlchalert_executable,
            const boost::filesystem::path& tlrmalert_executable
        );


    void closeAlert(
            Id id,
            CloseRemoveCallbackFn cb_fn
        );

    void removeAlert(
            Id id,
            CloseRemoveCallbackFn cb_fn
        );


private:

    boost::asio::io_service &_io_service;
    utility::ChildProcesses &_child_processes;

    boost::asio::strand _strand;

    boost::filesystem::path _tlchalert_executable;
    boost::filesystem::path _tlrmalert_executable;


    void _setConfigurationImpl(
            const boost::filesystem::path& tlchalert_executable,
            const boost::filesystem::path& tlrmalert_executable
        );

    void _closeAlertImpl(
            Id id,
            CloseRemoveCallbackFn cb_fn
        );

    void _removeAlertImpl(
            Id id,
            CloseRemoveCallbackFn cb_fn
        );
};


} } // namespace bgws::teal


#endif
