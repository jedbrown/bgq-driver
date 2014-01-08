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

import java.util.ArrayList;
import java.util.Iterator;
import java.util.List;

public class RasDecoder {

    private List<String> xmlFiles = new ArrayList<String>();

    public void go() {
	try {
	    for ( Iterator<String> i = xmlFiles.iterator(); i.hasNext(); ) {
		String xmlFile = i.next();
		Register r = new XmlParser().parseDocument(xmlFile);
		generateDecoder(r);
	    }
	}
	catch (Exception e) {
	    e.printStackTrace();
	}
    }

    String[] split(String s, int width) {

	if ( s == null ) {
	    return new String[] { null, null };
	}

	if ( s.length() < width ) {
	    return new String[] { s, null };
	}

	int N = s.length() - 1;
	int blank = width;

	while ( (blank < N) && (s.charAt(blank) != ' ') ) {
	    blank++;
	}

	String result = s.substring(0, blank);
	return new String[] { result, s.substring(blank).trim() };
    }

    private void generateDecoder(Register r) {

	int bitOffset = 0;

	switch ( Integer.parseInt(r.getWidth()) ) {
	case 32 :
	    bitOffset = 32;
	    break;
	case 64 : 
	    bitOffset = 0;
	    break;

	default :
	    throw new IllegalArgumentException("Unsupported register width: " + r.getWidth());
	}

	
	int fieldNameMaxWidth = 0;
	for ( Iterator<Field> i = r.getFields().iterator(); i.hasNext(); ) {
	    Field f = i.next();	
	    if ( fieldNameMaxWidth < f.getName().length() ) {
		fieldNameMaxWidth = f.getName().length();
	    }
	}

	System.out.println();
	System.out.println("  // Detailed summaries of specific error bits:");
		
	for ( Iterator<Field> i = r.getFields().iterator(); i.hasNext(); ) {
	    Field f = i.next();	
	    String first = firstSentence(f.getDescription());
	    if ( first.endsWith(".") ) {
		first = first.substring(0, first.length()-1);
	    }
	    System.out.println( 
			       "  const std::string " + 
			       pad( f.getName() + "_DESCR", fieldNameMaxWidth+6, false ) +
			       " = \" [" + f.getName().toUpperCase() + "] "+ first + "\";"
			       );
	}
		
	System.out.println();
	System.out.println("void decode_" + stringToCName(r.getName()) + "( ostringstream& str, uint64_t status ) {");
	System.out.println();
	System.out.println("  if ( status == 0 ) // If status is clear, there is nothing to do.");
	System.out.println("    return;" );
		
	System.out.println();
	System.out.println("  str << \"" + r.getName() + " error status: \";"); 
	System.out.println();
		
	for ( Iterator<Field> i = r.getFields().iterator(); i.hasNext(); ) {
	    Field f = i.next();
	    System.out.println();
	    System.out.println("  if ( ( status & ((uint64_t)1 << (63-" +  bitOffset + "-" + f.getBit() + ") ) ) != 0 ) // " + f.getName() );
	    System.out.println("    str << " + f.getName() + "_DESCR + \";\";");
	}
	System.out.println("}");
	System.out.println();
    }

    public static void main(String[] args) {
	new RasDecoder(args).go();
    }

    RasDecoder(String[] args) {
	for ( int i = 0; i < args.length; i++ ) {
	    if ( "--f".equals(args[i]) ) {
		i++;
		while ( (i < args.length) && (args[i].charAt(0) != '-') )
		    xmlFiles.add(args[i++]);
	    }
	}
    }

    String pad(String s, int length, boolean padRight ) {
		
	if ( s.length() >= length ) {
	    return s;
	}
		
	String result = s;
		
	for (int i = 0, N = length - s.length(); i < N; i++ ) {
	    if (padRight) {
		result = result + " ";
	    }
	    else {
		result = " " + result;
	    }
	}
	return result;
    }
	
    private String firstSentence(String s) {
		
	int period = s.indexOf('.');
		
	if ( period >= 0 ) {
	    return s.substring(0, period+1);
	}
	else {
	    return s;
	}
    }
	
    private String stringToCName(String s) {
	return s.replace(' ', '_');
    }
}
