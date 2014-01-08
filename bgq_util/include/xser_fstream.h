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

#ifndef __XSER_FSTREAM_H__
#define __XSER_FSTREAM_H__

/*!
// structure streaming base classes.
//
// This class is designed to use streams to send record oriented data over
// a streaming link.  In this case TCP.
//
// It encodes each record over the link as multiple run-length oriented chunks
// as follows:
//
//   bits:
//   |31    30..0                     |
//   ----------------------------------
//   | F | length                     |   -- chunk length (F == 0, more chunks follow, F=1 Final chunk)
//   ----------------------------------
//   |  length                        |
//   |     bytes                      |
//   |       data ...                 |
//   ----------------------------------
//
//
// A record consists of 1 or more chunks of data.  As a result, they can be really- really long
// or as short as needed.
//
// NOTE, it is valid to have a zero length block with the F bit == 1. indicating end of record
//       but no more data.
//
//
*/

#include <netinet/in.h>
#include <fcntl.h>
#include <errno.h>
#include <assert.h>
#include <iomanip>

#include <streambuf>

#define XSERIOTRACE_SEND(x) 
//#define XSERIOTRACE_SEND(x) x

#define XSERIOTRACE_RECV(x)
//#define XSERIOTRACE_RECV(x) x

#include "xser_stream.h"


enum {
    XSER_BUFLEN=(1024*64  )
};
/*!
// buffer class that performs the reading and writing from a record oritented buffer.
//
*/
class Xser_streambuf : public std::streambuf
{
public:
    Xser_streambuf(int fd = -1) : 
        _fd(fd),
        _eof(false),
        _currNetworkCount(0),
        _chunkRemaining(0)
         {}

    ~Xser_streambuf() {}

    void setFd(int fd) { 
        assert(_fd == -1); // initialize only once
        _fd = fd;
    }

    /*!
    // check if the stream buffer is at the end of the current record
    // @param none
    // @returns true if this is the end of record
    */
    bool is_endr() {
        if (in_avail())
            return(false);

        return ((_chunkRemaining == 0) && (_currNetworkCount & (1<<31)));
    };



    /*!
    // send end of record.
    //
    // @param none.  flush the current buffer and mark it as the end of record.
    // @param returns none.
    */
    void send_endr() {
        send(EOR);
    };

    /*!
    // Flush the output streasm.
    //
    // @param none.
    // @returns none
    */
    void flush() {
        sync();
    }

    /*!
    // indicate if we have seen an error in transmission.
    // this indicates the other end disconnected.
    */
    bool eof() {
        return(_eof); };


    /*!
    // go to the next record.
    //
    // @param none.
    // @returns none
    */
    void next_rec();


protected:
    // virtual functions called by input and output streams
    virtual int underflow();
    virtual int overflow(int c = EOF);
    virtual int sync();
    virtual std::streamsize xsgetn(char *s, std::streamsize n);
    int  _fd; 
    bool      _eof;
    char _memory[XSER_BUFLEN];
    uint32_t _currNetworkCount;
    unsigned _chunkRemaining;   // sizer of chunk remaining.

    enum {
        EOR = -2    // end of record flag.
    };
    //
    // going to need some sort of read state here...
    //
    int recv(); 
    int send(int eor = 0);

private:
    Xser_streambuf(const Xser_streambuf&) {}
    Xser_streambuf& operator=(const Xser_streambuf&) { return *this;}

};

//
// Tcp output stream
// 
//
class XserOfstream : public  XserOstream
{
public:
    XserOfstream(int fd = -1, bool shouldClose = true) : 
    _fd(fd), 
    _shouldClose(shouldClose), 
    _eof(false),
    _buf(fd) {};

    ~XserOfstream() {
        if (_shouldClose)
            close();
    };

    /*!
    // Open the file name specified.
    //
    // @param -- filename ot open
    // @returns -- 0 success, system errno if failure,
    */
    int open(const std::string &fileName) {
        if (_shouldClose)
            close();
        int fd = ::open(fileName.c_str(), O_CREAT | O_WRONLY | O_TRUNC | O_LARGEFILE , S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP);
        if (fd == -1)
            return(errno);
        setFd(fd, true);
        return(0);
    };


    void close()
    {
        if (_fd != -1)
        {
            flush();
            ::close(_fd);
            _fd = -1;
        }
    };
    /*!
    // Set the file/socket descriptor for this function.
    //
    // @param fd -- file/socket descriptor.
    // @param shouldClose -- true if we should close this descriptor
    //                       false if we should not self close it.
    // @returns none.
    */
    void setFd(int fd, bool shouldClose = true) { 
        assert(_fd == -1 && fd >= 0); // initialize only once
        _fd = fd;
        _shouldClose = shouldClose; 
        _buf.setFd(fd); 
    }

    /*!
    // Retrieve the file/socket descriptor.
    // @param none
    // @returns file/socket descriptor
    */
    int getFd() const { 
        return _fd;}

