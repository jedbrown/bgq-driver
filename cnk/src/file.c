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
/* (C) Copyright IBM Corp.  2007, 2012                              */
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "Kernel.h"

/*
 * Function that takes an IO node FD and assigns it to a local CNK file descriptor.
 * The function takes the descriptor that was assigned by the IO node and stores it in
 * the array of local descriptors.  The local descriptor subscript value is returned if
 * the IO Node descriptor can be stored in the local array. 
 * NOTE: A value of -2 is passed in for local descriptor requests.  This will allow support
 *       for functions such as shared memory that is implemented completly at the CNK level,
 *       with no call to the IO Node for a descriptor.
 * -1 is returned if we are out of local descriptors.  The caller will be responsible for
 *    closing the IO Node descriptor if one was assigned.
 */

/*
 */
int  File_RestoreCheckpoint(int core, CNK_Descriptors_t* newdesc)
{
    //! \todo Needs to work to integrate with pluggable file systems.
#if 0
    int fd;
    AppProcess_t* app = GetProcessByProcessorID(core);

    for (fd=0; fd<CNK_MAX_FDS; fd++)
    {
        if (newdesc->cnk_local_fd[fd].Type == FD_SHM)
        {
            Kernel_Lock(&app->DescriptorTableLock);
            app->App_Descriptors.cnk_local_fd[fd].Remote_FD = newdesc->cnk_local_fd[fd].Remote_FD;
            app->App_Descriptors.cnk_local_fd[fd].Type = newdesc->cnk_local_fd[fd].Type;
            File_SetBit(fd, app->App_Descriptors.cnk_fd_bits);
            Kernel_Unlock(&app->DescriptorTableLock);
        }
    }
#endif
    return 0;
}

__END_DECLS
