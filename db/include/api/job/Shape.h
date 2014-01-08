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

#ifndef BGQDB_JOB_SHAPE_H_
#define BGQDB_JOB_SHAPE_H_

#include <bgq_util/include/Location.h>

#include <iosfwd>
#include <string>

#include <stdint.h>

namespace BGQDB {
namespace job {

/*!
 * \brief 5-D shape (size in each dimension)
 */
class Shape
{
public:
    static const Shape Block; //!< Indicates use the block shape for the job.
    static const Shape Midplane; //!< Shape of a job on a midplane.
    static const Shape NodeBoard; //!< Shape of a job on a node board.
    static const Shape SingleNode; //!< Shape of a single-node job.
    static const Shape SingleCore; //!< Shape of a single-core job.

    /*!
     * \brief ctor.
     */
    Shape(
            uint16_t a = 0,
            uint16_t b = 0,
            uint16_t c = 0,
            uint16_t d = 0,
            uint16_t e = 0,
            uint16_t cores = bgq::util::Location::ComputeCardCoresOnBoard - 1
        );


    explicit Shape(
            const std::string& shape_str
        );

    /*!
     * \brief Get the value for the nodesUsed column.
     *
     * If it's a single-node job then the sizes will all be 1 and the value in the nodesUsed column should be 0 rather than 1.
     * Otherwise, you get the result of multiplying all the dimension sizes.
     */
    uint64_t getSize() const;

    uint16_t getA() const { return _a; } //!< Get A size.
    uint16_t getB() const { return _b; } //!< Get B size.
    uint16_t getC() const { return _c; } //!< Get C size.
    uint16_t getD() const { return _d; } //!< Get D size.
    uint16_t getE() const { return _e; } //!< Get E size.
    uint16_t getCores() const { return _cores; } //!< Get core size.

    bool operator==( const Shape& other ) const {
        return (_a == other._a &&
                _b == other._b &&
                _c == other._c &&
                _d == other._d &&
                _e == other._e &&
                _cores == other._cores);
    }


private:

    uint16_t _a, _b, _c, _d, _e, _cores;


    void _validate();
};


std::istream& operator>>( std::istream& is, Shape& shape_out );

std::ostream& operator<<( std::ostream& os, const Shape& shape );


} } // namespace BGQDB::job


#endif
