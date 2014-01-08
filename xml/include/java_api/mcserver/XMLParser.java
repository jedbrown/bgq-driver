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
 *  $Source: /BGP/CVS/bgp/xml/java_api/mcserver/XMLParser.java,v $
 *  $Id: XMLParser.java,v 1.3 2007/08/15 17:44:46 rjpoole Exp $
 *  $Revision: 1.3 $
 *  $Date: 2007/08/15 17:44:46 $
 *  $Author: rjpoole $
 */
package mcserver;

import java.util.*;
import org.xml.sax.*;
import org.xml.sax.helpers.DefaultHandler;

/**
 *  XML Parser for McServer messages
 *
 * @author     
 * @created    August 15, 2007
 */
public class XMLParser extends DefaultHandler {
	/**
	 *  Constructor for the XMLParser object
	 *
	 * @param  root  The root object, a complete McServer message when parsed
	 */
	public XMLParser( XMLSerializable root ) {
		_stack = new Stack<XMLSerializable>();
		_root = root;
	}


	private Stack<XMLSerializable> _stack;
	private XMLSerializable _root;


	/**
	 *  Gets the empty attribute of the XMLParser object.
	 *  This is needed by the parser filter to determine if the whole message 
	 *  has been parsed.
	 *
	 * @return    true if the stack is empty (the full object has been parsed)
	 */
	public boolean isEmpty() {
		return _stack.isEmpty();
	}


	//===========================================================
	// SAX DocumentHandler methods
	//===========================================================

	/**
	 *  Called at start of the document.
	 *
	 * @exception  SAXException  Thrown for parsing errors
	 */
	public void startDocument() throws SAXException {
		//System.out.println("starting document");
	}


	/**
	 *  Called at the end of the document
	 *
	 * @exception  SAXException  Thrown for parsing errors
	 */
	public void endDocument() throws SAXException {
		//System.out.println("ending document");
	}


	/**
	 *  Called at the start of a new element.
	 *
	 * @param  namespaceURI      
	 * @param  lName             local name
	 * @param  qName             qualified name
	 * @param  attrs             attributes
	 * @exception  SAXException  Thrown for parsing errors
	 */
	public void startElement( String namespaceURI,
			String lName,	
			String qName,   
			Attributes attrs ) throws SAXException {
		//System.out.println("starting element " + qName);

		//    for (int i = 0; i < attrs.getLength(); i++) {
		//        System.out.println(i + ": " + attrs.getQName(i) + ", " + attrs.getValue(i));
		//    }

		if ( _stack.isEmpty() ) {
			_root.setAttributes( attrs );
			_stack.push( _root );
		} else {
			XMLSerializable top = _stack.peek();
			XMLSerializable newTop = top.addSubEntity( qName, attrs );
			_stack.push( newTop );
		}

	}


	/**
	 *  Called at the end of an element.
	 *  SocketParser intercepts this event, calls this method, then
	 *  if the stack is empty, it sets the end of the input.
	 *
	 * @param  namespaceURI      
	 * @param  sName             simple name
	 * @param  qName             qualified name
	 * @exception  SAXException  Thrown for parsing errors
	 */
	public void endElement( String namespaceURI,
			String sName,
			String qName
	 ) throws SAXException {
		//System.out.println("ending element " + qName);
		_stack.pop();
	}

}

