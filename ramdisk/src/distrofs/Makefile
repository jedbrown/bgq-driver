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
# Temporary Make File to get things going.
#
# I will need to create something more appropriate later.
#

include ../../../Make.rules

export BGQ_DISTROFS_RPM_PATH=${BGQ_DISTROFS_PATH}/rpm
export BGQ_DISTROFS_TAR_PATH=${BGQ_DISTROFS_PATH}/tar

default: install
all: install

install: ${BGQ_DISTROFS_RPM_PATH} ${BGQ_DISTROFS_TAR_PATH}
	@${MAKE} --directory rpm install
	@${MAKE} --directory tar install

${BGQ_DISTROFS_RPM_PATH}:
	${MKDIR} -p ${BGQ_DISTROFS_RPM_PATH}

${BGQ_DISTROFS_TAR_PATH}:
	${MKDIR} -p ${BGQ_DISTROFS_TAR_PATH}

clean:
	@${MAKE} --directory rpm clean
	@${MAKE} --directory tar clean

distclean:
	@${MAKE} --directory rpm distclean
	@${MAKE} --directory tar distclean

