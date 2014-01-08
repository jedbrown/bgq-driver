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
 * \file security/Authorities.h
 */

#ifndef BGQ_HLCS_SECURITY_AUTHORITIES_H_
#define BGQ_HLCS_SECURITY_AUTHORITIES_H_

#include <hlcs/include/security/Authority.h>

#include <string>
#include <vector>

namespace hlcs {
namespace security {

/*!
 * \brief Authorities for an Object.
 */
class Authorities
{
public:
    /*!
     * \brief ctor
     */
    Authorities(
            const std::string& owner    //!< [in]
            );

    /*!
     * \brief Get owner.
     */
    const std::string& getOwner() const { return _owner; }

    /*!
     * \brief Get container.
     */
    const Authority::Container& get() const { return _container; }

    /*!
     * \brief Add an authority.
     */
    void add( const Authority& authority ) { _container.push_back(authority); }

private:
    Authority::Container _container;
    std::string _owner;
};

} // security
} // hlcs

#endif
