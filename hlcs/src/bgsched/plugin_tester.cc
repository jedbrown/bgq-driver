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

#include "bgsched/allocator/PluginContainer.h"

#include <log4cxx/basicconfigurator.h>
#include <log4cxx/consoleappender.h>
#include <log4cxx/logger.h>
#include <log4cxx/patternlayout.h>

int
main()
{
    using namespace bgsched::allocator;
    using namespace log4cxx;
    LayoutPtr layout_ptr(new PatternLayout( "%d{MMM dd HH:mm:ss.SSS} (%-5p) [%t] %c: %m%n" ));
    AppenderPtr appender_ptr(new ConsoleAppender( layout_ptr ));
    BasicConfigurator::configure( appender_ptr );
    LoggerPtr logger = Logger::getLogger( "plugin_tester" );
    try {
        PluginContainer foo("samples/bgsched.config");
    } catch (PluginContainer::Exception& e) {
        LOG4CXX_FATAL(logger, "caught exception: " << e.what());;
    }
    LOG4CXX_INFO(logger, "plugin tester is done");
}
