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
 * $Source: /BGP/CVS/bgp/xml/java_api/mcserver/McServerCommandFailed.java,v $
 * $Id: McServerCommandFailed.java,v 1.1 2007/03/21 04:18:06 jimvo Exp $
 * $Revision: 1.1 $
 * $Date: 2007/03/21 04:18:06 $
 * $Author: jimvo $
 */

package mcserver;

/**
 * Indicates an mcServer command failed.
 * 
 */
public class McServerCommandFailed extends McServerFailure
{
  private static final long serialVersionUID = 1L;

  /** The mcServer command that failed. */
  private String            command          = null;
  
  /** The return code of the command. */
  private int               rc               = -1;
  
  /** The return text for the command. */
  private String            rt               = null;

  /**
   * Constructs a McServerCommandFailed object.
   * 
   * @param message
   *          a message describing the failure.
   * @param command
   *          the command that failed.
   * @param rc
   *          the return code for the failed command.
   * @param rt
   *          the return text for the failed command.
   */
  public McServerCommandFailed(String message, String command, int rc, String rt)
  {
    super(message);
    this.command = command;
    this.rc = rc;
    this.rt = rt;
  }

  /**
   * Constructs a McServerCommandFailed object.
   * 
   * @param e
   *          the exception that prompted this exception to be thrown.
   * @param message
   *          a message describing the failure.
   * @param command
   *          the command that failed.
   * @param rc
   *          the return code for the failed command.
   * @param rt
   *          the return text for the failed command.
   */
  public McServerCommandFailed(Exception e, String message, String command, int rc, String rt)
  {
    super(e, message);
    this.command = command;
    this.rc = rc;
    this.rt = rt;
  }
  
  /**
   * Returns a short description of this exception.
   * 
   * @return a string containing a short description of the failure.
   * @see java.lang.Throwable#toString()
   */
  public String toString()
  {
    String message = getMessage() + ". Message " + getCommand() + " returned with rc == " + rc + " and rt == \"" + rt
                     + "\"";
    return message;
  }

  /**
   * Retrieves the command.
   * 
   * @return Returns the command.
   */
  public String getCommand()
  {
    return command;
  }

  /**
   * Sets the command.
   * 
   * @param command
   *          The command to set.
   */
  public void setCommand(String command)
  {
    this.command = command;
  }

  /**
   * Retrieves the rc.
   *
   * @return Returns the rc.
   */
  public int getRc()
  {
    return rc;
  }

  /**
   * Retrieves the rt.
   *
   * @return Returns the rt.
   */
  public String getRt()
  {
    return rt;
  }

  /**
   * Sets the rc.
   * 
   * @param rc The rc to set.
   */
  public void setRc(int rc)
  {
    this.rc = rc;
  }

  /**
   * Sets the rt.
   * 
   * @param rt The rt to set.
   */
  public void setRt(String rt)
  {
    this.rt = rt;
  }
}
