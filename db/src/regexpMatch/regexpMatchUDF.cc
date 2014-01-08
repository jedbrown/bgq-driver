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

/*

To have your sandbox version write a log file for debug:
CXXFLAGS=-DWRITE_LOG make

The log statements are written to "/tmp/regexpMatchUDF.log"


SQL to define the function to DB2, replace <hlcs_lib_install_dir> with the library name.

CREATE FUNCTION regexpMatch( in1 VARCHAR(256), in2 VARCHAR(256) )
  RETURNS INTEGER
  LANGUAGE C
  PARAMETER STYLE SQL
  NO SQL
  SCRATCHPAD
  FINAL CALL
  DISALLOW PARALLEL
  FENCED THREADSAFE
  DETERMINISTIC
  RETURNS NULL ON NULL INPUT
  NO EXTERNAL ACTION
  EXTERNAL NAME '<hlcs_lib_install_dir>/libregexpMatchUDF.so!regexpMatchUDF'


DROP FUNCTION regexpMatch


 */

#include <sqludf.h>

#include <regex.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

using namespace std;


struct ScratchpadContents {
    bool initialized;
    char *regex_str;
    regex_t regex;

    void cleanup() {
        free( regex_str );
        regex_str = NULL;

        regfree( &regex );

        initialized = false;
    }
};


#ifdef WRITE_LOG
#include <fstream>

#define INIT_LOG \
    ofstream log_file( "/tmp/regexpMatchUDF.log", ios_base::app )

#define LOG( expr ) \
    log_file << expr << endl

#else

#define INIT_LOG

#define LOG( expr )

#endif


extern "C"
SQL_API_RC SQL_API_FN regexpMatchUDF(
        SQLUDF_VARCHAR *regex_str,
        SQLUDF_VARCHAR *match_str,
        SQLUDF_INTEGER *result_out,
        SQLUDF_NULLIND *regex_str_nullind,
        SQLUDF_NULLIND *match_str_nullind,
        SQLUDF_NULLIND *result_nullind_out,
        SQLUDF_TRAIL_ARGS_ALL
    )
{
    INIT_LOG;

    int rc;

    ScratchpadContents *scratchpad_p(reinterpret_cast<ScratchpadContents*>(SQLUDF_SCRAT->data));

    LOG( "Called with:\n"
         "  regex_str=" << (SQLUDF_NULL( regex_str_nullind ) ? "NULL" : string("'") + regex_str + "'") << "\n"
         "  match_str=" << (SQLUDF_NULL( match_str_nullind ) ? "NULL" : string("'") + match_str + "'") << "\n"
         "  SQLUDF_CALLT=" <<
             (SQLUDF_CALLT == SQLUDF_FIRST_CALL ? "FIRST" :
              SQLUDF_CALLT == SQLUDF_NORMAL_CALL ? "NORMAL" :
              SQLUDF_CALLT == SQLUDF_FINAL_CALL ? "FINAL" : "UNEXPECTED" ) << "\n"
         "  sizeof(ScratchpadContents)=" << sizeof ( ScratchpadContents ) << " (< " << SQLUDF_SCRATCHPAD_LEN << ")"
       );

    if ( SQLUDF_CALLT == SQLUDF_FINAL_CALL ) {
        if ( scratchpad_p->initialized ) {
            scratchpad_p->cleanup();
        }
        return 0;
    }

    // If the regex_str or match_str is NULL, the result is NULL.
    // Note that this should never happen since should be "RETURNS NULL ON NULL INPUT"
    if ( SQLUDF_NULL( regex_str_nullind ) ||
         SQLUDF_NULL( match_str_nullind ) )
    {
        LOG( "Returning NULL argument was NULL." );
        *result_nullind_out = -1;
        return 0;
    }

    if ( SQLUDF_CALLT == SQLUDF_FIRST_CALL ) {
        scratchpad_p->initialized = false;
        LOG( "Compiling because first call." );
    } else if ( SQLUDF_CALLT == SQLUDF_NORMAL_CALL ) {
        if ( ! scratchpad_p->initialized ) {
            LOG( "Compiling because scratchpad wasn't initialized." );
        } else if ( strcmp( scratchpad_p->regex_str, regex_str ) != 0 ) {
            LOG( "Compiling because scratchpad contained different regex_str='" << scratchpad_p->regex_str << "'" );
            scratchpad_p->cleanup();
        }
    }

    if ( ! scratchpad_p->initialized ) {
        rc = regcomp( &scratchpad_p->regex, regex_str, REG_EXTENDED | REG_NOSUB );

        if ( rc != 0 ) {
            LOG( "Error compiling, setting sqlstate to 601." );

            strcpy( sqludf_sqlstate, SQLUDF_STATE_ERR );
            sqludf_sqlstate[2] = '6';
            sqludf_sqlstate[3] = '0';
            sqludf_sqlstate[4] = '1';
            regerror( rc, &scratchpad_p->regex, sqludf_msgtext, SQLUDF_MSGTEXT_LEN+1 );

            *result_nullind_out = -1;
            return 0;
        }

        scratchpad_p->regex_str = strdup( regex_str );

        scratchpad_p->initialized = true;
    }

    rc = regexec( &scratchpad_p->regex, match_str, 0, 0, 0 );

    if ( rc == 0 ) {
        LOG( "Result is success." );
        *result_nullind_out = 0;
        *result_out = 1;
    } else if ( rc == REG_NOMATCH ) {
        LOG( "Result is NO MATCH." );
        *result_nullind_out = 0;
        *result_out = 0;
    } else {
        LOG( "Error matching, setting sqlstate to 602." );
        strcpy( sqludf_sqlstate, SQLUDF_STATE_ERR );
        sqludf_sqlstate[2] = '6';
        sqludf_sqlstate[3] = '0';
        sqludf_sqlstate[4] = '2';
        regerror( rc, &scratchpad_p->regex, sqludf_msgtext, SQLUDF_MSGTEXT_LEN+1 );

        *result_nullind_out = -1;
        scratchpad_p->cleanup();
    }

    return 0;
}
