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
 * \file utility/include/Symlink.h
 */
#ifndef BGQ_UTILITY_SYMLINK
#define BGQ_UTILITY_SYMLINK

#include <boost/filesystem/path.hpp>

#include <string>

namespace bgq {
namespace utility {

/*!
 * \brief Wrapper around readlink(2)
 *
 * Example usage:
 *
 * \include test/Symlink/sample.cc
 */
class Symlink
{
public:
    /*!
     * \brief ctor.
     *
     * \throws boost::system::system_error if readlink(2) returns -1
     */
    Symlink(
            const boost::filesystem::path&  //!< [in]
           );

    /*!
     * \brief conversion to std::string.
     */
    operator const std::string&() const { return _link; }

private:
    std::string _link;
};

} // utility
} // bgq

#endif
