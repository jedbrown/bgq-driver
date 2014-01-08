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

#ifndef CXXDB_COLUMNS_H_
#define CXXDB_COLUMNS_H_

#include "Column.h"
#include "fwd.h"

#include <boost/utility.hpp>

#include <map>
#include <string>
#include <vector>

namespace cxxdb {

class StatementHandle;


/*! \brief The columns in the result set for a query. */
class Columns : boost::noncopyable
{
public:

    /*! \brief Calculate columns for the statement. */
    Columns(
            StatementHandle& statement_handle //!< [ref]
        );

    /*! \brief Access the column by index.
     *
     *  Note that the first column is 1 and not 0.
     *
     *  \throws std::logic_error The index is out of range.
     */
    const Column& operator[]( unsigned column_index ) const;

    /*! \brief Access the column by name.
     *
     *  \throws std::logic_error The name doesn't exist in the result set.
     */
    const Column& operator[]( const std::string& column_name ) const;

    /*! \brief Get the index for the column with the given name.
     *
     *  \throws std::logic_error The name doesn't exist in the result set.
     */
    unsigned columnNameToIndex( const std::string& name ) const;


private:

    typedef std::vector<Column::Ptr> _ColumnPtrs;
    typedef std::map<std::string,Column::Ptr> _NameToCol;


    _ColumnPtrs _col_ptrs;
    _NameToCol _name_to_col;
};

} // namespace cxxdb

#endif
