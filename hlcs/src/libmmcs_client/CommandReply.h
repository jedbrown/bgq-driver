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
/* (C) Copyright IBM Corp.  2005, 2011                              */
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

#ifndef MMCS_CLIENT_COMMAND_REPLY_H_
#define MMCS_CLIENT_COMMAND_REPLY_H_

#include <boost/utility.hpp>

#include <cassert>
#include <cstdio>
#include <iomanip>
#include <iostream>
#include <streambuf>

namespace mmcs_client {

class ConsolePort;

/*!
** The MMCS Command Reply protocol
**
** Classes which inherit from MMCSCommand receive an output stream
** upon which replies are sent to the command issuer. This reply
** stream observes a particular protocol.
**
** The first output on the stream must inserted by the user-defined
** manipulators OK and FAIL, which will also set the MMCS status code
** for the reply stream. Following the initial status, any data can be
** sent via the standard output operator (<<). A subsequent failure can
** be indicated by user-defined ABORT manipulator, which can be followed
** by an explanation of the failure using the standard output operator.
** The end of the command must be indicated by the use of the user-defined
** manipulator DONE.
**
**
** Example 1: Normal command completion with no additional output
**            reply << OK << DONE;
** Example 2: Normal command completion with additional output
**            reply << OK;
**            reply << "additional output\n";
**            reply << "more additional output\n";
**            reply << DONE;
** Example 3: Failures status and explanation
**            reply << FAIL << "reason for the failure" << DONE;
** Example 4: Failure detected after initial status reported
**            reply << OK;
**            reply << "additional output\n";
**            .. failure detected ..
**            reply << ABORT << "reason for the failure" << DONE;
**
*/
class CommandReply : protected std::streambuf, public std::ostream, private boost::noncopyable
{
public:
    /*!
    ** Reply status values
    */
    enum statusValue { STATUS_ABORT = -2, STATUS_FAIL = -1, STATUS_OK = 0, STATUS_NOT_SET = 1 };

    /*!
    ** Base constructor
    ** @param consolePort	ConsolePort if replies are written to a socket.
    **                          -- This is used by mmcs_server
    **				NULL if replies are written to stdout.
    **                          -- This is used by mmcs_hw
    ** @param replyFormat	0 == (single line format)   newlines are converted to ';' and a '\0' character (inserted by DONE) is replaced with a newline
    **				1 == (multiple line format) ';' are converted to newlines and a '\0' character (inserted by DONE) is replaced with "\n\0"
    ** @param enableWrite       true == replies are written to the console or stdout
    **                          false == replies are buffered but not written until enableWrite(true)
    */
    explicit CommandReply(ConsolePort* consolePort, int replyFormat=0, bool enableWrite=true);

    /*!
    ** Default constructor - replies are written to stdout, format 0, write disabled
    */
    CommandReply(unsigned fd=1, int replyFormat=0, bool enableWrite=false);

    /*!
    ** Base destructor
    */
    virtual ~CommandReply();

    /*!
    ** getStatus() returns the status set by the OK, FAIL, and ABORT manipulators on the reply stream
    ** @returns  statusValue
    */
    int getStatus();

    /*!
    ** setStatus() sets the command completion status for the reply stream
    ** @param    status -- see statusValue
    */
    void setStatus(int status);

    /*!
    ** setFd() sets the file descriptor for output
    ** @param    status -- see statusValue
    */
    void setFd(int fd)  { _fd = fd; }

    /*!
    ** getReplyFormat() returns the format for the command replies
    */
    unsigned getReplyFormat() { return _replyFormat; }

    /*!
    ** enableWrite() specified whether the buffered data is to be written or not
    **
    ** @param enable true  indicates that any buffered data may be written
    **                     If the enableWrite state is changed from false to true
    **                     and any data is buffered, it will be written.
    **               false indicates that buffered data may not written
    **
    ** @returns      previous enable value
    **
    ** @note         This method is intended for use when one command is calling
    **               another command internally, and wishes to check the return
    **               code and potentially change the status or reply message prior
    **               to the reply being sent to its destination.
    **
    ** @see   reset
    */
    bool enableWrite(bool enable);
    bool enableWrite() { return enableWrite(true); }
    bool disableWrite() { return enableWrite(false); }
    bool isWriteEnabled() { return _enableWrite; }

    /*
    ** reset() sets the reply back to its original state:
    ** status is set to zero, and any buffered data is discarded
    ** the enableWrite status and reply format are not changed
    */
    void reset();

    /*
    ** assign() sets the reply status and buffer contents from a string.
    ** The input string may be modified by this process
    ** The reply is reset to its original state before the string is processed.
    ** The first characters in the string must be "OK\n" or "FAIL\n".
    ** The end of the reply is indicated by a null terminator '\0'
    ** This method (and its partner method append) is used to recreate
    ** an CommandReply object from data received over a TCP/IP
    ** connection.
    */
    void assign(std::string& s);

    /*
    ** append() sets the reply status and buffer contents from a string
    ** The input string may be modified by this process
    ** The end of the reply is indicated by a null terminator '\0'
    ** This method (and its partner method assign) is used to recreate
    ** an CommandReply object from data received over a TCP/IP
    ** connection.
    */
    void append(std::string& s);

    /*
    ** str() returns the contents of the stream buffer as a standard string
    */
    std::string str(bool with_status=false) { return str(with_status, _replyFormat); }
    std::string str(bool with_status, unsigned replyFormat);

    /*
    ** isDone() returns whether DONE has been sent
    */
    bool isDone() { return _done; }

    /*
    ** sync() writes the buffer
    */
    virtual int sync() 	{  return send_buf(); }

protected:
    // overflow is called when the buffer needs to be written out
    virtual int overflow (int c = EOF);

private:
    ConsolePort* _consolePort;      // console port to write to
    int          _fd;	            // file descriptor to write to instead of server port
    bool         _enableWrite;  	// data is allowed to be written
    bool         _done;		        // DONE was sent
    unsigned     _replyFormat;		// 0 or 1
    unsigned     _bufsize;
    unsigned     _replylen;         // set by send_buf(), used by str(): amount of data in buffer
    unsigned     _totreplylen;      // set by send_buf(), checked by send_buf(): total amount of data sent for this reply
    char*        _outbuf;
    int send_buf();
    void parseString(std::string& s, unsigned startPos); // helper function used by assign() and append()
    friend std::ostream& OK(std::ostream& stream);
    friend std::ostream& FAIL(std::ostream& stream);
    friend std::ostream& ABORT(std::ostream& stream);
    friend std::ostream& DONE(std::ostream& stream);
};

/*!
** Manipulators for the reply stream
** OK    -- sets the status to OK and inserts "OK\n" in the output stream;
** FAIL  -- sets the status to FAIL and inserts "FAIL\n" in the output stream
** ABORT -- sets the status to FAIL and inserts "\nABORT\n" in the output stream
** DONE  -- inserts '\0' in the output stream. It does not change the status.
**
** The first output to an CommandReply stream should always be OK or FAIL.
** Subsequent output can consist of anything, including newlines
** If a fatal error is detected during output, an ABORT shoud be sent
** At the end of the stream an DONE should be sent to identify the end of the output
*/
std::ostream& OK(std::ostream& stream);
std::ostream& FAIL(std::ostream& stream);
std::ostream& ABORT(std::ostream& stream);
std::ostream& DONE(std::ostream& stream);

} // namespace mmcs_client

#endif
