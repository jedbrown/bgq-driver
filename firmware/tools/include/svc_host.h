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


#ifndef _SVC_HOST_H
#define _SVC_HOST_H

#include <stdio.h>
#include <firmware/include/personality.h>

#ifdef __cplusplus
extern "C" {
#endif

  typedef enum {
    _svc_error_none = 0,
    _svc_error_help,
    _svc_error_usage,
    _svc_error_warning,
    _svc_warning_env_ignored,
    _svc_warning_env_trunc,
    _svc_warning_kargs_ignored,
    _svc_error_invalid_elf,
    _svc_error_elf_not_found,
    _svc_error_elf_create_fail,
    _svc_error_env_not_found,
    _svc_error_mmap_failed,
    _svc_error_AppState_Size
  } _BGP_Svc_Host_ErrCodes;

    typedef struct _SvcFileCache {
	FILE* fp;
	char buffer[512];
	char* buffp;
    } SvcFileCache;

    void  svc_init_file( SvcFileCache* svc, FILE* fp);
    char* svc_next_arg( SvcFileCache* svc );

  //! @brief Walks the file (from the current file position) to locate the next 
  //!        svc_host argument.  Line comments start with a '#'.  It is assumed
  //!        that there is at most one argument per line.

  char* svc_next_file_arg(FILE *file_p, char* buff, int buffsize);

  //! @brief Parses the string as a single svc_host option and applies it to
  //!        the personality structure (e.g. svc_set_pers( "+TraceConfig", pers)).

  int svc_set_pers( char *p, Personality_t *pers);

  //! @brief Sends a formatted view of the personality to stdout.
  void svc_ShowPersonality( Personality_t *p );

#ifdef __cplusplus
}
#endif

#endif
