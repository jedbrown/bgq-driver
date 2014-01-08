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

/*
 *  $Source: /BGP/CVS/bgp/xml/java_api/mcserver/SocketInputStream.java,v $
 *  $Id: SocketInputStream.java,v 1.4 2008/06/27 20:02:40 clappi Exp $
 *  $Revision: 1.4 $
 *  $Date: 2008/06/27 20:02:40 $
 *  $Author: clappi $
 */
package mcserver;

import java.util.*;
import java.io.InputStream;
import java.io.FilterInputStream;
import java.io.BufferedInputStream;
import java.io.ByteArrayInputStream;
import java.io.InterruptedIOException;
import java.io.IOException;
import org.xml.sax.*;
import org.xml.sax.helpers.XMLFilterImpl;
import org.xml.sax.helpers.DefaultHandler;

/**
 * Implements an InputStream that properly processes the XML socket stream from McServer
 * 
 * @author rjpoole
 * @created August 15, 2007
 */
public class SocketInputStream extends FilterInputStream
{

  BufferedInputStream socketStream = null;
  boolean             endOfInput   = false;
  final static int    MAX_RETRY    = 100;

  /**
   * Constructor for the SocketInputStream object
   * 
   * @param is
   *          The input stream (socket)
   * @exception IOException
   *              Thrown for socket errors
   */
  public SocketInputStream(InputStream is) throws IOException
  {
    super(new ByteArrayInputStream(new byte[0]));
    socketStream = new BufferedInputStream(is);
    readLine();
    // load the input buffer
  }

  /**
   * Constructor for the SocketInputStream object
   * 
   * @param is
   *          The input stream (socket)
   * @param ssl
   *          true for the SSL secure connection, false otherwise
   * @exception IOException
   *              Thrown for socket errors
   */
  public SocketInputStream(InputStream is, boolean ssl) throws IOException
  {
    super(new ByteArrayInputStream(new byte[0]));
    socketStream = new BufferedInputStream(is);
    if (ssl)
    {
      readSslLine(); // Read the command name.
      readSslLine(); // Read the XML string.
    }
    else
    {
      readLine();
    }
    // load the input buffer
  }

  /** Close overidden to do nothing, client must close the socket */
  public void close()
  {
    // throws IOException

    // socketStream.close();
    // in.close();
  }

  /**
   * Not supporting mark and reset
   * 
   * @return Always return false
   */
  public boolean markSupported()
  {
    return false;
  }

  /**
   * Reads the next byte of data from this input stream.
   * 
   * @return The byte read, or -1 if end of input reached
   * @exception IOException
   *              Thrown for socket read error
   */
  public int read() throws IOException
  {
    if(endOfInput)
    {
      return -1;
    }

    while(in.available() < 1)
    {
      readLine();
    }

    return in.read();
  }

  /**
   * Reads up to byte.length bytes of data from this input stream into an array of bytes.
   * 
   * @param b
   *          Destination byte array to read data into
   * @return The number of bytes read, or -1 if end of input reached
   * @exception IOException
   *              Thrown for socket read error
   */
  public int read(byte[] b) throws IOException
  {
    if(endOfInput)
    {
      return -1;
    }

    while(in.available() <= 1)
    {
      readLine();
    }
    /*
     * if( in.available() < b.length ) { System.out.println( "SocketInputStream short read: b.length = "+b.length+"
     * available = "+in.available()); return in.read( b, 0, in.available() ); }
     */
    return in.read(b);
  }

  /**
   * Reads up to len bytes of data from this input stream into an array of bytes.
   * 
   * @param b
   *          Destination byte array to read data into
   * @param off
   *          Offset into destination buffer to start at
   * @param len
   *          Maximum number of bytes to read
   * @return The number of bytes read, or -1 if end of input reached
   * @exception IOException
   *              Thrown for socket read error
   */
  public int read(byte[] b, int off, int len) throws IOException
  {
    if(endOfInput)
    {
      return -1;
    }

    while(in.available() <= 1)
    {
      readLine();
    }
    /*
     * if( in.available() < len ) { System.out.println( "SocketInputStream short read: len = "+len+" available =
     * "+in.available()); return in.read( b, off, in.available() ); }
     */
    return in.read(b, off, len);
  }

  /**
   * Read a line of data from the socket input stream into the internal byte buffer. This will read and strip off the
   * 4-byte length at the beginning.
   * 
   * @exception IOException
   *              Thrown for socket read error
   */
  public void readLine() throws IOException
  {
    // Read length of object
    int availableDataLength = socketStream.available();

    int retry = 0;
    while(availableDataLength < 4)
    {
      // retry++;
      // if( retry > MAX_RETRY)
      // throw new IOException( "SocketInputStream readLine retry count limit exceded, waiting for 4 bytes");
      try
      {
        Thread.sleep(10);
      }
      catch(InterruptedException ie)
      {
        throw new InterruptedIOException(ie.toString());
      }
      availableDataLength = socketStream.available();
    }

    int b1 = socketStream.read();
    while(b1 == '\n')
    {
      b1 = socketStream.read();
    }
    int b2 = socketStream.read();
    int b3 = socketStream.read();
    int b4 = socketStream.read();
    int lineLength = ((b1 << 24) | (b2 << 16) | (b3 << 8) | b4);

    // System.out.println(String.format("lineLength bytes = %02X %02X %02X %02X, %d bytes",b1,b2,b3,b4,lineLength));

    // Ensure that all the bytes for this line of data are available.
    // retry = 0;
    while(availableDataLength < lineLength)
    {
      // retry++;
      // if( retry > MAX_RETRY)
      // throw new IOException( "SocketInputStream readLine retry count limit exceded, waiting for "+lineLength);
      try
      {
        Thread.sleep(10);
      }
      catch(InterruptedException ie)
      {
        throw new InterruptedIOException(ie.toString());
      }
      availableDataLength = socketStream.available();
    }

    byte[] byteBuffer = new byte[lineLength];
    int bytesRead = 0;
    int bytesToRead = lineLength;
    int offset = 0;
    do
    {
      bytesRead = socketStream.read(byteBuffer, offset, byteBuffer.length - offset);
      offset += bytesRead;
      bytesToRead -= bytesRead;
    } while(bytesRead < lineLength);

    if(in.available() == 0 || (in.available() == 1 && in.read() == '\n'))
    {
      in = new ByteArrayInputStream(byteBuffer);
    }
    else
    {
      // System.out.println( "Doing array copy");
      byte[] leftovers = new byte[in.available() + byteBuffer.length];
      bytesRead = in.read(leftovers, 0, in.available());
      System.arraycopy(byteBuffer, 0, leftovers, bytesRead, byteBuffer.length);
      in = new ByteArrayInputStream(leftovers);
    }
  }

