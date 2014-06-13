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
#include <string>
#include <utility>
#include <vector>

#include <stdint.h>

namespace runjob {

/*!
 * \brief coordinate to rank mapping.
 * \ingroup argument_parsing
 *
 * When the mapping type is a file, and validation is performed, the contents of the file
 * are packed into a 32 bit unsigned integer per rank. This container is then serialized
 * with the rest of the job description and sent to the runjob_server as part of the job
 * launch process where it is used for additional validation.
 *
 * Note (2/20/14): For calculating number of ranks to include in runjob_server and for mpmd
 * the entire contents of the mapping file is now packaged up in a vector of strings and
 * serialized over to runjob_server.
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
     * \brief Container of ranks
     */
    typedef std::vector<uint32_t> Rank;

    /*!
     * \brief Container representing full mapping file contents
     */
    typedef std::vector<std::string> FullMapFileContents;

public:
    /*!
     * \brief ctor.
     */
    Mapping(
            Type t = Type::None,                        //!< [in]
            const std::string& value = std::string(),   //!< [in]
            bool performValidation = true               //!< [in]
           );

    /*!
     * \brief get mapping type
     */
    static Type getType(
            const std::string& value    //!< [in]
            );

    Type type() const { return _type; }   //!< Get type.
    const std::string& value() const { return _value; } //!< Get value.
    operator const std::string&() const { return this->value(); }  //!< Conversion to const std::string&
    const std::vector<uint32_t>& fileContents() const { return _fileContents; }
    const std::vector<std::string>& fullMapFileContents() const { return _fullMapFileContents; }

private:
    void validateFile();

    void analyzeLine(
            std::string& line,      //!< [in]
            Rank& ranks,            //!< [in]
            size_t lineNumber       //!< [in]
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
        ar & _fileContents;
        ar & _fullMapFileContents;
    }

private:
    Type _type;
    std::string _value;
    std::vector<uint32_t> _fileContents; // encoded mapping file contents
    std::vector<std::string> _fullMapFileContents; // full mapping file contents
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
