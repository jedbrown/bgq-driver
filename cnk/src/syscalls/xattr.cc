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
/* (C) Copyright IBM Corp.  2008, 2012                              */
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
#include "fs/virtFS.h"

static uint64_t checkFD(int fd)
{
    if (!File_IsFDAllocated(fd)) 
        return CNK_RC_FAILURE(EBADF);
    return 0;
}
static uint64_t checkName(const char* name)
{
    if(!VMM_IsAppAddress(name, 64))
    {
        return CNK_RC_FAILURE(EFAULT);
    }
    return 0;
}
static uint64_t checkValue(const void* value, size_t size)
{
    if (!VMM_IsAppAddress(value, size)) 
    {
        return CNK_RC_FAILURE(EFAULT);
    }
    return 0;
}

// int fd, const char *name, void *value, size_t size
uint64_t sc_fgetxattr(SYSCALL_FCN_ARGS)
{
    uint64_t rc;
    int fd           = r3;
    const char* name = (char*)r4;
    void* value      = (void*)r5;
    size_t size      = r6;
    
    rc = checkFD(fd);             if(rc) return rc;
    rc = checkName(name);         if(rc) return rc;
    //An empty buffer of size 0 can be passed to get the current size
    if (size){
       rc = checkValue(value, size); if(rc) return rc;
    }
    return File_GetFSPtr(fd)->fgetxattr(fd, name, value, size);
}

uint64_t sc_getxattr(SYSCALL_FCN_ARGS)
{
    uint64_t rc;
    const char* path = (const char*)r3;
    const char* name = (char*)r4;
    void* value      = (void*)r5;
    size_t size      = r6;
    
    rc = validatePathname(path);  if(rc & _BN(0) ) return rc;
    rc = checkName(name);         if(rc) return rc;
    //An empty buffer of size 0 can be passed to get the current size
    if (size){
       rc = checkValue(value, size); if(rc) return rc;
    }
    return File_GetFSPtrFromPath(path)->getxattr(path, name, value, size);
}

uint64_t sc_lgetxattr(SYSCALL_FCN_ARGS)
{
    uint64_t rc;
    const char* path = (const char*)r3;
    const char* name = (char*)r4;
    void* value      = (void*)r5;
    size_t size      = r6;
    
    rc = validatePathname(path);  if(rc & _BN(0) ) return rc;
    rc = checkName(name);         if(rc) return rc;
    //An empty buffer of size 0 can be passed to get the current size
    if (size){
       rc = checkValue(value, size); if(rc) return rc;
    }
    return File_GetFSPtrFromPath(path)->lgetxattr(path, name, value, size);
}


// (int fd, char *list, size_t size)
uint64_t sc_flistxattr(SYSCALL_FCN_ARGS)
{
    uint64_t rc;
    int fd           = r3;
    char* list       = (char*)r4;
    size_t size      = r5;
    
    rc = checkFD(fd);             if(rc) return rc;
    //An empty buffer of size 0 can be passed to get the current size
    if (size){
       rc = checkValue(list, size); if(rc) return rc;
    }
    return File_GetFSPtr(fd)->flistxattr(fd, list, size);
}

uint64_t sc_listxattr(SYSCALL_FCN_ARGS)
{
    uint64_t rc;
    const char* path = (const char*)r3;
    char* list       = (char*)r4;
    size_t size      = r5;
    
    rc = validatePathname(path);  if(rc & _BN(0) ) return rc;
    //An empty buffer of size 0 can be passed to get the current size
    if (size){
       rc = checkValue(list, size); if(rc) return rc;
    }
    return File_GetFSPtrFromPath(path)->listxattr(path, list, size);
}

uint64_t sc_llistxattr(SYSCALL_FCN_ARGS)
{
    uint64_t rc;
    const char* path = (const char*)r3;
    char* list       = (char*)r4;
    size_t size      = r5;
    
    rc = validatePathname(path);  if(rc & _BN(0) ) return rc;
    //An empty buffer of size 0 can be passed to get the current size
    if (size){
       rc = checkValue(list, size); if(rc) return rc;
    }
    return File_GetFSPtrFromPath(path)->llistxattr(path, list, size);
}



// int fd, const char *name
uint64_t sc_fremovexattr(SYSCALL_FCN_ARGS)
{
    uint64_t rc;
    int fd           = r3;
    const char* name = (char*)r4;
    
    rc = checkFD(fd); if(rc) return rc;
    rc = checkName(name); if(rc) return rc;
    return File_GetFSPtr(fd)->fremovexattr(fd, name);
}

uint64_t sc_lremovexattr(SYSCALL_FCN_ARGS)
{
    uint64_t rc;
    const char* path = (const char*)r3;
    const char* name = (char*)r4;
    
    rc = validatePathname(path);  if(rc & _BN(0) ) return rc;
    rc = checkName(name);        if(rc) return rc;
    return File_GetFSPtrFromPath(path)->lremovexattr(path, name);
}

uint64_t sc_removexattr(SYSCALL_FCN_ARGS)
{
    uint64_t rc;
    const char* path = (const char*)r3;
    const char* name = (char*)r4;
    
    rc = validatePathname(path);  if(rc & _BN(0) ) return rc;
    rc = checkName(name);        if(rc) return rc;
    return File_GetFSPtrFromPath(path)->removexattr(path, name);
}

//  int fd, const char *name, const void *value, size_t size, int flags
uint64_t sc_fsetxattr(SYSCALL_FCN_ARGS)
{
    uint64_t rc;
    int fd           = r3;
    const char* name = (char*)r4;
    const void* value= (void*)r5;
    size_t size      = r6;
    int flags        = r7;
    
    rc = checkFD(fd); if(rc) return rc;
    rc = checkName(name); if(rc) return rc;
    rc = checkValue(value, size); if(rc) return rc;
    return File_GetFSPtr(fd)->fsetxattr(fd, name, value, size, flags);    
}

uint64_t sc_lsetxattr(SYSCALL_FCN_ARGS)
{
    uint64_t rc;
    const char* path = (const char*)r3;
    const char* name = (char*)r4;
    const void* value= (void*)r5;
    size_t size      = r6;
    int flags        = r7;
    
    rc = validatePathname(path);  if(rc & _BN(0) ) return rc;
    rc = checkName(name);         if(rc) return rc;
    rc = checkValue(value, size); if(rc) return rc;
    return File_GetFSPtrFromPath(path)->lsetxattr(path, name, value, size, flags);    
}


uint64_t sc_setxattr(SYSCALL_FCN_ARGS)
{
    uint64_t rc;
    const char* path = (const char*)r3;
    const char* name = (char*)r4;
    const void* value= (void*)r5;
    size_t size      = r6;
    int flags        = r7;
    rc = validatePathname(path);  if(rc & _BN(0) ) return rc; 
    rc = checkName(name);         if(rc) return rc;
    rc = checkValue(value, size); if(rc) return rc;
    return File_GetFSPtrFromPath(path)->setxattr(path, name, value, size, flags);    
}
