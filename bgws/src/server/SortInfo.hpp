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

#ifndef BGWS_SORT_INFO_HPP_
#define BGWS_SORT_INFO_HPP_


#include "SortSpec.hpp"

#include "utility/SortDirection.hpp"

#include <map>
#include <string>
#include <vector>


namespace bgws {


class SortInfo
{
public:

    typedef std::map<std::string,std::string> IdToCol;
    typedef std::vector<std::string> KeyCols;


    SortInfo(
            const IdToCol& id_to_col,
            const KeyCols& key_cols,
            const std::string& default_column_name,
            utility::SortDirection::Value default_direction
        );

    std::string calcSortClause( const SortSpec& sort_spec ) const;


private:

    IdToCol _id_to_col;
    KeyCols _key_cols;
    std::string _default_column_name;
    utility::SortDirection::Value _default_direction;

};


} // namespace bgws


#endif
