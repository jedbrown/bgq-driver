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
#ifndef RUNJOB_COORDINATES_H
#define RUNJOB_COORDINATES_H

#include <boost/serialization/access.hpp>

#include <boost/lexical_cast.hpp>

#include <iosfwd>
#include <limits>

#include <stdint.h>

namespace runjob {

/*!
 * \brief 5 dimensional coordinates.
 */
class Coordinates
{
public:
    /*!
     * \brief ctor.
     */
    Coordinates(
            unsigned a = std::numeric_limits<unsigned>::max(),  //!< [in]
            unsigned b = std::numeric_limits<unsigned>::max(),  //!< [in]
            unsigned c = std::numeric_limits<unsigned>::max(),  //!< [in]
            unsigned d = std::numeric_limits<unsigned>::max(),  //!< [in]
            unsigned e = std::numeric_limits<unsigned>::max()   //!< [in]
            ) :
        _a( a ),
        _b( b ),
        _c( c ),
        _d( d ),
        _e( e )
    {
        
    }

    unsigned a() const { return _a; }    //!< Get a dimension.
    unsigned b() const { return _b; }    //!< Get b dimension.
    unsigned c() const { return _c; }    //!< Get c dimension.
    unsigned d() const { return _d; }    //!< Get d dimension.
    unsigned e() const { return _e; }    //!< Get e dimension.

    /*!
     * \brief conversion to string.
     */
    operator std::string() const {
        return 
            boost::lexical_cast<std::string>(_a) + "," +
            boost::lexical_cast<std::string>(_b) + "," +
            boost::lexical_cast<std::string>(_c) + "," +
            boost::lexical_cast<std::string>(_d) + "," +
            boost::lexical_cast<std::string>(_e)
            ;
    }

    /*!
     * \brief
     */
    bool valid() const {
        return 
            _a != std::numeric_limits<unsigned>::max() &&
            _b != std::numeric_limits<unsigned>::max() &&
            _c != std::numeric_limits<unsigned>::max() &&
            _d != std::numeric_limits<unsigned>::max() &&
            _e != std::numeric_limits<unsigned>::max()
            ;
    }

private:
    friend class boost::serialization::access;
    template<class Archive>
    void __attribute__ ((visibility("hidden"))) serialize(
            Archive &ar,                //!< [in] archive
            const unsigned int          //!< [in] version number
            )
    {
        ar & _a;
        ar & _b;
        ar & _c;
        ar & _d;
        ar & _e;
    }

private:
    unsigned _a;
    unsigned _b;
    unsigned _c;
    unsigned _d;
    unsigned _e;
};

/*!
 * \brief Extraction operator.
 */
std::ostream&
operator<<(
        std::ostream& os,
        const Coordinates& c
        );

} // runjob

#endif
