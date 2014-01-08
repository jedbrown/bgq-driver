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
#ifndef BGQ_UTILITY_PERFORMANCE_DATABASE_OUTPUT_H
#define BGQ_UTILITY_PERFORMANCE_DATABASE_OUTPUT_H
/*!
 * \file utility/include/performance/DatabaseOutput.h
 * \brief \link bgq::utility::performance::DatabaseOutput DatabaseOutput\endlink definition and implementation.
 */

#include <db/include/api/tableapi/gensrc/DBTComponentperf.h>

#include <db/include/api/cxxdb/Connection.h>
#include <db/include/api/cxxdb/Statement.h>
#include <db/include/api/cxxdb/Transaction.h>

#include <db/include/api/tableapi/DBConnectionPool.h>

#include <utility/include/performance/Mode.h>

#include <utility/include/Log.h>

#include <boost/date_time/posix_time/posix_time.hpp>

#include <boost/assign/list_of.hpp>

#include <boost/foreach.hpp>

#include <string>

namespace bgq {
namespace utility {
namespace performance {

/*!
 * \brief Implements the \link bgq::utility::performance::StatisticSet StatisticSet\endlink log output policy.
 *
 * Sends all output to the database. Note since this policy class is header only you'll need to link your
 * application to libbgqdb.so like so
\verbatim
LDFLAGS += -L $(BGQ_INSTALL_DIR)/db/lib -lbgqdb
LDFLAGS += -Wl,-rpath,$(BGQ_INSTALL_DIR)/db/lib
\endverbatim
 *
 * You should also ensure to initialize the BGQDB library properly prior to using this policy.
 */
class DatabaseOutput
{
protected:
    /*!
     * \brief ctor.
     */
    DatabaseOutput()
    {
    
    }

    /*!
     * \brief Insert counters into database using the prepared statement
     *
     * This method obtains a database connection and creates a prepared statement each
     * time this method is invoked. 
     */
    template <typename StoragePolicy>
    void output(
            const std::string& type,    //!< [in]
            StoragePolicy& storage      //!< [in]
            ) 
    {
        // can't use LOG_DECLARE_FILE here since we're in a header
        const log4cxx::LoggerPtr log_logger_ = log4cxx::Logger::getLogger( "ibm.utility.performance" );

        // get connection
        const cxxdb::ConnectionPtr connection(
                cxxdb::ConnectionPtr( BGQDB::DBConnectionPool::instance().getConnection()
                    )
                );
        if ( !connection ) {
            LOG_ERROR_MSG( "could not get connection to database" );
            return;
        }

        // get statement if we don't have it
        const cxxdb::StatementPtr statement(
            this->getStatement( connection )
            );
        if ( !statement ) {
            LOG_ERROR_MSG( "could not get prepared statement" );
            return;
        }

        // get storage container
        typename StoragePolicy::Container container;
        storage.get( container );

        // wrap these inserts in a transaction
        const cxxdb::Transaction tx( *connection );

        // most of the columns in this table are strings, and since we cannot assume the values in
        // each datapoint will fit into the column sizes, they are truncated and a warning is logged.
        bool truncate;
        statement->parameters()[ BGQDB::DBTComponentperf::COMPONENT_COL ].set( type, &truncate );
        if ( truncate ) LOG_WARN_MSG( "column " << BGQDB::DBTComponentperf::COMPONENT_COL << " value truncated: " << type );

        const unsigned MicrosecondsPerSecond = 1000000;

        try {
            LOG_DEBUG_MSG( "statistics for " << type );
            LOG_DEBUG_MSG( container.size() << " statistics" );

            // iterate through results
            BOOST_FOREACH( const typename StoragePolicy::Container::value_type& i, container ) {
                // bind parameters
                this->setColumn( statement, BGQDB::DBTComponentperf::ID_COL, i.getId() );
                this->setColumn( statement, BGQDB::DBTComponentperf::FUNCTION_COL, i.getFunction() );
                this->setColumn( statement, BGQDB::DBTComponentperf::SUBFUNCTION_COL, i.getSubFunction() );
                this->setColumn( statement, BGQDB::DBTComponentperf::DETAIL_COL, i.getOtherData() );
                this->setColumn( statement, BGQDB::DBTComponentperf::QUALIFIER_COL, i.getQualifier() );
                this->setColumn( statement, BGQDB::DBTComponentperf::MODE_COL, Mode::toString(i.getMode()) );

                // store durations as seconds in the database by converting the total number of microseconds
                statement->parameters()[ BGQDB::DBTComponentperf::DURATION_COL ].set(
                        static_cast<double>( i.getDuration().total_microseconds() ) / MicrosecondsPerSecond
                        );

                // entry date is the start time of the event, or the current
                // time stamp
                statement->parameters()[ BGQDB::DBTComponentperf::ENTRYDATE_COL ].set( 
                        i.getStart().is_not_a_date_time() ?
                        boost::posix_time::microsec_clock::local_time() :
                        i.getStart()
                        );

                statement->execute();
            }
        } catch ( const cxxdb::DatabaseException& e ) {
            // log stack trace so we know where this exception game from
            if ( std::string const * stack = boost::get_error_info<cxxdb::stack_error_info>(e) ) {
                LOG_DEBUG_MSG( *stack );
            }
            LOG_ERROR_MSG( e.what() );
        } catch ( const std::exception& e ) {
            LOG_ERROR_MSG( e.what() );
        }

        connection->commit();
    }

