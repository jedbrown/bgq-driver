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
#ifndef RUNJOB_MUX_CLIENT_ID_H_
#define RUNJOB_MUX_CLIENT_ID_H_

#include <stdint.h>

namespace runjob {
namespace mux {
namespace client {

/*!
 * \brief Unique identifier for a runjob client.
 */
class Id
{
public:
    /*!
     * \brief Value type.
     */
    typedef uint64_t value_type;

public:
    /*!
     * \brief ctor
     */
    explicit Id(
            value_type value  //!< [in]
            ) :
        _value( value )
    {

    }

    /*!
     * \brief conversion to integer
     */
    operator value_type() const { return _value; }

private:
    const value_type _value;
};

} // client
} // mux
} // runjob

#endif
