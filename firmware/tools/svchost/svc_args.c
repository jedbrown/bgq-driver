/* begin_generated_IBM_copyright_prolog                             */
/*                                                                  */
/* This is an automatically generated copyright prolog.             */
/* After initializing,  DO NOT MODIFY OR MOVE                       */
/*  --------------------------------------------------------------- */
/*                                                                  */
/* Licensed Materials - Property of IBM                             */
/* Blue Gene/Q                                                      */
/* (C) Copyright IBM Corp.  2010, 2012                              */
/* US Government Users Restricted Rights - Use, duplication or      */
/*   disclosure restricted by GSA ADP Schedule Contract with IBM    */
/*   Corp.                                                          */
/*                                                                  */
/* This software is available to you under the Eclipse Public       */
/* License (EPL).                                                   */
/*                                                                  */
/*  --------------------------------------------------------------- */
/*                                                                  */
/* end_generated_IBM_copyright_prolog                               */


#include "svc_host.h"
#include <stdio.h>
#include <strings.h>
#include <ctype.h>


void svc_init_file(SvcFileCache* svc, FILE* fp) {
    svc->fp = fp;
    svc->buffp = svc->buffer;
    svc->buffer[0] = 0;
};


#define TRACE(x) printf x

char* svc_next_arg(SvcFileCache* svc) {

    while (1) {

      // If the buffer is exhausted, refresh it:

      if ( svc->buffp[0] == 0 ) {
	  if ( fgets(svc->buffer, sizeof(svc->buffer), svc->fp) == 0 ) {
	      return 0;
	  }
	  svc->buffp = svc->buffer;
      }


      // Skip over any white space:
      
      while ( isspace(svc->buffp[0]) ) 
	  (svc->buffp)++;

      // End of line ?
      if ( svc->buffp[0] == 0 )
	  continue; 

      // Line comment?
      if ( svc->buffp[0] == '#' ) {
	  svc->buffp[0] = 0; 
	  continue;
      }

      char* arg = svc->buffp;

      (svc->buffp)++;
    
      while ( ! isspace(svc->buffp[0]) && ( svc->buffp[0] != 0 ) )
	  (svc->buffp)++;

      if ( isspace(svc->buffp[0]) ) {
	  svc->buffp[0] = 0;
	  (svc->buffp)++;
      }

      return arg;
  }
}
 

char *svc_next_file_arg(FILE* fp, char* buff, int buffsize) {

  while (1) {
    
    /*
     * Outline:
     *   1) Read the next line in the file
     *   2) Advance to the first non-white space character
     *   3) Advance the tail pointer to the next white-space character (or EOL)
     */

    if ( fgets(buff, buffsize, fp) == 0 ) {
      return 0;
    }

    char* p = buff;
    
    while ( isspace(*p) ) p++; 

    if ( *p == 0 ) { // blank line ... go to the next line
      continue;
    }

    if ( *p == '#' ) { // comment ... go to the next line
      continue;
    }

    char* q = p+1;

    while ( ! isspace(*q) && (*q != 0) ) q++;

    *q = 0;

    return p;

  }
}

