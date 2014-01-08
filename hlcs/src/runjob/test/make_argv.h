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
#ifndef RUNJOB_TEST_CLIENT_MAKE_ARGV_H
#define RUNJOB_TEST_CLIENT_MAKE_ARGV_H

#include <boost/tokenizer.hpp>

/*!
 * \brief
 */
inline char**
make_argv(
        const std::string& string,      //!< [in]
        uint32_t& size                  //!< [in]
        )
{
    // tokenize on spaces
    typedef boost::tokenizer<boost::char_separator<char> > Tokenizer;
    boost::char_separator<char> sep(" ");
    Tokenizer tokens(string, sep);

    // set size return value
    size = static_cast<uint32_t>(std::distance( tokens.begin(), tokens.end() ) + 1);

    // allocate storage for result
    char** result = new char*[ size + 1 ];

    // first token is bogus program name
    const char* argv_zero = "tester";
    result[0] = new char[strlen(argv_zero) + 1];
    strncpy(result[0], argv_zero, strlen(argv_zero));
    result[0][strlen(argv_zero)] = '\0';

    // copy tokens into result
    for (Tokenizer::iterator i = tokens.begin(); i != tokens.end(); ++i) {
        // get token number
        size_t n = std::distance( tokens.begin(), i ) + 1;

        // allocate storage for string
        result[n] = new char[ i->size() + 1 ];

        // copy string
        strncpy(result[n], i->c_str(), i->size());

        // null terminate
        result[n][i->size()] = '\0';
    }

    // null terminate array
    result[ std::distance( tokens.begin(), tokens.end() ) + 1 ] = NULL;

    return result;
}

#endif
