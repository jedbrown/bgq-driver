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
#ifndef RUNJOB_SUB_BLOCK_H
#define RUNJOB_SUB_BLOCK_H

#include "common/Corner.h"
#include "common/Shape.h"

#include <boost/serialization/access.hpp>

namespace runjob {

/*!
 * \brief Container for a sub-block resource description.
 *
 * \see runjob::Shape
 * \see runjob::Corner
 */
class SubBlock
{
public:
    /*!
     * \brief Default constructor for an uninitialized sub-block resource.
     *
     * This object is invalid until the set method has been invoked.
     */
    SubBlock() throw();

    /*!
     * \brief ctor.
     */
    SubBlock(
            const Corner& corner,           //!< [in] 
            const Shape& shape              //!< [in]
            );

    /*!
     * \brief Set the corner location.
     */
    void setCorner(
            const Corner& corner   //!< [in] compute node location string
            );

    /*!
     * \brief Set the sub-block shape.
     */
    void setShape(
            const Shape& shape  //!< [in]
            );

    /*!
     * \brief Test if this object is valid.
     */
    bool isValid() const;

    const Corner& corner() const { return _corner; } //!< Get corner location.
    Corner& corner() { return _corner; } //!< Get corner location.
    const Shape& shape() const { return _shape; } //!< Get Shape.
    Shape& shape() { return _shape; } //!< Get Shape.

private:
    void __attribute__ ((visibility("hidden"))) validate();

    friend class boost::serialization::access;
    template<class Archive>
    void __attribute__ ((visibility("hidden"))) serialize(
            Archive &ar,                //!< [in] archive
            const unsigned int          //!< [in] version number
            )
    {
        ar & _corner;
        ar & _cornerSet;
        ar & _shape;
        ar & _shapeSet;
    }

private:
    Corner _corner;
    bool _cornerSet;
    Shape _shape;
    bool _shapeSet;
};

} // runjob

#endif
