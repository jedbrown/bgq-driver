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
#ifndef RUNJOB_STRACE_H
#define RUNJOB_STRACE_H

#include <boost/serialization/access.hpp>

#include <iostream>

#include <stdint.h>

namespace runjob {

/*!
 * \brief system call tracing.
 * \ingroup argument_parsing
 */
class Strace
{
public:
    /*!
     * \brief Possible strace scope values.
     */
    enum Scope
    {
        Node,
        None
    };

public:
    /*!
     * \brief ctor.
     */
    Strace(
            Scope scope = None,
            uint32_t rank = 0
          ) throw() :
        _scope( scope ),
        _rank( rank )
    {

    }

    Scope getScope() const { return _scope; }   //!< Get scope.
    uint32_t getRank() const { return _rank; }  //!< Get rank.

private:
    friend class boost::serialization::access;
    template<class Archive>
    void __attribute__ ((visibility("hidden"))) serialize(
            Archive &ar,                //!< [in] archive
            const unsigned int          //!< [in] version number
            )
    {
        ar & _scope;
        ar & _rank;
    }

private:
    Scope _scope;
    uint32_t _rank;
};

/*!
 * \brief insertion operator.
 * \ingroup argument_parsing
 */
std::ostream&
operator<<(
        std::ostream& stream,       //!< [in]
        const Strace& strace        //!< [in]
        );

/*!
 * \brief extraction operator.
 * \ingroup argument_parsing
 */
std::istream&
operator>>(
        std::istream& stream,       //!< [in]
        Strace& strace              //!< [in]
        );

} // runjob

#endif
