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

#include <dlfcn.h>
#include <getopt.h>
#include <sql.h>
#include <sqludf.h>
#include <syslog.h>
#include <unistd.h>

#include <cstring>
#include <iostream>
#include <string>
#include <vector>

#include "rtdbview.h"

using namespace std;

class command_handler
{
public:
   virtual string get_command_str() const =0;
   virtual string get_args_str() const =0;

   virtual int handle( rtdbview_t &rtdbview ) const =0;
};

class block_added_handler : public command_handler
{
public:
   block_added_handler() {}

   virtual string get_command_str() const {  return string( "block_added" ); }
   virtual string get_args_str() const { return string( "blockId status" ); }

   virtual int handle( rtdbview_t &rtdbview ) const {
      string blockId;
      char status;
      cin >> blockId >> status;

      int rc = rtdbview.block_added( &blockId, status );
      return rc;
   }
};

class block_state_change_handler : public command_handler
{
public:
   block_state_change_handler() {}

   virtual string get_command_str() const {  return string( "block_state_change" ); }
   virtual string get_args_str() const { return string( "blockId newStatus oldStatus" ); }

   virtual int handle( rtdbview_t &rtdbview ) const {
      string blockId;
      char new_status;
      char old_status;
      cin >> blockId >> new_status >> old_status;

      int rc = rtdbview.block_state_change( &blockId, new_status, old_status );
      return rc;
   }
};

class block_deleted_handler : public command_handler
{
public:
   block_deleted_handler() {}

   virtual string get_command_str() const {  return string( "block_deleted" ); }
   virtual string get_args_str() const { return string( "blockId" ); }

   virtual int handle( rtdbview_t &rtdbview ) const {
      string blockId;
      cin >> blockId;

      int rc = rtdbview.block_deleted( &blockId );
      return rc;
   }
};

class job_added_handler : public command_handler
{
public:
   job_added_handler() {}

   virtual string get_command_str() const {  return string( "job_added" ); }
   virtual string get_args_str() const { return string( "jobid blockId status" ); }

   virtual int handle( rtdbview_t &rtdbview ) const {
      int jobid;
      string blockId;
      char status;

      cin >> jobid >> blockId >> status;

      int rc = rtdbview.job_added( jobid, &blockId, status );
      return rc;
   }
};

class job_state_change_handler : public command_handler
{
public:
   job_state_change_handler() {}

   virtual string get_command_str() const {  return string( "job_state_change" ); }
   virtual string get_args_str() const { return string( "jobid blockId newStatus oldStatus" ); }

   virtual int handle( rtdbview_t &rtdbview ) const {
      int jobid;
      string blockId;
      char new_status;
      char old_status;

      cin >> jobid >> blockId >> new_status >> old_status;

      int rc = rtdbview.job_state_change( jobid, &blockId, new_status, old_status );
      return rc;
   }
};

class job_deleted_handler : public command_handler
{
public:
   job_deleted_handler() {}

   virtual string get_command_str() const {  return string( "job_deleted" ); }
   virtual string get_args_str() const { return string( "jobid blockId" ); }

   virtual int handle( rtdbview_t &rtdbview ) const {
      int jobid;
      string blockId;

      cin >> jobid >> blockId;

      int rc = rtdbview.job_deleted( jobid, &blockId );
      return rc;
   }
};

class bp_state_change_handler : public command_handler
{
public:
   bp_state_change_handler() {}

   virtual string get_command_str() const {  return string( "bp_state_change" ); }
   virtual string get_args_str() const { return string( "bpPosition newStatus oldStatus" ); }

   virtual int handle( rtdbview_t &rtdbview ) const {
      string bp_position;
      char new_status;
      char old_status;

      cin >> bp_position >> new_status >> old_status;

      int rc = rtdbview.bp_state_change( &bp_position, new_status, old_status );
      return rc;
   }
};

class nodecard_state_change_handler : public command_handler
{
public:
   nodecard_state_change_handler() {}

   virtual string get_command_str() const {  return string( "nodecard_state_change" ); }
   virtual string get_args_str() const { return string( "nodecardPosition bpPosition newStatus oldStatus" ); }

   virtual int handle( rtdbview_t &rtdbview ) const {
      string nodecard_position;
      string bp_position;
      char new_status;
      char old_status;

      cin >> nodecard_position >> bp_position >> new_status >> old_status;

      int rc = rtdbview.nodecard_state_change( &nodecard_position, &bp_position, new_status, old_status );
      return rc;
   }
};

class modloop_handler : public command_handler
{
public:
    modloop_handler() {}

    virtual string get_command_str() const { return string( "modloop" ); }
    virtual string get_args_str() const { return string( "[count]" ); }

    virtual int handle( rtdbview_t &rtdbview ) const {
        int rc( 0 );
        string blockId( "AAA" );

        for ( int i = 0 ; (0 == rc) && (i < 500) ; ++i ) {
           rc = rtdbview.block_state_change( &blockId, 'I', 'F' );
       }
       return rc;
    }
};


