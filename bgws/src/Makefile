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


dirs := \
 checkUserAdmin \
 chiron-json \
 capena-http \
 clients \
 common \
 command \
 doc \
 server \
 testsrc


targets_all := $(addsuffix .all,$(dirs))
targets_install := $(addsuffix .install,$(dirs))
targets_clean := $(addsuffix .clean,$(dirs))
targets_distclean := $(addsuffix .distclean,$(dirs))
targets_uninstall := $(addsuffix .uninstall,$(dirs))
targets_test := $(addsuffix .test,$(dirs))


targets := \
 $(targets_all) \
 $(targets_install) \
 $(targets_clean) \
 $(targets_distclean) \
 $(targets_uninstall) \
 $(targets_test)


default: all


all: $(targets_all)


# Dependencies between directories.

clients.all: $(addsuffix .all,capena-http chiron-json)
clients.install: $(addsuffix .install,capena-http chiron-json)

command.all: $(addsuffix .all,capena-http chiron-json common)
command.install: $(addsuffix .install,capena-http chiron-json common)

common.all: $(addsuffix .all,capena-http chiron-json)
common.install: $(addsuffix .install,capena-http chiron-json)

doc.all: server.all
doc.install: server.install

server.all: $(addsuffix .all,capena-http chiron-json common)
server.install: $(addsuffix .install,capena-http chiron-json common)

testsrc.all: $(addsuffix .all,capena-http)
testsrc.install: $(addsuffix .install,capena-http)


$(targets):
	$(MAKE) --directory $(basename $@) $(subst .,,$(suffix $@))


test: $(targets_test)


install: $(targets_install)

clean: $(targets_clean)

distclean: $(targets_distclean)

docs:

stats:

uninstall: $(targets_uninstall)


.PHONY: default all test install clean distclean docs status uninstall $(targets)

