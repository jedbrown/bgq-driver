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

import java.util.Iterator;

public class EnableInterrupts {

    private String xmlFile = null;
    private String prefix = "?";
    private boolean genInternalErrors = false;

    public void go() {
	try {
	    Register r = new XmlParser().parseDocument(xmlFile);
	    generateCode(r);
	}
	catch (Exception e) {
	    e.printStackTrace();
	}
    }

    static final int STATE_INITIAL = 0;
    static final int STATE_CONTROL_HIGH_ACTIVE = 1;
    static final int STATE_CONTROL_LOW_ACTIVE = 2;
	
    private void generateCode(Register r) {

	System.out.println("");
	System.out.println("void fw_XXX_enableErrors() {");
	System.out.println();

	int padding = 0;
		
	for ( Iterator<Field> i = r.getFields().iterator(); i.hasNext(); ) {
	    Field f = i.next();
	    int len = 
		prefix.length() + 
		"__".length() + 
		r.getName().length() + 
		"_CONTROL_HIGH__".length() + 
		f.getName().length() + 
		"_set(1)".length();
			
	    if (padding < len) {
		padding = len;
	    }
	}
		
	int state = STATE_INITIAL;
		
	for ( Iterator<Field> i = r.getFields().iterator(); i.hasNext(); ) {
			
	    Field f = i.next();
			
	    String set = "?";
			
	    if ( f.getBit() < 32 ) {
				
		if ( state == STATE_INITIAL ) {
		    System.out.println("  DCRWritePriv( " + prefix + "( " + r.getName() + "_CONTROL_HIGH), " );
		}
		state = STATE_CONTROL_HIGH_ACTIVE;

		set = prefix + "__" +  r.getName() + "_CONTROL_HIGH__" + f.getName() + "_set(1)";
	    }
	    else {
		if ( state == STATE_CONTROL_HIGH_ACTIVE ) {
		    System.out.println(
				       "      " + 
				       pad( set, padding+1, true ) + 
				       " | // " + 
				       f.getDescription() 
				       );	

		}
				
		if ( ( state == STATE_CONTROL_HIGH_ACTIVE ) || ( state == STATE_INITIAL ) ) {
		    System.out.println("  DCRWritePriv( " + prefix + "( " + r.getName() + "_CONTROL_LOW), " );
		}
		state = STATE_CONTROL_LOW_ACTIVE;
		set = prefix + "__" +  r.getName() + "_CONTROL_LOW__" +	f.getName() + "_set(1)";

	    }
	    System.out.println(
			       "      " + 
			       pad( set, padding+1, true ) + 
			       " | // " + 
			       f.getDescription() 
			       );	

	}
		
	if ( (state == STATE_CONTROL_HIGH_ACTIVE ) || (state == STATE_CONTROL_LOW_ACTIVE ) ) {
	    System.out.println("      0 );");
	}

	if (genInternalErrors) {
	    System.out.println();
	    System.out.println("  DCRWritePriv( " + prefix + "(INTERRUPT_INTERNAL_ERROR_CONTROL_HIGH), " );
	    System.out.println("      " + prefix + "__INTERRUPT_INTERNAL_ERROR_CONTROL_HIGH__BAD_ADDRESS_set(1)        | " );
	    System.out.println("      " + prefix + "__INTERRUPT_INTERNAL_ERROR_CONTROL_HIGH__BAD_PRIV_set(1)           | " );
	    System.out.println("      " + prefix + "__INTERRUPT_INTERNAL_ERROR_CONTROL_HIGH__BAD_DATA_PARITY_set(1)    | " );
	    System.out.println("      " + prefix + "__INTERRUPT_INTERNAL_ERROR_CONTROL_HIGH__BAD_ADDRESS_PARITY_set(1) | " );
	    System.out.println("      0 );");
	    System.out.println();
	}

	System.out.println();
	System.out.println("  uint64_t mask[3] = {" );
	System.out.println("    GEA_DCR__GEA_MAPPED_INTERRUPT_STATUSx_0__y_set(1),");
	System.out.println("    GEA_DCR__GEA_MAPPED_INTERRUPT_STATUSx_1__y_set(1),");
	System.out.println("    GEA_DCR__GEA_MAPPED_INTERRUPT_STATUSx_2__y_set(1) ");
	System.out.println("  };");
	System.out.println();
	System.out.println("  fw_installGeaHandler( fw_xxx_handler, mask );");
	System.out.println();

	System.out.println("}");
	System.out.println();
    }

    public static void main(String[] args) {
	new EnableInterrupts(args).go();
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
	
    EnableInterrupts(String[] args) {
	for ( int i = 0; i < args.length; i++ ) {
	    if ( "--f".equals(args[i]) ) {
		xmlFile = args[++i];
	    }
	    else if ( "--p".equals(args[i]) ) {
		prefix = args[++i];
	    }
	    else if ( "--i".equals(args[i]) ) {
		genInternalErrors = true;
	    }
	    else {
		System.out.println("Usage: EnableInterrupts --f <xml-file> --p <prefix> [--i(nternal-errors)] [--h]");
	    }
	}
    }
}
