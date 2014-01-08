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
#ifndef RUNJOB_SAMPLE_PLUGIN_PLUGIN_H_
#define RUNJOB_SAMPLE_PLUGIN_PLUGIN_H_

#include <hlcs/include/bgsched/runjob/Plugin.h>

#include <boost/thread/mutex.hpp>

#include <iosfwd>

/*!
 * \brief Sample runjob plugin.
 */
class Plugin : public bgsched::runjob::Plugin
{
public:
    /*!
     * \brief ctor.
     */
    Plugin();

    /*!
     * \brief dtor.
     */
    ~Plugin();

    /*!
     * \brief Verify we started the job.
     */
    void execute(
            bgsched::runjob::Verify& data    //!< [in]
            );

    /*!
     * \brief Notification when the job starts.
     */
    void execute(
            const bgsched::runjob::Started& data  //!< [in]
            );
    
    /*!
     * \brief Notification when the job terminates.
     */
    void execute(
            const bgsched::runjob::Terminated& data //!< [in]
            );

private:
    boost::mutex _mutex;
};

#endif
