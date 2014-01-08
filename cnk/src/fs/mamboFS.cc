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

// Note no member class implementations if ENABLE_MAMBOIO is not defined
#ifdef ENABLE_MAMBOIO

// Includes
#include "Kernel.h"
#include "util.h"
#include <sys/stat.h>
#include <fcntl.h>
#include "virtFS.h"
#include "mamboFS.h"
#include "callthru/callthru_config.h"

//
//  Mambo simulator call file support interfaces
//
// Please keep functions in alphabetical order for ease of maintenance
//
// Note that since the mambo interface does not support const or void we have to disable 
// these useful keywords. They probably ignore restrict also.

uint64_t mamboFS::chdir(const char *path)
{
    uint64_t rc;
    if (strncmp(path, ".", MIN(APP_MAX_PATHNAME, sizeof("."))) == 0) { // Allow a "change" to the same directory
       rc = CNK_RC_SUCCESS(0);
    }
    else {
        rc = CNK_RC_FAILURE(ENOSYS);
    }
    return rc;
}

uint64_t mamboFS::close(int fd)
{
    uint64_t rc;
    
    int remoteFD = File_GetRemoteFD(fd);
    if (remoteFD != AT_FDCWD) {
       if (MamboFileCloseExtended((FILE*)remoteFD) == 0) {
           File_FreeFD(fd);
           rc = CNK_RC_SUCCESS(0);
       }
       else {
           // Assume bad descriptor since no way to get mambo error code.
           rc = CNK_RC_FAILURE(EBADF);
       }
    }
    else {
       rc = CNK_RC_SUCCESS(0);
    }
    
    TRACE( TRACE_MamboFS, ("(I) mamboFS::close%s: fd=%d rc=%s\n", whoami(), fd, CNK_RC_STRING(rc)) );
    return rc;
}

uint64_t mamboFS::fstat(int fd, struct stat *statbuf)
{
    return fstat64(fd, (struct stat64 *)statbuf);
}

uint64_t mamboFS::fstat64(int fd, struct stat64 *statbuf)
{
    // Set just enough to keep the runtime happy.
    memset(statbuf, 0, sizeof(struct stat));
    statbuf->st_blksize = 512;
    statbuf->st_mode = S_IRWXU | S_IRWXG | S_IRWXO | S_IFREG;
    statbuf->st_blocks = 0;
    statbuf->st_ino = File_GetRemoteFD( fd );
    statbuf->st_nlink = 1;
    statbuf->st_dev = 42;
    
    off_t original_position = MamboFileTell((FILE*)statbuf->st_ino);
    MamboFileSeek((FILE*)statbuf->st_ino, 0, SEEK_END);
    statbuf->st_size = MamboFileTell((FILE*)statbuf->st_ino);
    MamboFileSeek((FILE*)statbuf->st_ino, original_position, SEEK_SET);
    
    TRACE( TRACE_MamboFS, ("(I) mamboFS::fstat64%s: fd=%d rc=0\n", whoami(), fd) );
    return CNK_RC_SUCCESS(0);
}

uint64_t mamboFS::ftruncate(int fd, off_t offset)
{
    // Just give a good return to fake out FORTRAN.  There isn't a mambo interface available to use.
    
    TRACE( TRACE_MamboFS, ("(I) mamboFS::ftruncate%s: fd=%d offset=%ld rc=0\n", whoami(), fd, offset) );
    return CNK_RC_SUCCESS(0);
}

uint64_t mamboFS::lseek(int fd, off_t offset, int whence)
{
    uint64_t rc;
    
    int remoteFD = File_GetRemoteFD(fd);
    if (MamboFileSeek((FILE *)remoteFD, offset, whence) == -1) {
	rc = CNK_RC_FAILURE(EINVAL);
    }
    else
    {
        rc = CNK_RC_SUCCESS(MamboFileTell((FILE *)remoteFD));
    }
    TRACE( TRACE_MamboFS, ("(I) mamboFS::lseek%s: fd=%d offset=%ld whence=%d rc=%s\n", whoami(), fd, offset, whence, CNK_RC_STRING(rc)) );
    return rc;
}

uint64_t mamboFS::open(const char *path, int oflags, mode_t mode)
{
    char rw[20];
    long hostFD;
    int  localFD;
    uint64_t rc;
        
    // Translate oflags to mambo flags.
    oflags &= ~O_LARGEFILE;     // turn off large file flag - only for test
    if (oflags == O_RDONLY) {
        rw[0] = 'r';
        rw[1] = 0;
    }
    else if (oflags & O_WRONLY) {
        rw[0] = 'w';
        rw[1] = 0;
    }
    else if (oflags & O_RDWR) {
        if (oflags & O_CREAT) {
            rw[0] = 'w';
            rw[1] = '+';
            rw[2] = 0;
        }
        else {
            rw[0] = 'r';
            rw[1] = '+';
            rw[2] = 0;
        }
    }
    else {
        printf("(E) mamboFS::open: unsupported oflags=%08x\n", oflags);
	return CNK_RC_FAILURE(EINVAL);
    }
    
    // Open the file thru mambo.
    hostFD = (long) (MamboFileOpenExtended((char*)path, rw));
    if (-1 == hostFD) {
        // Assume no file since no way to get mambo error code.
	return CNK_RC_FAILURE(ENOENT);
    }
    
    // Allocate a descriptor for the file.
    localFD = File_AllocateFD(hostFD, FD_FILE);
    if (-1 != localFD){ 
        rc = CNK_RC_SUCCESS(localFD);
    }
    else {
        // assume no free local descriptor
        mamboFS::close((int)hostFD);
	rc = CNK_RC_FAILURE(EMFILE);
    }
    
    TRACE( TRACE_MamboFS, ("(I) mamboFS::open%s: path=\"%s\" flags=%s rc=%s\n", whoami(), path, rw, CNK_RC_STRING(rc)) );
    return rc;
}

