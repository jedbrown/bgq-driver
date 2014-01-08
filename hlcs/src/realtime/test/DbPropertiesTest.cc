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
/* (C) Copyright IBM Corp.  2011, 2011                              */
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


using namespace std;


int main( int argc, char *argv[] ) {
   std::string filename_val;
   std::string *filename_p(0);

   if ( argc > 1 ) {
      filename_val = argv[1];
      filename_p = &filename_val;
   }

   const DbProperties db_properties( filename_p );
   if ( ! db_properties.isValid() ) {
      cerr << "The db properties file is not valid.\n";
      return 1;
   }

   const std::string *prop_names[] = {
      &DbProperties::DATABASE_NAME_PROP_NAME,
      &DbProperties::DATABASE_USER_PROP_NAME,
      &DbProperties::DATABASE_PASSWORD_PROP_NAME,
      &DbProperties::DATABASE_SCHEMA_PROP_NAME,
      &DbProperties::REALTIME_PORT_PROP_NAME };

   for ( int i(0) ; i < int(sizeof ( prop_names ) / sizeof ( prop_names[0] )) ; ++i ) {
      const std::string *prop_name( prop_names[i] );
      const std::string *val(db_properties.get(*prop_name));
      const std::string display_val(0 == val ? "null" :
                                               (std::string("'") + *val + "'"));
      cout << *prop_name << ": " << display_val << endl;
   }

   unsigned short realtime_port;
   if ( db_properties.get( DbProperties::REALTIME_PORT_PROP_NAME, &realtime_port ) ) {
      cout << "Got realtime port " << realtime_port << endl;
   } else {
      cout << "Failed to get realtime port.\n";
   }

   int db_name;
   if ( db_properties.get( DbProperties::DATABASE_NAME_PROP_NAME, &db_name ) ) {
      cout << "Got db_name " << db_name << endl;
   } else {
      cout << "Failed to get db_name.\n";
   }
} // main()
