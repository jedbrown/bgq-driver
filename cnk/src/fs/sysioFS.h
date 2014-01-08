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

#ifndef _CNK_SYSIOFS_H
#define _CNK_SYSIOFS_H

// Includes
#include "virtFS.h"
#include <ramdisk/include/services/MessageHeader.h>
#include <ramdisk/include/services/SysioMessages.h>
#include <ramdisk/include/services/UserMessages.h>
#include <cnk/include/Verbs.h>
#include <inttypes.h>
#include <errno.h>
#include <sys/types.h>
#include <spi/include/kernel/sendx.h>

      

class sysioFS : public virtFS
{

public:

   //! \brief  Default constructor.

   sysioFS(void) : virtFS() {};

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

   //! \brief  Check if pathname is match for this file system.
   //! \param  path Pathname to check.
   //! \return True (always since sysioFS is a default file system).

   bool isMatch(const char *path);

   uint64_t access(const char *pathname, int type);
   uint64_t accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen);
   uint64_t bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
   uint64_t chdir(const char *pathname);
   uint64_t chmod(const char *pathname, mode_t mode);
   uint64_t chown(const char *pathname, uid_t uid, gid_t gid);
   uint64_t close(int fd);
   uint64_t connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
   uint64_t fchown(int fd, uid_t uid, gid_t gid);
   uint64_t fchmod(int fd, mode_t mode);
   uint64_t fcntl(int fd, int cmd, uint64_t parm3);
   uint64_t fstat(int fd, struct stat *statbuf);
   uint64_t fstat64(int fd, struct stat64 *statbuf);
   uint64_t fstatfs(int fd, struct statfs *statbuf);
   uint64_t fstatfs64(int fd, struct statfs64 *statbuf);
   uint64_t ftruncate(int fd, off_t length);
   uint64_t ftruncate64(int fd, off64_t length);
   uint64_t fsync(int fd);
   uint64_t getdents(int fd, struct dirent *buffer, unsigned int length);
   uint64_t getdents64(int fd, struct dirent *buffer, unsigned int length);
   uint64_t getpeername(int sockfd, struct sockaddr *addr, socklen_t *addrlen);
   uint64_t getsockname(int sockfd, struct sockaddr *addr, socklen_t *addrlen);
   uint64_t getsockopt(int sockfd, int level, int optname, void *optval, socklen_t *optlen);
   uint64_t gpfsfcntl(int fd, const void* ptr, size_t length, int* gpfsresult);
   uint64_t ioctl(int fd, unsigned long int cmd, void *parm3);
   uint64_t lchown(const char *path, uid_t uid, gid_t gid);
   uint64_t link(const char *oldpathname, const char *newpathname);
   uint64_t listen(int sockfd, int backlog);
   uint64_t llseek(int fd, off64_t offset, off64_t *result, int whence);
   uint64_t lseek(int fd, off_t offset, int whence);
   uint64_t lstat(const char *__restrict__ path, struct stat  *__restrict__ statbuf);
   uint64_t lstat64(const char *path, struct stat64 *statbuf);
   uint64_t mkdir(const char *pathname, mode_t mode);
   uint64_t open(const char *pathname, int oflags, mode_t mode);
   uint64_t poll(struct pollfd *fds, nfds_t nfds, int timeout);
   uint64_t pread64(int fd, void *buffer, size_t length, off64_t position);
   uint64_t pwrite64(int fd, const void *buffer, size_t length, off64_t position);
   uint64_t read(int fd, void *buffer, size_t length);
   uint64_t readv(int fd, const struct iovec *iov, int iovcnt);
   uint64_t recv(int fd, void *buffer, size_t length, int flags);
   uint64_t recvfrom(int sockfd, void *buffer, size_t length, int flags, struct sockaddr *addr, socklen_t *addrlen);
   uint64_t readlink(const char *pathname, void *buffer, size_t length);
   uint64_t rename(const char *oldpathname, const char *newpathname);
   uint64_t rmdir(const char *pathname);
   uint64_t send(int sockfd, const void *buffer, size_t length, int flags);
   uint64_t sendto(int sockfd, const void *buffer, size_t length, int flags, const struct sockaddr *addr, socklen_t addrlen);
   uint64_t setsockopt(int sockfd, int level, int optname, const void *optval, socklen_t optlen);
   uint64_t shutdown(int sockfd, int how);
   uint64_t socket(int domain, int type, int protocol);
   uint64_t stat(const char *pathname, struct stat *statbuf);
   uint64_t stat64(const char *pathname, struct stat64 *statbuf);
   uint64_t statfs(const char *pathname, struct statfs *statbuf);
   uint64_t statfs64(const char *pathname, struct statfs64 *statbuf);
   uint64_t symlink(const char *oldpathname, const char *newpathname);
   uint64_t truncate(const char* pathname, off_t length);
   uint64_t truncate64(const char *pathname, off64_t length);
   uint64_t unlink(const char *pathname);
   uint64_t utime(const char *path, const struct utimbuf *buf);
   uint64_t write(int fd, const void *buffer, size_t length);
   uint64_t writev(int fd, const struct iovec *iov, int iovcnt);
   uint64_t writeImmediate(int fd, const void *buffer, size_t length);
   

   uint64_t getxattr(const char *path, const char *name, void *value, size_t size){
      return pathgetXattr(path, name, value, size, bgcios::sysio::PgetXattr);
   }
   uint64_t lgetxattr(const char *path, const char *name, void *value, size_t size){
      return pathgetXattr(path, name, value, size, bgcios::sysio::LgetXattr);
   }
   uint64_t pathgetXattr(const char *path, const char *name, void *value, size_t size, uint16_t type);
 
   uint64_t listxattr(const char *path, char *list, size_t size){
      return pathlistXattr(path, list,size, bgcios::sysio::PlistXattr);
   }
   uint64_t llistxattr(const char *path, char *list, size_t size){
      return pathlistXattr(path, list,size, bgcios::sysio::LlistXattr);
   }
   uint64_t pathlistXattr(const char *path, char *list, size_t size, uint16_t type);

   uint64_t setxattr(const char *path, const char *name, const void *value, size_t size, int flags){
      return pathsetXattr(path, name, value, size, flags, bgcios::sysio::PsetXattr);
   }
   uint64_t lsetxattr(const char *path, const char *name, const void *value, size_t size, int flags){
      return pathsetXattr(path, name, value, size, flags, bgcios::sysio::LsetXattr);
   }
   uint64_t pathsetXattr(const char *path, const char *name, const void *value, size_t size, int flags, uint16_t type);

   uint64_t lremovexattr(const char *path, const char *name){
      return pathRemoveXattr(path, name, bgcios::sysio::LremoveXattr);
   } 
   uint64_t removexattr(const char *path, const char *name){
      return pathRemoveXattr(path, name, bgcios::sysio::PremoveXattr);
   }  

   uint64_t pathRemoveXattr(const char *path, const char *name, uint16_t type);
   

   uint64_t fsetxattr(int fd, const char *name, const void *value, size_t size, int flags){
            return fxattr_setOrRemove(fd,name,bgcios::sysio::FsetXattr,value,size,flags);
   }
   uint64_t fgetxattr(int fd, const char *name, void *value, size_t size){
            return fxattr_retrieve(fd, name, bgcios::sysio::FgetXattr, value, size);
   }
   uint64_t fremovexattr(int fd, const char *name){
            return fxattr_setOrRemove(fd,name,bgcios::sysio::FremoveXattr,NULL,0,0);
   }  

   uint64_t flistxattr(int fd, char *list, size_t size){
            return fxattr_retrieve(fd, NULL, bgcios::sysio::FlistXattr, list, size);
   }

   uint64_t fxattr_setOrRemove(int fd, const char *name, uint16_t type, const void *value, size_t size, int flags);
   uint64_t fxattr_retrieve(int fd, const char *name, uint16_t type, void *target, size_t size);

  //! \brief Send User Message to sysiod with expected User message back 
  //! \param mInput is a char * alias for struct MsgInputs *
  //! \return Return code.
  int sendx(char * mInput);
