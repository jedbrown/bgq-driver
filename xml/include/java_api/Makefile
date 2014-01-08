# begin_generated_IBM_copyright_prolog                             
#                                                                  
# This is an automatically generated copyright prolog.             
# After initializing,  DO NOT MODIFY OR MOVE                       
# ================================================================ 
#                                                                  
# Licensed Materials - Property of IBM                             
#                                                                  
# Blue Gene/Q                                                      
#                                                                  
# (C) Copyright IBM Corp.  2011, 2011                              
#                                                                  
# US Government Users Restricted Rights -                          
# Use, duplication or disclosure restricted                        
# by GSA ADP Schedule Contract with IBM Corp.                      
#                                                                  
# This software is available to you under the                      
# Eclipse Public License (EPL).                                    
#                                                                  
# ================================================================ 
#                                                                  
# end_generated_IBM_copyright_prolog                               

include ../../../Make.rules

#IDLC = ../../src/xidl/xidlMain.py
IDLC = ../../include/scripts/generateCode.py

VPATH = ../../src/defs

.SUFFIXES: .java .class
#.SECONDARY:

GENERATED_PACKAGES = mcservermessagespec

PACKAGES = mcserver \
           $(GENERATED_PACKAGES)

GENERATED_JAVA_XML = MCServerMessageSpec.xml

MCSERVERPACKAGE = XMLParser.java \
                  XMLSerializable.java \
                  MCServerMessage.java

XML_JAVA = MCServerMessage.java

SRCS = $(XML_JAVA) \
       XMLParser.java \
       XMLSerializable.java \
       EchoClient.java \
       MMServlet.java         

OBJS = $(SRCS:%.java=%.class)

ifeq ($(shell uname),AIX)
 HAVE_JAVA6="no"
endif

default: install

all:
#all: mcserver.jar EchoClient.class

mcserver.jar: $(GENERATED_JAVA_XML) mcserver/XMLSerializable.java mcserver/XMLParser.java
	@echo Generating the Java source files.
	@for p in $(GENERATED_JAVA_XML); do \
		($(IDLC) --java ../../src/defs/$$p); \
	done;

ifeq ($(HAVE_JAVA6),yes)
	@echo Compiling the Java packages...
	@for p in $(PACKAGES); do \
		(echo "  $$p"); \
		($(JAVAC) -cp ./ $$p/*.java); \
	done;
	
	@echo Adding to mcserver.jar...
	@echo "  mcserver"
	@$(JAR) cfm mcserver.jar mcserver/manifest mcserver/*.class
	
	@for p in $(GENERATED_PACKAGES); do \
		(echo "  $$p"); \
		($(JAR) uf mcserver.jar $$p/manifest $$p/*.class); \
	done;
else
	@echo Must have Java 6 to compile.
endif
	
	@echo Done.
	
EchoClient.class: mcserver.jar EchoClient.java
ifeq ($(HAVE_JAVA6),yes)
	@echo Compiling the EchoClient MCServer demo.
	@$(JAVAC) -cp ./mcserver.jar EchoClient.java
else
	@echo Must have Java 6 to compile.
endif

$(BGQ_INSTALL_DIR)/xml/lib/%: %
	$(call bgq_install_bin,$<,$@)

$(BGQ_INSTALL_DIR)/xml/lib/mcserver.jar: mcserver.jar
ifeq ($(HAVE_JAVA6),yes)
	$(call bgq_install_bin,$<,$@)
else
	@echo Unable to build or install mcserver.jar since Java 6.0 is not installed on this machine.
endif

install: $(BGQ_INSTALL_DIR)/xml/lib/mcserver.jar


#	$(MKDIR) -p $(BGQ_INSTALL_DIR)/xml/lib
#ifeq ($(HAVE_JAVA6),yes)
#	$(CP) mcserver.jar $(BGQ_INSTALL_DIR)/xml/lib
#else
#	@echo Unable to build or install mcserver.jar since Java 6.0 is not installed on this machine.
#endif
#	$(CP) baremetal.jar $(BGQ_INSTALL_DIR)/xml/lib
#	$(CP) jdom.jar $(BGQ_INSTALL_DIR)/xml/lib
#	$(CP) junit-4.5.jar $(BGQ_INSTALL_DIR)/xml/lib

mc_xml_clean:
	@for p in $(GENERATED_PACKAGES); do \
	  ($(RM) -R $$p); \
	done;

clean: mc_xml_clean mm_xml_clean
	@$(RM) mcserver.jar
	@$(RM) mcserver/*.class
	@$(RM) *.class

distclean: clean
	$(RM) $(BGQ_INSTALL_DIR)/xml/lib/mcserver.jar
	$(RM) $(BGQ_INSTALL_DIR)/xml/lib/baremetal.jar
	$(RM) $(BGQ_INSTALL_DIR)/xml/lib/junit-4.5.jar

run_echo:
	$(JAVA) EchoClient



##########################################################################
# Original MM make targets
##########################################################################

mm_all: mm_xml_java $(OBJS)

mm_xml_java:
	@for hdr in $(XML_JAVA); \
		do ($(MAKE) $$HDR); \
	done;

%.java: %.xml
	$(IDLC) -j $< > $@

%.class: %.java
	$(JAVAC) $<
	

mm_xml_clean:
	@for hdr in $(XML_JAVA); do \
		(rm -f ./$$hdr); \
	done;


