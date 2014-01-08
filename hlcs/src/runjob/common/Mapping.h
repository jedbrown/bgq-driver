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
#ifndef RUNJOB_MAPPING_H
#define RUNJOB_MAPPING_H

#include <boost/serialization/access.hpp>

#include <iostream>
#include <set>
#include <utility>
#include <vector>

#include <stdint.h>

namespace runjob {

/*!
 * \brief coordinate to rank mapping.
 * \ingroup argument_parsing
 */
class Mapping
{
public:
    /*!
     * \brief Possible mapping types
     */
    enum class Type
    {
        None,           //
        File,           // user provided file
        Permutation     // ABCDET permutation
    };

    /*!
     * \brief Contaienr of ranks
     */
    typedef std::set<uint32_t> Rank;

    /*!
     * \brief Size and line number
     */
    typedef std::pair<int, unsigned> Max;

    /*!
     * \brief
     */
    typedef std::vector<Max> Dimensions;

public:
    /*!
     * \brief ctor.
     */
    Mapping(
            Type t = Type::None,                        //!< [in]
            const std::string& value = std::string()    //!< [in]
          );

    Type type() const { return _type; }   //!< Get type.
    const std::string& value() const { return _value; } //!< Get value.
    operator const std::string&() const { return this->value(); }  //!< Conversion to const std::string&
    const Dimensions& dimensions() const { return _maxDimensions; }
    unsigned lineCount() const { return _lineCount; }

private:
    void validateFile();

    void analyzeLine(
            std::string& line,      //!< [in]
            Rank& ranks             //!< [in]
            );

    friend class boost::serialization::access;
    template<class Archive>
    void __attribute__ ((visibility("hidden"))) serialize(
            Archive &ar,                //!< [in] archive
            const unsigned int          //!< [in] version number
            )
    {
        ar & _type;
        ar & _value;
        ar & _maxDimensions;
        ar & _lineCount;
    }

private:
    Type _type;
    std::string _value;
    Dimensions _maxDimensions; // mapping file specific
    unsigned _lineCount; // mapping file specific 
};

/*!
 * \brief insertion operator.
 * \ingroup argument_parsing
 */
std::ostream&
operator<<(
        std::ostream& stream,       //!< [in]
        const Mapping& mapping      //!< [in]
        );

/*!
 * \brief extraction operator.
 * \ingroup argument_parsing
 */
std::istream&
operator>>(
        std::istream& stream,       //!< [in]
        Mapping& mapping            //!< [in]
        );

} // runjob

#endif
