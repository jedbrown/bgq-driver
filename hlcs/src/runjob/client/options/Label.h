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
#ifndef RUNJOB_CLIENT_OPTIONS_LABEL_H
#define RUNJOB_CLIENT_OPTIONS_LABEL_H

#include <iosfwd>

namespace runjob {
namespace client {
namespace options {

/*!
 * \brief
 * \ingroup argument_parsing
 */
class Label
{
public:
    /*!
     * \brief
     */
    enum Scope
    {
        None,
        Short,
        Long
    };

public:
    /*!
     * \brief ctor.
     */
    Label(
            Scope scope = None
         );

    Scope getScope() const { return _scope; }   //!< Get scope.

private:
    Scope _scope;
};

/*!
 * \brief insertion operator.
 * \ingroup argument_parsing
 */
std::ostream&
operator<<(
        std::ostream& stream,       //!< [in]
        const Label& label          //!< [in]
        );

/*!
 * \brief extraction operator.
 * \ingroup argument_parsing
 */
std::istream&
operator>>(
        std::istream& stream,       //!< [in]
        Label& label                //!< [in]
        );

} // options
} // client
} // runjob

#endif
