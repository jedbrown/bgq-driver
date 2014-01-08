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

#ifndef _CNK_STDIOFS_H
#define _CNK_STDIOFS_H

// Includes
#include "virtFS.h"
#include <ramdisk/include/services/MessageHeader.h>
#include <cnk/include/Verbs.h>

//! \brief Standard I/O file system.

class stdioFS : public virtFS
{
public:

   //! \brief  Default constructor.

   stdioFS(void) : virtFS() {};

   //! \brief  Initialize file system when node is booted.
   //! \return 0 when successful, errno when unsuccessful.

   int init(void);

   //! \brief  Terminate file system when node is shutdown.
   //! \return 0 when successful, errno when unsuccessful.

   int term(void);

   //! \brief  Setup before running a job.
   //! \param  fs File system type.
   //! \return 0 when successful, errno when unsuccessful.

   int setupJob(int fs);

   //! \brief  Cleanup after running a job.
   //! \param  fs File system type.
   //! \return 0 when successful, errno when unsuccessful.

   int cleanupJob(int fs);

   // Please keep methods in alphabetical order for ease of maintenance.

   uint64_t close(int fd);
   uint64_t fcntl(int fd, int cmd, uint64_t parm3);
   uint64_t fstat(int fd, struct stat *statbuf);
   uint64_t fstat64(int fd, struct stat64 *statbuf);
   uint64_t ioctl(int fd, unsigned long int cmd, void *parm3);
   uint64_t llseek(int fd, off64_t offset, off64_t *result, int whence);
   uint64_t lseek(int fd, off_t offset, int whence);
   uint64_t read(int fd, void *buffer, size_t length);
   uint64_t write(int fd, const void *buffer, size_t length);
   uint64_t writev(int fd, const struct iovec *iov, int iovcnt);

private:

   //! \brief  Fill a message header with valid values.
   //! \param  header Pointer to message header.
   //! \param  type Message type value.
   //! \return Nothing.

   void fillHeader(bgcios::MessageHeader *header, uint16_t type);

   int exchangeMessages(struct cnv_sge *recvList, int numRecvElements, struct cnv_sge *sendList, int numSendElements);

   //! Sequence id for tracking message exchanges.
   uint32_t _sequenceId;

   //! True when termination has completed.
   //! \note Required since this file system is used for multiple descriptor types.
   bool _isTerminated;

   //! Device context.
   cnv_context *_context;

   //! Protection domain.
   cnv_pd _protectionDomain;

   //! Completion queue.
   cnv_cq _completionQ;

   //! Queue pair connected to stdiod.
   cnv_qp _queuePair;

   //! Memory region for outbound messages.
   struct cnv_mr _outMessageRegion;

   //! Memory region for inbound messages.
   struct cnv_mr _inMessageRegion;

   //! Lock to control access to message buffers.
   Lock_Atomic_t _lock;
};

#endif // _CNK_STDIOFS_H


