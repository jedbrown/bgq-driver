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
 * \file security/Authority.h
 */

#ifndef BGQ_HLCS_SECURITY_AUTHORITY_H_
#define BGQ_HLCS_SECURITY_AUTHORITY_H_

#include <hlcs/include/security/Action.h>

#include <string>
#include <vector>

namespace hlcs {
namespace security {

/*!
 * \brief tuple of user and Action representing an authority.
 */
class Authority
{
public:
    /*!
     * \brief Container type.
     */
    typedef std::vector<Authority> Container;

    /*!
     * \brief Where an Authority came from.
     */
    struct Source
    {
        /*!
         * \brief possible types.
         */
        enum Type {
            Granted,
            Properties,
            Invalid
        };
    };

public:
    /*!
     * \brief ctor.
     */
    Authority(
            const std::string& user,    //!< [in]
            Action::Type action         //!< [in]
            );

    /*!
     * \brief Get user name.
     */
    const std::string& user() const { return _user; }

    /*!
     * \brief Get action type.
     */
    Action::Type action() const { return _action; }

    /*!
     * \brief Set the source
     */
    void source( Source::Type source ) { _source = source; }

    /*!
     * \brief Get the source.
     */
    Source::Type source() const { return _source; }

private:
    std::string _user;
    Action::Type _action;
    Source::Type _source;
};

/*!
 * \brief Authority insertion operator.
 */
std::ostream&
operator<<(std::ostream&, const Authority&);

/*!
 * \brief Authorities::Source::Type insertion operator.
 */
std::ostream&
operator<<(std::ostream&, const Authority::Source::Type&);

} // security
} // hlcs

#endif
