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
 * $Source: /BGP/CVS/bgp/xml/java_api/mcserver/McServerReadMessageFailed.java,v $
 * $Id: McServerReadMessageFailed.java,v 1.1 2007/03/21 04:18:06 jimvo Exp $
 * $Revision: 1.1 $
 * $Date: 2007/03/21 04:18:06 $
 * $Author: jimvo $
 */

package mcserver;

/**
 * Indicates a read of an mcServer reply message failed.
 */
public class McServerReadMessageFailed extends McServerCommandFailed
{
  private static final long serialVersionUID   = 1L;

  /** Any available reply data at the time of the failure. */
  private String            availableReplyData = null;

  /**
   * Constructs a McServerReadMessageFailed object.
   * 
   * @param message
   *          a message describing the failure.
   * @param command
   *          the command that failed.
   */
  public McServerReadMessageFailed(String message, String command, String availableReplyData)
  {
    super(message, command, -1, null);
    this.availableReplyData = availableReplyData;
  }

  /**
   * Constructs a McServerReadMessageFailed object.
   * 
   * @param message
   *          a message describing the failure.
   * @param command
   *          the command that failed.
   */
  public McServerReadMessageFailed(Exception e, String message, String command, String availableReplyData)
  {
    super(e, message, command, -1, null);
    this.availableReplyData = availableReplyData;
  }

  /**
   * Retrieves the availableReplyData.
   * 
   * @return Returns the availableReplyData.
   */
  public String getAvailableReplyData()
  {
    return availableReplyData;
  }

  /**
   * Sets the availableReplyData.
   * 
   * @param availableReplyData
   *          The availableReplyData to set.
   */
  public void setAvailableReplyData(String availableReplyData)
  {
    this.availableReplyData = availableReplyData;
  }

  /**
   * Returns a short description of this exception.
   * 
   * @return a string containing a short description of the failure.
   * @see java.lang.Throwable#toString()
   */
  public String toString()
  {
    String message = getMessage() + ". Message: " + getCommand() + ". Available reply data at time of failure: <<"
                     + availableReplyData + ">>";
    return message;
  }
}
