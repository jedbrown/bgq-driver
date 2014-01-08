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
#ifndef RUNJOB_COMMON_ENVIRONMENT_H
#define RUNJOB_COMMON_ENVIRONMENT_H

#include <boost/serialization/access.hpp>

#include <iostream>
#include <string>

namespace runjob {

/*!
 * \brief An environment variable in key=value form.
 * \ingroup argument_parsing
 */
class Environment
{
public:
    /*!
     * \brief ctor.
     */
    Environment(
            const std::string& key = std::string(),
            const std::string& value = std::string()
            );

    /*!
     * \brief Get key.
     */
    const std::string& getKey() const { return _key; }

    /*!
     * \brief Get value.
     */
    const std::string& getValue() const { return _value; }

    /*!
     * \brief conversion to std::string.
     */
    operator std::string() const
    {
        return _key + "=" + _value;
    }

private:
    friend class boost::serialization::access;
    template<class Archive>
    void __attribute__ ((visibility("hidden"))) serialize(
            Archive &ar,                //!< [in] archive
            const unsigned int          //!< [in] version number
            )
    {
        ar & _key;
        ar & _value;
    }


private:
    std::string _key;
    std::string _value;
};

/*!
 * \brief extraction operator.
 * \ingroup argument_parsing
 */
std::ostream&
operator<<(
        std::ostream& stream,       //!< [in]
        const Environment& env      //!< [in]
        );

/*!
 * \brief insertion operator.
 * \ingroup argument_parsing
 */
std::istream&
operator>>(
        std::istream& stream,       //!< [in]
        Environment& env            //!< [in]
        );

} // runjob

#endif
