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
 * \file bgsched/runjob/Environment.h
 * \brief definition and implementation of bgsched::runjob::Environment class.
 */

#ifndef BGSCHED_RUNJOB_ENVIRONMENT_H_
#define BGSCHED_RUNJOB_ENVIRONMENT_H_

#include <iostream>
#include <string>

namespace bgsched {
namespace runjob {

/*!
 * \brief An environment variable in key=value form.
 */
class Environment
{
public:
    /*!
     * \brief ctor.
     */
    Environment(
            const std::string& key,     //!< [in]
            const std::string& value    //!< [in]
            ) :
        _key( key ),
        _value( value )
    {

    }

    /*!
     * \brief Get key.
     */
    const std::string& getKey() const { return _key; }

    /*!
     * \brief Get value.
     */
    const std::string& getValue() const { return _value; }

    /*!
     * \brief conversion to std::string.
     */
    operator std::string() const
    {
        return _key + "=" + _value;
    }

private:
    std::string _key;
    std::string _value;
};

} // runjob
} // bgsched

#endif
