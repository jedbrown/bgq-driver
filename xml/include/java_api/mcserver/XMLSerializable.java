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
 * $Source: /BGP/CVS/bgp/xml/java_api/mcserver/XMLSerializable.java,v $
 * $Id: XMLSerializable.java,v 1.6 2008/06/27 20:02:40 clappi Exp $
 * $Revision: 1.6 $
 * $Date: 2008/06/27 20:02:40 $
 * $Author: clappi $
 */

package mcserver;

import java.io.PrintStream;
import java.io.InputStream;
import java.io.BufferedInputStream;
import java.io.ByteArrayInputStream;
import java.io.StringReader;
import java.io.InterruptedIOException;
import org.xml.sax.Attributes;
import org.xml.sax.InputSource;
import javax.xml.parsers.SAXParser;
import javax.xml.parsers.SAXParserFactory;
import org.xml.sax.helpers.DefaultHandler;

/**
 * The abstract base class used by mcServer message classes.
 * 
 */
public abstract class XMLSerializable
{
  /** Controls the verbosity of debug information. */
  private int       debugLevel    = 1;

  /** The name of the message. */
  private String    messageName   = null;

  /** Length of data in message. */
  private int       messageLength = 0;

  /** A flag indicating if the socket connection is an SSL connection. */
  protected boolean sslConnection = false;

  /**
   * Constructs a XMLSerializable object.
   * 
   */
  public XMLSerializable()
  {
    // Determine the name of the message and stash it.
    Class c = this.getClass();
    messageName = c.getSimpleName();
  }

  /**
   * Sets the sslConnection flag.
   * 
   * @param isSslConnection
   *          the flag indicating if the connection being used is an SSL connection or not.
   */
  public void setSslConnection(boolean isSslConnection)
  {
    sslConnection = isSslConnection;
    //System.out.println("XMLSerializable." + messageName + ": setSslConnection=" + isSslConnection);
  }

  /**
   * Reads the mcServer message from an input stream.
   * 
   * @param is
   *          the input stream from which to read the mcServer message.
   * @throws McServerReadMessageFailed
   *           if there was a problem reading the message from the stream.
   * @throws InterruptedException
   *           if the thread is being interrupted.
   */
  public void read(InputStream is) throws McServerReadMessageFailed, InterruptedException
  {
    // System.out.println("read called for "+messageName);
    try
    {
      SocketInputStream sis = null;
      if (sslConnection)
      {
         //System.out.println("XMLSerializable: Contruct SocketInputStream for SSL connection.");
         sis = new SocketInputStream(is, true);  
      }
      else
      {
         //System.out.println("XMLSerializable: Contruct SocketInputStream for non-SSL connection.");
         sis = new SocketInputStream(is);  
      }
    	XMLParser handler = new XMLParser(this);
    	SAXParserFactory factory = SAXParserFactory.newInstance();

    	String name = sis.readName();
    	name.trim();
    	if(!name.equals(messageName))
    	{
    		String failureMsg = "Expecting class name " + messageName + " but got " + name;
    		McServerReadMessageFailed m = new McServerReadMessageFailed(failureMsg, messageName, name);
    		throw m;
    	}

    	// Parse the XML.
    	SAXParser saxParser = factory.newSAXParser();
    	SocketParser socketParser = new SocketParser(saxParser.getXMLReader());
    	socketParser.parse(sis, handler);
    }
    catch(McServerReadMessageFailed e)
    {
      // Propagate McServerReadMessageFailed exceptions forward.
      throw e;
    }
    catch(InterruptedIOException e)
    {
      // Reinterpret InterruptedIOExceptions into InterruptedExceptions and propagate forward.
      throw new InterruptedException(e.getMessage());
    }
    catch(Throwable t)
    {
      // Handle all unexpected failures thusly.
      t.printStackTrace();
    }
  }

  /**
   * Sets the attributes for the message.
   * 
   * @param attrs
   *          the attributes for the message.
   * @return the number of attributes.
   */
  public abstract int setAttributes(Attributes attrs);

  /**
   * Adds a sub-entity to the message.
   * 
   * @param name
   *          the name of the sub-entity.
   * @param attrs
   *          the attributes of the sub-entity.
   * @return an XMLSerializable object with the new sub-entity.
   */
  public abstract XMLSerializable addSubEntity(String name, Attributes attrs);

  /**
   * Serializes and writes the XMLSerializable object to a print stream, including the data length header.
   * 
   * @param ps
   *          the print stream to write the message to.
   */
  public abstract void write(PrintStream ps);

  /**
   * Writes a text string -- converting the XML special characters -- to a print stream, including the data length
   * header.
   * 
   * @param ps
   *          the print stream to write the message to.
   */
  public static String write_text(String text)
  {
    text = text.replaceAll("&", "&amp;");
    text = text.replaceAll("\"", "&quot;");
    text = text.replaceAll(">", "&gt;");
    text = text.replaceAll("<", "&lt;");
    text = text.replaceAll("'", "&apos;");

    // Need to also somehow recognize characters that are not printable

    return text;
  }

  /**
   * Generates an XML string representing the object.
   * 
   * @return an XML string for the object.
   */
  public abstract String getXMLString();

  /**
   * Sets the debugLevel.
   * 
   * @param debugLevel
   *          The debugLevel to set.
   */
  public void setDebugLevel(int debugLevel)
  {
    this.debugLevel = debugLevel;
  }

  /**
   * Prints a message but only if the debug level is set high enough.
   * 
   * @param message
   *          the message to print.
   * @param level
   *          the debug level for the message.
   */
  private void println(String message, int level)
  {
    print(message + "\n", level);
  }

  /**
   * Prints a message but only if the debug level is set high enough.
   * 
   * @param message
   *          the message to print.
   * @param level
   *          the debug level for the message.
   */
  private void print(String message, int level)
  {
    if(debugLevel >= level)
    {
      System.out.print(message);
    }
  }

  /**
   * Convert a 4-byte integer into the byte array.
   * 
   * @param value
   *          the 4-byte integer value.
   * @return an array of four bytes containing the specified 4-byte integer value.
   */
  public static byte[] intToByteArray(int value)
  {
    byte[] array = new byte[] { (byte)((value & 0xff000000) >>> 24),
                                (byte)((value & 0xff0000) >>> 16),
                                (byte)((value & 0xff00) >>> 8),
                                (byte)(value & 0xff)
                              };
    return array;
  }

}