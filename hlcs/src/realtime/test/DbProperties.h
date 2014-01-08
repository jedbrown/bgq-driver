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

#ifndef DB_PROPERTIES_H
#define DB_PROPERTIES_H

#include <map>
#include <string>


class DbProperties
{
public:
   static const std::string DATABASE_NAME_PROP_NAME;
   static const std::string DATABASE_USER_PROP_NAME;
   static const std::string DATABASE_PASSWORD_PROP_NAME;
   static const std::string DATABASE_SCHEMA_PROP_NAME;
   static const std::string REALTIME_PORT_PROP_NAME;

   explicit DbProperties( const std::string* filename );

   bool isValid() const { return _is_valid; }

   const std::string* get( const std::string& prop_name ) const;

   template<class T> bool get( const std::string& prop_name, T* val_out ) const;

private:
   static const char COMMENT_CHARACTER;
   static const char SEPARATOR_CHARACTER;
   static const std::string DEFAULT_FILENAME;

   bool _is_valid;

   typedef std::map<std::string,std::string> props_t;
   props_t _props;

   void _parseOneLine( std::string& line );
   void _removePadding( std::string& s );
}; // class DbProperties

#endif