    /*!
     * \brief dtor.
     */
    ~DatabaseOutput() { }

private:
    /*!
     * \brief Get a prepared statement.
     */
    cxxdb::StatementPtr getStatement(
            cxxdb::ConnectionPtr connection
            )
    {
        // create statement
        cxxdb::StatementPtr result(
                connection->prepare(
                    "INSERT INTO TBGQComponentperf ( " +
                    BGQDB::DBTComponentperf::ID_COL + ", " +
                    BGQDB::DBTComponentperf::COMPONENT_COL + ", " +
                    BGQDB::DBTComponentperf::FUNCTION_COL + ", " +
                    BGQDB::DBTComponentperf::SUBFUNCTION_COL + ", " +
                    BGQDB::DBTComponentperf::DURATION_COL + ", " +
                    BGQDB::DBTComponentperf::DETAIL_COL + ", " +
                    BGQDB::DBTComponentperf::ENTRYDATE_COL + ", " +
                    BGQDB::DBTComponentperf::QUALIFIER_COL + ", " +
                    BGQDB::DBTComponentperf::MODE_COL +
                    ") "
                    "VALUES ( ?, ?, ?, ?, ?, ?, ?, ?, ? )",
                    boost::assign::list_of
                    ( BGQDB::DBTComponentperf::ID_COL )
                    ( BGQDB::DBTComponentperf::COMPONENT_COL )
                    ( BGQDB::DBTComponentperf::FUNCTION_COL )
                    ( BGQDB::DBTComponentperf::SUBFUNCTION_COL )
                    ( BGQDB::DBTComponentperf::DURATION_COL )
                    ( BGQDB::DBTComponentperf::DETAIL_COL )
                    ( BGQDB::DBTComponentperf::ENTRYDATE_COL )
                    ( BGQDB::DBTComponentperf::QUALIFIER_COL )
                    ( BGQDB::DBTComponentperf::MODE_COL )
                    )
                );

        return result;
    }

    void setColumn(
            cxxdb::StatementPtr statement,
            const std::string& name,
            const std::string& value
            )
    {
        bool truncate;
        if ( value.empty() ) {
            statement->parameters()[ name ].setNull();
            return;
        }

        statement->parameters()[ name ].set( value, &truncate );
        if ( truncate ) {
            const log4cxx::LoggerPtr log_logger_ = log4cxx::Logger::getLogger( "ibm.utility.performance" );
            LOG_WARN_MSG( "column " << name << " value truncated: " << value );
        }
    }
};

} // performance
} // utility
} // bgq

#endif
