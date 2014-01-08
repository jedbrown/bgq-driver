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

// Includes
#include <sys/stat.h>
#include <fcntl.h>
#include "Kernel.h"
#include "util.h"
#include "filesetup.h"

// #include "callthru_config.h"  // for testing

#define MEMFS_MODE (S_IRUSR|S_IXUSR|S_IRGRP|S_IXGRP|S_IROTH|S_IXOTH|S_IFREG)

//
//  memory file system file support interfaces
//
// Please keep functions in alphabetical order for ease of maintenance
//

memFSdata_t *memFS::getBaseAddr(void)
{
    if(_baseAddr == NULL)
    {
        uint64_t vaddr, paddr;
        size_t vsize;
        
        AppProcess_t* AppProc = GetMyProcess();
        vmm_getSegment(AppProc->Tcoord, IS_RAMDISK, &vaddr, &paddr, &vsize);
        _baseAddr = (memFSdata_t*)(vaddr);
        
        if(_baseAddr == NULL)
        {
            _baseAddr = (memFSdata_t*)CONFIG_MEMFS_BASEADDR;
            
            vmm_MapUserSpace( APP_FLAGS_R | APP_FLAGS_W | APP_FLAGS_ESEL(3) | APP_FLAGS_NONSPECULATIVE,
                             (void*)(CONFIG_MEMFS_BASEADDR),
                             (void*)(CONFIG_MEMFS_BASEADDR),
                             1024*1024*1024,
                             0,
                             0);                                                                                             
        }
    }
    return _baseAddr;
}


int memFS::setupJob(int fs)
{
    int rc = 0;

    if (CONFIG_MEMFS_BASEADDR > NodeState.DomainDesc.ddrEnd) return -1;

    if(_baseAddr)
    {
        vmm_UnmapUserSpace((void*)CONFIG_MEMFS_BASEADDR, 1024*1024*1024, 0);
        _baseAddr = NULL;
    }
    vmm_MapUserSpace( APP_FLAGS_R | APP_FLAGS_W | APP_FLAGS_ESEL(3) | APP_FLAGS_NONSPECULATIVE,
                     (void*)(CONFIG_MEMFS_BASEADDR),
                     (void*)(CONFIG_MEMFS_BASEADDR),
                     1024*1024,
                     0,
                     0);                                                                                 
    
    char *memDataPtr = (char*)CONFIG_MEMFS_BASEADDR;
    memFSdata_t *memFSdataPtr = (memFSdata_t*) memDataPtr;   // point to memFS data structure
    
    if (0==(memcmp(MEMFS_ID,memFSdataPtr->id,8))) 
    {
        rc = vmm_addSegment(IS_RAMDISK, 0, (CONFIG_MEMFS_BASEADDR)/(1024*1024), ROUND_UP_1M(memFSdataPtr->totalFSsize)/(1024*1024), -1, VMM_SEGMENTCOREMASK_APP, true, false, false, true, true, false);
    }
    else 
    {
        rc = -1;
    }
    vmm_UnmapUserSpace((void*)CONFIG_MEMFS_BASEADDR, 1024*1024, 0);
    return rc;
}

uint64_t memFS::chdir(const char *path)
{
    uint64_t rc;
    AppProcess_t *app = GetMyProcess();
    CNK_Descriptors_t *pFD = &(app->App_Descriptors);

    // Allow a "change" to the same directory.
    if (strncmp(path, ".", MIN(APP_MAX_PATHNAME, sizeof("."))) == 0) {
       rc = CNK_RC_SUCCESS(0);
    }

    // Allow a change to the root directory of file system.
    else if (strncmp(path, DEVMEMFS_ROOT, MIN(sizeof(DEVMEMFS_ROOT), APP_MAX_PATHNAME)) == 0) {
       File_SetFD(CWD_FILENO, -1, FD_MEMFILE);
       strncpy(GetMyProcess()->CurrentDir, path, sizeof(GetMyProcess()->CurrentDir));
       rc = CNK_RC_SUCCESS(0);
    }

    // Otherwise there is no directory support for the file system.
    else {
        rc = CNK_RC_FAILURE(ENOSYS);
    }

    TRACE( TRACE_MemFS, ("(I) memFS::chdir%s: path=%s rc=%s\n", whoami(), path, CNK_RC_STRING(rc)) );
    return rc;
}

