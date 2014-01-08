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
# (C) Copyright IBM Corp.  2012, 2012                              
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
#
# ramdisk/src/services/distrofs Makefile
#
# Author: Jay S. Bryant <jsbryant@us.ibm.com>
#
include ../../../../Make.rules
include ../Make.rules

DIRS=$(shell find * -type d  | grep -ve "\.svn")
INSTALL_DIRS=$(addprefix ${BGQ_DISTROFS_CIOS_PATH}/, ${DIRS})
#BG_FILES=$(shell find * -type f \! -empty | grep -v Makefile | grep -ve "\.svn"; \
# 		 find * -type l \! -empty | grep -v Makefile | grep -ve "\.svn")
BG_FILES=$(shell find * -type f | grep -v Makefile | grep -ve "\.svn"; \
                 find * -type l \! -empty | grep -v Makefile | grep -ve "\.svn")
INSTALL_BG_FILES=$(addprefix ${BGQ_DISTROFS_CIOS_PATH}/, ${BG_FILES})
OS_FILES=$(shell find * -type f -empty | grep -ve "\.svn" | grep -v Makefile)
#INSTALL_OS_FILES=$(addprefix ${BGQ_DISTROFS_PATH}/, ${OS_FILES})


install: ${BGQ_DISTROFS_CIOS_PATH} ${INSTALL_DIRS} ${INSTALL_BG_FILES} INSTALL_BGQ_LIBS

.PHONY : FORCE 

.NOTPARALLEL:

${BGQ_DISTROFS_CIOS_PATH}:
	@${MKDIR} -p ${BGQ_DISTROFS_CIOS_PATH}
	@chmod 777 ${BGQ_DISTROFS_CIOS_PATH}

# Create directories in the install directory.
${INSTALL_DIRS}:  
	@${MKDIR} $@	
	@chmod 777 $@


# Copy any files we provide here to the install directory.
# If this rule is triggered it means there are changes we want to get into the
# distrofs.tar file.  We force a rebuild by removing the file.
${INSTALL_BG_FILES}: ${BGQ_DISTROFS_CIOS_PATH}/% : % 
	@${CP} -d $< $@
	@if [ -e ../distrofs.tar ] ; then rm ../distrofs.tar ; fi

INSTALL_BGQ_LIBS: ${BGQ_INSTALL_DIR}/utility/lib/libbgutility.so.1.0.0 ${BGQ_INSTALL_DIR}/extlib/lib/liblog4cxx.so.10.0.0
	${CP} -a ${BGQ_INSTALL_DIR}/utility/lib/libbgutility.so* ${BGQ_DISTROFS_CIOS_PATH}/lib
	${CP} -a ${BGQ_INSTALL_DIR}/extlib/lib/liblog4cxx.so* ${BGQ_DISTROFS_CIOS_PATH}/lib

#Debug
print-%: ; @echo $* is $($*)

default: install

all: install

clean:
	@echo "Cleaning services/distrofs"
	@rm -rf ${BGQ_DISTROFS_CIOS_PATH}/*

distclean: clean
	@echo "Dist-cleaning services/distrofs"
	@rm -f ./Make.depend


fresh:	clean all
