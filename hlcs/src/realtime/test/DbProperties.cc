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
/* (C) Copyright IBM Corp.  2007, 2011                              */
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


#include "DbProperties.h"

#include <iostream>
#include <fstream>
#include <sstream>


using namespace std;


const string DbProperties::DATABASE_NAME_PROP_NAME("database_name");
const string DbProperties::DATABASE_USER_PROP_NAME("database_user");
const string DbProperties::DATABASE_PASSWORD_PROP_NAME("database_password");
const string DbProperties::DATABASE_SCHEMA_PROP_NAME("database_schema_name");
const string DbProperties::REALTIME_PORT_PROP_NAME("realtime_port");

const char DbProperties::COMMENT_CHARACTER('#');
const char DbProperties::SEPARATOR_CHARACTER('=');

const string DbProperties::DEFAULT_FILENAME("db.properties");


DbProperties::DbProperties(
      const std::string* filename )
   : _is_valid( false )
{
   if ( 0 == filename ) {
      filename = &DEFAULT_FILENAME;
   }

   ifstream ifile(filename->c_str());

   if ( ! ifile ) {
      return;
   }

   _is_valid = true;

   string line;

   while ( getline( ifile, line ) ) {
      _parseOneLine( line );
   }
} // DbProperties()

const string* DbProperties::get( const std::string& prop_name ) const
{
   props_t::const_iterator i(_props.find(prop_name));
   if ( _props.end() == i ) {
      return 0;
   }
   return &(i->second);
} // DbProperties::get()

template<class T> bool DbProperties::get( const std::string& prop_name, T* val_out ) const
{
   const string* str_val(get( prop_name ));
   if ( 0 == str_val ) {
      return false;
   }

   istringstream iss( *str_val );
   iss >> *val_out;

   return bool(iss);
} // DbProperties::get()

void DbProperties::_parseOneLine( std::string& line )
{
   // Remove everything after the first comment character (#)
   string::size_type i(line.find( COMMENT_CHARACTER ));
   if ( i != string::npos ) {
      line.erase( i );
   }

   // Split the line at =
   i = line.find( SEPARATOR_CHARACTER );
   if ( i == string::npos ) {
      // No equals so not a property value.
      return;
   }

   string prop_name(line.substr( 0, i ));
   string prop_val(line.substr( i + 1 ));

   _removePadding(prop_name);
   _removePadding(prop_val);

   _props[prop_name] = prop_val;
} // DbProperties::_parseOneLine()

void DbProperties::_removePadding( std::string& s )
{
   const char ws_chars[] = " \t";
   string::size_type start(s.find_first_not_of( ws_chars ));
   if ( start == string::npos ) {
      // It's all whitespace!
      s = "";
      return;
   }
   s.erase( 0, start );

   string::size_type end(s.find_last_not_of( ws_chars ));
   s.erase( end + 1 );
} // DbProperties::_removePadding()

template bool DbProperties::get( const std::string& prop_name, int* val_out ) const;
template bool DbProperties::get( const std::string& prop_name, unsigned int* val_out ) const;
template bool DbProperties::get( const std::string& prop_name, short* val_out ) const;
template bool DbProperties::get( const std::string& prop_name, unsigned short* val_out ) const;
template bool DbProperties::get( const std::string& prop_name, long* val_out ) const;
template bool DbProperties::get( const std::string& prop_name, unsigned long* val_out ) const;
