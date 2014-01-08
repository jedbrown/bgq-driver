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
#ifndef SECURITY_RAS_H
#define SECURITY_RAS_H

#include <hlcs/include/security/Object.h>

#include <ras/include/RasEventImpl.h>

#include <utility/include/UserId.h>

#include <boost/lexical_cast.hpp>

#include <string>

#include <stdint.h>

namespace hlcs {
namespace security {

/*!
 * \brief RAII wrapper for inserting RAS events into the database.
 */
class Ras
{
public:
    /*!
     * \brief types of RAS events.
     *
     * Add new RAS events by appending them in this list, do not
     * sort or re-organize them.
     */
    enum Value {
        StartingAddress = 0x0062fff,
        ValidateFailure
    };

public:
    /*!
     * \brief factory.
     *
     * The compiler will elicit copy elision here so the destructor will only be triggered once.
     */
    static Ras create(
            Value type  //!< [in]
            );

    /*!
     * \brief dtor.
     */
    ~Ras();

    /*!
     * \brief set object.
     */
    Ras& object(
            const Object& object    //!< [in]
            );

    /*!
     * \brief set action.
     */
    Ras& action(
            Action::Type action    //!< [in]
            );

    /*!
     * \brief Set user.
     */
    Ras& user(
            const bgq::utility::UserId& user    //!< [in]
            );

private:
    __attribute__ ((visibility("hidden"))) Ras(
            Value type
       );

    void __attribute__ ((visibility("hidden"))) insert();

private:
    RasEventImpl _impl;
};

} // security
} // hlcs

#endif
