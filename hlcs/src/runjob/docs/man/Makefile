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

include ../../../../../Make.rules
include ../../../../Make.hlcs.rules
include ../../Make.rules


INSTALL_DIR = $(BGQ_INSTALL_DIR)/hlcs/man

HLCS_MAN_DIR 	= $(BGQ_INSTALL_DIR)/hlcs/man
RUNJOB_DOCS_DIR = $(BGQ_INSTALL_DIR)/hlcs/docs/runjob
MAN_DIR   	= $(BGQ_INSTALL_DIR)/man
MAN1_DIR  	= $(MAN_DIR)/man1
MAN8_DIR  	= $(MAN_DIR)/man8
DOXYFILE  	= section1.doxyfile
DOXYFILE       += section8.doxyfile 


all:


clean:
	rm -f doxygen.out


distclean: clean


install: doxygen.out


uninstall:
	rm -rf $(BGQ_INSTALL_DIR)/hlcs/man

$(MAN_DIR) $(HLCS_MAN_DIR) $(RUNJOB_DOCS_DIR):
	$(MKDIR) -p $@

$(MAN1_DIR) $(MAN8_DIR): $(MAN_DIR)
	$(MKDIR) -p $@


SRC  = $(wildcard ../../server/*/*/*.cc)
SRC  = $(wildcard ../../server/*/*/*.h)
SRC  = $(wildcard ../../server/*/*.cc)
SRC  = $(wildcard ../../server/*/*.h)
SRC  = $(wildcard ../../server/*.cc)
SRC  = $(wildcard ../../server/*.h)


doxygen.out: $(DOXYFILE) $(SRC) $(HLCS_MAN_DIR) $(RUNJOB_DOCS_DIR) $(MAN1_DIR) $(MAN8_DIR)
	@echo "generating documentation"
	cd ../../../../ && cat src/runjob/docs/man/section1.doxyfile | sed 's:BGQ_INSTALL_DIR:$(BGQ_INSTALL_DIR):' | doxygen - &> src/runjob/docs/man/doxygen.out
	cd ../../../../ && cat src/runjob/docs/man/section8.doxyfile | sed 's:BGQ_INSTALL_DIR:$(BGQ_INSTALL_DIR):' | doxygen - &> src/runjob/docs/man/doxygen.out
	cd $(MAN1_DIR) && cp --no-clobber --symbolic-link ../../hlcs/man/man1/* .
	cd $(MAN8_DIR) && cp --no-clobber --symbolic-link ../../hlcs/man/man8/* .
	@# this sed is a hack to work around some doxygen regression with man page output
	@# it seems to output a single quote before each period
	@sed -i "s/'\\\&//g" $(HLCS_MAN_DIR)/man1/*.1
	@sed -i "s/'\\\&//g" $(HLCS_MAN_DIR)/man8/*.8