    /*!
    // indicate if we have seen an error in transmission.
    // this indicates the other end disconnected.
    */
    bool eof() {
        return(_eof); };
    /*!
    // Write to the tcp output stream.
    //
    // @param s -- stream characters to write.
    // @n -- number of bytes to write
    // @returns -- reference to this stream.
    */
    void write ( const char* s , unsigned n ) {
        _buf.sputn(s,n);        // forward this to the buffer.
    }


    /*!
    // Flush the current contents of the stream buffer to the other end
    // in a chunk of stream data.
    // @param none.
    // @returns reference to this stream.
    */
    void flush ( ) {
        _buf.flush(); };

    /*!
    // Send the end of record chunk.  This implies a flush
    // @param none.
    // @returns -- reference to this stream.
    //
    */
    void write_endr ( ) {
        _buf.send_endr(); };

    
protected:
    int       _fd;
    bool      _shouldClose;
    bool      _eof;
    Xser_streambuf _buf;
};

class XserIfstream : public XserIstream
{
public:
    XserIfstream(int fd = -1, bool shouldClose = true) : 
        _fd(fd), 
        _shouldClose(shouldClose), 
        _rec_underflow(false),
        _buf(fd) {};

    ~XserIfstream() {
        if (_shouldClose)
            close();
    };

    /*!
    // Open the file name specified.
    //
    // @param -- filename ot open
    // @returns -- 0 success, system errno if failure,
    */
    int open(std::string fileName) {
        if (_shouldClose)
            close();
        int fd = ::open(fileName.c_str(), O_LARGEFILE);
        if (fd == -1)
            return(errno);
        setFd(fd);
        return(0);
    };

    void close()
    {
        if (_fd != -1)
            ::close(_fd), _fd = -1;

    };
    /*!
    // Set the file/socket descriptor for this function.
    //
    // @param fd -- file/socket descriptor.
    // @param shouldClose -- true if we should close this descriptor
    //                       false if we should not self close it.
    // @returns none.
    */
    void setFd(int fd, bool shouldClose = true) { 
        assert(_fd == -1 && fd >= 0); // initialize only once

        _fd = fd;
        _shouldClose = shouldClose; 
        _buf.setFd(fd); 
    };

    /*!
    // Retrieve the file/socket descriptor.
    // @param none
    // @returns file/socket descriptor
    */
    int getFd() const { 
        return _fd; };

    /*!
    // indicate if we have seen an error in transmission.
    // this indicates the other end disconnected.
    */
    bool eof() {
        return(_buf.eof()); };
    /*!
    // Read from the tcp input stream
    //
    // @param s -- stream characters to read
    // @n -- number of bytes to read
    // @returns -- number of bytes read
    */
    int read ( char* s , unsigned n );

    /*!
    // check if the stream buffer is at the end of the current record
    // @param none
    // @returns true if this is the end of record
    */
    bool is_endr() {
        return(_buf.is_endr()); };

    /*!
    // go to the next record.
    //
    // @param none.
    // @returns none
    */
    void next_rec() {
        _buf.next_rec();
        _rec_underflow = false;
    };

    /*!
    // Check to see if we saw a record underflow.
    //
    // @param none.
    // @returns true if we had a record underflow, false if not
    */
    bool rec_underflow() {
        return(_rec_underflow); };

protected:
    int          _fd;
    bool         _shouldClose;
    bool         _rec_underflow;
    Xser_streambuf _buf;
};


/*!
// Handle the buffer underflow condition.
//
// This is called by the streambuf stl class when there is no more data
// left in the buffer.  It triggers a read of underlying tcp layer.
//
// @param none.
// @returns none.
*/
inline int Xser_streambuf::underflow() 
{
    if (_fd == -1) 
        return EOF;


    if (gptr() == egptr())
    {
        int count = recv();

        if (count > 0)
        {
            setg(_memory, _memory, _memory + count);
        }
        else
        {
            setg(_memory, _memory, _memory);
            _eof = true;
            return EOF;
        }
    }

    return(unsigned char) *gptr();
}


/*!
// Handle the buffer overflow condition.  This drains the buffer
// by sending a chunk of data tothe other end of the buffer.
//
// @param c -- character write that is causing the overflow.
// @returns EOF -- if data cound not be sent, 0 if none.
*/
inline int Xser_streambuf::overflow(int c) 
{
    if (_fd == -1)
        return EOF;

    if (pptr() > pbase())
    {
        int count = send();

        if (count < 0)
            return EOF;
    }

    setp(_memory, _memory + XSER_BUFLEN);
    *pptr() = c;
    pbump(1);

    return 0;
}


/*!
// Synchronize the stream buffer, this flushs the bufffer and sends it on its way.
// @aprams none.
// @returns EOF if the file descriptor is closed, otherwise 0
*/
inline int Xser_streambuf::sync() 
{
    if (_fd == -1)
        return EOF;
    #if 0
        if (in_avail())
            setg(0, 0, 0);
    #endif

    if (pptr() > pbase())
    {
        int count = send();
        if (count < 0)
            return EOF;
        setp(_memory, _memory + XSER_BUFLEN);
    }

    return 0;
}

