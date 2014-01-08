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

#ifndef _CNK_RDMAFS_H
#define _CNK_RDMAFS_H

// Includes
#include "virtFS.h"
#include <ramdisk/include/services/MessageHeader.h>
#include <ramdisk/include/services/SysioMessages.h>
#include <cnk/include/Verbs.h>
#include <inttypes.h>
#include <errno.h>
#include <sys/types.h>
#include "spi/include/kernel/rdma.h"
#include <netinet/in.h>

      

class rdmaFS: public virtFS
{

public:

   //! \brief  Default constructor.

   rdmaFS(void) : virtFS()
{
    _fd=0;
    _destination_port = 0;
    clearUserRegions();

}

   //! \brief  Initialize file system when node is booted.
   //! \return 0 when successful, errno when unsuccessful.

   int init(void);

   //! \brief  Terminate file system when node is shutdown.
   //! \return 0 when successful, errno when unsuccessful.

   int term(void);

   //! \brief  Setup before running a job.
   //! \param  fs File system type.
   //! \return 0 when successful, errno when unsuccessful.

   int setupJob(int fs){
          return 0;
   }

   //! \brief  Cleanup after running a job.
   //! \param  fs File system type.
   //! \return 0 when successful, errno when unsuccessful.

   int cleanupJob(int fs){
          return 0;
   }

   //list of virtual functions in this derived class
   uint64_t close(int fd);
   uint64_t recv(int fd, void *buffer, size_t length, int lkey);
   uint64_t send(int fd, const void *buffer, size_t length, int lkey);
   uint64_t connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
   uint64_t socket(int domain, int type, int protocol);

   uint64_t registerMemory(int fd,void * usingRegion4RDMA);
   uint64_t deregisterMemory(int fd,void * usingRegion4RDMA);

   uint64_t pollCQ(int fd, unsigned long int num_entries, Kernel_RDMAWorkCompletion_t* WorkCompletionList);

   uint64_t ioctl(int fd, unsigned long int num_entries, void *parm3){
       Kernel_RDMAWorkCompletion_t* WorkCompletionList =  (Kernel_RDMAWorkCompletion_t*)parm3;
       return pollCQ(fd, num_entries, WorkCompletionList);
   }


protected:
private:
 int _fd;
 int _destination_port;
 struct sockaddr_in _destAddress;

    //! True when termination has completed.
   //! \note Required since this file system is used for multiple descriptor types.
   bool _isTerminated;

   //! Device context.
   cnv_context *_context;

   //! Protection domain.
   cnv_pd _protectionDomain;

   //! Completion queue.
   cnv_cq _completionQ;

   //! Queue pair 
   cnv_qp _queuePair;

   //! Memory region for outbound messages.
   struct cnv_mr _outMessageRegion;

   //! Memory region for inbound messages.
   struct cnv_mr _inMessageRegion;

   struct cnv_mr _userRegion[USER_MAX_RDMA_REGIONS];

   struct cnv_mr * findFreeUserRegion(){
     for (int i=0; i<USER_MAX_RDMA_REGIONS;i++){
       if (_userRegion[i].lkey == 0) return &_userRegion[i];
     }
     return NULL;
   }
   void freeUserRegion(struct cnv_mr * userRegion){ userRegion->lkey=0;}
   void clearUserRegions(){memset(_userRegion,0,sizeof(struct cnv_mr)* USER_MAX_RDMA_REGIONS);}

   struct cnv_mr * findUserRegion(uint64_t lkey){
     for (int i=0; i<USER_MAX_RDMA_REGIONS;i++){
       if (_userRegion[i].lkey == lkey) return &_userRegion[i];
     }
     return NULL;
   }

   void deregisterAllRegisteredMemory(){
     for (int i=0; i<USER_MAX_RDMA_REGIONS;i++){
       if (_userRegion[i].lkey){
          int err = cnv_dereg_mr(&_userRegion[i]);
          if (err != 0) 
            Kernel_WriteFlightLog(FLIGHTLOG, FL_CNVDRGMRE, (uint64_t)_userRegion[i].addr, _userRegion[i].length, _userRegion[i].lkey, err);
       }
     }
   }

};

#endif // _CNK_RDMAFS_H

