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

#include "BlueGene.hpp"

#include "dbConnectionPool.hpp"
#include "Error.hpp"

#include <bgq_util/include/Location.h>

#include <db/include/api/cxxdb/cxxdb.h>

#include <utility/include/Log.h>

#include <boost/lexical_cast.hpp>
#include <boost/throw_exception.hpp>

#include <stdexcept>
#include <string>


using boost::lexical_cast;

using std::runtime_error;
using std::string;


LOG_DECLARE_FILE( "bgws" );


namespace bgws {


BlueGene::BlueGene()
{
    auto conn_ptr(dbConnectionPool::getConnection());

    _machine_info.rows = 0;
    _machine_info.columns = 0;

    {
        BGQDB::DimensionSizes dim_sizes;
        auto status(BGQDB::getMachineBPSize( dim_sizes ));

        if ( status != BGQDB::OK ) {
            BOOST_THROW_EXCEPTION( std::runtime_error( string() +
                    "failed to get machine size, db status=" + lexical_cast<string>(status)
                ) );
        }

        _machine_info.mp_locations.resize(
                boost::extents[dim_sizes[BGQDB::Dimension::A]]
                              [dim_sizes[BGQDB::Dimension::B]]
                              [dim_sizes[BGQDB::Dimension::C]]
                              [dim_sizes[BGQDB::Dimension::D]]
           );

        const string SQL = "SELECT location, torusA, torusB, torusC, torusD FROM bgqMidplane";

        auto rs_ptr(conn_ptr->query( SQL ));

        _machine_info.midplane_count = 0;

        while ( rs_ptr->fetch() ) {
            const auto &cols(rs_ptr->columns());

            auto mp_loc(cols["location"].getString());

            _machine_info.mp_locations[cols["torusA"].as<int64_t>()][cols["torusB"].as<int64_t>()][cols["torusC"].as<int64_t>()][cols["torusD"].as<int64_t>()] = mp_loc;
            ++_machine_info.midplane_count;

            bgq::util::Location loc( mp_loc );

            _machine_info.rows = std::max( _machine_info.rows, uint32_t(loc.getRackRow() + 1) );
            _machine_info.columns = std::max( _machine_info.columns, uint32_t(loc.getRackColumn() + 1) );

            LOG_DEBUG_MSG( "Adding midplane " << mp_loc << ". midplane_count=" << _machine_info.midplane_count );
        }
    }

    {

        static const std::string sql =

 "SELECT location"
 " FROM bgqIoDrawer"
 " ORDER BY location"

                ;

        auto rs_ptr(conn_ptr->query( sql ));

        while ( rs_ptr->fetch() ) {
            _machine_info.io_drawer_locations.push_back( rs_ptr->columns()["location"].getString() );
        }
    }

}


} // namespace bgws
