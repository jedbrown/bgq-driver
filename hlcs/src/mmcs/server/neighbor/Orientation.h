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
/* (C) Copyright IBM Corp.  2004, 2011                              */
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

#ifndef MMCS_SERVER_NEIGHBOR_ORIENTATION_H_
#define MMCS_SERVER_NEIGHBOR_ORIENTATION_H_

namespace mmcs {
namespace server {
namespace neighbor {

/*!
 * \brief Wrapper class to emulate c++11 enum class
 */
struct Orientation
{
    /*!
     * \brief positive or negative torus orientations.
     */
    enum Value {
        Plus,
        Minus,
        Invalid
    };
};

} } } // namespace mmcs::server::neighbor

#endif