uint64_t memFS::close(int fd)
{
    // Make sure file descriptor is valid.
    int index = File_GetRemoteFD(fd);
    if (index < 0) {
        return CNK_RC_FAILURE(EBADF);
    }

    File_FreeFD(fd);

    TRACE( TRACE_MemFS, ("(I) memFS::close%s: fd=%d rc=%s\n", whoami(), fd, CNK_RC_STRING(0)) );
    return CNK_RC_SUCCESS(0);
}

uint64_t memFS::fstat(int fd, struct stat *statbuf)
{
    // Make sure file descriptor is valid.
    int index = File_GetRemoteFD( fd );
    if (index < 0) {
        return CNK_RC_FAILURE(EBADF);
    }

    fillStatStructure(statbuf, index);
    TRACE( TRACE_MemFS, ("(I) memFS::fstat%s: fd=%d rc=%s\n", whoami(), fd, CNK_RC_STRING(0)) );
    return CNK_RC_SUCCESS(0);
}

int memFS::init()
{
    int rc = 0;

    if (CONFIG_MEMFS_BASEADDR > NodeState.DomainDesc.ddrEnd)
    {
        TRACE( TRACE_MemFS, ("(I) memFS::init%s: base address %x is beyond the end of memory at %llx\n",
                             whoami(), CONFIG_MEMFS_BASEADDR, NodeState.DomainDesc.ddrEnd) );
        return -1;
    }

    vmm_MapUserSpace( APP_FLAGS_R | APP_FLAGS_W | APP_FLAGS_ESEL(3) | APP_FLAGS_NONSPECULATIVE,
                     (void*)(CONFIG_MEMFS_BASEADDR),
                     (void*)(CONFIG_MEMFS_BASEADDR),
                     1024*1024,
                     0,
                     0);                                                                                 
        
    char *memDataPtr = (char*)CONFIG_MEMFS_BASEADDR;
    memFSdata_t *memFSdataPtr = (memFSdata_t*) memDataPtr;   // point to memFS data structure
    
    if (0==(memcmp(MEMFS_ID,memFSdataPtr->id,8)))
    {
        rc = 0;
    }
    else
    {
        TRACE( TRACE_MemFS, ("(I) memFS::init%s: ramdisk is not loaded at base address %x\n", whoami(), CONFIG_MEMFS_BASEADDR) );
        rc = -1;
    }
    
    vmm_UnmapUserSpace((void*)CONFIG_MEMFS_BASEADDR, 1024*1024, 0);
    return rc;
}

uint64_t memFS::lseek(int fd, off_t offset, int whence)
{
    // Make sure file descriptor is valid.
    int index = File_GetRemoteFD(fd);
    if (index < 0) {
        return CNK_RC_FAILURE(EBADF);
    }

    long fileLen = getBaseAddr()->entry[index].fileSize;
    long currOffset = File_GetCurrentOffset(fd);

    switch (whence) {
       case SEEK_END:
            if (0 < offset) {
               return CNK_RC_FAILURE(EINVAL);
            } 
            currOffset = fileLen + offset;
            if (0 > currOffset) {
               return CNK_RC_FAILURE(EINVAL);
            } 
            break;
       case SEEK_CUR:
            currOffset += offset;
            if ((0 > currOffset) || (currOffset > fileLen)) {
               return CNK_RC_FAILURE(EINVAL);
            } 
            break;
       case SEEK_SET:
            if ((0 > offset) || (offset > fileLen)) {
               return CNK_RC_FAILURE(EINVAL);
            } 
            currOffset = offset;
            break;
       default:
            return CNK_RC_FAILURE(EINVAL);
            break;
    }
    File_SetCurrentOffset(fd,currOffset);

    TRACE( TRACE_MemFS, ("(I) memFS::lseek%s: fd=%d offset=%ld whence=%d rc=%s\n", whoami(), fd, offset, whence, CNK_RC_STRING(currOffset)) );
    return CNK_RC_SUCCESS(currOffset);
}

