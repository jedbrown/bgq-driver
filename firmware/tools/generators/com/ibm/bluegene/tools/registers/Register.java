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
import java.util.List;

public class Register {
	
	private String name;
	private String desription;
	private String prefix;
	private String width;
	
	private List<Field> fields = new ArrayList<Field>();

	Register( String name, String description, String prefix, String width ) {
		this.name = name;
		this.desription = description;
		this.prefix = prefix;
		this.width = width;
	}

	public String getName() {
		return name;
	}

	public String getDesription() {
		return desription;
	}

	public String getPrefix() {
		return prefix;
	}

	public String getWidth() {
		return width;
	}
	
	public void addField(Field f) {
		fields.add(f);
	}
	
	public List<Field> getFields() {
		return fields;
	}
}
