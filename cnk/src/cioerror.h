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
/* (C) Copyright IBM Corp.  2004, 2012                              */
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

#ifndef CIOERROR_H
#define CIOERROR_H

/* ================================================================ */
/*                                                                  */
/* WARNING: DO NOT MODIFY THIS FILE.                                */
/*          This header file is a part of the interface between     */
/*          CIOD and the control system.                            */
/*                                                                  */
/* ================================================================ */

class CioError {

    // Error codes for CioStream protocol failures.

public:

    enum Error {
       CIO_DB_ERROR =            -1,
       CIO_NO_ERROR =             0,
       CIO_LOGIN_STATE,
       CIO_LOAD_STATE,
       CIO_START_STATE,
       CIO_GET_GROUPS,
       CIO_SET_GROUPS,
       CIO_SET_GID,
       CIO_SET_UID,
       CIO_CWD_CHG,
       CIO_MAP_OPEN,
       CIO_MAP_EOF,
       CIO_MAP_BAD_FIELDS,
       CIO_MAP_BAD_X,
       CIO_MAP_BAD_Y,
       CIO_MAP_BAD_Z,
       CIO_MAP_BAD_T,
       CIO_MAP_BAD_CANONICAL,
       CIO_MAP_DUP_CANONICAL,
       CIO_MAP_DUP_LOGICAL,
       CIO_MAP_NODES,
       CIO_APP_PGM_TYPE,
       CIO_APP_AUTH,
       CIO_APP_OPEN,
       CIO_APP_STATUS,
       CIO_APP_HDR_SIZE,
       CIO_APP_READ,
       CIO_APP_SEEK,
       CIO_APP_MAGIC,
       CIO_APP_32BIT,
       CIO_APP_ENDIAN,
       CIO_APP_OSABI, 
       CIO_APP_FILE_TYPE,
       CIO_APP_PPC,
       CIO_APP_NO_CODE,
       CIO_APP_ARGS_SIZE,
       CIO_APP_SECTION_SIZE,
       CIO_APP_ALIGNMENT,
       CIO_APP_NUM_SEGS,
       CIO_TOOL_PIPE,
       CIO_TOOL_FORK,
       CIO_TOOL_ISACTIVE,
       CIO_TOOL_ACCESS,
       CIO_STREAM_INPUT,
       CIO_STREAM_OUTPUT,
       CIO_WRONG_MSG,
       CIO_VERSION_MISMATCH,
       CIO_MODE_MISMATCH,
       CIO_AUTH_FAIL,
       CIO_EXITPGM_FORK,
       CIO_EXITPGM_EXEC,
       CIO_EXITPGM_WAIT,
       CIO_EXITPGM_FAILED,
       CIO_EXITPGM_TIMEDOUT,
       CIO_EXITPGM_SIGNALED,
       CIO_IOPROXY_FORK,
       CIO_WRONG_JOBID,
       CIO_WRONG_JOBMODE,
       CIO_BAD_RUN_COUNT,
       CIO_MSG_TOO_BIG,
       CIO_BAD_TARGET,
       CIO_MSG_SEND,
       CIO_JOB_PIPE,
       CIO_JOB_FORK,
       CIO_JOBSDIR_CREATE,
       CIO_JOBSDIR_OBJECT,
       CIO_LAST_ERROR
    };