uint64_t memFS::open(const char* path, int oflags, mode_t mode)
{
    int  index;    // index of file entry in memFSdata
    int  localFD;
    char fileName[NAME_LEN];
    char *namePtr;
    unsigned  pathLen;
    pathLen = strlen(path);
    if (pathLen >= sizeof(fileName)) {
        return CNK_RC_FAILURE(ENAMETOOLONG);
    }
    if(memcmp(path, DEVMEMFS_PREFIX, sizeof(DEVMEMFS_PREFIX)) == 0)
    {
        namePtr = (char*)path+sizeof(DEVMEMFS_PREFIX)-1;
    }
    else 
    {
        namePtr = (char*)path;
    }
    
    strncpy(fileName,namePtr, sizeof(fileName));
    
    if (oflags != O_RDONLY )
    {
        return CNK_RC_FAILURE(ENOTSUP);
    }
    
    index = findfile(fileName);
    if (-1  == index) { 
       // error
       return CNK_RC_FAILURE(ENOENT); // no file
       
    }
    
    localFD =  File_GetFD(3); // skip STDxxx descriptors
    if (-1 != localFD){ 
        File_SetFD(localFD,index, FD_MEMFILE);
        File_SetCurrentOffset(localFD,0);

        TRACE( TRACE_MemFS, ("(I) memFS::open%s: path=\"%s\" oflags=%08x mode=%08x rc=%s\n", whoami(), path, oflags, mode, CNK_RC_STRING(localFD)) ); 
        return CNK_RC_SUCCESS(localFD);
    }
    else {
        // assume no free local descriptor
        return CNK_RC_FAILURE(EMFILE);
    }
}

uint64_t memFS::pread64(int fd, void *buffer, size_t length, off64_t offset)
{
   // Make sure file descriptor is valid.
   int index = File_GetRemoteFD(fd);
   if (index < 0) {
       return CNK_RC_FAILURE(EBADF);
   }

   // Just return if asked to read zero bytes.
   if (0 == length) {
       return CNK_RC_SUCCESS(0);
   }

   // Get current state of the file.
   long fileLen = getBaseAddr()->entry[index].fileSize;
   char *fileDataPtr = ((char*)getBaseAddr()) + (getBaseAddr()->entry[index].fileOffset);

   // Just return if already at the end of the file. 
   if (offset == fileLen) {
       return CNK_RC_SUCCESS(0);
   }

   // Calculate how much to read from file.
   fileDataPtr += offset;
   long copyBytes = ((long)(offset+length) <= fileLen) ? length : fileLen-offset;

   // There is data available to read from file.
   if (0 < copyBytes) {
       memcpy(buffer, fileDataPtr, copyBytes);
   }

   TRACE( TRACE_MemFS, ("(I) memFS::pread64%s: fd=%d buffer=%p length=%ld offset=%lu rc=%s\n",
                        whoami(), fd, buffer, length, offset, CNK_RC_STRING(copyBytes)) );
   return CNK_RC_SUCCESS(copyBytes);
}

uint64_t memFS::read(int fd, void *buffer, size_t length)
{
    // Make sure file descriptor is valid.
    int index = File_GetRemoteFD(fd);
    if (index < 0) {
        return CNK_RC_FAILURE(EBADF);
    }

    // Just return if asked to read zero bytes.
    if (0 == length) {
        return CNK_RC_SUCCESS(0);
    }

    // Get current state of the file.
    long fileLen = getBaseAddr()->entry[index].fileSize;
    char *fileDataPtr = ((char*)getBaseAddr()) + (getBaseAddr()->entry[index].fileOffset);
    long currentOffset = File_GetCurrentOffset(fd);

    // Just return if already at the end of the file. 
    if (currentOffset == fileLen) {
        return CNK_RC_SUCCESS(0);
    }

    // Calculate how much to read from file.
    fileDataPtr += currentOffset;
    long copyBytes = ((long)(currentOffset+length) <= fileLen) ? length : fileLen-currentOffset;

    // There is data available to read from file.
    if (0 < copyBytes) {
        memcpy(buffer, fileDataPtr, copyBytes);
        currentOffset += copyBytes;
        File_SetCurrentOffset(fd, currentOffset); 
    }

    TRACE( TRACE_MemFS, ("(I) memFS::read%s: fd=%d buffer=%p length=%ld rc=%s\n", whoami(), fd, buffer, length, CNK_RC_STRING(copyBytes)) );
    return CNK_RC_SUCCESS(copyBytes);
}

