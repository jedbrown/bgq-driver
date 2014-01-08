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

#ifndef BGWS_SORT_SPEC_HPP_
#define BGWS_SORT_SPEC_HPP_


#include "utility/SortDirection.hpp"

#include <iosfwd>
#include <string>


namespace bgws {


class SortSpec {
public:
    explicit SortSpec( const std::string& str = std::string() );

    bool isDefault() const  { return _column_id.empty(); }
    const std::string& getColumnId() const  { return _column_id; }
    utility::SortDirection::Value getDirection() const  { return _direction; }

private:
    std::string _column_id;
    utility::SortDirection::Value _direction;
};


std::istream& operator>>( std::istream& is, SortSpec& ss_out );


} // namespace bgws

#endif