    static const char* getErrorText( Error errnum )
    {
	    
       // Notice that there are printf formatting codes embedded in the messages.
       // This allows these strings to be used with *printf with additional information
       // filled in.  In a non-*printf usage the strings will work, but might be a little
       // goofy looking to the end user.
	    
       static const char* errorText[] = {
          "No error",                                                                              // CIO_NO_ERROR
          "For LOGIN message, %u compute nodes are not in the WAITING_TO_LOAD state",              // CIO_LOGIN_STATE
          "For LOAD message, %u compute nodes are not in the WAITING_TO_LOAD state",               // CIO_LOAD_STATE
          "For START message, %u compute nodes are not in the WAITING_TO_START state",             // CIO_START_STATE
          "Error getting supplementary groups: %s",                                                // CIO_GET_GROUPS
          "Error setting supplementary groups: %s",                                                // CIO_SET_GROUPS
          "Error setting group id: %s",                                                            // CIO_SET_GID
          "Error setting user id: %s",                                                             // CIO_SET_UID 
          "Error changing to initial current working directory: %s",                               // CIO_CWD_CHG
          "Error opening the node rank map file: %s",                                              // CIO_MAP_OPEN
          "Unexpected end of file in node rank map file on line %u",                               // CIO_MAP_EOF
          "Missing or invalid fields in node rank map file on line %u",                            // CIO_MAP_BAD_FIELDS 
          "The X coordinate exceeds the physical dimension in node rank map file on line %u",      // CIO_MAP_BAD_X
          "The Y coordinate exceeds the physical dimension in node rank map file on line %u",      // CIO_MAP_BAD_Y
          "The Z coordinate exceeds the physical dimension in node rank map file on line %u",      // CIO_MAP_BAD_Z
          "The T coordinate exceeds the physical dimension in node rank map file on line %u",      // CIO_MAP_BAD_T
          "Canonical rank exceeds partition size in node rank map file on line %u",                // CIO_MAP_BAD_CANONICAL
          "There is a duplicate canonical-rank to logical-rank in node rank map file on line %u",  // CIO_MAP_DUP_CANONICAL
          "There is a duplicate logical-rank to canonical-rank in node rank map file on line %u",  // CIO_MAP_DUP_LOGICAL
          "There are not enough lines for %u nodes in the node rank map file",                     // CIO_MAP_NODES
          "The ELF program type in executable file is invalid",                                    // CIO_APP_PGM_TYPE
          "No execute authority to executable file",                                               // CIO_APP_AUTH
          "Error opening executable file: %s",                                                     // CIO_APP_OPEN
          "Error getting status for executable file: %s",                                          // CIO_APP_STATUS
          "Size of executable file is smaller than ELF header",                                    // CIO_APP_HDR_SIZE
          "Error reading from executable file: %s",                                                // CIO_APP_READ
          "Error seeking in executable file: %s",                                                // CIO_APP_SEEK
          "Magic value in ELF header of executable file is invalid",                               // CIO_APP_MAGIC
          "Executable file is not a 32-bit ELF file",                                              // CIO_APP_32BIT
          "Data encoding of executable file is not big Endian",                                    // CIO_APP_ENDIAN
          "OSABI value of executable file is not UNIX System V",                                   // CIO_APP_OSABI 
          "File type of executable file is not an ELF executable",                                 // CIO_APP_FILE_TYPE
          "Architecture of executable file is not PowerPC",                                        // CIO_APP_PPC
          "There are no code sections in executable file",                                         // CIO_APP_NO_CODE
          "Argument list size %u is too big",                                                      // CIO_APP_ARGS_SIZE
          "Code section with %d bytes is too big to fit in available memory",                      // CIO_APP_SECTION_SIZE
          "Program segment is not 1MB aligned",                                                    // CIO_APP_ALIGNMENT
          "Number of program segments %d is too big",                                              // CIO_APP_NUM_SEGS
          "Error creating pipes for external tool process: %s",                                    // CIO_TOOL_PIPE
          "Error creating external tool process: %s",                                              // CIO_TOOL_FORK
          "External tool is already active",                                                       // CIO_TOOL_ISACTIVE
          "Error accessing external tool executable: %s",                                          // CIO_TOOL_ACCESS
          "Input error on stream: %s",                                                             // CIO_STREAM_INPUT
          "Output error on stream: %s",                                                            // CIO_STREAM_OUTPUT
          "Message %u is not the expected message",                                                // CIO_WRONG_MSG
          "CioStream protocol version %u does not match",                                          // CIO_VERSION_MISMATCH
          "Job mode %u is invalid",                                                                // CIO_MODE_MISMATCH
          "Authorization failure",                                                                 // CIO_AUTH_FAIL
          "Error creating exit program process: %s",                                               // CIO_EXITPGM_FORK
          "Error starting exit program executable: %s",                                            // CIO_EXITPGM_EXEC
          "Error waiting for exit program: %s",                                                    // CIO_EXITPGM_WAIT
          "Exit program failed with exit status %d",                                               // CIO_EXITPGM_FAILED
          "Exit program timed out after %d seconds",                                               // CIO_EXITPGM_TIMEDOUT
          "Exit program ended by signal %d",                                                       // CIO_EXITPGM_SIGNALED
          "Error starting ioproxy process: %s",                                                    // CIO_IOPROXY_FORK
          "Error reconnecting to job, job %d is currently running",                                // CIO_WRONG_JOBID
          "Error reconnecting to job, current job is running in mode %u",                          // CIO_WRONG_JOBMODE
          "Error reconnecting to job, only %d processors are currently running",                   // CIO_BAD_RUN_COUNT
          "Message with %d bytes is too big for internal buffer",                                  // CIO_MSG_TOO_BIG  
          "Target control daemon %u is invalid",                                                   // CIO_BAD_TARGET   
          "Error sending message to secondary daemon, %s",                                         // CIO_MSG_SEND     
          "Error creating pipes for job process: %s",                                              // CIO_JOB_PIPE
          "Error creating job process: %s",                                                        // CIO_JOB_FORK
          "Error creating /jobs subdirectory: %s",                                                 // CIO_JOBSDIR_CREATE
          "Error creating object in /jobs subdirectory: %s",                                       // CIO_JOBSDIR_OBJECT
       };

       if ( (errnum > CIO_DB_ERROR) && (errnum < CIO_LAST_ERROR) ) {
          return errorText[ errnum ];
       }

       return "UNKNOWN";
    }
};

#endif // CIOERROR_H
