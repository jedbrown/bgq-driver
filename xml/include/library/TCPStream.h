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
/* (C) Copyright IBM Corp.  2010, 2011                              */
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

#ifndef TCPSTREAM_HEADER
#define TCPSTREAM_HEADER


#include <unistd.h>
#include <errno.h> 
#include <assert.h>
#include <netinet/in.h>
#include <iostream>

// Optional user-provided polling/interrupt callback for the TCPRead function
// If provided, the callback is called by TCPRead prior to reading data
// The callback is passed the file descriptor of a socket to be polled. 
// It returns >0 when data is available
//             0 when no data is available (a timeout occurred)
//            -1 when the read has been cancelled
// When the return code is -1, TCPread sets errno to EINTR and returns -1
// When the return code is 0, TCPread calls the PollInterruptCallback routine again
typedef int (*PollInterruptCallback)(int fd);


// Read from a tcp socket.
// Taken:    socket to read
//           place to put data
//           number of bytes expected
//           (optional) poll/interrupt callback function
// Returned: cnt=success
//             0=connection closed
//            -1=error
//

static inline int
TCPread(unsigned fd, unsigned char *dst, unsigned cnt, PollInterruptCallback pollCallback=NULL)
   {
   unsigned requested = cnt;
   unsigned received  = 0;
   
   while (received != requested)
      {
      if (pollCallback)
         {
	     while(1)
	        {
		int poll_rc = pollCallback(fd);
		if (poll_rc > 0) // data is available
		    break;
	        if (poll_rc < 0) // cancellation or error
	           {
		   errno = EINTR;
		   return -1;
	           }
		}
	 }    
      int rc = read(fd, dst, cnt);
      if (rc == -1) 
         {
         if (errno == EINTR)
            {
//          fprintf(stderr, "TCPread: interrupted (retrying)\n");
            continue;
            }
         if (errno != ECONNRESET)
            perror("TCPread"); // something other than "connection reset by peer"
         return -1;
         }
      if (rc == 0)
         {
//       fprintf(stderr, "TCPread: connection closed\n");
         return 0;
         }

      received += rc;
      dst      += rc;
      cnt      -= rc;
      
//    if (received != requested)
//       fprintf(stderr, "TCPread: short read: socket=%d, requested=%d, received=%d (retrying)\n", fd, requested, received);
      }

   return received;
   }

// Includes
#include <stdio.h>
#include <unistd.h>

// Write to a tcp socket.
// Taken:    socket to write
//           data to write
//           number of bytes to write
// Returned: cnt=success
//             0=connection closed
//            -1=error
//

static inline int
TCPwrite(unsigned fd, unsigned char *src, unsigned cnt)
   {
   unsigned requested = cnt;
   unsigned delivered = 0;
   
   while (delivered != requested)
      {
      int rc = write(fd, src, cnt);

      if (rc == -1) 
         {
         if (errno == EINTR)
            {
            fprintf(stderr, "TCPwrite: interrupted (retrying)\n");
            continue;
            }
//       perror("TCPwrite");
         return -1;
         }

      if (rc == 0)
         {
         fprintf(stderr, "TCPwrite: connection closed\n");
         return 0;
         }

      delivered += rc;
      src       += rc;
      cnt       -= rc;
      
//    if (delivered != requested)
//       fprintf(stderr, "TCPwrite: short write: socket=%d, requested=%d, delivered=%d (retrying)\n", fd, requested, delivered);
      }

   return delivered;
   }


//#define TCPBUFLEN 1024
#define TCPBUFLEN 16384


// buffer class in the iostream hierarchy used to provide correct communication
// through sockets. Every read and write buffer, first reads (or writes) a size
// and then the contents of the payload. To function correctly, the buffers on
// both extrems should be of the same size
class TCPBuffer : public std::streambuf {
public:
  TCPBuffer(int fd = -1, bool selfDescribingIO = true) 
    : _fd(fd), _selfDescribingIO(selfDescribingIO), _pollInterruptCallback(NULL), _bytesSent(0), _bytesRecv(0) {}

