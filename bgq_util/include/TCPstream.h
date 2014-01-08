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

#ifndef TCPSTREAMH
#define TCPSTREAMH

// A binary data stream that runs over a tcp socket.
//

#include <stdio.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <string>

   #define TRACE_TCPSTREAM(x)
// #define TRACE_TCPSTREAM(x) printf x

class TCPstream
   {
private:
   // Streams for reading/writing.
   //
   FILE *_in;  // input
   FILE *_out; // output

   // Status from open/read/write ops.
   // These are "sticky": once set, they stay set.
   //
   bool  _isInputFailure;
   bool  _isOutputFailure;

public:
   // Create a stream on a socket.
   // Taken:    socket
   //           enable input buffering on this socket?
   //
   // Note: this function transfers ownership of the socket to
   // this TCPstream. You should neither close the socket nor read
   // from it any more. You may, however, issue a select()-for-input if desired.
   //
   // Note: If you'll be doing a select()-for-input on the socket then you should
   // specify "enableInputBuffering=false". Failure to do so could result in the stream
   // state becoming inconsistent with the results returned by select. For example,
   // select() might indicate "nothing ready to read" even though this TCPstream
   // has bytes in its buffer (and would return them without blocking).
   //
   //
   TCPstream(int connectionFd, bool enableInputBuffering)
      {
     _isInputFailure  = false;
     _isOutputFailure = false;

     if (!(_in = fdopen(connectionFd, "r")))
        recordInputFailure();

     if (!(_out = fdopen(dup(connectionFd), "w")))
        recordOutputFailure();

     if (_in && !enableInputBuffering)
        setbuf(_in, 0);
     }

  ~TCPstream()
      {
      if (_in)  TRACE_TCPSTREAM(("TCPstream: closing %d\n", fileno(_in)));
      if (_out) TRACE_TCPSTREAM(("TCPstream: closing %d\n", fileno(_out)));

      if(_in)  fclose(_in);
      if(_out) fclose(_out);
      }

   int
   getInputFd()
      {
      return fileno(_in);
      }

   FILE *
   getInputStream()
      {
      return _in;
      }
      
   FILE *
   getOutputStream()
      {
      return _out;
      }
      
   void
   putByte(unsigned char val)
      {
    //printf("PUTBYTE\n");
      if (fputc(val, _out) == -1)
         return recordOutputFailure();
      }

   void
   putWord(unsigned val)
      {
    //printf("PUTWORD\n");
      val = htonl(val);
      if (fwrite(&val, sizeof(val), 1, _out) != 1)
         return recordOutputFailure();
      }

   void
   putString(const std::string& val)
      {
      const char *cp = val.c_str();
      return putBlock(cp, strlen(cp) + 1);
      }

   void
   putBlock(const char *val, unsigned len)
      {
    //printf("PUTBLOCK %u\n", len);
      int netlen = htonl(len);
      if (fwrite(&netlen, sizeof(netlen), 1, _out) != 1)
        return recordOutputFailure();
      if (fwrite(val, sizeof(*val), len, _out) != len)
        return recordOutputFailure();
      }

   void
   putFlush()
      {
    //printf("FLUSH\n");
      if (fflush(_out))
         return recordOutputFailure();
      }

   unsigned char
   getByte()
      {
    //printf("GETBYTE\n");
      int val = fgetc(_in);
      if (val == -1)
         return recordInputFailure(), 0;
      return val & 0xff;
      }

   unsigned
   getWord()
      {
    //printf("GETWORD\n");
      unsigned val;
      if (fread(&val, sizeof(val), 1, _in) != 1)
         return recordInputFailure(), 0;
      return ntohl(val);
      }

   void
   getString(std::string *dst)
      {
    //printf("GETSTRING\n");
      unsigned len;
      char  *str = getBlock(&len);
      *dst = str ? str : "";
      delete str;
      }

   // Read a block of data.
   // Taken:    place to put block length
   // Returned: new'd block (or 0 if input failure)
   //
   char *
   getBlock(unsigned *len)
      {
      char *val = 0;
      if (fread(len, sizeof(*len), 1, _in) != 1)
        return recordInputFailure(), val;

      *len = ntohl(*len);
    //printf("GETBLOCK %u\n", *len);

      val = new char[*len]; *val = 0;
      if (fread(val, sizeof(*val), *len, _in) != *len)
        return recordInputFailure(), val;

      return val;
      }

   // I/O can fail for two reasons: eof, and i/o error.
   // The following functions allow us to determine which.
   // The error conditions persist until a new connection is 
   // established with setConnectionFd();
   //
   bool isInputFailure() { return _isInputFailure; }
   bool isInputError()  { return _in ? (ferror(_in) != 0) : 1; };
   bool isInputEof()    { return _in ? (feof(_in)   != 0) : 0; };

   bool isOutputFailure() { return _isOutputFailure; }
   bool isOutputError() { return _out ? (ferror(_out) != 0) : 1; };
   bool isOutputEof()   { return _out ? (feof(_out)   != 0) : 0; };

private:
   void
   recordInputFailure()
      {
      _isInputFailure = true;
      TRACE_TCPSTREAM(("TCPstream: input error=%d eof=%d\n", isInputError(), isInputEof()));
      }

   void
   recordOutputFailure()
      {
      _isOutputFailure = true;
      TRACE_TCPSTREAM(("TCPstream: output error=%d eof=%d\n", isOutputError(), isOutputEof()));
      }
   };

#endif /* TCPSTREAMH */
