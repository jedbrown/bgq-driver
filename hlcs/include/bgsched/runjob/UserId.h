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
 * \file bgsched/runjob/UserId.h
 * \brief definition and implementation of bgsched::runjob::UserId class.
 */

#ifndef BGSCHED_RUNJOB_USER_ID_H
#define BGSCHED_RUNJOB_USER_ID_H

#include <boost/shared_ptr.hpp>

#include <vector>

#include <sys/types.h>

namespace bgsched {
namespace runjob {

/*!
 * \brief
 */
class UserId
{
public:
    /*!
     * \brief
     */
    typedef std::vector<gid_t> Groups;

public:
    /*!
     * \brief
     */
    uid_t uid() const;

    /*!
     * \brief
     */
    const Groups& groups() const;

    /*!
     * \brief Implementation type.
     */
    class Impl;

    /*!
     * \brief Pointer to implementation type.
     */
    typedef boost::shared_ptr<Impl> Pimpl;

    /*!
     * \brief ctor.
     */
    UserId(
            const Pimpl& impl   //!< [in]
          );

private:
    Pimpl _impl;
};

} // runjob
} // bgsched

#endif
