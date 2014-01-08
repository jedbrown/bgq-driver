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
#ifndef RUNJOB_COMMON_TOOL_SUBSET_RANGE_H
#define RUNJOB_COMMON_TOOL_SUBSET_RANGE_H

#include <boost/serialization/access.hpp>

#include <iosfwd>
#include <string>

namespace runjob {
namespace tool {

/*!
 * \brief
 */
class SubsetRange
{
public:
    /*!
     * \brief ctor.
     */
    SubsetRange(
            const std::string& token = std::string()    //!< [in]
            );

    /*!
     * \brief
     */
    bool match(
            unsigned rank   //!< [in]
            ) const;

    int begin() const { return _begin; }
    int end() const { return _end; }
    int stride() const { return _stride; }

private:
    int __attribute__ ((visibility("hidden"))) parseRank(
            const std::string& token
            ) const;

    std::string __attribute__ ((visibility("hidden"))) parseStride(
            const std::string& token
            );

    friend class boost::serialization::access;
    template<class Archive>
    void __attribute__ ((visibility("hidden"))) serialize(
            Archive &ar,                //!< [in] archive
            const unsigned int          //!< [in] version number
            )
    {
        ar & _begin;
        ar & _end;
        ar & _stride;
    }

private:
    friend std::ostream& operator<<(std::ostream&, const SubsetRange&);
    int _begin;
    int _end;
    int _stride;
};

/*!
 * \brief Insertion operator.
 */
std::ostream&
operator<<(
        std::ostream& os,
        const SubsetRange& range
        );

} // tool
} // runjob

#endif
