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
/* (C) Copyright IBM Corp.  2004, 2011                              */
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

#ifndef MMCS_SERVER_DEFAULT_LISTENER_H_
#define MMCS_SERVER_DEFAULT_LISTENER_H_

// Handles RAS events from mcServer that when there is no specific ControlEventListener

#include "DBBlockController.h"

#include <boost/thread.hpp>
#include <boost/utility.hpp>

#include <map>

class BGQBlockNodeConfig;

namespace mmcs {
namespace server {

class DefaultListener : public DBBlockController, boost::noncopyable
{
public:
    /*!
     * \brief Mapping of I/O node location to compute block ID
     */
    typedef std::multimap<std::string, std::string> IoMap;

public:
    static DefaultListener* get();
    ~DefaultListener();
    void disconnect();
    int processRASMessage(RasEvent& rasEvent);
    void processConsoleMessage(MCServerMessageSpec::ConsoleMessage& consoleMessage);
    void add( const BGQBlockNodeConfig* block );
    void remove( const std::string& block );

protected:
    DefaultListener();
private:
    void prepareQuery();
private:
    static DefaultListener*  _instance;
    cxxdb::ConnectionPtr     _connection;
    cxxdb::QueryStatementPtr _computeQuery;
    boost::mutex             _mutex;
    IoMap                    _io;
};

} } // namespace mmcs::server

#endif