static string expand_file_path( const string &path_or_file )
{
   if ( path_or_file.length() == 0 ) {
      return path_or_file;
   }

   if ( path_or_file[0] == '/' ) {
      return path_or_file;
   }

   char cwd_buf[1024];
   getcwd( cwd_buf, sizeof ( cwd_buf ) );

   string ret( string( cwd_buf ) + "/" + path_or_file );
   return ret;
} // expand_file_path()


int main( int argc, char *argv[] )
{
  string db_properties_filename_val;
  string log_filename_val;
  int log_level;

  string *db_properties_filename_param( 0 );
  string *log_filename_param( 0 );
  int *log_level_param( 0 );

  int db_properties_opt_ind(0);
  const char *DB_PROPERTIES_FILENAME_OPT_NAME("db-properties");

  int log_filename_opt_ind(0);
  const char *LOG_FILENAME_OPT_NAME("log-filename");

  int log_level_opt_ind(0);
  const char *LOG_LEVEL_OPT_NAME("log-level");

  const char *HELP_OPT_NAME("help");
  const int HELP_OPT_VAL('h');

  const struct option OPTS[] = {
    { DB_PROPERTIES_FILENAME_OPT_NAME, required_argument, &db_properties_opt_ind, 1 },
    { LOG_FILENAME_OPT_NAME, required_argument, &log_filename_opt_ind, 1 },
    { LOG_LEVEL_OPT_NAME, required_argument, &log_level_opt_ind, 1 },
    { HELP_OPT_NAME, no_argument, NULL, HELP_OPT_VAL }
  };

  bool done_processing(false);
  while ( ! done_processing ) {
    int getopt_rc(getopt_long_only( argc, argv, "h", OPTS, NULL ));

    switch ( getopt_rc ) {
      case -1:
        done_processing = true;
        break;
      case '?':
        exit(1);
        break;
      case HELP_OPT_VAL:
        cout << "Arguments:"
            " -" << DB_PROPERTIES_FILENAME_OPT_NAME << "=FILENAME"
            " -" << LOG_FILENAME_OPT_NAME << "=FILENAME"
            " -" << LOG_LEVEL_OPT_NAME << "=LEVEL"
            " -" << HELP_OPT_NAME << endl;
        exit(0);
        break;
      case 0: {
          if ( 1 == db_properties_opt_ind ) {
            db_properties_opt_ind = 0;
            db_properties_filename_val = optarg;
            db_properties_filename_param = &db_properties_filename_val;
          } else if ( 1 == log_filename_opt_ind ) {
            log_filename_opt_ind = 0;
            log_filename_val = optarg;
            log_filename_param = &log_filename_val;
          } else if ( 1 == log_level_opt_ind ) {
            log_level_opt_ind = 0;
            log_level = atoi( optarg );
            log_level_param = &log_level;
          }
        }
        break;
    }
  }

  cout << "db_properties_filename = '" << db_properties_filename_val << "'\n";
  cout << "log_filename = '" +  log_filename_val << "'\n";
  cout << "log_level=" << log_level << "\n";

  try {
    rtdbview_t rtdbview( string( "./bg_realtime.so" ) );
    rtdbview.init( db_properties_filename_param, log_filename_param, log_level_param );

    typedef vector<command_handler*> command_handlers_t;
    command_handlers_t command_handlers;

    block_added_handler pah; command_handlers.push_back( &pah );
    block_state_change_handler psch; command_handlers.push_back( &psch );
    block_deleted_handler pdh; command_handlers.push_back( &pdh );
    job_added_handler jah; command_handlers.push_back( &jah );
    job_state_change_handler jsch; command_handlers.push_back( &jsch );
    job_deleted_handler jdh; command_handlers.push_back( &jdh );
    bp_state_change_handler bsch; command_handlers.push_back( &bsch );
    nodecard_state_change_handler nsch; command_handlers.push_back( &nsch );

    modloop_handler mlh; command_handlers.push_back( &mlh );

    string inputstr;
    while ( cout << "test_rtdbview $ " << flush, cin >> inputstr, !cin.eof() ) {
      if ( inputstr == "help" ) {
        cout << " * Commands:\n";
        for ( command_handlers_t::iterator i = command_handlers.begin();
              i != command_handlers.end();
              ++i ) {
                command_handler* ch = *i;
                cout << " * " << ch->get_command_str() << " " << ch->get_args_str() << "\n";
              }
              continue;
      }
      for ( command_handlers_t::iterator i = command_handlers.begin();
            i != command_handlers.end();
            ++i ) {
              command_handler* ch = *i;
              if ( ch->get_command_str() == inputstr ) {
                ch->handle( rtdbview );
              }
            }
    }
    cout << endl;
  } catch ( symbol_not_found_t &s )
  {
    cerr << "Symbol not found: " << s.get_symname() << endl;
  }
} // main()

// End of test_rtdbview.C
