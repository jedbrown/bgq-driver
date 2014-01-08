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
/* (C) Copyright IBM Corp.  2010, 2012                              */
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


#ifndef COMMON_HPP_
#define COMMON_HPP_


#include "portConfiguration/PortConfiguration.h"

#include "Log.h"
#include "Properties.h"

#include <iostream>


//---------------------------------------------------------------------
// Functions for checking different types of output types.


namespace std {

inline ostream& operator<<(
        ostream& os,
        const bgq::utility::PortConfiguration::Pair& pair
    )
{
    os << "(" << pair.first << "," << pair.second << ")";
    return os;
}


inline ostream& operator<<(
        ostream& os,
        const bgq::utility::PortConfiguration::Pairs& pairs
    )
{
    os << "[";
    for ( bgq::utility::PortConfiguration::Pairs::const_iterator i(pairs.begin()) ; i != pairs.end() ; ++i ) {
        if ( i != pairs.begin() )  os << ',';
        os << *i;
    }
    os << "]";
    return os;
}

}

inline bool operator==(
        const bgq::utility::PortConfiguration::Pairs& ps1,
        const bgq::utility::PortConfiguration::Pairs& ps2
    )
{
    if ( ps1.size() != ps2.size() )  { return false; }
    for ( int i(0) ; i < ps1.size() ; ++i ) {
        if ( ps1.at(i) != ps2.at(i) )  return false;
    }
    return false;
}


//---------------------------------------------------------------------
// A global fixture to initialize logging.


class InitializeLoggingFixture
{
public:
    InitializeLoggingFixture()
    {
        bgq::utility::initializeLogging( *bgq::utility::Properties::create() );
    }
};


#endif
