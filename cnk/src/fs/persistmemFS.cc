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
/* (C) Copyright IBM Corp.  2009, 2012                              */
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

// Includes
#include "Kernel.h"
#include "util.h"
#include "virtFS.h"
#include "persistmemFS.h"

#include <sys/stat.h>
#include <fcntl.h>

//
//  persist class - persistent memory supported functions 
//
// Please keep functions in alphabetical order for ease of maintenance


int persistmemFS::init()
{
   _FD_type = FD_PERSIST_MEM;
   return 0;
}


int persistmemFS::cleanupJob(int fs)
{
    // Initialize the persistent memory object. We want to keep our base allocation but reset the 
    // allocated mmap blocks so we do not touch VStartMB, PStartMB, or SizeMB
    // Initialize number of distinct shm_opens
    Kernel_Lock(&ShareLock);
    for (uint32_t index = 0; index < SHM_MAX_OPENS; index++ )
    {
        if(SharedPool[index].owner == getShmManager())
        {
            SharedPool[index].LinkCount = 0;   // Set linkcount to zero, no processes are open against it.  
        }
    }
    Kernel_Unlock(&ShareLock);
    
    return 0;
}


