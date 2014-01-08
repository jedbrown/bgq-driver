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


include ../../../Make.hlcs.rules
include ../../../../Make.rules


doxygen_docs_install_dir := $(bgq_tld_install_dir)/doc/log_merge

doxygen_version := $(subst 1.7,1.6,$(findstring 1.6, $(shell doxygen --version))$(findstring 1.7, $(shell doxygen --version)))
required_doxygen_version := 1.6

man1_install_dir := $(bgq_tld_install_dir)/man/man1


man1_pages := log_merge

man1_pages_installed := ${man1_pages:%=$(man1_install_dir)/%.1}

man_pages_links := \
 ${man1_pages:%=$(BGQ_INSTALL_DIR)/man/man1/%.1}



targets := 

ifeq ($(doxygen_version),$(required_doxygen_version))
targets += doxygen_generated.ind
endif

# Only install man pages if DOCBOOK is set
ifneq ($(DOCBOOK), no)

targets += $(man1_pages_installed) $(man8_pages_installed) $(man_pages_links)

endif


default all install: $(targets)


doxygen_generated.ind: Doxyfile $(wildcard ../*.hpp ../*.cc)
	$(MKDIR) -p $(doxygen_docs_install_dir)
	cat Doxyfile | sed -e 's^@OUTPUT_DIR@^$(doxygen_docs_install_dir)^' | doxygen - > /dev/null
	$(TOUCH) $@


$(man1_install_dir)/%.1: %.xml
	$(MKDIR) -p $(dir $@)
	xmllint --noout --valid $<
	xsltproc -o $@ $(DOCBOOK) $<

$(BGQ_INSTALL_DIR)/man/man1/%:
	$(MKDIR) -p $(dir $@)
	$(RM) -f $@
	$(LN) -sf ../../hlcs/man/man1/$(notdir $@) $@


uinstall:
	$(RM) -r $(doxygen_docs_install_dir)/html


clean distclean:
	$(RM) doxygen_generated.ind


.PHONY: default all install uninstall clean distclean
