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
#ifndef RUNJOB_SERVER_JOB_TOOL_RANK_H
#define RUNJOB_SERVER_JOB_TOOL_RANK_H

#include "common/Uci.h"

namespace runjob {
namespace tool {

/*!
 * \brief
 */
class Rank
{
public:
    /*!
     * \brief
     */
    Rank();

    /*!
     * \brief
     */
    Rank(
            const Uci& io,  //!< [in]
            unsigned rank,  //!< [in]
            unsigned pid    //!< [in]
        );

    void setIo( const Uci& io ) { _io = io; }
    const Uci& io() const { return _io; }
    unsigned rank() const { return _rank; }
    unsigned pid() const { return _pid; }

private:
    friend class boost::serialization::access;
    template<class Archive>
    void __attribute__ ((visibility("hidden"))) serialize(
            Archive &ar,                //!< [in] archive
            const unsigned int          //!< [in] version number
            )
    {
        ar & _io;
        ar & _rank;
        ar & _pid;
    }

private:
    Uci _io;
    unsigned _rank;
    unsigned _pid;
};

} // tool
} // runjob

#endif

