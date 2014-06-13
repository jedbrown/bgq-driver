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
#include "cnk/include/Config.h"
#include "util.h"
#include <unistd.h>
#include <fcntl.h>
#include <inttypes.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "virtFS.h"

typedef struct
{
    AppProcess_t* owner;
    __ino64_t     inode;
    uint64_t      start;
    size_t        length;
    dev_t         deviceID;
} FLMStatus_t;

FLMStatus_t FLMStatus[CONFIG_FLMSIZE];

int FLM_Init()
{
    memset(FLMStatus, 0, sizeof(FLMStatus));
    return 0;
}

uint64_t FLM_TouchFD(int fd, int whence)
{
    dev_t deviceID  = 0;
    __ino64_t inode = 0;
    struct stat64 buf;
    size_t size;
    uint64_t rc = 0;
    File_GetDeviceINode(fd, &deviceID, &inode, &size);
    if(((deviceID == 0) && (inode == 0)) || (whence == SEEK_END))
    {
        rc = File_GetFSPtr(fd)->fstat64(fd, &buf);
        if(rc == 0)
        {
            File_SetDeviceINode(fd, buf.st_dev, buf.st_ino, buf.st_size);
        }
    }
    return rc;
}

static off_t FLM_AdjustWhence(int fd, uint64_t start, size_t size, int whence)
{
    switch(whence)
    {
        case SEEK_SET:
            return start;
        case SEEK_CUR:
            return start + File_GetCurrentOffset(fd);            
        case SEEK_END:
            return size - start;
    }
    return 0;
}

int FLM_HasOverlap(int fd, uint64_t start, size_t length, int whence)
{
    int x;
    dev_t deviceID;
    __ino64_t inode;
    size_t size;
    File_GetDeviceINode(fd, &deviceID, &inode, &size);
    start = FLM_AdjustWhence(fd, start, size, whence);
    
    assert(deviceID);
    assert(inode);
    
    AppProcess_t* myprocess = GetProcessByProcessorID(ProcessorID());
    for(x=0; x<CONFIG_FLMSIZE; x++)
    {
        if((FLMStatus[x].owner != NULL) && (FLMStatus[x].owner != myprocess) && 
           (FLMStatus[x].deviceID == deviceID) && (FLMStatus[x].inode == inode))
        {
            // range starts within region.
            if((FLMStatus[x].start <= start) && (FLMStatus[x].start + FLMStatus[x].length > start))
                return true;
            
            // range ends within region
            if((FLMStatus[x].start <= start + length) && (FLMStatus[x].start + FLMStatus[x].length > start + length))
                return true;
            
            // range starts below region and ends above region.  (i.e., covers whole region)
            if((FLMStatus[x].start > start) && (FLMStatus[x].start + FLMStatus[x].length <= start + length))
                return true;
        }
    }
    return false;
}

int FLM_Acquire(int fd, uint64_t start, size_t length, int whence)
{
    int x;
    dev_t deviceID;
    __ino64_t inode;
    size_t size;
    File_GetDeviceINode(fd, &deviceID, &inode, &size);
    start = FLM_AdjustWhence(fd, start, size, whence);
    assert(deviceID);
    assert(inode);
    
    AppProcess_t* myprocess = GetProcessByProcessorID(ProcessorID());
    for(x=0; x<CONFIG_FLMSIZE; x++)
    {
        if(FLMStatus[x].owner == NULL)
        {
            FLMStatus[x].owner    = myprocess;
            FLMStatus[x].deviceID = deviceID;
            FLMStatus[x].inode    = inode;
            FLMStatus[x].start    = start;
            FLMStatus[x].length   = length;
            break;
        }
    }
    if(x == CONFIG_FLMSIZE)
        return -1;
    
    return 0;
}

int FLM_Release(int fd, uint64_t start, size_t length, int whence)
{
    int x;
    uint64_t tmp;
    dev_t    deviceID;
    __ino64_t  inode;
    size_t   size;
    AppProcess_t* myprocess = GetProcessByProcessorID(ProcessorID());
    
    File_GetDeviceINode(fd, &deviceID, &inode, &size);
    start = FLM_AdjustWhence(fd, start, size, whence);
    assert(deviceID);
    assert(inode);
    
    for(x=0; x<CONFIG_FLMSIZE; x++)
    {
        if((FLMStatus[x].owner == myprocess) && (FLMStatus[x].deviceID == deviceID) && (FLMStatus[x].inode == inode))
        {
            // range starts within region.
            //     |   FLM       |
            //  |-----|
            
            if((FLMStatus[x].start >= start) && (FLMStatus[x].start < start + length))
            {
                // adjust start of FLM range upward, adjust length of new FLM range.
                tmp = FLMStatus[x].start;
                FLMStatus[x].start = MIN(start + length, FLMStatus[x].start + FLMStatus[x].length);
                FLMStatus[x].length -= FLMStatus[x].start - tmp;
            }
            
            // range ends within region.
            //     |   FLM       |
            //                |-----|
            if((FLMStatus[x].start + FLMStatus[x].length > start) && (FLMStatus[x].start + FLMStatus[x].length < start + length))
            {
                // adjust tail of FLM range.
                FLMStatus[x].length = FLMStatus[x].start + FLMStatus[x].length - start;
            }
            
            // range ends within region.
            //     |   FLM       |
            //         |-----|
            if((FLMStatus[x].start > start) && (FLMStatus[x].start + FLMStatus[x].length < start + length))
            {
                // end up with two regions.
                FLM_Acquire(deviceID, inode, start + length, FLMStatus[x].start + FLMStatus[x].length - start - length);
                FLMStatus[x].length = FLMStatus[x].start + FLMStatus[x].length - start;
            }
            
            // range ends within region. 
            //      |   FLM       |
            //   |--------------------|
            // This case is covered by previous tests.
            
            
            // length is region is now zero bytes, free slot.  
            if(FLMStatus[x].length == 0)
                FLMStatus[x].owner = NULL;
        }
    }
    return 0;
}

int FLM_ReleaseFile(int fd)
{
    int       x;
    dev_t     deviceID;
    __ino64_t inode;
    size_t    size;
    File_GetDeviceINode(fd, &deviceID, &inode, &size);
    if((deviceID == 0) && (inode == 0))
        return 0;
    
    AppProcess_t* myprocess = GetProcessByProcessorID(ProcessorID());
    for(x=0; x<CONFIG_FLMSIZE; x++)
    {
        if((FLMStatus[x].owner == myprocess) && (FLMStatus[x].deviceID == deviceID) && (FLMStatus[x].inode == inode))
        {
            FLMStatus[x].owner = NULL;
        }
    }
    return 0;
}
