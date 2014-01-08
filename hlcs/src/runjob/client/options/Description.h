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
#ifndef RUNJOB_CLIENT_OPTIONS_DESCRIPTION_H
#define RUNJOB_CLIENT_OPTIONS_DESCRIPTION_H

#include <boost/program_options.hpp>
#include <boost/shared_ptr.hpp>

#include <ostream>

namespace runjob {

namespace client {
namespace options {

/*!
 * \brief Abstract base for options descriptions.
 */
class Description
{
public:
    /*!
     * \brief pointer type.
     */
    typedef boost::shared_ptr<Description> Ptr;

public:
    /*!
     * \brief ctor.
     */
    Description(
            const std::string& name //!< [in]
            ) :
        _options( name )
    {
        
    }

    /*!
     * \brief dtor.
     */
    virtual ~Description() = 0;

    /*!
     * \brief Combine options into a single source.
     */
    void combine(
            boost::program_options::options_description& options    //!< [in]
            ) const
    {
        options.add( _options );
    }

    /*!
     * \brief display help text.
     */
    void help(
            std::ostream& stream    //!< [in]
            ) const
    {
        stream << _options << std::endl;
    }

protected:
    boost::program_options::options_description _options;   //!< the options.
};

} // options
} // clienmt
} // runjob

#endif
