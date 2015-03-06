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

/*!
 * \file bgsched/runjob/Plugin.h
 * \brief definition and implementation of bgsched::runjob::Plugin class.
 */

#ifndef BGSCHED_RUNJOB_PLUGIN_H_
#define BGSCHED_RUNJOB_PLUGIN_H_

#include <bgsched/runjob/Started.h>
#include <bgsched/runjob/Terminated.h>
#include <bgsched/runjob/Verify.h>

namespace bgsched {
namespace runjob {

/*!
 * \brief The runjob plug-in interface for job schedulers.
 *
 * This documentation was generated for driver DRIVER_NAME with revision VERSION_NUMBER on DOCUMENTATION_DATE.
 *
 * \section Overview
 *
 * This abstract class is used to define in interface between the runjob_mux and
 * a shared library created by a scheduler. At various points during the lifetime
 * of a job, runjob_mux will invoke methods described in this interface to provide
 * information to the scheduler about the job. The image below shows how the plug-in
 * interacts with other components.
 *
 * \image html plugin.jpg
 *
 * Two symbols are <b>required</b> to be present in the shared library for
 * runjob_mux to successfully load it. They are
\verbatim
bgsched::runjob::Plugin* create()
\endverbatim
* and
\verbatim
void destroy(bgsched::runjob::Plugin*)
\endverbatim
 *
 * They must be declared as extern "C" to prevent mangling of symbol names. The path to
 * the plug-in, and any flags required for dlopen  can be specified in the bg.properties file
 * in the following section.
\verbatim
[runjob.mux]
plugin          = /full/path/to/plugin.so
plugin_flags    = 0x0101 # RTLD_LAZY | RTLD_GLOBAL
\endverbatim
 *
 * After a successful load, you should see the following output from runjob_mux
\verbatim
2011-05-31 09:54:34.446 (DEBUG) [0xfff62bff0d0] ibm.runjob.mux.Plugin: using plugin path /full/path/to/plugin.so with dlopen flags 0x0001
\endverbatim
 *
 * \section Example
 *
 * The example below shows a sample implementation of the Plugin interface.
 *
 * Plugin.h
 * \include src/runjob/samples/plugin/Plugin.h
 *
 * Plugin.cc
 * \include src/runjob/samples/plugin/Plugin.cc
 *
 * ProcessTree.h
 * \include src/runjob/samples/plugin/ProcessTree.h
 *
 * ProcessTree.cc
 * \include src/runjob/samples/plugin/ProcessTree.cc
 *
 * \note the previous example uses the auto C++0x keyword, you'll need to build using the -std=c++0x flag to
 * gcc.
 *
 * \section linking Linking
 *
 * When linking your plugin, you will need to link to bgsched like so:
 * <pre>
 * hlcs_library_dir=\$(BGQ_INSTALL_DIR)/hlcs/lib
 * </pre>
 *
 * Then use these LDFLAGs:
 * <pre>
 * LDFLAGS += -L\$(hlcs_library_dir) -lbgsched -Wl,-rpath,\$(hlcs_library_dir)
 * </pre>
 *
 * \section thread_safety Thread Safety
 *
 * The plug-in can be invoked from multiple threads. If your plug-in makes use of static
 * data or shared resources, you should take explicit precautions to guard against this
 * by using mutexes.
 *
 * \section Warning
 *
 * The plug-in must be compiled with the exact same version of gcc as the runjob_mux:
 * GCC_VERSION.
 */
class Plugin
{
public:
    /*!
     * \brief ctor.
     */
    Plugin() { }

    /*!
     * \brief dtor.
     */
    virtual ~Plugin() { }

    /*!
     * \brief Verify this job was started from the scheduler.
     *
     * \note the job will not start until the plugin returns from this method
     */
    virtual void execute(
            Verify& data   //!< [in,out]
            ) = 0;

    /*!
     * \brief Invoked when the job starts.
     */
    virtual void execute(
            const Started& data //!< [in]
            ) = 0;

    /*!
     * \brief Invoked when the job terminates.
     *
     * \note the runjob shadow process will not terminate until the plugin returns from this method
     */
    virtual void execute(
            const Terminated& data  //!< [in]
            ) = 0;
};

} // runjob
} // bgsched

extern "C" {

/*!
 * \brief declaration of library method to create a Plugin.
 * \returns pointer to a Plugin object.
 */
bgsched::runjob::Plugin* create();

/*!
 * \brief declaration of a library method to destroy a Plugin.
 * \param[in] plugin pointer to the Plugin to destroy.
 */
void destroy(bgsched::runjob::Plugin* plugin);

}

#endif
