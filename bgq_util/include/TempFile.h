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
/* (C) Copyright IBM Corp.  2004, 2011                              */
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

#ifndef _TEMPFILE_H
#define _TEMPFILE_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

//
// This class creates a temporary file (using mkstemp)

// The last six characters of tempname must be XXXXXX and these are replaced with
// a string that makes the filename unique. The file is then created with mode
// read/write and permissions 0600. Since it will be modified, tempname must not
// be a string constant, but should be declared as a character array.

class TempFile
{
public:
    int fd;			// file descriptor
    char fname[50];		// file name
    bool keep;			// if true, destructor won't remove file
    TempFile(const char* tempname) : fd(-1), keep(false)
	{
	    if (strlen(tempname) < 45) {
		strcpy(fname, "/tmp/");	// create the temp file in directory /tmp
		strcat(fname, tempname);
		fd = mkstemp(fname);
	    }
	}
    ~TempFile()
	{
	    if (fd >= 0)
	    {
		close(fd);
		if (!keep)
		    remove(fname);
		fd = -1;
	    }
	}
};
#endif