  /**
   * Read a line of data from the socket input stream into the internal byte buffer. This will read and strip off the
   * 4-byte length at the beginning.
   * 
   * @exception IOException
   *              Thrown for socket read error
   */
  public void readSslLine() throws IOException
  {
    // Read length of object
    int availableDataLength = socketStream.available();
    //System.out.println("SocketInputStream.readSslLine() availableDataLength = " + availableDataLength); 

    int b1 = socketStream.read();
    while(b1 == '\n')
    {
      b1 = socketStream.read();
    }
    int b2 = socketStream.read();
    int b3 = socketStream.read();
    int b4 = socketStream.read();
    int lineLength = ((b1 << 24) | (b2 << 16) | (b3 << 8) | b4);

    //System.out.println(String.format("SocketInputStream.readSslLine() lineLength bytes = %02X %02X %02X %02X, %d bytes", b1, b2, b3, b4, lineLength));

    int offset = 0;
    if (in.available() > 0) // Already read the class name. Read the XML message next.
    {
      offset = 1;
    }
    byte[] byteBuffer = new byte[lineLength + offset];
    if (offset == 1)
    {
      byteBuffer[0] = ' '; // Add a space between the class name and the XML message. 
    }
    int bytesToRead = lineLength;
    int bytesRead = 0;
    do
    {
      try
      {
        Thread.sleep(10);
      }
      catch(InterruptedException ie)
      {
        throw new InterruptedIOException(ie.toString());
      }
      bytesRead = socketStream.read(byteBuffer, offset, byteBuffer.length - offset);
      //System.out.println("SocketInputStream.readSslLine() bytesRead = " + bytesRead); 
      offset += bytesRead;
      bytesToRead -= bytesRead;
    } while(bytesToRead > 0);
    //System.out.println("SocketInputStream.readSslLine() bytesRead = " + bytesRead); 

    if(in.available() == 0 || (in.available() == 1 && in.read() == '\n'))
    {
      in = new ByteArrayInputStream(byteBuffer);
    }
    else
    {
      //System.out.println( "Doing array copy");
      byte[] leftovers = new byte[in.available() + byteBuffer.length];
      bytesRead = in.read(leftovers, 0, in.available());
      System.arraycopy(byteBuffer, 0, leftovers, bytesRead, byteBuffer.length);
      in = new ByteArrayInputStream(leftovers);
    }

    //System.out.println("SocketInputStream.readSslLine() in.available() = " + in.available()); 
  }

  /**
   * Read the raw class name. The class name comes before the XML structure and is not part of it.
   * 
   * @return The class name read from the stream.
   * @exception IOException
   *              Thrown for socket read error
   * @exception McServerReadMessageFailed
   *              Thrown if we didn't read a name from the stream.
   */
  public String readName() throws IOException, McServerReadMessageFailed
  {
    // System.out.println("readName");
    // Read name (not part of xml object)
    while(in.available() <= 1)
    {
      readLine();
    }

    byte[] nameBuffer = new byte[in.available()];
    String name = null;
    int bytesRead = 0;
    int bytesToRead = nameBuffer.length;
    int offset = 0;
    int space = 0;

    do
    {
      in.mark(nameBuffer.length);
      bytesRead = read(nameBuffer, offset, nameBuffer.length - offset);
      offset += bytesRead;
      bytesToRead -= bytesRead;

      name = new String(nameBuffer);
      space = name.indexOf(' ');

    } while(space == -1 && bytesRead < nameBuffer.length);

    if(space == -1)
    {
      String failureMsg = "Didn't get class name; length is " + bytesRead;
      McServerReadMessageFailed m = new McServerReadMessageFailed(failureMsg, "", name);
      throw m;
    }

    // if we read past the end of the name, unread the extra bytes
    if(space < bytesRead)
    {
      in.reset();
      offset -= bytesRead;
      bytesToRead += bytesRead;
      // re-read the name
      bytesRead = read(nameBuffer, offset, space - offset);
      offset += bytesRead;
      bytesToRead -= bytesRead;
    }

    name = new String(nameBuffer, 0, bytesRead);
    // System.out.println( "name = "+name );
    return name;
  }

  /**
   * Sets the end of input attribute of the SocketInputStream object. Tells parser to stop parsing.
   */
  public void setEnd()
  {
    endOfInput = true;
  }

  /**
   * Reset end of input attribute of the SocketInputStream object. Lets parser start over on new object.
   */
  public void setStart()
  {
    endOfInput = false;
  }

}
