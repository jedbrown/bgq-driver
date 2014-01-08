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

#ifndef _XSER_STREAM_H_
#define _XSER_STREAM_H_

//
// Abstract base class for Istream and Ostream of bluegene system level accelerator.
//
class XserIstream
{
public:
    virtual ~XserIstream() {};
    /*!
    // indicate if we have seen an error in transmission.
    // this indicates the other end disconnected.
    */
    virtual bool eof()  = 0;
    /*!
    // Read from the tcp input stream
    //
    // @param s -- stream characters to read
    // @n -- number of bytes to read
    // @returns -- number of bytes read
    */
    virtual int read ( char* s , unsigned n ) = 0;

    /*!
    // check if the stream buffer is at the end of the current record
    // @param none
    // @returns true if this is the end of record
    */
    virtual bool is_endr() = 0;

    /*!
    // go to the next record.
    //
    // @param none.
    // @returns none
    */
    virtual void next_rec() = 0;

    /*!
    // Check to see if we saw a record underflow.
    //
    // @param none.
    // @returns true if we had a record underflow, false if not
    */
    virtual bool rec_underflow() = 0;

protected:

private:

};

class XserOstream
{
public:
    virtual ~XserOstream() {};
    /*!
    // indicate if we have seen an error in transmission.
    // this indicates the other end disconnected.
    */
    virtual bool eof() = 0;
    /*!
    // Write to the tcp output stream.
    //
    // @param s -- stream characters to write.
    // @n -- number of bytes to write
    // @returns -- reference to this stream.
    */
    virtual void write ( const char* s , unsigned n )  = 0;


    /*!
    // Flush the current contents of the stream buffer to the other end
    // in a chunk of stream data.
    // @param none.
    // @returns reference to this stream.
    */
    virtual void flush ( ) = 0;

    /*!
    // Send the end of record chunk.  This implies a flush
    // @param none.
    // @returns -- reference to this stream.
    //
    */
    virtual void write_endr ( ) = 0;

protected:

private:

};


#endif

