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

import java.io.BufferedReader;
import java.io.FileReader;
import java.util.StringTokenizer;

public class Define2Xml {

	public static void main(String[] args) {
		// TODO Auto-generated method stub
		Define2Xml d2xml = new Define2Xml();
		
		for ( int i = 0; i < args.length; i++ ) {
			if ( "--f".equals(args[i])) {
				d2xml.dataFile = args[++i];
			}
			else if ( "--p".equals(args[i])) {
				d2xml.prefix = args[++i];
			}
			else if ( "--v".equals(args[i])) {
				d2xml.verbose = true;
			}
			else {
				System.out.println("Usage: Define2Xml --f <define-file> --p <prefix> [--v(erbose)]");
			}
		}
		
		d2xml.go();
	}

	final static String INTBIT = "intbit";
	
	private boolean verbose = false;
	private String dataFile = null;
	private String prefix = "_tbd_";
	
	void go() {

		try {
			BufferedReader in = new BufferedReader(new FileReader(dataFile));
			String next;
			int lineNo = 0;

			System.out.println("<register width=\"64\" name=\"\" prefix=\"" + prefix + "\" description=\"tbd\">");
			
			while ( (next = in.readLine()) != null ) {

				lineNo++;
				
				if (verbose) {
					System.out.println( "" + lineNo + " : " + next);
				}
				
				StringTokenizer tokens = new StringTokenizer(next);

				if ( tokens.hasMoreTokens() ) {
					String token = tokens.nextToken();
					if ( INTBIT.equals(token) ) {
						parseIntBit(tokens);
					}
				}
			}
			System.out.println("</register>");
		}
		catch (Exception e) {
			e.printStackTrace();
		}
	}
	
	void parseIntBit(StringTokenizer tokens) {
		/*String statusRegister = */tokens.nextToken();
		int bit = Integer.parseInt(tokens.nextToken());
		String name = tokens.nextToken();
		String comment = tokens.nextToken();
		while ( tokens.hasMoreTokens() ) {
			comment += ( " " + tokens.nextToken() );
		}
		
		/*
		if ( comment.startsWith("\"")) {
			comment = comment.substring(1);
		}*/
		
		if (comment.endsWith(";")) {
			comment = comment.substring(0, comment.length()-1);
		}
		
		System.out.println("  <field bit=\"" + bit + "\" name=\"" + name.toUpperCase() + "\" description=" + comment + " />");
		//(D) reg=" + statusRegister + " bit=" + bit + " name=" + name + " cmnt=" + comment );
	}


}
