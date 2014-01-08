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
#ifndef RUNJOB_SHAPE_H
#define RUNJOB_SHAPE_H

#include "common/Uci.h"

#include <string>

#include <boost/serialization/access.hpp>
#include <boost/serialization/map.hpp>
#include <boost/serialization/vector.hpp>

#include <sstream>
#include <map>

#include <stdint.h>

namespace runjob {

/*!
 * \brief 5 dimensional shape for sub-block jobs.
 * \ingroup argument_parsing
 *
 * \see runjob::SubBlock
 * \see runjob::Corner
 */
class Shape
{
public:
    /*!
     * \brief Node board container.
     *
     * node board jtag position is the key
     * vector of compute card jtag positions is the value
     */
    typedef std::map< uint8_t, std::vector<uint8_t> > Nodes;

public:
    /*!
     * \brief ctor.
     *
     * A valid shape is a string containing
     * - 5 dimensions separated by x characters
     * - All dimensions greater than 0
     * - A, B, C, and D dimension are 4 or less
     * - E dimension is 2 or less
     *
     * \throws runjob::Exception if the shape is invalid
     */
    Shape(
            const std::string& shape = std::string()   //!< [in]
         );

    /*!
     * \brief equality operator.
     */
    bool operator==(
            const Shape& other  //!< [in]
            ) const;

    /*!
     * \brief Get the size in nodes.
     */
    uint32_t size() const { return _a * _b * _c * _d * _e; }

    unsigned a() const { return _a; } //!< Get A size.
    unsigned b() const { return _b; } //!< Get B size.
    unsigned c() const { return _c; } //!< Get C size.
    unsigned d() const { return _d; } //!< Get D size.
    unsigned e() const { return _e; } //!< Get E size.

    /*!
     * \brief Get the nodes.
     */
    const Nodes& nodes() const { return _nodes; }

    /*!
     * \brief 
     */
    void addNode(
            const Uci& location    //!< [in]
            );

    /*!
     * \brief Conversion to string.
     *
     * returns an empty string if the shape has not been set yet.
     */
    operator std::string() const;

    /*!
     * \brief Test if Shape is valid.
     */
    bool isValid() const { return _a && _b && _c && _d && _e; }

private:
    friend class boost::serialization::access;
    template<class Archive>
    void __attribute__ ((visibility("hidden"))) serialize(
            Archive &ar,                //!< [in] archive
            const unsigned int          //!< [in] version number
            )
    {
        ar & _a & _b & _c & _d & _e;
        ar & _nodes;
    }

private:
    unsigned _a;
    unsigned _b;
    unsigned _c;
    unsigned _d;
    unsigned _e;
    Nodes _nodes;
};

/*!
 * \brief extraction operator.
 * \ingroup argument_parsing
 */
std::istream&
operator>>(
        std::istream& stream,       //!< [in]
        Shape& shape                //!< [in]
        );

/*!
 * \brief insertion poerator.
 * \ingroup argument_parsing
 */
std::ostream&
operator<<(
        std::ostream& os,           //!< [in]
        const Shape& shape          //!< [in]
        );

} // runjob

#endif
