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

include ../../../../Make.rules

man8_install_dir := $(bgq_tld_install_dir)/man/man8

src_dirs := ../common ../server ../server/db2 ../utilities

src_pats := \
 ${addsuffix /*.cc,$(src_dirs)} \
 ${addsuffix /*.h,$(src_dirs)} \

man8_pages := \
 bg_realtime_server \
 realtime_server_log_level \
 realtime_server_status

man8_pages_installed := ${man8_pages:%=$(man8_install_dir)/%.8}

man_pages_links := \
 ${man8_pages:%=$(BGQ_INSTALL_DIR)/man/man8/%.8}


# Only install man pages if DOCBOOK is set
ifneq ($(DOCBOOK), no)

man_install_targets := $(man8_pages_installed) $(man_pages_links)

else

man_install_targets :=

endif


default: all


all:


doxygen.out: Doxyfile ${wildcard $(src_pats)}
	sed 's:!OUTPUT_DIR!:$(BGQ_INSTALL_DIR)/hlcs/docs/realtime:' < Doxyfile | doxygen - &> $@


$(man8_install_dir)/%.8: %.xml
	$(MKDIR) -p $(dir $@)
	xmllint --noout --valid $<
	xsltproc -o $@ $(DOCBOOK) $<

$(BGQ_INSTALL_DIR)/man/man8/%:
	$(MKDIR) -p $(dir $@)
	$(RM) -f $@
	$(LN) -sf ../../hlcs/man/man8/$(notdir $@) $@


install: doxygen.out $(man_install_targets)


uninstall:
	$(RM) -r $(BGQ_INSTALL_DIR)/hlcs/docs/realtime


clean:
	$(RM) doxygen.out


distclean:
	$(RM) doxygen.out
	

.PHONY: default all install uninstall clean distclean
