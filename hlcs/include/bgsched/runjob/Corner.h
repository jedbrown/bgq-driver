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
 * \file bgsched/runjob/Corner.h
 * \brief definition and implementation of bgsched::runjob::Corner class.
 */

#ifndef BGSCHED_RUNJOB_CORNER_H_
#define BGSCHED_RUNJOB_CORNER_H_

#include <bgsched/runjob/Coordinates.h>
#include <string>

namespace bgsched {
namespace runjob {

/*!
 * \brief Corner location and coordinates.
 *
 * \note the coordinates are relative to the block.
 *
 * \see Shape
 * \see Verify
 */
class Corner
{
public:
    /*!
     * \brief ctor.
     */
    Corner();

    /*!
     * \brief ctor.
     */
    Corner(
            const unsigned coordinates[5]    //!< [in]
          );

    /*!
     * \brief set Coordinates.
     */
    Corner& coordinates(
            const Coordinates& coordinates  //!< [in]
            );

    Corner& location(
            const std::string& location //!< [in]
            );

    const std::string& location() const;
    const Coordinates& coordinates() const;

    /*!
     * \brief Implementation type.
     */
    class Impl;

    /*!
     * \brief Pointer to implementation type.
     */
    typedef boost::shared_ptr<Impl> Pimpl;

private:
    Pimpl _impl;
};

} // runjob
} // bgsched

#endif
