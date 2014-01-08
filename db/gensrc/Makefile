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
# (C) Copyright IBM Corp.  2004, 2011                              
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

include ../../Make.rules

done_indicator_file=codegen_done

generated_files=\
 $(done_indicator_file) \
 ../include/api/tableapi/gensrc/*.h \
 ../src/api/tableapi/gensrc/*.cc

.PHONY: default all install clean distclean

default: install

all: install

$(done_indicator_file): codegen ../schema/create_schema_done functiongen.awk headergen.awk copyright.ch.txt
	./$< $(DB2PATH) ../src/api/tableapi/gensrc ../include/api/tableapi/gensrc $(BGQDB_DBNAME) $(BGQDB_USER) $(BGQDB_PWD) $(BGQDB_SCHEMA)
	$(TOUCH) $@

install: $(done_indicator_file)

clean:
#Do nothing

distclean:
	$(RM) $(generated_files)