  ~TCPBuffer() {}

  void setFd(int fd) { 
    assert(_fd == -1); // initialize only once
    _fd = fd;
  }

  void myunderflow() {
     underflow();
  }

  // user-provided poll/interrupt callback
  void setPollInterruptCallback(PollInterruptCallback fn) { _pollInterruptCallback = fn; }
    
  // virtual functions called by input and output streams
  virtual int underflow();
  virtual int overflow(int c = EOF);
  virtual int sync();

  unsigned getBytesSent() const { return _bytesSent; }
  unsigned getBytesRecv() const { return _bytesRecv; }

protected:
  int  _fd; // just a cached copy for reading and writing; _fd is managed
            // by TCPstream classes
  bool _selfDescribingIO; // specificies if every writes sends a "bufffer size"
  // to assist with blocking IO;
  PollInterruptCallback _pollInterruptCallback;
  char _memory[TCPBUFLEN];

  unsigned _bytesSent;
  unsigned _bytesRecv;

  int recv(); 
  int send();

private:
  TCPBuffer(const TCPBuffer&) {}
  TCPBuffer& operator=(const TCPBuffer&) { return *this; }
  
};


class TCPOstream : public std::ostream {
public:
  TCPOstream(int fd = -1, bool shouldClose = true)
    : std::ostream(&_buf), _fd(fd), _shouldClose(shouldClose), _buf(fd) {}

  ~TCPOstream() {
    if (_shouldClose && _fd >= 0) { close(_fd); }
  }

  void setFd(int fd, bool shouldClose = true) { 
    assert(_fd == -1 && fd >= 0); // initialize only once

    _fd = fd;
    _shouldClose = shouldClose; 
    _buf.setFd(fd); 
  }

  // user-provided poll/interrupt callback
  void setPollInterruptCallback(PollInterruptCallback fn) { _buf.setPollInterruptCallback(fn); }
    
  int getFd() const { return _fd; }
  unsigned getBytesSent() const { return _buf.getBytesSent(); }
  unsigned getBytesRecv() const { return _buf.getBytesRecv(); }

protected:
  int       _fd;
  bool      _shouldClose;
  TCPBuffer _buf;
};


class TCPIstream : public std::istream {
public:
  TCPIstream(int fd = -1, bool shouldClose = true)
    : std::istream(&_buf), _fd(fd), _shouldClose(shouldClose), _buf(fd) {}

  ~TCPIstream() {
    if (_shouldClose && _fd >= 0) { close(_fd); }
  }

  void setFd(int fd, bool shouldClose = true) { 
    assert(_fd == -1 && fd >= 0); // initialize only once

    _fd = fd;
    _shouldClose = shouldClose; 
    _buf.setFd(fd); 
  }

  int getFd() const { return _fd; }
  unsigned getBytesSent() const { return _buf.getBytesSent(); }
  unsigned getBytesRecv() const { return _buf.getBytesRecv(); }

  // user-provided poll/interrupt callback
  void setPollInterruptCallback(PollInterruptCallback fn) { _buf.setPollInterruptCallback(fn); }

protected:
  int       _fd;
  bool      _shouldClose;
  TCPBuffer _buf;
};



class TCPIOstream : public std::istream, public std::ostream {
public:
  TCPIOstream(int fd = -1, bool shouldClose = true)
    : std::istream(&_ibuf), std::ostream(&_obuf), 
    _fd(fd), _shouldClose(shouldClose), _ibuf(fd), _obuf(fd) {}

  ~TCPIOstream() {
    if (_shouldClose && _fd >= 0) { close(_fd); }
  }

  void setFd(int fd, bool shouldClose = true) {
    assert(_fd == -1 && fd >= 0); // initialize only once

    _fd = fd;
    _shouldClose = shouldClose;
    _ibuf.setFd(fd);
    _obuf.setFd(fd); 
  }

