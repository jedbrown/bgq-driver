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
#ifndef RUNJOB_SERVER_RECONNECT_H
#define RUNJOB_SERVER_RECONNECT_H

#include <iosfwd>

namespace runjob {
namespace server {

/*!
 * \brief Command line parameter for the reconnection scope.
 * \ingroup argument_parsing
 *
 * # None: does nothing
 * # Blocks: reconnects to (I)nitialized compute and I/O blocks
 * # Jobs: also reconnects to (R)unning jobs
 */
class Reconnect
{
public:
    /*!
     * \brief
     */
    enum class Scope {
        Blocks,
        Jobs,
        None
    };

    Scope scope() const { return _scope; }  //!< Get scope.

public:
    /*!
     * \brief ctor.
     */
    Reconnect(
            Scope s = Scope::None
            );

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
        const Reconnect& reconnect  //!< [in]
        );

/*!
 * \brief extraction operator.
 * \ingroup argument_parsing
 */
std::istream&
operator>>(
        std::istream& stream,       //!< [in]
        Reconnect& reconnect        //!< [in]
        );


} // server
} // runjob

#endif
