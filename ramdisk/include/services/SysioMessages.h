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
/* (C) Copyright IBM Corp.  2011, 2012                              */
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

//! \file  SysioMessages.h
//! \brief Declarations for bgcios::sysio message classes.

#ifndef SYSIOMESSAGETYPES_H
#define SYSIOMESSAGETYPES_H

// Includes
#include <ramdisk/include/services/MessageHeader.h>
#include <sys/stat.h>
#include <sys/statfs.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <inttypes.h>
#include <termios.h>
#include <poll.h>

#ifdef __cplusplus
namespace bgcios
{

namespace sysio
{
#endif

const uint16_t ErrorAck           = 4000;
const uint16_t Access             = 4001;
const uint16_t AccessAck          = 4002;
const uint16_t Bind               = 4003;
const uint16_t BindAck            = 4004;
const uint16_t Chmod              = 4005;
const uint16_t ChmodAck           = 4006;
const uint16_t Chown              = 4007;
const uint16_t ChownAck           = 4008;
const uint16_t Close              = 4009;
const uint16_t CloseAck           = 4010;
const uint16_t Fchmod             = 4011;
const uint16_t FchmodAck          = 4012;
const uint16_t Fchown             = 4013;
const uint16_t FchownAck          = 4014;
const uint16_t Fstat64            = 4015;
const uint16_t Fstat64Ack         = 4016;
const uint16_t Fstatfs64          = 4017;
const uint16_t Fstatfs64Ack       = 4018;
const uint16_t Ftruncate64        = 4019;
const uint16_t Ftruncate64Ack     = 4020;
const uint16_t Fsync              = 4021;
const uint16_t FsyncAck           = 4022;
const uint16_t Getdents64         = 4023;
const uint16_t Getdents64Ack      = 4024;
const uint16_t Getpeername        = 4025;
const uint16_t GetpeernameAck     = 4026;
const uint16_t Getsockname        = 4027;
const uint16_t GetsocknameAck     = 4028;
const uint16_t Getsockopt         = 4029;
const uint16_t GetsockoptAck      = 4030;
const uint16_t Ioctl              = 4031;
const uint16_t IoctlAck           = 4032;
const uint16_t Link               = 4033;
const uint16_t LinkAck            = 4034;
const uint16_t Listen             = 4035;
const uint16_t ListenAck          = 4036;
const uint16_t Lseek64            = 4037;
const uint16_t Lseek64Ack         = 4038;
const uint16_t Mkdir              = 4039;
const uint16_t MkdirAck           = 4040;
const uint16_t Open               = 4041;
const uint16_t OpenAck            = 4042;
const uint16_t Readlink           = 4043;
const uint16_t ReadlinkAck        = 4044;
const uint16_t Rename             = 4045;
const uint16_t RenameAck          = 4046;
const uint16_t Resolvepath        = 4047;
const uint16_t ResolvepathAck     = 4048;
const uint16_t Setsockopt         = 4049;
const uint16_t SetsockoptAck      = 4050;
const uint16_t Shutdown           = 4051;
const uint16_t ShutdownAck        = 4052;
const uint16_t Socket             = 4053;
const uint16_t SocketAck          = 4054;
const uint16_t Stat64             = 4055;
const uint16_t Stat64Ack          = 4056;
const uint16_t Statfs64           = 4057;
const uint16_t Statfs64Ack        = 4058;
const uint16_t Symlink            = 4059;
const uint16_t SymlinkAck         = 4060;
const uint16_t Truncate64         = 4061;
const uint16_t Truncate64Ack      = 4062;
const uint16_t Unlink             = 4063;
const uint16_t UnlinkAck          = 4064;//x0FE0
const uint16_t Utimes             = 4065;
const uint16_t UtimesAck          = 4066;
const uint16_t Accept             = 4067;
const uint16_t AcceptAck          = 4068;
const uint16_t Connect            = 4069;
const uint16_t ConnectAck         = 4070;
const uint16_t Fcntl              = 4071;
const uint16_t FcntlAck           = 4072;
const uint16_t Poll               = 4073;
const uint16_t PollAck            = 4074;

const uint16_t WriteImmediate     = 4075;
const uint16_t WriteImmediateAck  = 4076;

const uint16_t Pread64            = 4077;
const uint16_t Pread64Ack         = 4078;
const uint16_t Pwrite64           = 4079;
const uint16_t Pwrite64Ack        = 4080;//x0FF0
const uint16_t Read               = 4081;
const uint16_t ReadAck            = 4082;
const uint16_t Recv               = 4083;
const uint16_t RecvAck            = 4084;
const uint16_t Recvfrom           = 4085;
const uint16_t RecvfromAck        = 4086;
const uint16_t Send               = 4087;
const uint16_t SendAck            = 4088;
const uint16_t Sendto             = 4089;
const uint16_t SendtoAck          = 4090;
const uint16_t Write              = 4091;
const uint16_t WriteAck           = 4092;

const uint16_t FsetXattr          = 4912; //x1330
const uint16_t FsetXattrAck       = 4913;
const uint16_t FgetXattr          = 4914;
const uint16_t FgetXattrAck       = 4915;
const uint16_t FremoveXattr       = 4916;
const uint16_t FremoveXattrAck    = 4917;
const uint16_t FlistXattr         = 4918;
const uint16_t FlistXattrAck      = 4919;

const uint16_t LsetXattr          = 4922; //x133A
const uint16_t LsetXattrAck       = 4923;
const uint16_t LgetXattr          = 4924;
const uint16_t LgetXattrAck       = 4925;
const uint16_t LremoveXattr       = 4926;
const uint16_t LremoveXattrAck    = 4927;
const uint16_t LlistXattr         = 4928; //x1340
const uint16_t LlistXattrAck      = 4929;

const uint16_t PsetXattr          = 4932; //x1344
const uint16_t PsetXattrAck       = 4933;
const uint16_t PgetXattr          = 4934;
const uint16_t PgetXattrAck       = 4935;
const uint16_t PremoveXattr       = 4936;
const uint16_t PremoveXattrAck    = 4937;
const uint16_t PlistXattr         = 4938; 
const uint16_t PlistXattrAck      = 4939;

const uint16_t GpfsFcntl          = 4940;
const uint16_t GpfsFcntlAck       = 4941;

const uint16_t KINTERNALBIT         = 0x8000;
const uint16_t WriteImmediateKernelInternal  = WriteImmediate | KINTERNALBIT;
const uint16_t WriteKernelInternal  = 4091 | KINTERNALBIT;
const uint16_t OpenKernelInternal   = 4041 | KINTERNALBIT;
const uint16_t CloseKernelInternal  = 4009 | KINTERNALBIT;
const uint16_t SetupJob             = 4093 | KINTERNALBIT;
const uint16_t SetupJobAck          = 4094 | KINTERNALBIT;
const uint16_t CleanupJob           = 4095 | KINTERNALBIT;
const uint16_t CleanupJobAck        = 4096 | KINTERNALBIT;

//! Request message for chmod system call.

struct FsetOrRemoveXattrMessage
{
   struct MessageHeader header;        //!< Message header.
   int fd;                             //!< file descriptor.
   int flags;                          //!< flags for set
   size_t valueSize;                   //!< Size of value and offset to attribute name string
   int nameSize;                       //!<Size of attribute name
   char value[0];                      //!<Mark location of value for attribute;
   // char value[valueSize]
   // char name[nameSize]
};

struct FretrieveXattrMessage
{
   struct MessageHeader header;        //!< Message header.
   uint64_t address;                   //!< Caller provided area for retrieval
   uint32_t rkey;                      //!< rkey of memory region
   uint64_t userListNumBytes;          //!< Caller area size in bytes
   int fd;                             //!< file descriptor if fd operation
   int nameSize;                       //!<Size of attribute name (nonzero if appended)
   int pathSize;                       //!<Size of pathName (nonzero if appended)
   char pathname[0];                   //!<Path if pathname, appended
   char name[0];                       //!<Name of attribute if get, appended to data
};

struct FxattrMessageAck
{
   struct MessageHeader header;        //!< Message header.
   ssize_t returnValue;
};

struct PopXattrMessage
{
   struct MessageHeader header;        //!< Message header.
   int pathNameSize;                   //!< Size of pathName
   int flags;                          //!< flags for set
   int valueSize;                      //!< Size of value and offset to attribute name string
   int nameSize;                       //!<Size of attribute name
   char value[0];                      //!<Mark location of value for attribute;
   // char value[valueSize]
   // char name[nameSize]
   // char name[pathNameSize]
};

struct PathRemoveXattrMessage
{
   struct MessageHeader header;        //!< Message header.
   int nameSize;                       //!<Size of attribute name (nonzero if appended)
   int pathSize;                       //!<Size of pathName (nonzero if appended)
   char pathname[0];                       //!< 
   char name[0];
};

struct PathSetXattrMessage
{
   struct MessageHeader header;        //!< Message header.
   int flags;                          //!< flags for set
   size_t valueSize;                   //!< Size of value and offset to attribute name string
   int nameSize;                       //!<Size of attribute name (nonzero if appended)
   int pathSize;                       //!<Size of pathName (nonzero if appended)
   char value[0];                      //!<Mark location of value for attribute
   char pathname[0];                   //!< 
   char name[0];
};

//! Base port number for RDMA connections.
const uint16_t BaseRdmaPort = 7102;

//! Current version of protocol.
const uint8_t ProtocolVersion = 10;

//! Maximum number of secondary groups.
const int MaxGroups = 64;

//! Maximum length of a socket address.
const socklen_t MaxAddrLength = 128;

//! Maximum length of socket option.
const socklen_t MaxOptionLength = 100;

//! Maximum length of an ioctl data buffer.
const uint32_t MaxIoctlBufferLength = 256;

//! Maximum number of file descriptors to poll
const uint32_t PollSize = 50;

//! \note All acknowledge messages have a maximum size of 512 bytes (the size of a torus packet).
//!       If additional data needs to be returned as a part of the acknowledgement, it must be
//!       sent separately using a DMA operation.

//! Message to acknowledge a message that is in error.

struct ErrorAckMessage
{
   struct MessageHeader header;        //!< Message header.
};

//! Request message for access system call.

struct AccessMessage
{
   struct MessageHeader header;        //!< Message header.
   int dirfd;                          //!< Working directory descriptor.
   int type;                           //!< Type of access to check.
   int flags;                          //!< Control flags. 
   int offset;                         //!< Offset to path name in message.
};

//! Response message for access system call.

struct AccessAckMessage
{
   struct MessageHeader header;        //!< Message header.
};

//! Request message for bind system call.

struct BindMessage
{
   struct MessageHeader header;        //!< Message header.
   int sockfd;                         //!< Socket descriptor.
   char addr[MaxAddrLength];           //!< Address to bind to socket.
   socklen_t addrlen;                  //!< Length of address.
};

//! Response message for bind system call.

struct BindAckMessage
{
   struct MessageHeader header;        //!< Message header.
};

//! Request message for chmod system call.

struct ChmodMessage
{
   struct MessageHeader header;        //!< Message header.
   int dirfd;                          //!< Working directory descriptor.
   mode_t mode;                        //!< New permissions for file.
   int flags;                          //!< Control flags. 
   int offset;                         //!< Offset to path name in message.
};

//! Response message for chmod system call.

struct ChmodAckMessage
{
   struct MessageHeader header;        //!< Message header.
};

//! Request message for chown system call.

struct ChownMessage
{
   struct MessageHeader header;        //!< Message header.
   int dirfd;                          //!< Working directory descriptor.
   uid_t uid;                          //!< New user id.
   gid_t gid;                          //!< New group id.
   int flags;                          //!< Control flags. 
   int offset;                         //!< Offset to path name in message.
};

//! Response message for chown system call.

struct ChownAckMessage
{
   struct MessageHeader header;        //!< Message header.
};

//! Request message for close system call.

struct CloseMessage
{
   struct MessageHeader header;        //!< Message header.
   int fd;                             //!< File descriptor.
};

//! Response message for close system call.

struct CloseAckMessage
{
   struct MessageHeader header;        //!< Message header.
   int fd;                             //!< File descriptor.
   mode_t mode;                        //!< New permissions for file.
};

//! Request message for fchmod system call.

struct FchmodMessage
{
   struct MessageHeader header;        //!< Message header.
   int fd;                             //!< File descriptor.
   mode_t mode;                        //!< New permissions for file.
};

//! Response message for fchmod system call.

struct FchmodAckMessage
{
   struct MessageHeader header;        //!< Message header.
};

//! Request message for fchown system call.

struct FchownMessage
{
   struct MessageHeader header;        //!< Message header.
   int fd;                             //!< File descriptor.
   uid_t uid;                          //!< New user id.
   gid_t gid;                          //!< New group id.
};

//! Response message for fchown system call.

struct FchownAckMessage
{
   struct MessageHeader header;        //!< Message header.
};

//! Request message for fstat64 system call.

struct Fstat64Message
{
   struct MessageHeader header;        //!< Message header.
   int fd;                             //!< File descriptor.
};

//! Response message for fstat64 system call.

struct Fstat64AckMessage
{
   struct MessageHeader header;        //!< Message header.
   struct stat64 buf;                  //!< Buffer for stat structure.
};

//! Request message for fstatfs64 system call.

struct Fstatfs64Message
{
   struct MessageHeader header;        //!< Message header.
   int fd;                             //!< File descriptor.
};

//! Response message for fstatfs64 system call.

struct Fstatfs64AckMessage
{
   struct MessageHeader header;        //!< Message header.
   struct statfs64 buf;                //!< Buffer for statfs structure.
};

//! Request message for ftruncate64 system call.

struct Ftruncate64Message
{
   struct MessageHeader header;        //!< Message header.
   int fd;                             //!< File descriptor.
   off64_t length;                     //!< Length to truncate to.
};

//! Reply message for ftruncate64 system call.

struct Ftruncate64AckMessage
{
   struct MessageHeader header;        //!< Message header.
};

//! Request message for fsync system call.

struct FsyncMessage
{
   struct MessageHeader header;        //!< Message header.
   int fd;                             //!< File descriptor.
};

//! Response message for fsync system call.

struct FsyncAckMessage
{
   struct MessageHeader header;        //!< Message header.
};

//! Request message for getdents64 system call.

struct Getdents64Message
{
   struct MessageHeader header;        //!< Message header.
   int fd;                             //!< File descriptor.
   size_t length;                      //!< Length of buffer for directory entries.
   uint64_t address;                   //!< Address of buffer.
   uint32_t rkey;                      //!< Remote key of memory region.
};

//! Response message for getdents64 system call.

struct Getdents64AckMessage
{
   struct MessageHeader header;        //!< Message header.
   ssize_t bytes;                      //!< Amount of data returned in buffer.
};

//! Request message for getpeername system call.

struct GetpeernameMessage
{
   struct MessageHeader header;        //!< Message header.
   int sockfd;                         //!< Socket descriptor.
   socklen_t addrlen;                  //!< Length of address.
};

//! Response message for getpeername system call.

struct GetpeernameAckMessage
{
   struct MessageHeader header;        //!< Message header.
   char addr[MaxAddrLength];           //!< Address of peer socket.
   socklen_t addrlen;                  //!< Length of address.
};

//! Request message for getsockname system call.

struct GetsocknameMessage
{
   struct MessageHeader header;        //!< Message header.
   int sockfd;                         //!< Socket descriptor.
   socklen_t addrlen;                  //!< Length of address.
};

//! Response message for getsockname system call.

struct GetsocknameAckMessage
{
   struct MessageHeader header;        //!< Message header.
   char addr[MaxAddrLength];           //!< Address of socket.
   socklen_t addrlen;                  //!< Length of address.
};

//! Request message for getsockopt system call.

struct GetsockoptMessage
{
   struct MessageHeader header;        //!< Message header.
   int sockfd;                         //!< Socket descriptor.
   int level;                          //!< Protocol level.  
   int name;                           //!< Option name.
   socklen_t optlen;                   //!< Length of option buffer.
};

//! Response message for getsockopt system call.

struct GetsockoptAckMessage
{
   struct MessageHeader header;        //!< Message header.
   socklen_t optlen;                   //!< Length of option.
   char value[MaxOptionLength];        //!< Buffer with option value.
};

//! Request message for ioctl system call.

struct IoctlMessage
{
   struct MessageHeader header;        //!< Message header.
   int fd;                             //!< File descriptor.
   unsigned long int cmd;              //!< Command.
   int arg;                            //!< Argument for command.
};

//! Response message for ioctl system call.

struct IoctlAckMessage
{
   struct MessageHeader header;        //!< Message header.
   int retval;                         //!< Return value (depends on command).
   int arg;                            //!< Argument for command.
   struct termios termios;             //!< Terminal I/O interface structure.
};

//! Request message for link system call.

struct LinkMessage
{
   struct MessageHeader header;        //!< Message header.
   int olddirfd;                       //!< Working directory descriptor for old path.
   int newdirfd;                       //!< Working directory descriptor for new path.
   int flags;                          //!< Control flags. 
   int oldoffset;                      //!< Offset to old path name in message.
   int newoffset;                      //!< Offset to new path name in message.
};

//! Response message for link system call.

struct LinkAckMessage
{
   struct MessageHeader header;        //!< Message header.
};

//! Request message for listen system call.

struct ListenMessage
{
   struct MessageHeader header;        //!< Message header.
   int sockfd;                         //!< Socket descriptor.
   int backlog;                        //!< Number of connections in listen queue.
};

//! Response message for listen system call.

struct ListenAckMessage
{
   struct MessageHeader header;        //!< Message header.
};

//! Request message for lseek64 system call.

struct Lseek64Message
{
   struct MessageHeader header;        //!< Message header.
   int fd;                             //!< File descriptor.
   off64_t offset;                     //!< File offset to seek to.
   int whence;                         //!< Starting point for seek operation.
};

//! Reply message for lseek64 system call.

struct Lseek64AckMessage
{
   struct MessageHeader header;        //!< Message header.
   off64_t result;                     //!< Resulting file offset.
};

//! Request message for mkdir system call.

struct MkdirMessage
{
   struct MessageHeader header;        //!< Message header.
   int dirfd;                          //!< Working directory descriptor.
   mode_t mode;                        //!< Permissions for new directory.
   int offset;                         //!< Offset to path name in message.
};

//! Response message for mkdir system call.

struct MkdirAckMessage
{
   struct MessageHeader header;        //!< Message header.
};

//! Request message for open system call.

struct OpenMessage
{
   struct MessageHeader header;        //!< Message header.
   int dirfd;                          //!< Working directory descriptor.
   int flags;                          //!< File creation and file status flags.
   mode_t mode;                        //!< Permissions when creating new file.
   int offset;                         //!< Offset to path name in message.
};

//! Response message for open system call.

struct OpenAckMessage
{
   struct MessageHeader header;        //!< Message header.
   int fd;                             //!< File descriptor.
};

//! Request message for readlink system call.

struct ReadlinkMessage
{
   struct MessageHeader header;        //!< Message header.
   int dirfd;                          //!< Working directory descriptor.
   size_t length;                      //!< Length of buffer.
   uint64_t address;                   //!< Address of buffer for returned path name.
   uint32_t rkey;                      //!< Remote key of memory region.
   int offset;                         //!< Offset to path name in message.
};

//! Response message for readlink system call.

struct ReadlinkAckMessage
{
   struct MessageHeader header;        //!< Message header.
   ssize_t length;                     //!< Length of path name.
};

//! Request message for rename system call.

struct RenameMessage
{
   struct MessageHeader header;        //!< Message header.
   int olddirfd;                       //!< Working directory descriptor for old path.
   int newdirfd;                       //!< Working directory descriptor for new path.
   int flags;                          //!< Control flags. 
   int oldoffset;                      //!< Offset to old path name in message.
   int newoffset;                      //!< Offset to new path name in message.
};

//! Response message for rename system call.

struct RenameAckMessage
{
   struct MessageHeader header;        //!< Message header.
};

//! Request message for setsockopt system call.

struct SetsockoptMessage
{
   struct MessageHeader header;        //!< Message header.
   int sockfd;                         //!< Socket descriptor.
   int level;                          //!< Protocol level.  
   int name;                           //!< Option name.
   socklen_t optlen;                   //!< Length of option.
   char value[MaxOptionLength];        //!< Buffer with option value.
};

//! Response message for setsockopt system call.

struct SetsockoptAckMessage
{
   struct MessageHeader header;        //!< Message header.
};

//! Request message for shutdown system call.

struct ShutdownMessage
{
   struct MessageHeader header;        //!< Message header.
   int sockfd;                         //!< Socket descriptor.
   int how;                            //!< Type of shutdown.
};

//! Response message for shutdown system call.

struct ShutdownAckMessage
{
   struct MessageHeader header;        //!< Message header.
};

//! Request message for socket system call.

struct SocketMessage
{
   struct MessageHeader header;        //!< Message header.
   int domain;                         //!< Communication domain.
   int type;                           //!< Type of communication semantics.
   int protocol;                       //!< Protocol.
};

//! Response message for socket system call.

struct SocketAckMessage
{
   struct MessageHeader header;        //!< Message header.
   int sockfd;                         //!< Socket descriptor.
};

//! Request message for stat64 system call.

struct Stat64Message
{
   struct MessageHeader header;        //!< Message header.
   int dirfd;                          //!< Working directory descriptor.
   int flags;                          //!< Control flags. 
   int offset;                         //!< Offset to path name in message.
};

//! Response message for stat64 system call.

struct Stat64AckMessage
{
   struct MessageHeader header;        //!< Message header.
   struct stat64 buf;                  //!< Buffer for stat structure.
};

//! Request message for statfs64 system call.

struct Statfs64Message
{
   struct MessageHeader header;        //!< Message header.
   int dirfd;                          //!< Working directory descriptor.
   int offset;                         //!< Offset to path name in message.
};

//! Response message for statfs64 system call.

struct Statfs64AckMessage
{
   struct MessageHeader header;        //!< Message header.
   struct statfs64 buf;                //!< Buffer for statfs structure.
};

//! Request message for symlink system call.

struct SymlinkMessage
{
   struct MessageHeader header;        //!< Message header.
   int newdirfd;                       //!< Working directory descriptor for new path.
   int oldoffset;                      //!< Offset to old path name in message.
   int newoffset;                      //!< Offset to new path name in message.
};

//! Response message for symlink system call.

struct SymlinkAckMessage
{
   struct MessageHeader header;        //!< Message header.
};

//! Request message for truncate64 system call.

struct Truncate64Message
{
   struct MessageHeader header;        //!< Message header.
   int dirfd;                          //!< Working directory descriptor.
   off64_t length;                     //!< Length to truncate file to.
   int offset;                         //!< Offset to path name in message.
};

//! Response message for truncate64 system call.

struct Truncate64AckMessage
{
   struct MessageHeader header;        //!< Message header.
};

//! Request message for unlink system call.

struct UnlinkMessage
{
   struct MessageHeader header;        //!< Message header.
   int dirfd;                          //!< Working directory descriptor.
   int flags;                          //!< Control flags. 
   int offset;                         //!< Offset to path name in message.
};

//! Response message for unlink system call.

struct UnlinkAckMessage
{
   struct MessageHeader header;        //!< Message header.
};

//! Request message for utimes system call.

struct UtimesMessage
{
   struct MessageHeader header;        //!< Message header.
   int dirfd;                          //!< Working directory descriptor.
   struct timeval newtimes[2];         ///! New values for access time and modification time.
   bool now;                           ///! True to use current time (newtimes field is ignored).
   int offset;                         //!< Offset to path name in message.
};

//! Response message for utimes system call.

struct UtimesAckMessage
{
   struct MessageHeader header;        //!< Message header.
};

//! Request message for accept system call.

struct AcceptMessage
{
   struct MessageHeader header;        //!< Message header.
   int sockfd;                         //!< Socket descriptor.
   char addr[MaxAddrLength];           //!< Address to connect socket to.
   socklen_t addrlen;  
};

//! Response message for accept system call.

struct AcceptAckMessage
{
   struct MessageHeader header;        //!< Message header.
   int sockfd;                         //!< Socket descriptor.
   char addr[MaxAddrLength];           //!< Address to connect socket to.
   socklen_t addrlen;  
};


//! Request message for connect system call.

struct ConnectMessage
{
   struct MessageHeader header;        //!< Message header.
   int sockfd;                         //!< Socket descriptor.
   char addr[MaxAddrLength];           //!< Address to connect socket to.
   socklen_t addrlen;                  //!< Length of address.
};

//! Response message for connect system call.

struct ConnectAckMessage
{
   struct MessageHeader header;        //!< Message header.
};

//! Request message for fcntl system call.

struct FcntlMessage
{
   struct MessageHeader header;        //!< Message header.
   int fd;                             //!< File descriptor.
   int cmd;                            //!< Command.
   int arg;                            //!< Argument for command.
   struct flock lock;                  //!< Lock structure for advisory lock commands.
};

//! Response message for fcntl system call.

struct FcntlAckMessage
{
   struct MessageHeader header;        //!< Message header.
   int retval;                         //!< Return value (depends on command).
   struct flock lock;                  //!< Lock structure for advisory lock commands.
};


struct PollBasic{
   int timeout;                        //!< Time to wait in milliseconds or -1 to wait forever.
   nfds_t nfd;                         //!< Number of file descriptors in list.
   struct pollfd fds[PollSize];         //!< List to monitor
};

//! Request message for poll system call.

struct PollMessage
{
   struct MessageHeader header;        //!< Message header.
   struct PollBasic pollBasic;

};

//! Response message for poll system call.

struct PollAckMessage
{
   struct MessageHeader header;        //!< Message header.
   struct PollBasic  pollBasic;      
};

//! Request message for pread64 system call.

struct Pread64Message
{
   struct MessageHeader header;        //!< Message header.
   int fd;                             //!< File descriptor.
   size_t length;                      //!< Length of buffer.
   off64_t position;                   //!< File position (from start of the file) to read from.
   uint64_t address;                   //!< Address of data.
   uint32_t rkey;                      //!< Remote key of memory region.
};

//! Response message for pread64 system call.

struct Pread64AckMessage
{
   struct MessageHeader header;        //!< Message header.
   ssize_t bytes;                      //!< Amount of data read from descriptor.
};

//! Request message for pwrite64 system call.

struct Pwrite64Message
{
   struct MessageHeader header;        //!< Message header.
   int fd;                             //!< File descriptor.
   size_t length;                      //!< Length of data.
   off64_t position;                   //!< File position (from start of the file) to write to.
   uint64_t address;                   //!< Address of data.
   uint32_t rkey;                      //!< Remote key of memory region.
};

//! Response message for pwrite64 system call.

struct Pwrite64AckMessage
{
   struct MessageHeader header;        //!< Message header.
   ssize_t bytes;                      //!< Amount of data written to descriptor.
   size_t  ION_rdma_buffer_offset;     //!< Track offset into rdma_buffer
};

struct ReadMessage
{
   struct MessageHeader header;        //!< Message header.
   int fd;                             //!< File descriptor.
   size_t length;                      //!< Length of buffer.
   uint64_t address;                   //!< Address of data.
   uint32_t rkey;                      //!< Remote key of memory region.
};

//! Response message for read system call.

struct ReadAckMessage
{
   struct MessageHeader header;        //!< Message header.
   ssize_t bytes;                      //!< Amount of data written to descriptor.
   size_t  ION_rdma_buffer_offset;     //!< Track offset into rdma_buffer
};

//! Request message for recv system call.

struct RecvMessage
{
   struct MessageHeader header;        //!< Message header.
   int sockfd;                         //!< Socket descriptor.
   size_t length;                      //!< Length of buffer.
   int flags;                          //!< Flags.
   uint64_t address;                   //!< Address of data.
   uint32_t rkey;                      //!< Remote key of memory region.
};

//! Response message for recv system call.

struct RecvAckMessage
{
   struct MessageHeader header;        //!< Message header.
   ssize_t bytes;                      //!< Amount of data read from descriptor.
};

//! Request message for recvfrom system call.

struct RecvfromMessage
{
   struct MessageHeader header;        //!< Message header.
   int sockfd;                         //!< Socket descriptor.
   size_t length;                      //!< Length of buffer.
   int flags;                          //!< Flags.
   socklen_t addrlen;                  //!< Length of address.
   uint64_t address;                   //!< Address of data.
   uint32_t rkey;                      //!< Remote key of memory region.
};

//! Response message for recvfrom system call.

struct RecvfromAckMessage
{
   struct MessageHeader header;        //!< Message header.
   ssize_t bytes;                      //!< Amount of data read from descriptor.
   socklen_t addrlen;                  //1< Length of address.
   char addr[MaxAddrLength];           //!< Address data was received from.
};

//! Request message for send system call.

struct SendMessage
{
   struct MessageHeader header;        //!< Message header.
   int sockfd;                         //!< Socket descriptor.
   int flags;                          //!< Flags.
   size_t length;                      //!< Length of data.
   uint64_t address;                   //!< Address of data.
   uint32_t rkey;                      //!< Remote key of memory region.
};

//! Response message for send system call.

struct SendAckMessage
{
   struct MessageHeader header;        //!< Message header.
   ssize_t bytes;                      //!< Amount of data written to descriptor.
   size_t  ION_rdma_buffer_offset;     //!< Track offset into rdma_buffer
};

//! Request message for sendto system call.

struct SendtoMessage
{
   struct MessageHeader header;        //!< Message header.
   int sockfd;                         //!< Socket descriptor.
   size_t length;                      //!< Length of data.
   int flags;                          //!< Flags.
   socklen_t addrlen;                  //1< Length of address.
   char addr[MaxAddrLength];           //!< Address data was received from.
   uint64_t address;                   //!< Address of data.
   uint32_t rkey;                      //!< Remote key of memory region.
};

//! Response message for sendto system call.

struct SendtoAckMessage
{
   struct MessageHeader header;        //!< Message header.
   ssize_t bytes;                      //!< Amount of data written to descriptor.
};

//! Request message for write system call.

struct WriteMessage
{
   struct MessageHeader header;        //!< Message header.
   int fd;                             //!< File descriptor.
   ssize_t data_length;                //!< Length of data to process
   uint64_t address;                   //!< Address of data on requester
   uint32_t rkey;                      //!< Remote key of memory region.        
};

struct WriteAckMessage
{
   struct MessageHeader header;        //!< Message header.
   ssize_t bytes;                      //!< Amount of data written to descriptor.
   size_t  ION_rdma_buffer_offset;     //!< Track offset into rdma_buffer
};


// int fd goes into returnCode
struct WriteImmediateMessage
{
   struct MessageHeader header;        //!< Message header.
   char data[480];
};

struct WriteImmediateAckMessage
{
   struct MessageHeader header;        //!< Message header.
   ssize_t bytes;                      //!< Amount of data written to descriptor.
};

//! Request message for gpfs_fcntl system call.

struct GpfsFcntlMessage
{
    struct MessageHeader header;        //!< Message header.
    int fd;                             //!< File descriptor.
    ssize_t data_length;                //!< Length of data to process
    uint64_t address;                   //!< Address of data on requester
    uint32_t rkey;                      //!< Remote key of memory region.        
};

struct GpfsFcntlAckMessage
{
    struct MessageHeader header;        //!< Message header.
    int gpfsresult;                     //!< gpfs_fcntl result code
};


//! Message to setup for running a new job.

struct SetupJobMessage
{
   struct MessageHeader header;        //!< Message header.
   uid_t userId;                       //!< User id.
   gid_t groupId;                      //!< Primary group id.
   uint16_t numGroups;                 //!< Number of secondary group ids.
   gid_t secondaryGroups[MaxGroups];   //!< List of secondary group ids.
   int32_t posixMode;                  //!< Setting for posix_mode property, -1 means use default property, 0 means off, 1 means on.
   int32_t logJobStatistics;           //!< Setting for log_job_statistics property, -1 means use default property, 0 means off, 1 means on.
   int32_t logFunctionShipErrors;      //!< Setting for log_function_ship_errors property, -1 means use default property, 0 means off, 1 means on.
};

//! Message to acknowledge setting up for a new job.

struct SetupJobAckMessage
{
   struct MessageHeader header;        //!< Message header.
   int64_t sysiod_pid;                 //!< Process ID of sysiod target
   int64_t sysiod_serviceId;           //!< Service ID of sysiod target
   int32_t posixMode;                  //!< Final setting for posix_mode property, 0 means off, 1 means on.
   int32_t logJobStatistics;           //!< Confirm setting for log_job_statistics property, 0 means off, 1 means on.
   int32_t logFunctionShipErrors;      //!< Final setting for log_function_ship_errors property,  0 means off, 1 means on.
   int32_t isUsingShortCircuitPath;    //!< Final setting for whether using shortCircuitPath, 0 means off, nonzero means on 
   uint64_t rdmaBufferVirtAddr;        //!< RDMA buffer virtual address
   uint64_t rdmaBufferLength;          //!< length of said buffer  
   uint32_t memoryKeyOfBuffer;         //!< memory key of the buffer 
};

//! Message to cleanup after a running a job.

struct CleanupJobMessage
{
   struct MessageHeader header;        //!< Message header.
};

//! Message to acknowledge cleaning up after running a job.

struct CleanupJobAckMessage
{
   struct MessageHeader header;        //!< Message header.
};

#ifdef __cplusplus
} // namespace sysio

} // namespace bgcios
#endif

#endif // SYSIOMESSAGETYPES_H


