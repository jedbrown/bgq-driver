# begin_generated_IBM_copyright_prolog                             
#                                                                  
# This is an automatically generated copyright prolog.             
# After initializing,  DO NOT MODIFY OR MOVE                       
#  --------------------------------------------------------------- 
#                                                                  
# Licensed Materials - Property of IBM                             
# Blue Gene/Q                                                      
# (C) Copyright IBM Corp.  2010, 2012                              
# US Government Users Restricted Rights - Use, duplication or      
#   disclosure restricted by GSA ADP Schedule Contract with IBM    
#   Corp.                                                          
#                                                                  
# This software is available to you under the Eclipse Public       
# License (EPL).                                                   
#                                                                  
#  --------------------------------------------------------------- 
#                                                                  
# end_generated_IBM_copyright_prolog                               
default:	install

all:	install

install:	


clean:
	rm -f `find com -name '*.class'`
	rm -f pcie_utl.h _pcie_decoder.c

distclean:	clean

GENERATOR = com/ibm/bluegene/tools/registers/SimpleGenerator
DECODER  = com/ibm/bluegene/tools/registers/RasDecoder
DEFINE2XML = com/ibm/bluegene/tools/registers/Define2Xml
INTBITS = com/ibm/bluegene/tools/registers/EnableInterrupts

really:	pcie_utl.h _pcie_decoder.c $(GENERATOR).class $(DECODER).class $(DEFINE2XML).class $(INTBITS).class


UTL_XML_FILES = \
	UTL_System_Bus_Agent_Status_Register.xml \
	UTL_System_Bus_Agent_Error_Severity_Register.xml \
	UTL_System_Bus_Agent_Interrupt_Enable_Register.xml \
	UTL_PCI_Express_Port_Status_Register.xml \
	UTL_PCI_Express_Port_Error_Severity_Register.xml \
	UTL_PCI_Express_Port_Interrupt_Enable_Register.xml \
	Root_Complex_Status_Register.xml

PCIE_DECODER_XML_FILES = \
	UTL_System_Bus_Agent_Status_Register.xml \
	UTL_PCI_Express_Port_Status_Register.xml \
	Root_Complex_Status_Register.xml \
	PCIE_Interrupt_State_Register.xml

pcie_utl.h:	$(GENERATOR).class $(UTL_XML_FILES)
	cat pcie_utl.h.header > pcie_utl.h
	java -classpath . com.ibm.bluegene.tools.registers.SimpleGenerator --f $(UTL_XML_FILES) >> pcie_utl.h
	cat pcie_utl.h.footer >> pcie_utl.h

_pcie_decoder.c:	$(DECODER).class $(PCIE_DECODER_XML_FILES)
	java -classpath . com.ibm.bluegene.tools.registers.RasDecoder --f $(PCIE_DECODER_XML_FILES) > _pcie_decoder.c

$(GENERATOR).class:	$(GENERATOR).java
	javac $(GENERATOR).java

$(DECODER).class:	$(DECODER).java
	javac $(DECODER).java

$(DEFINE2XML).class:	$(DEFINE2XML).java
	javac $(DEFINE2XML).java

$(INTBITS).class:	$(INTBITS).java
	javac $(INTBITS).java


define2xml:	$(DEFINE2XML).class
	@java -classpath . com.ibm.bluegene.tools.registers.Define2Xml $(parms)

intbits:	$(INTBITS).class
	java -classpath . com.ibm.bluegene.tools.registers.EnableInterrupts $(parms)
