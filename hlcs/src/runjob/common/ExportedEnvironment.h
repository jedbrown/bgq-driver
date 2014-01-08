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
#ifndef RUNJOB_COMMON_EXPORTED_ENVIRONMENT_H
#define RUNJOB_COMMON_EXPORTED_ENVIRONMENT_H

#include "common/Environment.h"

#include <iostream>
#include <string>

namespace runjob {

/*!
 * \brief Export an environment variable from the current environment.
 * \ingroup argument_parsing
 */
class ExportedEnvironment : public Environment
{
public:
    /*!
     * \brief ctor.
     */
    ExportedEnvironment(
            const std::string& key = std::string(),
            const std::string& value = std::string()
            );
};

/*!
 * \brief extraction operator.
 * \ingroup argument_parsing
 */
std::ostream&
operator<<(
        std::ostream& stream,           //!< [in]
        const ExportedEnvironment& env  //!< [in]
        );

/*!
 * \brief insertion operator.
 * \ingroup argument_parsing
 */
std::istream&
operator>>(
        std::istream& stream,       //!< [in]
        ExportedEnvironment& env    //!< [in]
        );

} // runjob

#endif
