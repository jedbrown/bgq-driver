# begin_generated_IBM_copyright_prolog                             
#                                                                  
# This is an automatically generated copyright prolog.             
# After initializing,  DO NOT MODIFY OR MOVE                       
#  --------------------------------------------------------------- 
#                                                                  
# (C) Copyright IBM Corp.  2008, 2011                              
# IBM EPL License                                                  
#                                                                  
#  --------------------------------------------------------------- 
#                                                                  
# end_generated_IBM_copyright_prolog                               

include ../../../Make.rules

TARGET=all
SILENT=

SDEBUG_PROXY          := sdebug_proxy
SDEBUG_PROXY_CC       += toolintf.cc
SDEBUG_PROXY_CC       += proxy.cc
SDEBUG_PROXY_CC       += CoreFile.cc
SDEBUG_PROXY_CC       += Job.cc
SDEBUG_PROXY_CC       += network.cc
SDEBUG_PROXYOBJ       := $(SDEBUG_PROXY_CC:%.cc=%.o)

SDEBUG_SERVER          := sdebug
SDEBUG_SERVER_CC       += server.cc
SDEBUG_SERVER_CC       += network.cc
SDEBUG_SERVEROBJ       := $(SDEBUG_SERVER_CC:%.cc=%.o)



LINUX_GNU_LIB = $(BGP_GNU_LINUX_PATH)/powerpc-bgp-linux/lib

CFLAGS  = -O2 -c -g -MD
CFLAGS += -isystem $(BGQ_INSTALL_DIR)
CFLAGS += -isystem $(BGQ_INSTALL_DIR)/extlib/include
CFLAGS += -pthread
 
IOS_LIBRARY_DIR=$(BGQ_DISTROFS_PATH)/cios/lib64

LDFLAGS  += -pthread -dynamic
LDFLAGS  += -Wl,-rpath,$(IOS_LIBRARY_DIR)
LDFLAGS  += -L $(IOS_LIBRARY_DIR) -lbgcios
LDFLAGS  += -Wl,-rpath,$(BGQ_INSTALL_DIR)/utility/lib
LDFLAGS  += -L $(BGQ_INSTALL_DIR)/utility/lib -lbgutility -lcxxsockets
LDFLAGS  += -Wl,-rpath,$(BGQ_INSTALL_DIR)/extlib/lib -L $(BGQ_INSTALL_DIR)/extlib//lib/
LDFLAGS  += -lboost_system-mt
LDFLAGS  += -lboost_filesystem-mt

.PHONY : all clean depend install distclean

.SUFFIXES:

ifeq ("${WITH_BEAM}","yes")
%.o: %.cc
        @echo "BEAM: ${CXX} $@ "`$(BASENAME) $$PWD`"/"`$(BASENAME) $<`
        @$(CXX) $(CFLAGS) $< -o $*.o ; $(BEAM_BIN) ${CFLAGS} $(BEAM_OPTIONS_CXX) -c $<
%.o: %.c
        @echo "BEAM: ${CC} $@ "`$(BASENAME) $$PWD`"/"`$(BASENAME) $<`
        @$(CC) $(CFLAGS) $< -o $*.o ; $(BEAM_BIN) ${CFLAGS} $(BEAM_OPTIONS_C) -c $<
else
%.o: %.cc
	@echo "Compiling $<"
	$(CXX) $(CFLAGS) $< -o $*.o

%.o: %.c
	@echo "Compiling $<"
	$(CC) $(CFLAGS) $< -o $*.o
endif

default: all install

all: $(SDEBUG_PROXY) $(SDEBUG_SERVER)

$(SDEBUG_PROXY): $(SDEBUG_PROXYOBJ)
	@echo $(SDEBUG_PROXYOBJ)
	@echo "Linking $(SDEBUG_PROXY)"
ifeq ($(OS_ARCH),x86_64)
	@touch $(SDEBUG_PROXY)
else
	$(CXX) -o $(SDEBUG_PROXY) $(SDEBUG_PROXYOBJ) $(LDFLAGS)
endif

$(SDEBUG_SERVER): $(SDEBUG_SERVEROBJ)
	@echo $(SDEBUG_SERVEROBJ)
	@echo "Linking $(SDEBUG_SERVER)"
ifeq ($(OS_ARCH),x86_64)
	@touch $(SDEBUG_SERVER)
else
	$(CXX) -o $(SDEBUG_SERVER) $(SDEBUG_SERVEROBJ) $(LDFLAGS)
endif


install: all
	@echo "Installing $(SDEBUG_PROXY)"
	@chmod 755 $(SDEBUG_PROXY)

clean:
	@echo "Cleaning $(PCFGD)"
	$(RM) -f $(SDEBUG_PROXY) -f $(SDEBUG_SERVER) *.o *.o_64 *.d *.objdump

distclean: clean
	@echo "Dist-cleaning `$(BASENAME) $$PWD`"
	$(RM) -f ./Make.depend

fresh:	clean depend all

# Include dependency rules
-include *.d

