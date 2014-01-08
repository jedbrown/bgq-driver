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
#ifndef RUNJOB_COMMON_TOOL_SUBSET_H
#define RUNJOB_COMMON_TOOL_SUBSET_H

#include "common/tool/SubsetRange.h"

#include <boost/serialization/access.hpp>
#include <boost/serialization/vector.hpp>

#include <iosfwd>
#include <string>
#include <vector>

namespace runjob {
namespace tool {

/*!
 * \brief
 */
class Subset
{
public:
    /*!
     * \brief
     */
    typedef std::vector<unsigned> Ranks;

    /*!
     * \brief
     */
    typedef std::vector<SubsetRange> Ranges;

    /*!
     * \brief
     */
    static const size_t MaximumLength;

public:
    /*!
     * \brief ctor.
     *
     * \throws std::runtime_error if the string is larger than MaximumSize
     * \throws std::logic_error
     * \throws std::invalid_argument
     */
    Subset(
            const std::string& value = std::string()
          );

    /*!
     * \brief
     */
    bool match(
            unsigned rank
            ) const;

    /*!
     * \brief
     */
    const std::string& value() const { return _value; }

private:
    void __attribute__ ((visibility("hidden"))) parse(
            const std::string& token
            );

    unsigned __attribute__ ((visibility("hidden"))) rank(
            const std::string& token
            );

    friend class boost::serialization::access;
    template<class Archive>
    void __attribute__ ((visibility("hidden"))) serialize(
            Archive &ar,                //!< [in] archive
            const unsigned int          //!< [in] version number
            )
    {
        ar & _value;
        ar & _min;
        ar & _max;
        ar & _ranks;
        ar & _ranges;
    }

private:
    friend std::ostream& operator<<(std::ostream&, const Subset&);
    std::string _value;
    unsigned _min;
    unsigned _max;
    Ranks _ranks;
    Ranges _ranges;
};

/*!
 * \brief Extraction operator.
 */
std::istream&
operator>>(
        std::istream& is,   //!< [in]
        Subset& subset  //!< [in]
        );

/*!
 * \brief Insertion operator.
 */
std::ostream&
operator<<(
        std::ostream& os,           //!< [in]
        const Subset& subset    //!< [in]
        );

} // tool
} // runjob

#endif
