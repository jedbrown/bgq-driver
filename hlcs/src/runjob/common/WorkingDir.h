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
#ifndef RUNJOB_WORKING_DIR_H
#define RUNJOB_WORKING_DIR_H

#include <iosfwd>
#include <string>

namespace runjob {

/*!
 * \brief
 */
class WorkingDir
{
public:
    /*!
     * \brief ctor.
     */
    WorkingDir(
            const std::string& value = std::string()    //!< [in]
            );

    /*!
     * \brief
     */
    operator const std::string&() const { return _value; }

    /*!
     * \brief
     */
    const std::string& get() const { return _value; }

private:
    std::string _value;
};

/*!
 * \brief insertion operator.
 * \ingroup argument_parsing
 */
std::ostream&
operator<<(
        std::ostream& stream,       //!< [in]
        const WorkingDir& cwd       //!< [in]
        );

/*!
 * \brief extraction operator.
 * \ingroup argument_parsing
 */
std::istream&
operator>>(
        std::istream& stream,       //!< [in]
        WorkingDir& cwd             //!< [in]
        );

} // runjob

#endif
