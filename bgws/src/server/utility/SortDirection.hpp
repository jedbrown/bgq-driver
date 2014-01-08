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

#ifndef BGWS_UTILITY_SORT_DIRECTION_HPP_
#define BGWS_UTILITY_SORT_DIRECTION_HPP_


#include <string>


namespace bgws {
namespace utility {


class SortDirection {
public:

    enum Value {
        Ascending,
        Descending
    };


    static const std::string ASCENDING_SQL;
    static const std::string DESCENDING_SQL;


    static const std::string& toSql( Value d )  { return (d == Ascending ? ASCENDING_SQL : DESCENDING_SQL); }

};


} } // namespace bgws::utility

#endif