/*!
// overide the xsgetn to give us recored oriented get data
//
// @param s -- place to put the data.
// @param n -- number of bytes to get.
// @returns number of bytes retrieved.
*/
inline std::streamsize Xser_streambuf::xsgetn(char *s, std::streamsize n)
{
    if (is_endr())       // stall out the read
        return 0;           // until nextrec is called.
    unsigned count = 0;
    unsigned avail = in_avail();

    if (!avail) 
        underflow();

    avail = in_avail();
    if (avail)
    {
        unsigned cnt = (((unsigned)n < avail) ? n : avail);
        memcpy(s, gptr(), cnt);
        gbump(cnt);
        count = cnt;
    }
    return(count);

}

/*!
// set the internal state to read the next record
//
// @param none.
// @return none
*/
inline void Xser_streambuf::next_rec() 
{
    // records never cross memory block boundaries, flush the internal memory buffer
    setg(_memory, _memory, _memory);    
    // then check if we are at the end of the record
    while (!is_endr()) 
    {
        setg(_memory, _memory, _memory);
        int rc = recv();
        if (rc < 0)
            break;      
    }
    if (is_endr())       // only do this if we are at the current end of record..
        _currNetworkCount = 0;

}




/*!
// Receive a chunk or a partial chunk of data from the remote end of this connection.
// @param none.
// @returns -- number of characters received
*/
inline int Xser_streambuf::recv() 
{

    int count = 0;
    int rc;

    if (_chunkRemaining == 0)       // nothing left in the chunk??
    {
        rc = ::read(_fd, (unsigned char *) &_currNetworkCount, sizeof(uint32_t));
        if (rc == 0)
            return(0);
        if (rc != sizeof(uint32_t))
            return -1;
        XSERIOTRACE_RECV(std::cout << "-" << std::hex << std::setw(8) << std::setfill('0') << ntohl(_currNetworkCount) << std::endl);
        _currNetworkCount = ntohl(_currNetworkCount);
        _chunkRemaining =  _currNetworkCount & 0x7FFFFFFF;
    }
    count = ((_chunkRemaining<XSER_BUFLEN) ?  _chunkRemaining : XSER_BUFLEN);
    rc = read(_fd, (unsigned char *) _memory, count);
    if (rc != count)
        return -1;      

    XSERIOTRACE_RECV(for (int n = 0; n < count; n++)
                  std::cout << std::hex << std::setw(2) << std::setfill('0') << ((unsigned)(_memory[n])&0xFF) << " ";
               std::cout << std::endl << std::flush; );

    _chunkRemaining -= count;
    return count;
}


/*!
// send a data chunk to the other end of this link
//
// @param none
// @returns number of characters sent
*/
inline int Xser_streambuf::send(int eor) 
{
    int count = pptr() - pbase();
    int rc = -1;
    unsigned networkCount;

    if (eor == EOR)
        networkCount = htonl(count | (1<<31));
    else
        networkCount = htonl(count);

    XSERIOTRACE_SEND(std::cout << "+" << std::hex << std::setw(8) << std::setfill('0') << htonl(networkCount) << ": " << std::endl);
    rc = ::write(_fd, (unsigned char *) &networkCount, sizeof(unsigned));
    if (rc != sizeof(unsigned))
    {
        _eof = true;
        return -1;      
    }
    XSERIOTRACE_SEND(for (int n = 0; n < count; n++)
                  std::cout << std::hex << std::setw(2) << std::setfill('0') << ((unsigned)(pbase()[n])&0xFF) << " ";
               std::cout << std::endl << std::flush; );
    rc = ::write(_fd, (unsigned char *) pbase(), count);
    if (rc != count)
    {
        _eof = true;
        return -1;
    }
    // we have drained the pointers, reset the send buffer
    setp(_memory, _memory + XSER_BUFLEN);


    return count;    
}

/*!
// Read from the tcp input stream
//
// @param s -- stream characters to read
// @n -- number of bytes to read
// @returns -- number of bytes read
*/
inline int XserIfstream::read ( char* s , unsigned n ) 
{

    unsigned countRemaining = n;
    char *p = s;
    int nchars = 0;

    while (countRemaining)
    {
        if (eof())
        {
            for (unsigned n = 0; n < countRemaining; n++)       // don't leave uninitialize stuff around
                *p++ = 0;
            break;
        }
        if (!is_endr())
        {
            unsigned count = _buf.sgetn(p,countRemaining);
            countRemaining -= count;
            p += count;
            nchars += count;
       }
       else
       {
           memset(p, 0, countRemaining);
           nchars += countRemaining;
           countRemaining = 0;
           _rec_underflow = true;
       }
    }

    return(nchars);   

}

#endif

