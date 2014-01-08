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
 * \file security/Action.h
 */

#ifndef BGQ_HLCS_SECURITY_ACTION_H_
#define BGQ_HLCS_SECURITY_ACTION_H_

#include <ostream>

namespace hlcs {
namespace security {

/*!
 * \brief an action on an object.
 */
struct Action
{
    /*!
     * \brief types of actions.
     */
    enum Type {
        Create,      //!< create an object
        Delete,      //!< delete an object
        Execute,     //!< execute an object
        Read,        //!< read an object
        Update,      //!< update an object
        Invalid      //!< invalid action
    };
};


/*!
 * \brief Action insertion operator
 */
std::ostream&
operator<<(std::ostream&, Action::Type);

/*!
 * \brief Action extraaction operator.
 */
std::istream&
operator>>(std::istream&, Action::Type&);

} // security
} // hlcs

#endif
