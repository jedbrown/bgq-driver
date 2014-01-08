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


#include "RefreshBgwsServerConfiguration.hpp"

#include "chiron-json/json.hpp"

#include <utility/include/Log.h>

#include <boost/lexical_cast.hpp>
#include <boost/throw_exception.hpp>

#include <iostream>
#include <string>


using boost::lexical_cast;

using std::string;


LOG_DECLARE_FILE( "bgws.common" );


namespace bgws {
namespace common {


namespace statics {

static const std::string OPERATION_PROPERTY_NAME( "operation" );

static const std::string PROPERTIES_FILENAME_PROPERTY_NAME( "propertiesFile" );

}


RefreshBgwsServerConfiguration::PathNotComplete::PathNotComplete( const boost::filesystem::path& path )
    : std::invalid_argument( "the path is not a complete path. The path is '" + lexical_cast<string>( path ) + "'" ),
      _path(path)
{
    // Nothing to do.
}


RefreshBgwsServerConfiguration::NotObjectError::NotObjectError()
    : std::runtime_error( "the json document is not an object" )
{
    // Nothing to do.
}


RefreshBgwsServerConfiguration::NoOperationError::NoOperationError()
    : std::runtime_error( "no operation provided for BGWS server operation" )
{
    // Nothing to do.
}


RefreshBgwsServerConfiguration::OperationNotStringError::OperationNotStringError()
    : std::runtime_error( "operation is not a string" )
{
    // Nothing to do.
}


RefreshBgwsServerConfiguration::UnexpectedOperationError::UnexpectedOperationError( const std::string& operation )
    : std::runtime_error( string() + "unexpected operation for BGWS server. The operation is '" + operation + "'" ),
      _operation(operation)
{
    // Nothing to do.
}


const std::string RefreshBgwsServerConfiguration::OPERATION_NAME( "refreshConfig" );

const RefreshBgwsServerConfiguration RefreshBgwsServerConfiguration::RereadCurrent = RefreshBgwsServerConfiguration( RefreshBgwsServerConfiguration::Type::RereadCurrent );
const RefreshBgwsServerConfiguration RefreshBgwsServerConfiguration::ReadDefault = RefreshBgwsServerConfiguration( RefreshBgwsServerConfiguration::Type::ReadDefault );

RefreshBgwsServerConfiguration RefreshBgwsServerConfiguration::ReadNew( const boost::filesystem::path& path )
{
    return RefreshBgwsServerConfiguration( Type::ReadNew, path );
}


RefreshBgwsServerConfiguration::RefreshBgwsServerConfiguration(
        const json::Value& json_value
    )
{
    if ( ! json_value.isObject() ) {
        BOOST_THROW_EXCEPTION( NotObjectError() );
    }

    const json::Object &obj(json_value.getObject());

    if ( ! obj.contains( statics::OPERATION_PROPERTY_NAME ) ) {
        BOOST_THROW_EXCEPTION( NoOperationError() );
    }

    if ( ! obj.isString( statics::OPERATION_PROPERTY_NAME ) ) {
        BOOST_THROW_EXCEPTION( OperationNotStringError() );
    }

    if ( obj.getString( statics::OPERATION_PROPERTY_NAME ) != OPERATION_NAME ) {
        BOOST_THROW_EXCEPTION( UnexpectedOperationError( obj.getString( statics::OPERATION_PROPERTY_NAME ) ) );
    }

    if ( obj.contains( statics::PROPERTIES_FILENAME_PROPERTY_NAME ) ) {

        if ( obj.isString( statics::PROPERTIES_FILENAME_PROPERTY_NAME ) ) {

            const std::string &filename_str(obj.getString( statics::PROPERTIES_FILENAME_PROPERTY_NAME ));
            if ( filename_str == "" ) {
                _type = Type::ReadDefault;
            } else {
                _type = Type::ReadNew;
                _path = filename_str;

                if ( ! _path.is_complete() ) {
                    BOOST_THROW_EXCEPTION( PathNotComplete( _path ) );
                }
            }

        } else {

            LOG_WARN_MSG( "refresh BGWS server configuration, JSON contains " << obj.contains( statics::PROPERTIES_FILENAME_PROPERTY_NAME ) << " but is not a string, ignoring and will reread current configuration file." );

            _type = Type::RereadCurrent;
        }

    } else {
        _type = Type::RereadCurrent;
    }
}


RefreshBgwsServerConfiguration::RefreshBgwsServerConfiguration(
        Type type,
        const boost::filesystem::path& path
    ) :
        _type(type),
        _path(path)
{
    if ( _path != boost::filesystem::path() && ! _path.is_complete() ) {
        BOOST_THROW_EXCEPTION( PathNotComplete( path ) );
    }
}


const boost::filesystem::path& RefreshBgwsServerConfiguration::getFilename() const
{
    if ( _type != Type::ReadNew ) {
        BOOST_THROW_EXCEPTION( std::logic_error( "cannot call getFilename on RefreshBgwsServerConfiguration that is not ReadNew" ) );
    }
    return _path;
}


json::ValuePtr RefreshBgwsServerConfiguration::toJson() const
{
    json::ObjectValuePtr obj_val_ptr(json::Object::create());
    json::Object &obj(obj_val_ptr->get());
    obj.set( statics::OPERATION_PROPERTY_NAME, OPERATION_NAME );

    if ( _type == Type::ReadDefault ) {
        obj.set( statics::PROPERTIES_FILENAME_PROPERTY_NAME, "" );
    } else if ( _type == Type::ReadNew ) {
        obj.set( statics::PROPERTIES_FILENAME_PROPERTY_NAME, _path.string() );
    }

    return obj_val_ptr;
}


std::ostream& operator<<( std::ostream& os, RefreshBgwsServerConfiguration::Type type )
{
    os << (type == RefreshBgwsServerConfiguration::Type::RereadCurrent ? "RereadCurrent" :
           type == RefreshBgwsServerConfiguration::Type::ReadDefault ? "ReadDefault" :
           type == RefreshBgwsServerConfiguration::Type::ReadNew ? "ReadNew" :
           "!!!FORGOT_TO_ADD!!!"
        );
    return os;
}


} } // namespace bgws::common
