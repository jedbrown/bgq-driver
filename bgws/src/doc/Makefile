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
include ../../Make.bgws.rules


external_install_dir := $(bgq_tld_install_dir)/doc
internal_install_dir := $(bgq_tld_install_dir)/doc/bgws_internal
man1_install_dir := $(bgq_tld_install_dir)/man/man1
man8_install_dir := $(bgq_tld_install_dir)/man/man8


doc_dirs := \
  ../clients \
  ../command \
  ../common \
  ../server \
  ../server/blue_gene \
  ../server/blue_gene/diagnostics \
  ../server/blue_gene/service_actions \
  ../server/query \
  ../server/query/diagnostics \
  ../server/query/env \
  ../server/responder \
  ../server/responder/diagnostics \
  ../server/responder/environmentals \
  ../server/responder/hardware \
  ../server/responder/summary \
  ../server/teal \
  ../server/utility

source_pattern := ${addsuffix /*.cc,$(doc_dirs)} ${addsuffix /*.hpp,$(doc_dirs)}

source_files := ${wildcard $(source_pattern)}

man1_pages := \
 list_blocks \
 list_jobs

man8_pages := \
 bgws_server \
 bgws_server_log_level \
 bgws_server_refresh_config \
 bgws_server_status \
 create_block \
 delete_block

man1_pages_installed := ${man1_pages:%=$(man1_install_dir)/%.1}
man8_pages_installed := ${man8_pages:%=$(man8_install_dir)/%.8}

man_pages_links := \
 ${man1_pages:%=$(BGQ_INSTALL_DIR)/man/man1/%.1} \
 ${man8_pages:%=$(BGQ_INSTALL_DIR)/man/man8/%.8}


# Only install man pages if DOCBOOK is set
ifneq ($(DOCBOOK), no)

man_install_targets := $(man1_pages_installed) $(man8_pages_installed) $(man_pages_links)

else

man_install_targets :=

endif


install_targets := doxygen.out doxygen_internal.out $(man_install_targets)


default all install: $(install_targets)


doxygen.out: Doxyfile $(source_files)
	$(MKDIR) -p $(external_install_dir)
	cat $< | sed -e "s^@INSTALL_DIR@^$(external_install_dir)^" -e "s^@DOC_DIRS@^$(doc_dirs)^" | doxygen - > /dev/null
	touch $@

doxygen_internal.out: Doxyfile_internal ../server/documentation.hpp
	$(MKDIR) -p $(internal_install_dir)
	cat $< | sed -e "s^@INSTALL_DIR@^$(internal_install_dir)^g" -e "s^@DOC_DIRS@^$(doc_dirs)^" | doxygen - > /dev/null
	touch $@

$(man1_install_dir)/%.1 $(man8_install_dir)/%.8: %.xml
	$(MKDIR) -p $(dir $@)
	xmllint --noout --valid $<
	xsltproc -o $@ $(DOCBOOK) $<

$(BGQ_INSTALL_DIR)/man/man1/%:
	$(MKDIR) -p $(dir $@)
	$(RM) -f $@
	$(LN) -sf ../../bgws/man/man1/$(notdir $@) $@

$(BGQ_INSTALL_DIR)/man/man8/%:
	$(MKDIR) -p $(dir $@)
	$(RM) -f $@
	$(LN) -sf ../../bgws/man/man8/$(notdir $@) $@


test:


clean:
	$(RM) -r html doxygen.out doxygen_internal.out


distclean: clean


uninstall:
	$(RM) -r $(install_dir)


.PHONY: default all test install clean distclean uninstall
