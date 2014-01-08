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
# (C) Copyright IBM Corp.  2010, 2011                              
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
include ../../Make.utility.rules


default: all


all install: $(etc_install_dir)/security/admin/cert.pem


$(etc_install_dir)/security/admin/cert.pem: ../../src/generate_security_certificates.sh
	$(MKDIR) -p $(etc_install_dir)
	mycwd=`pwd`; cd $(etc_install_dir); $$mycwd/$< --noperms


clean:


distclean: clean

uninstall: distclean
	$(RM) -r $(etc_install_dir)/security


.PHONY: default all install clean distclean uninstall
