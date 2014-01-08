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

#include "SortInfo.hpp"

#include <utility/include/Log.h>

#include <boost/foreach.hpp>


using std::string;


LOG_DECLARE_FILE( "bgws" );


namespace bgws {


SortInfo::SortInfo(
        const IdToCol& id_to_col,
        const KeyCols& key_cols,
        const std::string& default_column_name,
        utility::SortDirection::Value default_direction
    ) :
        _id_to_col(id_to_col),
        _key_cols(key_cols),
        _default_column_name(default_column_name),
        _default_direction(default_direction)
{
    // Nothing to do.
}


std::string SortInfo::calcSortClause( const SortSpec& sort_spec ) const
{
    string ret;

    utility::SortDirection::Value sort_direction(_default_direction);

    string sort_col;

    if ( ! sort_spec.isDefault() ) {
        IdToCol::const_iterator i(_id_to_col.find( sort_spec.getColumnId() ));

        if ( i == _id_to_col.end() ) {

            LOG_WARN_MSG( "Didn't find column '" << sort_spec.getColumnId() << "' in sort fields, will use default."  );

            for ( IdToCol::const_iterator i2 = _id_to_col.begin() ; i2 != _id_to_col.end() ; ++i2 ) {
                LOG_DEBUG_MSG( "field: " << i2->first << " -> " << i2->second );
            }

            sort_col = _default_column_name;
            sort_direction = sort_spec.getDirection();
        } else {
            sort_col = i->second;
            sort_direction = sort_spec.getDirection();
        }

        ret += sort_col + " " + utility::SortDirection::toSql( sort_direction );
    }

    BOOST_FOREACH( const string &key_col, _key_cols ) {
        if ( sort_col == key_col )  continue;

        if ( ! ret.empty() )  ret += ", ";
        ret += key_col + " " + utility::SortDirection::toSql( sort_direction );
    }

    return ret;
}


} // namespace bgws
