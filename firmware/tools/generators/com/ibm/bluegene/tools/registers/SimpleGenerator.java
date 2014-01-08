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

public class SimpleGenerator {

	private List<String> xmlFiles = new ArrayList<String>();

	public void go() {
		try {
			for ( Iterator<String> i = xmlFiles.iterator(); i.hasNext(); ) {
				String xmlFile = i.next();
				Register r = new XmlParser().parseDocument(xmlFile);
				generateHeaderData(r);
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

	private void generateHeaderData(Register r) {

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

		System.out.println();
		System.out.println("// Register: " + r.getName());
		String[] descr = split(r.getDesription(), 96);

		do {
			System.out.println("// " + descr[0]);
			descr = split(descr[1], 96);
		}
		while ( descr[0] != null );
		System.out.println();

		String bitOffsetStr = (bitOffset > 0) ? ("" + bitOffset + "+") : "";

		for ( Iterator<Field> i = r.getFields().iterator(); i.hasNext(); ) {
			Field f = i.next();
			descr = split(f.getDescription(), 64);
			System.out.println("#define " + r.getPrefix() + f.getName() + " _BN(" + bitOffsetStr + f.getBit() + ") // " + descr[0]);

			while ( (descr[1] != null) && (descr[1].trim().length() > 0) ) {
				descr = split(descr[1], 96);
				System.out.println("          // " + descr[0]);
			}
		}
		System.out.println();
	}

	public static void main(String[] args) {
		new SimpleGenerator(args).go();
	}

	SimpleGenerator(String[] args) {
		for ( int i = 0; i < args.length; i++ ) {
			if ( "--f".equals(args[i]) ) {
				i++;
				while ( (i < args.length) && (args[i].charAt(0) != '-') )
					xmlFiles.add(args[i++]);
			}
		}
	}
}
