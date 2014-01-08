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
#ifndef SAMPLE_PLUGIN_H_
#define SAMPLE_PLUGIN_H_

#include "Plugin.h"

class SamplePlugin : public Plugin
{
public:
    /*!
     * \brief ctor.
     */
    SamplePlugin();

    /*!
     * \brief dtor.
     */
    ~SamplePlugin();

    void doSomething(
            std::string& something
            );
};

extern "C" {

Plugin* create();

void destroy(Plugin* plugin);

}

#endif
