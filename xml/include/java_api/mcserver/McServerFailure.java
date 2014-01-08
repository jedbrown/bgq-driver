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
 * $Source: /BGP/CVS/bgp/xml/java_api/mcserver/McServerFailure.java,v $
 * $Id: McServerFailure.java,v 1.1 2007/03/21 04:18:06 jimvo Exp $
 * $Revision: 1.1 $
 * $Date: 2007/03/21 04:18:06 $
 * $Author: jimvo $
 */

package mcserver;

/**
 * Indicates a generic mcServer failure.
 */
public class McServerFailure extends Exception
{
  private static final long serialVersionUID = 1L;

  /**
   * Constructs a McServerFailure object.
   * 
   * @param message
   *          a message describing the failure.
   */
  public McServerFailure(String message)
  {
    super(message);
  }

  /**
   * Constructs a McServerFailure object.
   * 
   * @param e
   *          the exception that prompted this exception to be thrown.
   * @param message
   *          a message describing the failure.
   */
  public McServerFailure(Exception e, String message)
  {
    super(message, e);
  }
}
