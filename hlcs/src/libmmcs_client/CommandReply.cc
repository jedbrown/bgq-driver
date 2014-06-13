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

#include "CommandReply.h"

#include "ConsolePort.h"

#include <utility/include/Log.h>

#include <utility/include/cxxsockets/types.h>

#include <assert.h>
#include <errno.h>
#include <unistd.h>
#include <sstream>
#include <stdio.h>

LOG_DECLARE_FILE( "mmcs_client" );

#define BUFSIZE 1024
#define FORMAT0_REPLY_LIMIT 1024*16	// max size for a format 0 reply
#define OK_TOKEN "OK\n"
#define OK_TOKEN_LEN 3
#define FAIL_TOKEN "FAIL\n"
#define FAIL_TOKEN_LEN 5
#define ABORT_TOKEN "\nABORT\n"
#define ABORT_TOKEN_LEN 7
#define DONE_TOKEN '\0'
#define DONE_TOKEN_LEN 1

namespace mmcs_client {

/*!
** ios_base::xalloc() returns a unique index associated with the ostream
** that we can use with ios_base::iword() to keep track of the MMCS command status
*/
int mmcsCommandStatusIndex = std::ios_base::xalloc();

/*!
** ios_base::xalloc() returns a unique index associated with the ostream
** that we can use with ios_base::pword() to keep track of the CommandReply object associated with the stream
*/
int mmcsCommandReplyIndex = std::ios_base::xalloc();

/*!
** Default constructor - replies are written to stdout, format 0, write disabled
*/
CommandReply::CommandReply(unsigned fd, int replyFormat, bool enableWrite)
    : std::streambuf(),
      std::ostream((std::streambuf*)this),
      _consolePort(NULL),
      _fd(fd),
      _enableWrite(enableWrite),
      _replyFormat(replyFormat),
      _bufsize(BUFSIZE),
      _outbuf(NULL)
{
    _outbuf = new char[_bufsize+2];	// allocate bufsize plus additional byte for terminating null
    assert(_outbuf);
    std::ostream::pword(mmcsCommandReplyIndex) = this;
    reset();
}

/*!
** Base constructor
** @param consolePort	MMCSServerPort if replies are written to a socket.
**				NULL if replies are written to stdout.
*/
CommandReply::CommandReply(ConsolePort* consolePort, int replyFormat, bool enableWrite)
    : std::streambuf(),
      std::ostream((std::streambuf*)this),
      _consolePort(consolePort),
      _fd(1),
      _enableWrite(enableWrite),
      _replyFormat(replyFormat),
      _bufsize(BUFSIZE),
      _outbuf(NULL)
{
    _outbuf = new char[_bufsize+2];	// allocate bufsize plus additional byte for terminating null
    assert(_outbuf);
    std::ostream::pword(mmcsCommandReplyIndex) = this;
    reset();
}

/*!
** Base destructor
*/
CommandReply::~CommandReply()
{
    if (_enableWrite &&	 	       // OK to write reply?
            pptr() - pbase() > 0)  // Is there data in the buffer?
    {
        assert(_done);             // Make sure DONE was sent
        sync();
    }

    if (_outbuf) {
        delete[] _outbuf;
    }
    std::ostream::pword(mmcsCommandReplyIndex) = NULL;
}

/*!
** enableWrite() specified whether the buffered data is to be written or not
** @param enable true  indicates that any buffered data may be written
**                     If the enableWrite state is changed from false to true
**                     and any data is buffered, it will be written.
**               false indicates that buffered data may not written
**                     This setting might be used when one command is calling
**                     another command internally, and wishes to check the return
**                     code and potentially change the status or message prior to
**                     it being sent to its destination.
** @returns      previous enable value
**
** @see   reset
*/
bool
CommandReply::enableWrite(bool enable)
{
    bool priorstate = _enableWrite;	// Save prior state
    _enableWrite = enable;	    	// Set new state
    if (enable &&			        // Did we enable from a disabled state?
            priorstate == false &&
            pptr() - pbase() > 0)   // Is there data in the buffer?
    {
        sync();			            // Write the data
    }
    return priorstate;
}

/*
** reset() sets the reply status and buffer back to the original state:
** status is set to zero, and any buffered data is discarded
** the enableWrite status and reply format are not changed
*/
void
CommandReply::reset()
{
    setp(_outbuf, _outbuf + _bufsize);
    std::ostream::iword(mmcsCommandStatusIndex) = STATUS_NOT_SET;
    _done = false;
    _replylen = 0;
    _totreplylen = 0;
}

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
void
CommandReply::assign(std::string& s)
{
    reset();

    if (!s.empty()) {
        // 'normalize' string to replyformat 1
        for (unsigned i = 0; i < s.length(); ++i)
            if (s[i] == ';')
                s[i] = '\n';

        if (s.compare(0,OK_TOKEN_LEN, OK_TOKEN) == 0) {
            *this << OK;
            parseString(s, OK_TOKEN_LEN); // look for "\nABORT\n" or '\0' and add text to buffer
        } else if (s.compare(0, FAIL_TOKEN_LEN, FAIL_TOKEN) == 0) {
            *this << FAIL;
            parseString(s, FAIL_TOKEN_LEN); // look for "\nABORT\n" or '\0' and add text to buffer
        } else  {
            throw std::logic_error("Invalid use of CommandReply::assign()");
        }
    } else {
        LOG_ERROR_MSG("Bad reply string: " << s);
        throw std::logic_error("Invalid use of CommandReply::assign()");
    }
}

/*
** append() sets the reply status and buffer contents from a string
** The input string may be modified by this process
** The end of the reply is indicated by a null terminator '\0'
** This method (and its partner method assign) is used to recreate
** an CommandReply object from data received over a TCP/IP
** connection.
*/
void
CommandReply::append(std::string& s)
{
    if (getStatus() == CommandReply::STATUS_NOT_SET || _done) {
        throw std::logic_error("Invalid use of CommandReply::append()");
    }

    if (!s.empty()) {
        // 'normalize' string to replyformat 1
        for (unsigned i = 0; i < s.length(); ++i)
            if (s[i] == ';')
                s[i] = '\n';

        parseString(s, 0); // look for "\nABORT\n" or '\0' and add text to buffer
    }
}

/*!
** parseString() is an internal helper method for assign() and append()
** It scans an input string for "\ABORT\n" or '\0' and sets the reply status
** It adds the string text to the reply buffer
*/
void
CommandReply::parseString(std::string& s, unsigned startPos)
{
    std::string::size_type endPos1, startPos2, endPos2;
    endPos1 = s.find(ABORT_TOKEN, startPos);
    endPos2 = s.find(DONE_TOKEN, startPos);

    if (endPos1 != std::string::npos)
        if (endPos2 != std::string::npos) { // found ABORT & DONE
            assert(endPos2 > endPos1);
            *this << s.substr(startPos, endPos1 - startPos); // copy up to ABORT
            *this << ABORT;
            startPos2 = endPos1 + ABORT_TOKEN_LEN;
            *this << s.substr(startPos2, endPos2 - startPos2);	// copy up to DONE
            *this << DONE;
        } else {		  // found ABORT but not DONE
            *this << s.substr(startPos, endPos1 - startPos); // copy up to ABORT
            *this << ABORT;
            startPos2 = endPos1 + ABORT_TOKEN_LEN;
            if (startPos2 < s.size())
                *this << s.substr(startPos2, s.size() - startPos2); // copy rest of string
    } else if (endPos2 != std::string::npos) { // found DONE but not ABORT
        *this << s.substr(startPos, endPos2 - startPos);	// copy up to DONE
        *this << DONE;
    } else {		   // neither DONE nor ABORT
        *this << s.substr(startPos);;
    }
}

/*!
** str() returns the contents of the stream buffer as a standard string
*/
std::string
CommandReply::str(bool with_status, unsigned replyFormat)
{
    char* reply_start = _outbuf;       // start of reply data in _outbuf
    int  reply_len = pptr() - pbase(); // length of data in _outbuf
    if (reply_len == 0)		       // handle case where _outbuf has already been emptied
        reply_len = _replylen;	       // use previous length of _outbuf data
    else if (*(reply_start+reply_len-1) == DONE_TOKEN) // don't include DONE_TOKEN in reply string
        --reply_len;

    if (reply_len > 0 && !with_status) { // caller doesn't wants 'OK','FAIL','ABORT' status in reply
        if (strncmp(_outbuf, OK_TOKEN, OK_TOKEN_LEN) == 0) {
            reply_start +=  OK_TOKEN_LEN;
            reply_len -= OK_TOKEN_LEN;
        } else if (strncmp(_outbuf, FAIL_TOKEN, FAIL_TOKEN_LEN) == 0) {
            reply_start += FAIL_TOKEN_LEN;
            reply_len -= FAIL_TOKEN_LEN;
        } else if (strncmp(_outbuf, ABORT_TOKEN, ABORT_TOKEN_LEN) == 0) {
            reply_start += ABORT_TOKEN_LEN;
            reply_len -= ABORT_TOKEN_LEN;
        }
    }

    std::string replystr(reply_start, reply_len);
    if (reply_len > 0) {	// return data in reply format
        char from, to;
        if (replyFormat == 0) {	// replace newlines with semicolons
            from = '\n';
            to = ';';
        } else {		// replace semicolons with newlines
            from = ';';
            to = '\n';
        }

        for (int i = 0; i < reply_len; ++i) {
            if (replystr[i] == from)
                replystr[i] = to;
        }
        if (replystr[reply_len-1] == to) // remove trailing ';' or '\n'
            replystr.resize(reply_len-1);
    }
    return replystr;
}

/*!
// overflow is called when the buffer needs to be written out
*/
int
CommandReply::overflow (int c)
{
    if (send_buf() < 0) 	    // write the output
        return EOF;
    else if (c != EOF)
        return sputc(c);
    else
        return std::streambuf::traits_type::not_eof(c); // don't return eof, it indicates an error
}

/*!
// This function is called when the output buffer is full
// It will format the reply buffer according to the reply format
// and write it to the ConsolePort or stdout
//
// _replyFormat    	0 == newlines are converted to ';'
//                           a '\0' character (inserted by DONE)
//                           is replaced with a newline
//			1 == newlines are allowed.
//                           a '\0' character (inserted by DONE)
//                           is replaced with a newline
*/
int
CommandReply::send_buf()
{
    int len = pptr() - pbase();	    // how much data is in the buffer?
    int replyStatus = getStatus();  // get the current reply status set by OK, FAIL, or ABORT

    if (len > 0) {
        assert(replyStatus != STATUS_NOT_SET);
        if (!_enableWrite) {         // don't write the data if _enableWrite is false
            _totreplylen = len;	    // total reply length == current data length because we aren't writing

            if (len == (int) _bufsize) {        // extend the buffer if it is full
                int newbufsize = _bufsize + BUFSIZE;
                char* newbuf = new char[newbufsize+1]; // allocate bufsize plus additional byte for terminating null
                memcpy(newbuf, _outbuf, len);
                delete[] _outbuf;
                _outbuf = newbuf;
                _bufsize = newbufsize;
                setp(_outbuf, _outbuf + _bufsize);
                pbump(len);
            }

            // return an error when the reply length limit is exceeded
            if (_totreplylen >= FORMAT0_REPLY_LIMIT && !_done && replyStatus == 0 && _replyFormat == 0) {
                *this << ABORT << "Reply limit exceeded" << DONE;
                return -1;
            }
        } else {
            int i;              // loop index
            bool found_null = false;        // true if this buffer contains terminating null (note: this is not the same meaning as CommandReply::_done)
            // format output
            if (_replyFormat == 0) { // replace newlines with semicolons, '\0' with newline
                for (i = 0; i < len; ++i) {
                    if (_outbuf[i] == '\n')
                        _outbuf[i] = ';';
                    if (_outbuf[i] == DONE_TOKEN) {
                        if (_done) {
                            len = i+1; // stop at first null
                            found_null = true;
                        } else {
                            _outbuf[i] = '@';
                        }
                    }
                }

                if (found_null)	{ // This buffer contains '\0' added by DONE
                    --i;		// index to last character ('\0')
                    assert(_outbuf[i] == DONE_TOKEN);

                    if (_outbuf[i-1] == ';') // don't send double newlines at end
                        --i;

                    // send a terminating newline at the end of the last buffer
                    _outbuf[i++] = '\n'; // add a terminating newline
                    len = i;		 // set length for write
                }
            } else  { // _replyFormat == 1:  replace semicolons with newlines, '\0' with newline
                for (i = 0; i < len; ++i) {
                    if (_outbuf[i] == ';')
                        _outbuf[i] = '\n';
                    if (_outbuf[i] == DONE_TOKEN) {
                        if (_done) {
                            len = i+1; // stop at first null
                            found_null = true;
                        } else {
                            _outbuf[i] = '@';
                        }
                    }
                }

                if (found_null)	{	// This buffer contains '\0' added by DONE
                    --i;		// index to last character
                    assert(_outbuf[i] == DONE_TOKEN);

                    if (i > 0 && _outbuf[i-1] == '\n') // don't send double newlines at end
                        --i;

                    // send a terminating newline at the end of the last buffer
                    _outbuf[i++] = '\n'; // add a terminating newline

                    // send a terminating null at the end of the last buffer
                    _outbuf[i++] = DONE_TOKEN; // in format1, add a terminating null
                    _outbuf[i++] = '\n'; // perl scripts also need a terminating newline
                    len = i;		 // set length for write
                }
            }

            _replylen = len;		// save the replylen for str()
            if (len > 1 && *(_outbuf+len-2) == DONE_TOKEN) {
                _replylen -= 2;		// don't include terminating null in str()
            }
            _totreplylen += _replylen;  // keep track of total reply length

            // write output
            if (_consolePort != NULL) {	// write to mmcs server port
                try {
                    CxxSockets::Message msg;
                    msg.write(_outbuf, len);
                    if(_outbuf[len - 1] == '\0' || (len > 1 && _outbuf[len - 2] == '\0')) {
                        msg << std::ends;
                    }
                    LOG_TRACE_MSG("msg=" << msg.str());
                    LOG_TRACE_MSG("msg.len()=" << msg.str().length() << " len=" << len
                            << " replylen=" << _replylen << " totreply=" << _totreplylen);
                    _consolePort->write(msg);
                } catch (const ConsolePort::Error& e) {
                    std::ostringstream msg;
                    msg << __FUNCTION__ << " - " << e.what() << ", errno=" << e.errcode;
                    LOG_ERROR_MSG( msg.str() );
                    setp(_outbuf, _outbuf + _bufsize); // reset buffer pointers
                    *this << ABORT << msg.str() << DONE;
                    return -1;
                }
            } else {			// write to stdout
                ssize_t rc = ::write(_fd, _outbuf, _replylen);	// use _replylen so terminating null won't be written to stdout
                if (rc != (int) _replylen) {
                    std::ostringstream msg;
                    msg << __FUNCTION__ << " - " << strerror(errno);
                    LOG_ERROR_MSG(msg.str());
                    setp(_outbuf, _outbuf + _bufsize); // reset buffer pointers
                    *this << ABORT << msg.str() << DONE;
                    return -1;
                }
            }
            setp(_outbuf, _outbuf + _bufsize); // reset buffer pointers

            // return an error when the reply length limit is exceeded
            if (_totreplylen >= FORMAT0_REPLY_LIMIT && // reply limit exceeded?
                    !_done &&		// reply not done?
                    _replyFormat == 0 &&	// only applies to reply format 0
                    replyStatus == 0)	// bend  the rules to enable ABORT error messages
            {
                *this << ABORT << "Reply limit exceeded" << DONE;
                return -1;
            }
        }
    }
    return 0;
}

/*!
** getStatus() returns the status set by the OK, FAIL, and ABORT manipulators on the reply stream
** @returns  statusValue
*/
int
CommandReply::getStatus()
{
    return iword(mmcsCommandStatusIndex);
}

/*!
** setStatus() sets the command completion status for the reply stream
** @param    status -- see statusValue
*/
void
CommandReply::setStatus(int status)
{
    iword(mmcsCommandStatusIndex) = status;
}

/*!
** Manipulators for the reply stream
** OK    -- sets the status to OK and inserts "OK\n" in the output stream;
** FAIL  -- sets the status to FAIL and inserts "FAIL\n" in the output stream
** ABORT -- sets the status to ABORT and inserts "\nABORT\n" in the output stream
** DONE  -- inserts '\0' in the output stream. It does not change the status.
**
** The first output to an CommandReply stream should always be OK or FAIL.
** Subsequent output can consist of anything, including newlines
** If a fatal error is detected during output, an ABORT shoud be sent
** At the end of the stream an DONE should be sent to identify the end of the output
*/
std::ostream&
OK(std::ostream& stream)
{
    CommandReply* reply = (CommandReply*) stream.pword(mmcsCommandReplyIndex); // get the CommandReply for this stream
    assert(reply != NULL);

    reply->reset(); // reset the status and reply state
    stream.iword(mmcsCommandStatusIndex) = 0; // save the MMCS command status
    stream << OK_TOKEN;
    return stream;
}

std::ostream&
FAIL(std::ostream& stream)
{
    CommandReply* reply = (CommandReply*) stream.pword(mmcsCommandReplyIndex); // get the CommandReply for this stream
    assert(reply != NULL);

    reply->reset();
    stream.iword(mmcsCommandStatusIndex) = CommandReply::STATUS_FAIL; // save the MMCS command status
    stream  << FAIL_TOKEN;
    return stream;
}

std::ostream&
ABORT(std::ostream& stream)
{
    CommandReply* reply = (CommandReply*) stream.pword(mmcsCommandReplyIndex); // get the CommandReply for this stream
    assert(reply != NULL);

    // If a reply is complete (ie, DONE has been sent), ABORT resets the status and state
    if (reply->_done)
        reply->reset();

    // ABORT becomes FAIL if no status has been set
    if (stream.iword(mmcsCommandStatusIndex) == CommandReply::STATUS_NOT_SET)
        return FAIL(stream);

    stream.iword(mmcsCommandStatusIndex) = CommandReply::STATUS_ABORT; // save the MMCS command status
    stream << ABORT_TOKEN;
    return stream;
}

std::ostream&
DONE(std::ostream& stream)
{
    CommandReply* reply = (CommandReply*) stream.pword(mmcsCommandReplyIndex); // get the CommandReply for this stream
    assert(reply != NULL);

    // Only one DONE can be sent
    if (!reply->_done) {
        assert(stream.iword(mmcsCommandStatusIndex) != CommandReply::STATUS_NOT_SET);
        reply->_done = true;
        stream << DONE_TOKEN;
    }
    return stream;
}

} // namespace mmcs_client
