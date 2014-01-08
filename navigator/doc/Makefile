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

include ../../Make.rules


doc_install_dir := $(bgq_tld_install_dir)/doc

jsdocs_install_dir := $(doc_install_dir)/jsdoc

jsdoc_toolkit_unzip_dir := ${basename ${notdir $(JSDOC_ZIP)}}

jsdoc_toolkit_exe := $(jsdoc_toolkit_unzip_dir)/jsdoc-toolkit/jsrun.sh

jsdoc_toolkit_install_dir := $(dir $(jsdoc_toolkit_exe))

navigator_scripts_dir := ../script

navigator_script_files := ${shell find $(navigator_scripts_dir) -name "*.js"}

jsdoc_options := \
 -t=$(jsdoc_toolkit_install_dir)/templates/jsdoc \
 -d=$(jsdocs_install_dir) \
 -r $(navigator_scripts_dir)

install_targets := \
 $(doc_install_dir)/index.html \
 $(jsdocs_install_dir)/index.html


default all install: $(install_targets)


$(jsdoc_toolkit_exe): $(JSDOC_ZIP) jsdoc_toolkit-2.4.0-bg-0.patch
	$(RM) -r $(jsdoc_toolkit_unzip_dir)
	unzip $< > /dev/null
	patch -p0 <jsdoc_toolkit-2.4.0-bg-0.patch
	chmod +x $@
	touch $@


$(jsdocs_install_dir)/index.html: $(navigator_script_files) $(jsdoc_toolkit_exe)
	$(MKDIR) -p $(jsdocs_install_dir)
	JSDOCDIR=$(jsdoc_toolkit_install_dir) $(jsdoc_toolkit_exe) $(jsdoc_options)


$(doc_install_dir)/index.html: index.html
	$(MKDIR) -p $(doc_install_dir)
	$(CP) $< $@


clean:


distclean:
	$(RM) -r $(jsdoc_toolkit_unzip_dir)


uninstall:
	$(RM) -r $(doc_install_dir)


.PHONY: default all install clean distclean uninstall

