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
#ifndef BGQ_UTILITY_PERFORMANCE_H
#define BGQ_UTILITY_PERFORMANCE_H
/*!
 * \file utility/include/performance.h
 * \brief Initialization methods for bgq::utility::performance library.
 */

#include <utility/include/Properties.h>

namespace bgq {
namespace utility {

/*!
 *
\brief Classes providing basic functions to record performance statistics.

Initialize the library using the \link bgq::utility::performance::init init\endlink method in the
performance.h header.  For linking, you probably want to do something like:
  
\verbatim
utility_library_dir=$(BGQ_INSTALL_DIR)/utility/lib
\endverbatim

Then use these LDFLAGs:
\verbatim
LDFLAGS += -L$(utility_library_dir) -lbgutility -Wl,-rpath,$(utility_library_dir)
\endverbatim

Unit tests are located in the utility/test/performance directory.

\see\link bgq::utility::performance::StatisticSet StatisticSet\endlink
 *
 */
namespace performance {

/*!
 * \brief Initialize the API.
 */
void init(
        Properties::ConstPtr properties //!< [in]
        );

/*!
 * \brief Get properties file.
 *
 * \pre library has been initialized with a call to bgq::utility::performance::init
 */
Properties::ConstPtr getProperties();

} // performance
} // utility
} // bgq

#endif
