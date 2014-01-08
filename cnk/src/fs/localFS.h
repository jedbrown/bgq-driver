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
/* (C) Copyright IBM Corp.  2010, 2012                              */
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

#ifndef _LOCALMEM_H_ // Prevent multiple inclusion.
#define _LOCALMEM_H_

// Includes
#include "shmFS.h"

#define DEVLOCAL_PREFIX "/dev/local/"
#define DEVLOCAL_ROOT   "/dev/local"


//  sharedmemFS class - shared memory supported functions are defined here
//                implementation is in sharedmemFS.cc
// Please keep functions in alphabetical order for ease of maintenance


class localFS : public shmFS
{
public:

   //! \brief  Default constructor.

  localFS(void) : shmFS() { };

   //! \brief  Check if pathname is match for this file system.
   //! \param  path Pathname to check.
   //! \return True if pathname is a match, otherwise false.

    bool isMatch(const char *path) { return strncmp(path, DEVLOCAL_PREFIX, strlen(DEVLOCAL_PREFIX)) == 0 ? true : false; }

   int init(void);
   virtual ShmMgr_t* getShmManager() { return &(GetMyProcess()->LocalDisk); };
   virtual uint64_t  getDeviceID()   { return 0x01021996; };
};

#endif
