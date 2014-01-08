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
 *  $Source: /BGP/CVS/bgp/xml/java_api/mcserver/SocketParser.java,v $
 *  $Id: SocketParser.java,v 1.2 2007/08/15 17:44:46 rjpoole Exp $
 *  $Revision: 1.2 $
 *  $Date: 2007/08/15 17:44:46 $
 *  $Author: rjpoole $
 */
package mcserver;

import java.util.*;
import java.io.IOException;
import org.xml.sax.*;
import org.xml.sax.helpers.XMLFilterImpl;

/**
 *  Filter XML parsing for McServer socket connection processing.
 *
 * @author     rjpoole
 * @created    August 15, 2007
 */
public class SocketParser extends XMLFilterImpl {

	SocketInputStream sis;
	XMLParser myHandler;


	/**
	 *  Constructor for the SocketParser object.
	 *
	 * @param  parent  XMLReader for parsing input.
	 */
	public SocketParser( XMLReader parent ) {
		setParent( parent );
	}


	/**
	 *  Overrides parse routine to set up McServer socket connection processing.
	 *
	 * @param  input             Socket connection to McServer
	 * @param  handler           The XML parser
	 * @exception  SAXException  Thrown on parsing errors
	 * @exception  IOException   Thrown on socket errors
	 */
	public void parse( SocketInputStream input, XMLParser handler )
			 throws SAXException, IOException {
		sis = input;
		input.setStart();
		myHandler = handler;
		setContentHandler( handler );
		parse( new InputSource( input ) );
	}


	/**
	 *  Intercept endElement events.
	 *  endElement method on the XMLParser is called, then, if the stack is empty,
	 *  the end of input marker is set for the socket stream so that the parser will
	 *  stop parsing.
	 *  Other events are processed directly by the XMLParser.
	 *
	 * @param  uri               Passed to XMLParser
	 * @param  localName         Passed to XMLParser
	 * @param  qName             Passed to XMLParser
	 * @exception  SAXException  Thrown on parsing error
	 */
	public void endElement( String uri,
			String localName,
			String qName )
			 throws SAXException {
		myHandler.endElement( uri, localName, qName );

		if ( myHandler.isEmpty() ) {
			sis.setEnd();
		}
	}
}

