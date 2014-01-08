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
#if __KERNEL__
#include "virtFS.h"
#endif

#define DEVMEMFS_PREFIX "/dev/cnkmemfs/"
#define DEVMEMFS_ROOT   "/dev/cnkmemfs"

#define MEMFS_ID "cnkmemfs"
#define NUM_FILES 64
#define NAME_LEN 256
#define PAD_SIZE  8-((NAME_LEN+4)%8)

// memFS directory entry format
typedef struct memFSentry_t {
    uint32_t nameLen;
    char name[NAME_LEN];
    char pad[PAD_SIZE];
    uint32_t fileOffset;
    uint32_t fileSize;
} memFS_entry_t;

// memFS header - system information and directory
typedef struct memFSdata_t {
    char id[8];
    uint32_t totalFSsize;
    uint32_t totalFSfiles;
    struct memFSentry_t entry[NUM_FILES];
} memFSdata_t;

#if __KERNEL__
//  mem class - mem supported functions are defined here
//                implementation is in memFS.cc
//
// Please keep functions in alphabetical order for ease of maintenance
//
class memFS : public virtFS 
{
public:

    //! \brief  Default constructor.

    memFS() : virtFS(), _baseAddr(NULL) { };

    //! \brief  Initialize file system when node is booted.
    //! \return 0 when successful.

    int init(void);

    //! \brief  Setup before running a job.
    //! \param  fs File system type.
    //! \return 0 when successful, -1 when unsuccessful.

    int setupJob(int fs);

    //! \brief  Check if pathname is match for this file system.
    //! \param  path Pathname to check.
    //! \return True if pathname is a match, otherwise false.

    bool isMatch(const char *path);

    uint64_t chdir(const char *path);
    uint64_t close(int fd);
    uint64_t fstat(int fd, struct stat *statbuf);
    uint64_t lseek(int fd, off_t offset, int whence);
    uint64_t open(const char *path, int oflags, mode_t mode);
    uint64_t read(int fd, void *buffer, size_t length);
    uint64_t pread64(int fd, void *buffer, size_t length, off64_t position);
    uint64_t stat(const char *path, struct stat *statbuf);
    
private:

    //! \brief  Find file from name.
    //! \return Index to entry for file when found, -1 when file is not found.

    int findfile(const char* fileName);

    //! \brief  Get pointer to base address of file system header.
    //! \return Pointer to memFSdata structure.

    memFSdata_t *getBaseAddr(void);

    //! \brief  Fill in stat structure.
    //! \param  index Index to entry for file.
    //! \return Nothing.

    void fillStatStructure(struct stat *statbuf, int index);

    //! Pointer to storage that contains file system.
    memFSdata_t* _baseAddr;
};

#endif

