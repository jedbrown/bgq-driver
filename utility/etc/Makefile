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

TOP_DIR=utility
BGQ_BIN_INSTALL_DIR=$(BGQ_INSTALL_DIR)/$(TOP_DIR)/etc

include ../../Make.rules


.PHONY: default all install clean distclean


default: all


install_parts  = bg.properties.tpl
install_parts += bg.properties.ship.tpl
install_parts += bg.bringup.properties.tpl
install_parts += bg.properties.v1r1m0.tpl
install_parts += bg.properties.v1r1m1.tpl
install_parts += bg.properties.v1r1m2.tpl
install_parts += bg.properties.v1r2m0.tpl


install_targets=${install_parts:%=$(BGQ_BIN_INSTALL_DIR)/%}


bg.properties: bg.properties.template
	sed -e "s/%%BGQDB_DBNAME%%/$(BGQDB_DBNAME)/" $< | \
	sed -e "s/%%BGQDB_USER%%/$(BGQDB_USER)/" | \
	sed -e "s/%%BGQDB_PWD%%/$(BGQDB_PWD)/" | \
	sed -e "s/%%BGQDB_SCHEMA%%/$(BGQDB_SCHEMA)/" | \
	sed -e "s,%%BGQ_INSTALL_DIR%%,$(BGQ_INSTALL_DIR)," | \
	sed -e "s,%%CERTIFICATE_DIR%%,$(BGQ_INSTALL_DIR)/utility/etc/security," > $@

bg.properties.ship.tpl: bg.properties.template
	sed -e "s/%%BGQDB_DBNAME%%/BGDB0/" $< | \
	sed -e "s/user=%%BGQDB_USER%%/#user=bgqsysdb/" | \
	sed -e "s/password=%%BGQDB_PWD%%/#password=xxxxxx/" | \
	sed -e "s/%%BGQDB_SCHEMA%%/bgqsysdb/" | \
	sed -e "s,%%BGQ_INSTALL_DIR%%,/bgsys/drivers/ppcfloor," | \
	sed -e "s,%%CERTIFICATE_DIR%%,/bgsys/local/etc/security," > $@

bg.bringup.properties: bg.bringup.properties.template
	sed -e "s,%%BGQ_INSTALL_DIR%%,$(BGQ_INSTALL_DIR)," $< >$@

%.tpl: %
	$(CP) $< $@

all: bg.properties bg.properties.ship.tpl bg.bringup.properties
	$(MAKE) --directory security $@


install: $(install_targets)
	$(MAKE) --directory security $@

clean:
	$(RM) bg.properties bg.properties.ship.tpl bg.bringup.properties
	$(MAKE) --directory security $@

distclean: clean
	$(MAKE) --directory security $@

uninstall:
	$(RM) $(etc_install_dir)/bg.properties.tpl
	$(RM) $(etc_install_dir)/bg.bringup.properties.tpl
	$(MAKE) --directory security $@
