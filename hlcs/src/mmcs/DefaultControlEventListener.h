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

#ifndef DEFAULT_CONTROL_EVENT_LISTENER_H
#define DEFAULT_CONTROL_EVENT_LISTENER_H

// Handles RAS events from mcServer that when there is no specific
// ControlEventListener

#include "DBBlockController.h"

#include <boost/utility.hpp>

class DefaultControlEventListener: public DBBlockController, boost::noncopyable
{
public:
    static  DefaultControlEventListener* getDefaultControlEventListener();
    ~DefaultControlEventListener();
    void    disconnect();
    int processRASMessage(RasEvent& rasEvent);
protected:
    DefaultControlEventListener();
private:
    void prepareQuery();
private:
    // singleton instance
    static DefaultControlEventListener* _defaultControlEventListener;
    cxxdb::ConnectionPtr _connection;
    cxxdb::QueryStatementPtr _query;
};

#endif