uint64_t mamboFS::read(int fd, void *buff, size_t cnt)
{
    uint64_t rc;
    int remoteFD = File_GetRemoteFD(fd);
    int nbytes = MamboFileReadExtended((char*)buff, 1, cnt, (FILE*)remoteFD);
    if (nbytes == -1) {
	// Assume invalid parameters since no way to get mambo error code. 
	rc = CNK_RC_FAILURE(EINVAL);
    }
    else {
	rc = CNK_RC_SUCCESS(nbytes);
    }
    
    TRACE( TRACE_MamboFS, ("(I) mamboFS::read%s: fd=%d rc=%s\n", whoami(), fd, CNK_RC_STRING(rc)) );
    return rc;
}

uint64_t mamboFS::stat(const char *__restrict__ path, struct stat *__restrict__ statbuf)
{
    return stat64(path, (struct stat64 *)statbuf);
}

uint64_t mamboFS::stat64(const char *__restrict__ path, struct stat64 *__restrict__ statbuf)
{
    // Test for host file using simple interface which returns an error when file not found.
    if (MamboFileOpen((char*)path, 'r') < 0) {
        return CNK_RC_FAILURE(ENOENT); // assume error is no file
    }
    
    // Close the file now that we know it exists.
    MamboFileClose();
    
    // Set just enough to keep the runtime happy.
    memset(statbuf, 0, sizeof(struct stat));
    statbuf->st_blksize = 512;
    statbuf->st_mode = S_IRWXU | S_IRWXG | S_IRWXO | S_IFREG;
    statbuf->st_blocks = 1;
    statbuf->st_nlink = 1;
    statbuf->st_dev = 42;
    
    TRACE( TRACE_MamboFS, ("(I) mamboFS::stat64%s: path=%s rc=0\n", whoami(), path) );
    return CNK_RC_SUCCESS(0);
}

uint64_t mamboFS::unlink(const char* path)
{
    // Just give a good return to fake out FORTRAN.  There isn't a mambo interface available to use.
    // printf("(I) mamboFS::unlink%s: returned success for path %s\n", whoami(), path);
    
    TRACE( TRACE_MamboFS, ("(I) mamboFS::unlink%s: path=\"%s\" rc=0\n", whoami(), path) );
    return CNK_RC_SUCCESS(0);
}

uint64_t mamboFS::write(int fd, const void *buff, size_t cnt)
{
    uint64_t rc;
    
    // write of zero bytes is a special case.  no error.
    if (cnt == 0) {
	return CNK_RC_SUCCESS(0);
    }
    
    int remoteFD = File_GetRemoteFD(fd);
    int nbytes = MamboFileWriteExtended((char*)buff, 1, cnt, (FILE*)remoteFD);
    if (-1 == nbytes) {
        // Assume invalid parameters since no way to get mambo error code. 
	rc = CNK_RC_FAILURE(EINVAL);
    }
    else {
	rc = CNK_RC_SUCCESS(nbytes);
    }
    
    TRACE( TRACE_MamboFS, ("(I) mamboFS::write%s: fd=%d, rc=%s\n", whoami(), fd, CNK_RC_STRING(rc)) );
    return rc;
}

bool mamboFS::isMatch(const char *path)
{
   return ((IsAppAgent()) ? false : true);
}

//
//       The following is a generic dump routine for debug use
//
int dumphex(unsigned char *dmpPtr, int len) {
    
    while (len >= 16) {    // do 16 byte chunks
        printf("%.16llx    %x %x %x %x  %x %x %x %x  %x %x %x %x  %x %x %x %x \n",
               (long long unsigned int) dmpPtr,dmpPtr[0],dmpPtr[1],dmpPtr[2],dmpPtr[3],dmpPtr[4],dmpPtr[5],dmpPtr[6],dmpPtr[7],
               dmpPtr[8],dmpPtr[9],dmpPtr[10],dmpPtr[11],dmpPtr[12],dmpPtr[13],dmpPtr[14],dmpPtr[15]);
        dmpPtr +=16;
        len -= 16;
    }
    if (0 != len) {     // more to do
        printf("%.16llx    ",(long long unsigned int) dmpPtr);
        while (len >= 4) {       // 4 byte chunks
            printf("%x %x %x %x  ",dmpPtr[0],dmpPtr[1],dmpPtr[2],dmpPtr[3]);
            dmpPtr +=4;
            len -= 4;
        }
        while (len > 0) {     // left over bytes
            printf("%x ",dmpPtr[0]);
            dmpPtr++;
            len--;
        }
        printf(" \n");
    }
    return 0;
}   //dumphex

#endif

