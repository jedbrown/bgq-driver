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
#ifndef RUNJOB_COMMON_TOOL_DAEMON_H
#define RUNJOB_COMMON_TOOL_DAEMON_H

#include "common/tool/Subset.h"

#include <boost/serialization/access.hpp>
#include <boost/serialization/string.hpp>
#include <boost/serialization/vector.hpp>

#include <string>
#include <vector>

namespace runjob {
namespace tool {

/*!
 * \brief
 */
class Daemon
{
public:
    /*!
     * \brief
     */
    typedef std::string Executable;

    /*!
     * \brief
     */
    typedef std::vector<std::string> Arguments;

public:
    /*!
     * \brief Ctor.
     */
    Daemon();

    /*!
     * \brief
     */
    void setExecutable(
            const Executable& exe   //!< [in]
            );

    /*!
     * \brief
     */
    const Executable& getExecutable() const { return _exe; }

    /*!
     * \brief
     */
    void splitArguments(
            const std::string& args
            );

    /*!
     * \brief
     */
    void setArguments(
            const Arguments& args
            );

    /*!
     * \brief
     */
    const Arguments& getArguments() const { return _args; }

    /*!
     * \brief
     */
    void setSubset(
            const Subset& subset    //!< [in]
            );

    /*!
     * \brief
     */
    const Subset& getSubset() const { return _subset; }

private:
    void __attribute__ ((visibility("hidden"))) validate();

    friend class boost::serialization::access;
    template<class Archive>
    void __attribute__ ((visibility("hidden"))) serialize(
            Archive &ar,                //!< [in] archive
            const unsigned int          //!< [in] version number
            )
    {
        ar & _exe;
        ar & _exeSet;
        ar & _args;
        ar & _argsSet;
        ar & _subset;
        ar & _subsetSet;
    }

private:
    Executable _exe;
    bool _exeSet;
    Arguments _args;
    bool _argsSet;
    Subset _subset;
    bool _subsetSet;
};

} // tool
} // runjob

#endif