  int getFd() const                { return _fd; }
  unsigned getInBytesSent() const  { return _ibuf.getBytesSent(); }
  unsigned getInBytesRecv() const  { return _ibuf.getBytesRecv(); }
  unsigned getOutBytesSent() const { return _obuf.getBytesSent(); }
  unsigned getOutBytesRecv() const { return _obuf.getBytesRecv(); }

  // user-provided poll/interrupt callback
  void setPollInterruptCallback(PollInterruptCallback fn) {
      _ibuf.setPollInterruptCallback(fn);
      _obuf.setPollInterruptCallback(fn);
  }
    
protected:
  int       _fd;
  bool      _shouldClose;
  TCPBuffer _ibuf;
  TCPBuffer _obuf;
};




inline int TCPBuffer::underflow() {
   //   std::cerr << "TCP-underflow called" << std::endl;
  if (_fd == -1) {
    return EOF;
  }

  if (gptr() == egptr()) {
    int count = recv();
    if (count >= 0) {
      setg(_memory, _memory, _memory + count);
    } else {
      setg(0, 0, 0);
      return EOF;
    }
  }

  return (unsigned char) *gptr();
}



inline int TCPBuffer::overflow(int c) {
   //   std::cerr << "TCP-oveflow called" << std::endl;

  if (_fd == -1) {
    return EOF;
  }

  if (pptr() > pbase()) {
    int count = send();
    
    if (count < 0) {
      return EOF;
    }
  }
  
  setp(_memory, _memory + TCPBUFLEN);
  *pptr() = c;
  pbump(1);
  
  return 0;
}


inline int TCPBuffer::sync() {
   //   std::cerr << "TCP-sync called" << std::endl;
  if (_fd == -1) {
    return EOF;
  }

  if (in_avail()) {
    setg(0, 0, 0);
  }
  
  if (pptr() > pbase()) {
    int count = send();
    if (count < 0) {
      return EOF;
    }
    setp(_memory, _memory + TCPBUFLEN);
  }
  
  return 0;
}


inline int TCPBuffer::recv() {
   //  std::cerr << "TCP-waiting to receive" << std::endl;

  int count = 0;
  int rc;

  if (_selfDescribingIO) {
    unsigned networkCount;
    rc = TCPread(_fd, (unsigned char *) &networkCount, sizeof(unsigned), _pollInterruptCallback);
    if (rc != sizeof(unsigned)) {
      return -1;
    }

    count = ntohl(networkCount);

    rc = TCPread(_fd, (unsigned char *) _memory, count, _pollInterruptCallback);
    if (rc != count) {
      return -1;
    }
    //    std::cerr << "TCP-received(1): " << count << " bytes ("  << rc << ")" << std::endl;
  } else {
    rc = read(_fd, _memory, TCPBUFLEN);
  
    if (rc == -1) {
      if (errno == EINTR) {
	fprintf(stderr, "TCPStream: interrupted (retrying)\n");
      }
    } else {
      count = rc;
    }
  }
  
  _bytesRecv += count;
  return count;
}


inline int TCPBuffer::send() {
  //  std::cerr << "TCP-trying to send" << std::endl;
  //  char *q = pbase();
  //  int n = pptr() - q;

  int count = pptr() - pbase();
  int rc = -1;
  _bytesSent += count;

  if (_selfDescribingIO) {
    unsigned networkCount = htonl(count);

    rc = TCPwrite(_fd, (unsigned char *) &networkCount, sizeof(unsigned));
    if (rc != sizeof(unsigned)) {
      return -1;
    }
  }
    
  rc = TCPwrite(_fd, (unsigned char *) pbase(), count);
  if (rc != count) {
    return -1;
  }

  //  std::cerr << "TCP-sending: " << n << " bytes " << std::endl;
  return count;    
}



#endif