private:

   //! \brief Initialize send message subregions in _outMessage 
   void initSendHeaders()
{
   
   for (int index=0;index< CONFIG_MAX_HWTHREADS  ;index++){
     bgcios::MessageHeader *header = (bgcios::MessageHeader *)((uint64_t)_outMessageRegion.addr + (index *  bgcios::ImmediateMessageSize));
     header->service = bgcios::SysioService;
     header->version = bgcios::sysio::ProtocolVersion;
     header->type = 0;
     header->rank = 0;
     header->sequenceId = 0;
     header->returnCode = 0;
     header->errorCode = 0;
     header->length = sizeof(bgcios::MessageHeader);
     header->jobId = 0;
   }
   return;
}

  //! \brief Send User Message to sysiod with expected User message back 
  //! \param mInput is a char * alias for struct MsgInputs *
  //! \return Return code.
int sendxDataOnly(struct MsgInputs * mInput);
int sendxDataPlus(struct MsgInputs * mInput);

  //! \brief Update user message to sysiod protocol needs 
  //! \param sendMessage is the user message needing update to the header to fulfill protocol considerations 
  //! \param Protocol version number.
  //! \param Type of message.
  //! \param Service to process message.
void fillUserHeader(bgcios::UserMessage * sendMessage, uint8_t  version=0, uint16_t type=0, uint8_t  service=bgcios::SysioUserService)
{
   sendMessage->header.service = service;
   sendMessage->header.version = version;
   sendMessage->header.type = type;
   sendMessage->header.rank = GetMyProcess()->Rank;
   uint32_t proc_ID = ProcessorID();    
   sendMessage->header.sequenceId = (proc_ID << 24)|(0x00FFFFFF&_procSequenceId[proc_ID]++);
   sendMessage->header.jobId = GetMyAppState()->JobID;

   return;
}


   //! \brief  Fill a message header with valid values.
   //! \param  header Pointer to message header.
   //! \param  type Message type value.
   //! \return Nothing.

