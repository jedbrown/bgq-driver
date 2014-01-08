/* begin_generated_IBM_copyright_prolog                             */
/*                                                                  */
/* This is an automatically generated copyright prolog.             */
/* After initializing,  DO NOT MODIFY OR MOVE                       */
/*  --------------------------------------------------------------- */
/*                                                                  */
/* Licensed Materials - Property of IBM                             */
/* Blue Gene/Q                                                      */
/* (C) Copyright IBM Corp.  2010, 2012                              */
/* US Government Users Restricted Rights - Use, duplication or      */
/*   disclosure restricted by GSA ADP Schedule Contract with IBM    */
/*   Corp.                                                          */
/*                                                                  */
/* This software is available to you under the Eclipse Public       */
/* License (EPL).                                                   */
/*                                                                  */
/*  --------------------------------------------------------------- */
/*                                                                  */
/* end_generated_IBM_copyright_prolog                               */
package com.ibm.bluegene.tools.registers;

import javax.xml.parsers.SAXParserFactory;

import org.xml.sax.Attributes;
import org.xml.sax.SAXException;
import org.xml.sax.helpers.DefaultHandler;

public class XmlParser extends DefaultHandler {
	
    static final String REGISTER = "register";
    static final String FIELD = "field";
    static final String WIDTH = "width";
    static final String NAME = "name";
    static final String BIT = "bit";
    static final String PREFIX = "prefix";
    static final String DESCRIPTION = "description";

    String tempVal;
    private Register register;
    private boolean verbose = false;
	
    public Register parseDocument(String xmlFile) throws Exception {
	///*"src/com/ibm/bluegene/tools/registers/Example.xml"*/
	SAXParserFactory.newInstance().newSAXParser().parse( xmlFile, this );
	return register;
    }

	

    public void startElement( String uri, String localName, String qName, Attributes attributes ) throws SAXException {

	if (verbose) System.out.println("(>) qname=" + qName + "name=" + attributes.getValue(NAME) );		

	tempVal = ""; // reset
		
	if ( REGISTER.equals(qName) ) {
	    register = new Register( 
				    attributes.getValue( NAME ), 
				    attributes.getValue( DESCRIPTION ), 
				    attributes.getValue( PREFIX ), 
				    attributes.getValue( WIDTH )
				    );
	    /*
	      if ( width != null ) {
	      switch ( Integer.parseInt(width) ) {
	      case 32 : bitOffset = 32; break;
	      default : throw new IllegalArgumentException( "Unsupported register width: " + width );
	      }
	      }*/
	}
	else if ( FIELD.equals(qName) ) {
	    int bit = Integer.parseInt( attributes.getValue( BIT ) );
	    register.addField( new Field( bit, attributes.getValue( NAME ), attributes.getValue( DESCRIPTION ) ) );
	}
	else {
	    System.out.println( new IllegalStateException("Unknown XML element: " + qName ) );
	}
    }
	

    public void characters(char[] ch, int start, int length) throws SAXException {
	tempVal = new String(ch,start,length); // accumulate characters
    }
	
    public void endElement(String uri, String localName, String qName) throws SAXException {
	if (verbose) System.out.println("(<) qname=" + qName );		
    }
}
