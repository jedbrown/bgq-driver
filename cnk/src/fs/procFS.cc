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
/* (C) Copyright IBM Corp. 2012                                     */
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
#include "procFS.h"

#include <sys/stat.h>
#include <fcntl.h>

//
//  shm class - shared memory supported functions 
//
// Please keep functions in alphabetical order for ease of maintenance

extern Lock_Atomic_t ShareLock;
ShmMgr_t   ProcFSMemory;
char procFSMemoryPool[CONFIG_PROCFS_SIZE];

int procFS::init()
{
   _FD_type = FD_PROC_MEM;
   ProcFSMemory.VStart = ProcFSMemory.PStart = (uint64_t)((void*)procFSMemoryPool);
   ProcFSMemory.Size   = sizeof(procFSMemoryPool);
   return 0;
}

ShmMgr_t* procFS::getShmManager() 
{ 
    return &ProcFSMemory;
}

// mmap not support by procfs
uint64_t procFS::mmap(int fd, uint64_t length, uint64_t offset)
{
    TRACE( TRACE_VirtFS, ("(E) mmap method not supported by file system for descriptor %d\n", fd) );
    return CNK_RC_FAILURE(ENOSYS);
}

uint64_t procFS::readlink(const char *pathname, void *buf, size_t bufsize)
{
    if(strstr(pathname, "/exe") != NULL)
    {
        strncpy((char*)buf, GetMyProcess()->app->App_Args, bufsize);
        return 0;
    }
    else if(strstr(pathname, "/cwd") != NULL)
    {
        strncpy((char*)buf, GetMyProcess()->CurrentDir, bufsize);
        return 0;
    }
    return CNK_RC_FAILURE(ENOSYS);
}

uint64_t procFS::open(const char *pathname, int oflags, mode_t mode)
{
    char tmpname[64];
    if(GetMyKThread()->KernelInternal == false)
    {
        if(((oflags & O_ACCMODE) == O_WRONLY) || ((oflags & O_ACCMODE) == O_RDWR))
        {
            return CNK_RC_FAILURE(EPERM);
        }
        
        const char* suffix = strstr(pathname, "/self/");
        if(suffix != NULL)
        {
            suffix += 6;
            snprintf(tmpname, sizeof(tmpname), "/proc/%d/%s", GetPID(), suffix);
            pathname = tmpname;
        }
        
        // regenerate /proc/maps for this process
        if(strstr(pathname, "/maps") != NULL)
        {
            ProcFS_GenMaps(GetMyProcess());
        }
        else if(strstr(pathname, "/exe") != NULL)
        {
            pathname = GetMyProcess()->app->App_Args;
            return File_GetFSPtrFromPath(pathname)->open(pathname, oflags, mode);
            
        }
        else if(strstr(pathname, "/cwd") != NULL)
        {
            pathname = GetMyProcess()->CurrentDir;
            return File_GetFSPtrFromPath(pathname)->open(pathname, oflags, mode);
            
        }
    }
    
    return shmFS::open(pathname, oflags, mode);
}

uint64_t procFS::write(int fd, const void* buffer, size_t length)
{
    if(GetMyKThread()->KernelInternal)
    {
        return shmFS::write(fd, buffer, length);
    }
    return CNK_RC_FAILURE(EBADF);
}

int procFS::cleanupJob(int fs)
{
    // Initialize the procFS memory object. We want to keep our base allocation but reset the 
    // allocated mmap blocks so we do not touch VStartMB, PStartMB, or SizeMB
    // Initialize number of distinct shm_opens
    Kernel_Lock(&ShareLock);
    for (uint32_t index = 0; index < SHM_MAX_OPENS; index++ )
    {
        if(SharedPool[index].owner == getShmManager())
        {
            memset(&SharedPool[index], 0, sizeof(SharedPool[index]));
        }
    }
    Kernel_Unlock(&ShareLock);
    
    return 0;
}


bool procFS::isMatch(const char *pathname)
{
    ShmMgrEntry_t* entry = NULL;
    if(strncmp(pathname, "/proc", 5) != 0)
        return false;
    
    if(strstr(pathname, "/proc/self/fd/") != NULL)
    {
        return false;
    }
    
    if(GetMyKThread()->KernelInternal)  // kernel accesses to /proc always match
        return true;

    char tmpname[64];
    // handle /proc/self symlink
    
    
    const char* suffix = strstr(pathname, "/self/");
    if(suffix != NULL)
    {
        suffix += 6;
        snprintf(tmpname, sizeof(tmpname), "/proc/%d/%s", GetPID(), suffix);
        pathname = tmpname;
    }
    
    Kernel_Lock(&ShareLock);    
    findByName(pathname, entry, true);
    Kernel_Unlock(&ShareLock);
    
    if(entry)
        return true;
    
    return false;
}