void fillHeader(bgcios::MessageHeader *header, uint16_t type, size_t length)
{
   header->service = bgcios::SysioService;
   header->version = bgcios::sysio::ProtocolVersion;
   header->type = type;
   header->rank = GetMyProcess()->Rank;
   uint32_t proc_ID = ProcessorID();    
   header->sequenceId = (proc_ID << 24)|(0x00FFFFFF&_procSequenceId[proc_ID]++);
   header->length = (uint32_t)length;
   header->jobId = GetMyAppState()->JobID;
   return;
}

   //! \brief  Simple exchange of request and reply messages.
   //! \param  outMsg Pointer to outbound request message.
   //! \param  inMsg Pointer to address of inbound reply message.
   //! \return Return code.

   uint64_t exchange(void *outMsg, void **inMsg);
   uint64_t exchangeInline(void *outMsg, void **inMsg)
{
   // Build scatter/gather element for outbound request message.
   bgcios::MessageHeader *requestMsg = (bgcios::MessageHeader *)outMsg;
   uint64_t * data = (uint64_t * )outMsg;
   Kernel_WriteFlightLog(FLIGHTLOG, FL_SYSMSGSND, data[0],data[1],data[2],data[3] );
   struct cnv_sge sge;
   sge.addr = (uint64_t)outMsg;
   sge.length = requestMsg->length;
   sge.lkey = _outMessageRegion.lkey;
   uint64_t rc = exchangeMessages(&sge, 1, inMsg,requestMsg->sequenceId);
   return rc;
}

   //! \brief  Simple exchange of request (with one path name) and reply messages.
   //! \param  outMsg Pointer to outbound request message.
   //! \param  pathname Pointer to path name string.
   //! \param  inMsg Pointer to address of inbound reply message.
   //! \return Return code.

   uint64_t exchange(void *outMsg, const char *pathname, void **inMsg);

   //! \brief  Simple exchange of request (with two path names) and reply messages.
   //! \param  outMsg Pointer to outbound request message.
   //! \param  pathname1 Pointer to first path name string.
   //! \param  pathname2 Pointer to second path name string.
   //! \param  inMsg Pointer to address of inbound reply message.
   //! \return Return code.

   uint64_t exchange(void *outMsg, const char *pathname1, const char *pathname2, void **inMsg);

   //! \brief  Exchange request and reply messages with the I/O node.
   //! \param  sendList List of scatter/gather elements for data in request message.
   //! \param  sendListSize Number of elements in send sge list.
   //! \param  inMsg Pointer to address of inbound reply message.
   //! \param  sequenceID Match this sequenceID of the message
   //! \param  postsecond 0=no second receive posted, nonzero a second receive posted
   //! \return Return code.

   uint64_t exchangeMessages(cnv_sge *sendList, int sendListSize, void **inMsg,  uint32_t sequenceID);

   //! \brief  Get the address of the first receive buffer for this hardware thread.
   //! \return Address of receive buffer.
   //! \note   Each hardware thread is given its own area in the memory region for inbound messages.

   uint64_t getRecvBuffer(int index)
   {
      return (uint64_t)_inMessageRegion.addr + (index *  bgcios::ImmediateMessageSize);
   }

     uint64_t getSendBuffer(int index)
   {
      return (uint64_t)_outMessageRegion.addr + (index *  bgcios::ImmediateMessageSize);
   }


   //! \brief  Post a receive to the queue pair.
   //! \param  addr Address of memory region.
   //! \param  sequenceID Match this sequenceID of the message
   //! \return 0 when successful, errno when unsuccessful.

   int postRecv(uint64_t addr, uint32_t sequenceID);

   //! \brief  Post a send to the queue pair.
   //! \param  sendList List of scatter/gather elements for data in request message.
   //! \param  sendListSize Number of elements in send sge list.
   //! \param  sequenceID Match this sequenceID of the message
   //! \return 0 when successful, errno when unsuccessful.

   int postSend(cnv_sge *sendList, int listSize, uint32_t sequenceID);

   //! \brief Post an RDMA write to cn verbs
   //! \param  sgeList List of scatter/gather elements for data in request message.
   //! \param  listSize Number of elements in send sge list.
   //! \param  sequenceID Match this sequenceID of the message
   //! \param  virtual address of remote memory
   //! \param  rkey  rmemote key, the key for the remote memory region
   //! \return 0 when successful, errno when unsuccessful.

   int postRdmaWrite(struct cnv_sge *sgeList, int listSize, uint32_t sequenceID,uint64_t remoteAddr,uint32_t remoteKey);

   //! Sequence id for tracking message exchanges.

   uint32_t _procSequenceId[CONFIG_MAX_HWTHREADS];

   //! True when termination has completed.
   //! \note Required since this file system is used for multiple descriptor types.
   bool _isTerminated;

   //! Device context.
   cnv_context *_context;

   //! Protection domain.
   cnv_pd _protectionDomain;

   //! Completion queue.
   cnv_cq _completionQ;

   //! Queue pair connected to sysiod.
   cnv_qp _queuePair;

   //! Memory region for outbound messages.
   struct cnv_mr _outMessageRegion;

   //! Memory region for inbound messages.
   struct cnv_mr _inMessageRegion;

   uint64_t _rdmaBufferVirtAddr;  //!< RDMA buffer virtual address from last Setup message
   uint64_t _rdmaBufferLength;    //!< length of said buffer from last Setup message
   uint32_t _remotekey;                //!< memory key of the remote buffer, a/k/a remote key

   //! Lock for serializing message exchanges.
   Lock_Atomic_t _lock;
};

#endif // _CNK_SYSIOFS_H

