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
#ifndef RUNJOB_COMMON_PROPERTIES_H
#define RUNJOB_COMMON_PROPERTIES_H

#include <utility/include/Properties.h>

#include <string>

namespace runjob {

namespace server {

/*!
 * \brief runjob_server properties section
 */
extern const std::string PropertiesSection;

namespace commands {

/*!
 * \brief runjob_server commands properties section
 */
extern const std::string PropertiesSection;

} // commands
} // server

namespace mux {

/*!
 * \brief runjob_mux properties section
 */
extern const std::string PropertiesSection;

namespace commands {

/*!
 * \brief runjob_mux commands properties section
 */
extern const std::string PropertiesSection;

} // commands
} // mux
} // runjob

#endif
