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
/* (C) Copyright IBM Corp.  2007, 2011                              */
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
#ifndef RASEVENTHANDLERCHAIN_H_
#define RASEVENTHANDLERCHAIN_H_

#include <vector>
#include <string>
#include <pthread.h>
#include "RasEventHandler.h"

#ifdef __linux__
#include <utility/include/Properties.h>
#endif

/** 
 * \class RasEventHandlerChain
 *
 * RasEventHandlerChain is responsible for passing a RasEvent 
 * to a series of RasEventHandlers.
 */

class RasEventHandlerChain
{
public:
    /** \brief Pass the RasEvent event to a series of handlers.
     */
    static void handle(RasEvent& event);
    static void setEnvironment(std::string env);
    static std::string getEnvironment();
    static void setFilterFile(std::string file);
    static std::string getFilterFile();
    static void initChain();
#ifdef __linux__
    static void setProperties(const bgq::utility::Properties::ConstPtr properties);
    static bgq::utility::Properties::ConstPtr getProperties();
#endif
    static void reinitialized() { initialized = false; } 
    static void clear();
private:
    static pthread_mutex_t chainLock; // Sync access to chain initialization
    static bool initialized;
    static void readRasFilters();
    static std::vector<RasEventHandler*> handlers;
    static std::string env;
    static std::string filterFile;
#ifdef __linux__
    static bgq::utility::Properties::ConstPtr properties;
#endif
    static void readRasProperties();
};

#endif /*RASEVENTHANDLERCHAIN_H_*/
