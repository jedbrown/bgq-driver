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


include ../Make.utility.rules
include ../../Make.rules


DIRS = cxxsockets 


TARGETS_ALL		= $(addsuffix .all,$(DIRS))
TARGETS_INSTALL		= $(addsuffix .install,$(DIRS))
TARGETS_CLEAN		= $(addsuffix .clean,$(DIRS))
TARGETS_DISTCLEAN	= $(addsuffix .distclean,$(DIRS))
TARGETS_UNINSTALL	= $(addsuffix .uninstall,$(DIRS))


TARGETS  = $(TARGETS_CLEAN)
TARGETS += $(TARGETS_DISTCLEAN)
TARGETS += $(TARGETS_UNINSTALL)


.PHONY: install all default clean distclean $(TARGETS) $(DIRS)


lib_base       = bgutility
lib_major_vers = 1
lib_minor_vers = 0
lib_minor_rev  = 0
lib_short      = lib$(lib_base).so
lib_vers       = $(lib_short).$(lib_major_vers)
lib_full       = $(lib_vers).$(lib_minor_vers).$(lib_minor_rev)
static_lib     = lib$(lib_base).a


INSTALL_LIST   = $(call INSTALL_LIB_TARGET,$(lib_short) $(lib_vers) $(lib_full) $(static_lib))
INSTALL_LIST  += $(call INSTALL_SBIN_TARGET,generate_security_certificates.sh)
INSTALL_LIST  += $(call INSTALL_BIN_TARGET,svn_revision)


CXXFLAGS += -Wall
CXXFLAGS += -Wextra
CXXFLAGS += -Wconversion
CXXFLAGS += -Wsign-conversion
CXXFLAGS += -Wlogical-op
CXXFLAGS += -Wvla
CXXFLAGS += -Wmissing-include-dirs
CXXFLAGS += -fpic
CXXFLAGS += -MD -MP
CXXFLAGS += -O2

ifeq ($(BGQ_HOST_OS),x86_64)
CFLAGS    := $(subst -Werror,,$(CFLAGS))
CXXFLAGS  := $(subst -Werror,,$(CXXFLAGS))
endif



LDFLAGS += -pthread
LDFLAGS += -Wl,-soname,$(lib_vers)
LDFLAGS += -llog4cxx
LDFLAGS += -lboost_serialization-mt
LDFLAGS += -lboost_filesystem-mt
LDFLAGS += -lboost_system-mt
LDFLAGS += -lboost_thread-mt
LDFLAGS += -lboost_program_options-mt
LDFLAGS += -lboost_regex-mt
LDFLAGS += -lboost_filesystem-mt
LDFLAGS += -lssl
LDFLAGS += -Wl,--enable-new-dtags # sets DT_RUNPATH

ar_srcs  = Log.cc
ar_srcs += LoggingProgramOptions.cc
ar_srcs += Properties.cc
ar_srcs += XMLEntity.cc
ar_srcs += Exec.cc
ar_srcs += version.cc
ar_srcs += performance/Mode.cc
ar_srcs += performance/properties.cc

ar_objs=${ar_srcs:%.cc=%.o}


so_srcs = $(ar_srcs)
so_srcs += BoolAlpha.cc
so_srcs += CableBadWires.cc
so_srcs += ExitStatus.cc
so_srcs += Inotify.cc
so_srcs += Symlink.cc
so_srcs += UserId.cc
so_srcs += portConfiguration/Acceptor.cc
so_srcs += portConfiguration/ClientPortConfiguration.cc
so_srcs += portConfiguration/Connector.cc
so_srcs += portConfiguration/Handshaker.cc
so_srcs += portConfiguration/PairConnector.cc
so_srcs += portConfiguration/pc_util.cc
so_srcs += portConfiguration/PortConfiguration.cc
so_srcs += portConfiguration/ServerPortConfiguration.cc
so_srcs += portConfiguration/ssl_util.cc
so_srcs += portConfiguration/SslConfiguration.cc


so_objs=${so_srcs:%.cc=%.o}


default: all


all: $(TARGETS_ALL)


$(lib_short): $(lib_vers)
	ln -sf $< $@


$(lib_vers): $(lib_full)
	ln -sf $< $@


$(lib_full): $(so_objs) Makefile
	$(CXX) $(CXXFLAGS) -shared -o $@ $(so_objs) $(LDFLAGS) 


$(static_lib): $(ar_objs) Makefile
	$(AR) crv  $@ $(ar_objs)

install: $(TARGETS_INSTALL) $(INSTALL_LIST)


clean: $(TARGETS_CLEAN)
	$(RM) *.o *.d performance/*.o portConfiguration/*.o performance/*.d portConfiguration/*.d
	$(RM) $(lib_short) $(lib_vers) $(lib_full) $(static_lib)
	$(RM) version.cc


distclean: clean $(TARGETS_DISTCLEAN)


uninstall: $(TARGETS_UNINSTALL)
	$(RM) $(INSTALL_LIST)


$(TARGETS)::
	$(MAKE) --directory $(basename $@) $(subst .,,$(suffix $@))

$(TARGETS_INSTALL):: $(lib_short)
	$(MAKE) --directory $(basename $@) $(subst .,,$(suffix $@))

$(TARGETS_ALL):: $(lib_short) svn_revision
	$(MAKE) --directory $(basename $@) $(subst .,,$(suffix $@))


SVN_URL = $(shell cat $(BGQ_INSTALL_DIR)/svn_revision | grep URL | cut -f2 -d\ )
DRIVER_TAG = $(shell basename $(SVN_URL))
SVN_REV = $(shell cat $(BGQ_INSTALL_DIR)/svn_revision | grep "Last Changed Rev" | cut -f4 -d\ )

version.cc: version.cc.template $(BGQ_INSTALL_DIR)/svn_revision
	sed -e "s/REVISION/$(SVN_REV)/" version.cc.template | sed -e "s/DRIVER_NAME/$(DRIVER_TAG)/" > $@

svn_revision: svn_revision.o | $(lib_short)
	$(CXX) -o $@ $^ -Wl,-rpath,$(BGQ_LIB_INSTALL_DIR) -L. -l$(lib_base)

-include *.d
-include performance/*.d
-include portConfiguration/*.d
