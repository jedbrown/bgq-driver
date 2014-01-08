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
#ifndef RUNJOB_MUX_CLIENT_STATUS_H_
#define RUNJOB_MUX_CLIENT_STATUS_H_

#include <string>

#include <iosfwd>

namespace runjob {
namespace mux {
namespace client {

/*!
 * \brief runjob client status values.
 */
class Status
{
public:
    /*!
     * \brief All Status values.
     */
    enum Value {
        Debug,
        Error,
        Initializing,
        Inserting,
        Running,
        Starting,
        Terminated,
        Invalid
    };

    /*!
     * \brief Convert a Status::Value to a string.
     */
    static const char* toString(
            Value s //!< [in]
            )
    {
        static const char* strings[] = {
            "Debug",
            "Error",
            "Initializing",
            "Inserting",
            "Running",
            "Starting",
            "Terminated"
        };

        if ( s >= Debug && s < Invalid ) {
            return strings[s];
        } else {
            return "Invalid";
        }
    }

public:
    /*!
     * \brief ctor.
     */
    Status();

    /*!
     * \brief Set the Status.
     */
    void set(
            Status::Value status    //!< [in]
            )
    {
        _value = status;
    }

    /*!
     * \brief Get status value.
     */
    Value get() const { return _value; }

    /*!
     * \brief equality operator.
     */
    bool operator==(
            Status::Value other //!< [in]
            ) const
    {
        return _value == other;
    }

    /*!
     * \brief Conversion to string operator.
     */
    operator std::string() const {
        return this->toString(_value);
    }

private:
    Value _value;
};

std::ostream&
operator<<(
        std::ostream&,
        const Status& status
        );

} // client
} // mux
} // runjob

#endif
