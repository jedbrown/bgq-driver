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
#include "shmFS.h"

#define DEVPERSIST_PREFIX "/dev/persist/"
#define DEVPERSIST_ROOT   "/dev/persist"

//  persist class - persistent  memory supported functions are defined here
//                implementation is in shmFS.cc
// Please keep functions in alphabetical order for ease of maintenance

class persistmemFS : public shmFS
{
public:
    persistmemFS(void) : shmFS() { };

    int init(void);
    int cleanupJob(int fs);
    bool isMatch(const char *path) { return strncmp(path, DEVPERSIST_PREFIX, strlen(DEVPERSIST_PREFIX)) == 0 ? true : false; }
    virtual ShmMgr_t* getShmManager() { return &(NodeState.PersistentMemory); };
    virtual uint64_t  getDeviceID()   { return 0x01021995; };
};

