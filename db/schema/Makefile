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
# (C) Copyright IBM Corp.  204, 2011                              
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

TOP_DIR=db
BGQ_BIN_INSTALL_DIR=$(BGQ_INSTALL_DIR)/$(TOP_DIR)/schema


include ../../Make.rules


DDLFILES = \
	ddl/create_table_bgq.sql \
	ddl/create_view_bgq.sql \
	ddl/create_teal.sql

TRIGGERFILES = \
	trigger/create_trigger_bgq.sql


install_parts:=\
 createBGQSchema \
 dropBGQSchema \
 dbPopulate.pl \
 dbPurge.pl \
 dbNetConfig.pl \
 dbCustomize.pl \
 dbProcessRAS.pl \
 dbUpdate.pl \
 nodeFailures.pl \
 dbIsolatedIO.sql \
 dbStandardIO.sql \
 $(DDLFILES) \
 $(TRIGGERFILES)

install_targets=${install_parts:%=$(BGQ_BIN_INSTALL_DIR)/%}


default: install


create_schema_done: createBGQSchema $(DDLFILES) $(TRIGGERFILES)
	./createBGQSchema $(DB2PATH) $(BGQDB_DBNAME) $(BGQDB_USER) $(BGQDB_PWD) $(BGQDB_SCHEMA) 
	$(TOUCH) $@

$(BGQ_BIN_INSTALL_DIR)/configs:
	$(MKDIR) -p $@

configs: configs.tar.bz2 | $(BGQ_BIN_INSTALL_DIR)/configs
	tar -x -j --overwrite --touch --file=$^
	$(CP) configs/*.txt $(BGQ_BIN_INSTALL_DIR)/configs

install: create_schema_done $(install_targets) 

$(install_targets): configs

clean:
# Do nothing, use distclean to recreate the scratch schema.

distclean: clean
	$(RM) create_schema_done createBGQSchema.log
