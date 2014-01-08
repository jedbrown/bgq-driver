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
/* (C) Copyright IBM Corp.  2012, 2012                              */
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


#include "Machine.hpp"

#include "../BlockingOperationsThreadPool.hpp"
#include "../dbConnectionPool.hpp"

#include "chiron-json/json.hpp"

#include <db/include/api/cxxdb/cxxdb.h>

#include <boost/bind.hpp>

#include <string>


using boost::bind;

using std::string;


namespace bgws {
namespace query {


Machine::Ptr Machine::create(
            BlockingOperationsThreadPool &blocking_operations_thread_pool
        )
{
    return Ptr( new Machine(
            blocking_operations_thread_pool
        ) );
}


Machine::Machine(
        BlockingOperationsThreadPool &blocking_operations_thread_pool
    ) :
        _blocking_operations_thread_pool(blocking_operations_thread_pool),
        _strand( blocking_operations_thread_pool.getIoService() ),
        _not_available_obj_p(NULL)
{
    // Nothing to do.
}


void Machine::start(
        CompleteCb complete_cb
    )
{
    _complete_cb = complete_cb;

    _obj_val_ptr = json::Object::create();


    _blocking_operations_thread_pool.post( bind(
            &Machine::_startMachineDetailsQuery, shared_from_this()
        ) );

    _blocking_operations_thread_pool.post( bind(
            &Machine::_startComputeRacks, shared_from_this()
        ) );

    _blocking_operations_thread_pool.post( bind(
            &Machine::_startIoRacks, shared_from_this()
        ) );

    _blocking_operations_thread_pool.post( bind(
            &Machine::_startUnavailableHardware, shared_from_this()
        ) );

    _blocking_operations_thread_pool.post( bind(
            &Machine::_startNotAvailableServiceCard, shared_from_this()
        ) );

    _blocking_operations_thread_pool.post( bind(
            &Machine::_startNotAvailableSwitches, shared_from_this()
        ) );

    _blocking_operations_thread_pool.post( bind(
            &Machine::_startNotAvailableNodes, shared_from_this()
        ) );

    _blocking_operations_thread_pool.post( bind(
            &Machine::_startNotAvailableIoNodes, shared_from_this()
        ) );

    _blocking_operations_thread_pool.post( bind(
            &Machine::_startNotAvailableDcas, shared_from_this()
        ) );

    _blocking_operations_thread_pool.post( bind(
            &Machine::_startNotAvailablePowerModules, shared_from_this()
        ) );
}


Machine::~Machine()
{
    _complete_cb(
            _exc_ptr,
            _obj_val_ptr
        );
}


void Machine::_startMachineDetailsQuery()
{
    if ( _exc_ptr != 0 )  return;

    try {
        static const string SQL( "SELECT * FROM bgqMachine" );


        auto conn_ptr(dbConnectionPool::getConnection());

        cxxdb::ResultSetPtr rs_ptr(conn_ptr->query( SQL ));

        _strand.post( bind(
                &Machine::_machineDetailsQueryComplete, shared_from_this(),
                conn_ptr,
                rs_ptr
            ) );
    } catch ( std::exception& e ) {
        _exc_ptr = std::current_exception();
    }
}


void Machine::_machineDetailsQueryComplete(
        cxxdb::ConnectionPtr,
        cxxdb::ResultSetPtr rs_ptr
    )
{
    if ( _exc_ptr != 0 )  return;

    try {
        rs_ptr->fetch();

        json::Object &obj(_obj_val_ptr->get());

        obj.set( "serialNumber", rs_ptr->columns()["serialNumber"].getString() );
        obj.set( "productId", rs_ptr->columns()["productId"].getString() );
        if ( ! rs_ptr->columns()["description"].isNull() )  obj.set( "description", rs_ptr->columns()["description"].getString() );
        obj.set( "status", rs_ptr->columns()["status"].getString() );
        obj.set( "hasEthernetGateway", rs_ptr->columns()["hasEthernetGateway"].getString() );
        obj.set( "mtu", rs_ptr->columns()["mtu"].as<int64_t>() );
        obj.set( "clockHz", rs_ptr->columns()["clockHz"].as<int64_t>() );
        if ( ! rs_ptr->columns()["bringupOptions"].isNull() )  obj.set( "bringupOptions", rs_ptr->columns()["bringupOptions"].getString() );
    } catch ( std::exception& e ) {
        _exc_ptr = std::current_exception();
    }
}


void Machine::_startComputeRacks()
{
    if ( _exc_ptr != 0 )  return;

    try {
        static const string SQL( "SELECT DISTINCT LEFT( location, 3 ) AS location FROM bgqMidplane ORDER BY location" );

        auto conn_ptr(dbConnectionPool::getConnection());

        cxxdb::ResultSetPtr rs_ptr(conn_ptr->query( SQL ));

        _strand.post( bind(
                &Machine::_computeRacksComplete, shared_from_this(),
                conn_ptr,
                rs_ptr
            ) );
    } catch ( std::exception& e ) {
        _exc_ptr = std::current_exception();
    }
}


void Machine::_computeRacksComplete(
        cxxdb::ConnectionPtr,
        cxxdb::ResultSetPtr rs_ptr
    )
{
    if ( _exc_ptr != 0 )  return;

    try {
        json::Object &obj(_obj_val_ptr->get());

        json::Array &racks_arr(obj.createArray( "racks" ));

        while ( rs_ptr->fetch() ) {
            json::Object& rack_obj(racks_arr.addObject());

            string location(rs_ptr->columns()["location"].getString());

            rack_obj.set( "location", location );
            rack_obj.set( "URI", string() + "/bg/machine/" + location );
        }
    } catch ( std::exception& e ) {
        _exc_ptr = std::current_exception();
    }
}


void Machine::_startIoRacks()
{
    if ( _exc_ptr != 0 )  return;

    try {
        static const string SQL( "SELECT location, status FROM bgqIoRack ORDER BY location" );

        auto conn_ptr(dbConnectionPool::getConnection());

        cxxdb::ResultSetPtr rs_ptr(conn_ptr->query( SQL ));

        _strand.post( bind(
                &Machine::_ioRacksComplete, shared_from_this(),
                conn_ptr,
                rs_ptr
            ) );
    } catch ( std::exception& e ) {
        _exc_ptr = std::current_exception();
    }
}


void Machine::_ioRacksComplete(
        cxxdb::ConnectionPtr,
        cxxdb::ResultSetPtr rs_ptr
    )
{
    if ( _exc_ptr != 0 )  return;

    try {
        json::Object &obj(_obj_val_ptr->get());

        json::Array *io_racks_arr_p(NULL);

        while ( rs_ptr->fetch() ) {
            if ( ! io_racks_arr_p )  io_racks_arr_p = &(obj.createArray( "ioRacks" ));

            json::Object& io_rack_obj(io_racks_arr_p->addObject());

            string location(rs_ptr->columns()["location"].getString());

            io_rack_obj.set( "location", location );
            io_rack_obj.set( "URI", string() + "/bg/machine/" + location );

            io_rack_obj.set( "status", rs_ptr->columns()["status"].getString() );
        }
    } catch ( std::exception& e ) {
        _exc_ptr = std::current_exception();
    }
}


void Machine::_startUnavailableHardware()
{
    if ( _exc_ptr != 0 )  return;

    try {
        static const string SQL(

 "SELECT location, status FROM bgqMidplane WHERE status <> 'A'"
" UNION ALL"
" SELECT location, status FROM bgqNodeCard WHERE status <> 'A'"
" UNION ALL"
" SELECT location, status FROM bgqIoRack WHERE status <> 'A'"
" UNION ALL"
" SELECT location, status FROM bgqIoDrawer WHERE status <> 'A'"

            );

        auto conn_ptr(dbConnectionPool::getConnection());

        cxxdb::ResultSetPtr rs_ptr(conn_ptr->query( SQL ));

        _strand.post( bind(
                &Machine::_unavailableHardwareComplete, shared_from_this(),
                conn_ptr,
                rs_ptr
            ) );
    } catch ( std::exception& e ) {
        _exc_ptr = std::current_exception();
    }
}


void Machine::_unavailableHardwareComplete(
        cxxdb::ConnectionPtr,
        cxxdb::ResultSetPtr rs_ptr
    )
{
    if ( _exc_ptr != 0 )  return;

    try {
        while ( rs_ptr->fetch() ) {
            if ( ! _not_available_obj_p )  _not_available_obj_p = &(_obj_val_ptr->get().createObject( "notAvailable" ));

            json::Object &na_obj(_not_available_obj_p->createObject( rs_ptr->columns()["location"].getString() ));

            na_obj.set( "status", rs_ptr->columns()["status"].getString() );
        }
    } catch ( std::exception& e ) {
        _exc_ptr = std::current_exception();
    }
}


void Machine::_startNotAvailableServiceCard()
{
    if ( _exc_ptr != 0 )  return;

    try {
        static const string SQL(

"SELECT midplanePos, status FROM bgqServiceCard WHERE status <> 'A'"

            );

        auto conn_ptr(dbConnectionPool::getConnection());

        cxxdb::ResultSetPtr rs_ptr(conn_ptr->query( SQL ));

        _strand.post( bind(
                &Machine::_notAvailableServiceCardComplete, shared_from_this(),
                conn_ptr,
                rs_ptr
            ) );
    } catch ( std::exception& e ) {
        _exc_ptr = std::current_exception();
    }
}


void Machine::_notAvailableServiceCardComplete(
        cxxdb::ConnectionPtr,
        cxxdb::ResultSetPtr rs_ptr
    )
{
    if ( _exc_ptr != 0 )  return;

    try {
        while ( rs_ptr->fetch() ) {
            string midplane_pos(rs_ptr->columns()["midplanePos"].getString());

            if ( ! _not_available_obj_p )  _not_available_obj_p = &(_obj_val_ptr->get().createObject( "notAvailable" ));

            json::Object *na_obj_p;

            json::Object::iterator i(_not_available_obj_p->find( midplane_pos ));

            if ( i == _not_available_obj_p->end() ) {
                na_obj_p = &(_not_available_obj_p->createObject( midplane_pos ));
            } else {
                na_obj_p = &(i->second.get()->getObject());
            }

            na_obj_p->set( "serviceCardStatus", rs_ptr->columns()["status"].getString() );
        }
    } catch ( std::exception& e ) {
        _exc_ptr = std::current_exception();
    }

}


void Machine::_startNotAvailableSwitches()
{
    if ( _exc_ptr != 0 )  return;

    try {
        static const string SQL(

"SELECT midplanePos, COUNT(*) AS c FROM bgqSwitch WHERE status <> 'A' GROUP BY midplanePos"

            );

        auto conn_ptr(dbConnectionPool::getConnection());

        cxxdb::ResultSetPtr rs_ptr(conn_ptr->query( SQL ));

        _strand.post( bind(
                &Machine::_notAvailableSwitchesComplete, shared_from_this(),
                conn_ptr,
                rs_ptr
            ) );
    } catch ( std::exception& e ) {
        _exc_ptr = std::current_exception();
    }

}


void Machine::_notAvailableSwitchesComplete(
        cxxdb::ConnectionPtr,
        cxxdb::ResultSetPtr rs_ptr
    )
{
    if ( _exc_ptr != 0 )  return;

    try {
        while ( rs_ptr->fetch() ) {
            string midplane_pos(rs_ptr->columns()["midplanePos"].getString());

            if ( ! _not_available_obj_p )  _not_available_obj_p = &(_obj_val_ptr->get().createObject( "notAvailable" ));

            json::Object *na_obj_p;

            json::Object::iterator i(_not_available_obj_p->find( midplane_pos ));

            if ( i == _not_available_obj_p->end() ) {
                na_obj_p = &(_not_available_obj_p->createObject( midplane_pos ));
            } else {
                na_obj_p = &(i->second.get()->getObject());
            }

            na_obj_p->set( "switchCount", rs_ptr->columns()["c"].as<int64_t>() );
        }
    } catch ( std::exception& e ) {
        _exc_ptr = std::current_exception();
    }
}


void Machine::_startNotAvailableNodes()
{
    if ( _exc_ptr != 0 )  return;

    try {
        static const string SQL(

"SELECT midplanePos || '-' || nodeCardPos AS location, c"
" FROM ( SELECT midplanePos, nodeCardPos, COUNT(*) AS c FROM bgqComputeNode"
       " WHERE status <> 'A'"
       " GROUP BY midplanePos, nodeCardPos"
   " ) AS t"

            );

        auto conn_ptr(dbConnectionPool::getConnection());

        cxxdb::ResultSetPtr rs_ptr(conn_ptr->query( SQL ));

        _strand.post( bind(
                &Machine::_notAvailableNodesComplete, shared_from_this(),
                conn_ptr,
                rs_ptr
            ) );
    } catch ( std::exception& e ) {
        _exc_ptr = std::current_exception();
    }

}


void Machine::_notAvailableNodesComplete(
        cxxdb::ConnectionPtr,
        cxxdb::ResultSetPtr rs_ptr
    )
{
    if ( _exc_ptr != 0 )  return;

    try {
        while ( rs_ptr->fetch() ) {
            if ( ! _not_available_obj_p )  _not_available_obj_p = &(_obj_val_ptr->get().createObject( "notAvailable" ));

            const string nodecard_pos(rs_ptr->columns()["location"].getString());

            json::Object *na_obj_p;

            json::Object::iterator i(_not_available_obj_p->find( nodecard_pos ));

            if ( i == _not_available_obj_p->end() ) {
                na_obj_p = &(_not_available_obj_p->createObject( nodecard_pos ));
            } else {
                na_obj_p = &(i->second.get()->getObject());
            }

            na_obj_p->set( "nodeCount", rs_ptr->columns()["c"].as<uint64_t>() );
        }
    } catch ( std::exception& e ) {
        _exc_ptr = std::current_exception();
    }
}


void Machine::_startNotAvailableIoNodes()
{
    if ( _exc_ptr != 0 )  return;

    try {
        static const string SQL(

"SELECT ioPos AS location, COUNT(*) AS c FROM bgqIoNode"
" WHERE status <> 'A'"
" GROUP BY ioPos"

            );

        auto conn_ptr(dbConnectionPool::getConnection());

        cxxdb::ResultSetPtr rs_ptr(conn_ptr->query( SQL ));

        _strand.post( bind(
                &Machine::_notAvailableIoNodesComplete, shared_from_this(),
                conn_ptr,
                rs_ptr
            ) );
    } catch ( std::exception& e ) {
        _exc_ptr = std::current_exception();
    }

}


void Machine::_notAvailableIoNodesComplete(
        cxxdb::ConnectionPtr,
        cxxdb::ResultSetPtr rs_ptr
    )
{
    if ( _exc_ptr != 0 )  return;

    try {
        while ( rs_ptr->fetch() ) {
            if ( ! _not_available_obj_p )  _not_available_obj_p = &(_obj_val_ptr->get().createObject( "notAvailable" ));

            const string io_drawer_pos(rs_ptr->columns()["location"].getString());

            json::Object *na_obj_p;

            json::Object::iterator i(_not_available_obj_p->find( io_drawer_pos ));

            if ( i == _not_available_obj_p->end() ) {
                na_obj_p = &(_not_available_obj_p->createObject( io_drawer_pos ));
            } else {
                na_obj_p = &(i->second.get()->getObject());
            }

            na_obj_p->set( "ioNodeCount", rs_ptr->columns()["c"].as<uint64_t>() );
        }
    } catch ( std::exception& e ) {
        _exc_ptr = std::current_exception();
    }
}


void Machine::_startNotAvailableDcas()
{
    if ( _exc_ptr != 0 )  return;

    try {
        static const string SQL(

"SELECT midplanePos || '-' || nodeCardPos AS location, c"
" FROM ( SELECT midplanePos, nodeCardPos, COUNT(*) AS c FROM bgqNodeCardDCA"
       " WHERE status <> 'A'"
       " GROUP BY midplanePos, nodeCardPos"
   " ) AS t"

            );

        auto conn_ptr(dbConnectionPool::getConnection());

        cxxdb::ResultSetPtr rs_ptr(conn_ptr->query( SQL ));

        _strand.post( bind(
                &Machine::_notAvailableDcasComplete, shared_from_this(),
                conn_ptr,
                rs_ptr
            ) );

    } catch ( std::exception& e ) {
        _exc_ptr = std::current_exception();
    }

}


void Machine::_notAvailableDcasComplete(
        cxxdb::ConnectionPtr,
        cxxdb::ResultSetPtr rs_ptr
    )
{
    if ( _exc_ptr != 0 )  return;

    try {
        while ( rs_ptr->fetch() ) {
            if ( ! _not_available_obj_p )  _not_available_obj_p = &(_obj_val_ptr->get().createObject( "notAvailable" ));

            const string nodecard_pos(rs_ptr->columns()["location"].getString());

            json::Object *na_obj_p;

            json::Object::iterator i(_not_available_obj_p->find( nodecard_pos ));

            if ( i == _not_available_obj_p->end() ) {
                na_obj_p = &(_not_available_obj_p->createObject( nodecard_pos ));
            } else {
                na_obj_p = &(i->second.get()->getObject());
            }

            na_obj_p->set( "DCACount", rs_ptr->columns()["c"].as<uint64_t>() );
        }
    } catch ( std::exception& e ) {
        _exc_ptr = std::current_exception();
    }
}


void Machine::_startNotAvailablePowerModules()
{
    if ( _exc_ptr != 0 )  return;

    try {
        static const string SQL(

"SELECT rackLoc, COUNT(*) AS c"
" FROM ( SELECT LEFT(location,3) AS rackLoc"
       " FROM bgqBulkPowerSupply"
       " WHERE status <> 'A' AND location LIKE 'R%'"
   " ) AS t"
" GROUP BY rackLoc"

            );

        auto conn_ptr(dbConnectionPool::getConnection());

        cxxdb::ResultSetPtr rs_ptr(conn_ptr->query( SQL ));

        _strand.post( bind(
                &Machine::_notAvailablePowerModulesComplete, shared_from_this(),
                conn_ptr,
                rs_ptr
            ) );
    } catch ( std::exception& e ) {
        _exc_ptr = std::current_exception();
    }
}


void Machine::_notAvailablePowerModulesComplete(
        cxxdb::ConnectionPtr,
        cxxdb::ResultSetPtr rs_ptr
    )
{
    if ( _exc_ptr != 0 )  return;

    try {
        while ( rs_ptr->fetch() ) {
            if ( ! _not_available_obj_p )  _not_available_obj_p = &(_obj_val_ptr->get().createObject( "notAvailable" ));

            const string rack_loc(rs_ptr->columns()["rackLoc"].getString());

            json::Object *na_obj_p;

            json::Object::iterator i(_not_available_obj_p->find( rack_loc ));

            if ( i == _not_available_obj_p->end() ) {
                na_obj_p = &(_not_available_obj_p->createObject( rack_loc ));
            } else {
                na_obj_p = &(i->second.get()->getObject());
            }

            na_obj_p->set( "powerModuleCount", rs_ptr->columns()["c"].as<uint64_t>() );
        }
    } catch ( std::exception& e ) {
        _exc_ptr = std::current_exception();
    }
}


} } // namespace bgws::query
