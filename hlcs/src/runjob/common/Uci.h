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
#ifndef RUNJOB_COMMON_UCI_H
#define RUNJOB_COMMON_UCI_H

#include <hwi/include/common/uci.h>

#include <boost/serialization/access.hpp>

#include <iosfwd>
#include <string>

namespace runjob {

/*!
 * \brief Wrapper around the BG_UniversalComponentIdentifier (uci) type defined in hwi/include/common/uci.h
 *
 * This class is suitable for storing as a key in an associative container.
 */
class Uci
{
public:
    /*!
     * \brief Ctor.
     *
     * \note implicit on purpose.
     */
    Uci(
            const std::string& location = std::string() //!< [in]
       );

    /*!
     * \brief Ctor.
     *
     * \note implicit on purpose.
     */
    Uci(
            BG_UniversalComponentIdentifier location    //!< [in]
       );

    /*!
     * \brief
     */
    BG_UniversalComponentIdentifier get() const { return _location; }

    /*!
     * \brief Equality operator
     */
    bool operator==( const Uci& other ) const { return other._location == _location; }

    /*!
     * \brief Inequality operator
     */
    bool operator!=( const Uci& other ) const { return other._location != _location; }

    /*!
     * \brief less-than operator
     */
    bool operator<( const Uci& other ) const { return _location < other._location; }

private:
    friend class boost::serialization::access;
    template<class Archive>
    void __attribute__ ((visibility("hidden"))) serialize(
            Archive &ar,                //!< [in] archive
            const unsigned int          //!< [in] version number
            )
    {
        ar & _location;
    }

private:
    BG_UniversalComponentIdentifier _location;
};

std::ostream&
operator<<(
        std::ostream& os,
        const Uci& uci
        );

} // runjob

#endif
