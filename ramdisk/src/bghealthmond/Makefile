# begin_generated_IBM_copyright_prolog                             
#                                                                  
# This is an automatically generated copyright prolog.             
# After initializing,  DO NOT MODIFY OR MOVE                       
# ================================================================ 
#                                                                  
# Licensed Materials - Property of IBM                             
# Blue Gene/Q                                                      
# (C) Copyright IBM Corp. 2012 All Rights Reserved                 
# US Government Users Restricted Rights - Use,                     
# duplication or disclosure restricted by GSA ADP                  
# Schedule contract with IBM Corp.                                 
#                                                                  
# This software is available to you under the                      
# GNU General Public License (GPL).                                
#                                                                  
# ================================================================ 
#                                                                  
# end_generated_IBM_copyright_prolog                               
#
# Top level makefile for ramdisk/src/bghealthmond
#
# Author: Jay S. Bryant <jsbryant@us.ibm.com>
#
#
include ../Make.rules

default: install

install: ${BGQ_DISTROFS_PATH}/rpm/usr/sbin/bghealthmond.sh 

${BGQ_DISTROFS_PATH}/rpm/usr/sbin/bghealthmond.sh: ./bghealthmond.sh | ${BGQ_DISTROFS_PATH}/rpm/usr/sbin
	@${CP} $^ $@

${BGQ_DISTROFS_PATH}/rpm/usr/sbin:
	@${MKDIR} -p $@

clean:
	@echo "Cleaning bghealthmond ..."
	@${RM} ${BGQ_DISTROFS_PATH}/rpm/usr/sbin/bghealthmond.sh

distclean: clean
