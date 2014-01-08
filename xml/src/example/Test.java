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

import java.io.*;


public class Test {
    public static void main(String[] args) throws IOException {
	BufferedReader fin = null;
	FileInputStream is = null;

	try {
	    is = new FileInputStream(args[0]);
	    //	    fin = new BufferedReader(is);
	} catch (IOException e) {
	    System.err.println("Could not open file: " + args[0]);
	    System.exit(1);
	}

	// dump the input
	//	String line;
	//	while ((line = fin.readLine()) != null) {
	//	    System.out.println(line);
	//	}

	XMLSerializable root = new Picture();
	root.read(is);
	root.write(System.out);
    }


};