uint64_t memFS::stat(const char *path, struct stat *statbuf)
{
    char fileName[NAME_LEN];
    uint64_t rc;

    // Find the file name in the path.
    const char *namePtr;
    if (strncmp(path, DEVMEMFS_PREFIX, strlen(DEVMEMFS_PREFIX)) == 0) {
        namePtr = path + sizeof(DEVMEMFS_PREFIX)-1;
    }
    else {
        namePtr = path;
    }

    // Make sure the file name is not too long.
    int nameLen = strlen(namePtr) + 1;
    if (NAME_LEN < nameLen) {
        rc = CNK_RC_FAILURE(ENAMETOOLONG);
        TRACE( TRACE_MemFS, ("(I) memFS::stat%s: path=\"%s\" statbuf=%p rc=%s\n", whoami(), path, statbuf, CNK_RC_STRING(rc)) );
        return rc;
    }
    memcpy(fileName, namePtr, nameLen);

    // Find the file. 
    int index = findfile(fileName);
    if (0 > index) {
        rc = CNK_RC_FAILURE(ENOENT);
        TRACE( TRACE_MemFS, ("(I) memFS::stat%s: path=\"%s\" statbuf=%p rc=%s\n", whoami(), path, statbuf, CNK_RC_STRING(rc)) );
        return rc;
    }
    else {
        fillStatStructure(statbuf, index);
        rc = 0;
    }

    TRACE( TRACE_MemFS, ("(I) memFS::stat%s: path=\"%s\" statbuf=%p rc=%s\n", whoami(), path, statbuf, CNK_RC_STRING(rc)) );
    return CNK_RC_SUCCESS(rc);
}

int memFS::findfile(const char* fileName) 
{    
    //  search memFS directory for file name. return index if found, else -1
    //  This is an internal routine and assumes the caller has checked for a valid name
    
    char searchName[NAME_LEN];
    uint32_t  searchLen;
    uint32_t  numFiles;
    uint32_t  i;
    
    for(i=strlen(fileName)-1; i>0; i--)
    {
        if(fileName[i] == '/')
        {
            fileName = &fileName[i+1];
            break;
        }
    }
    
    searchLen = strlen(fileName);
    memcpy(searchName,fileName,searchLen);
    numFiles = getBaseAddr()->totalFSfiles;
    
    for(i=0; i<numFiles; i++) 
    {
        if((getBaseAddr()->entry[i].nameLen == searchLen) &&     // name length match
           (0==(memcmp(getBaseAddr()->entry[i].name,searchName,searchLen)))) 
        {   // name match
            return i;   // found it
        }
    }
    return -1;
} 

void memFS::fillStatStructure(struct stat *statbuf, int index)
{
   long size = getBaseAddr()->entry[index].fileSize;

   memset(statbuf, 0, sizeof(struct stat));
   statbuf->st_blksize = 512;
   statbuf->st_mode = MEMFS_MODE;
   statbuf->st_ino = index;
   statbuf->st_nlink = 1;
   statbuf->st_dev = 42;
   statbuf->st_size = size;
   statbuf->st_blocks = (size + 511)/512;  // round up number of blocks
   return;
}

bool memFS::isMatch(const char *path) 
{ 
    return((IsAppAgent() && strncmp(path, DEVMEMFS_PREFIX, strlen(DEVMEMFS_PREFIX))) == 0 ? true : false); 
}
