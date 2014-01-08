include ../../Make.rules

#
# flags for service host code ...
#
SCAN_RING_BIN=$(BGP_BRINGUP_PATH)/testint/bin

INC_PATH=-I$(BGQ_INSTALL_DIR) -I$(BGQ_BUILD_DIR)/hwverify/src/testint/libtestint

CFLAGS_SVC=$(INC_PATH)
CXXFLAGS_SVC=$(INC_PATH)
LFLAGS_SVC=-Wl,-L$(BGP_XML_PATH)/lib \
           -Wl,-L$(BGP_RAS_PATH)/lib \
           -Wl,-L$(BGP_CONTROL_PATH)/lib \
	   -Wl,-L../lib -Wl,-L$(BGP_EXPAT_PATH)/lib

TARGET	= coreprocessor.pl

OBJS      =  

CXXFLAGS_SVC += -Wall -m64 -O0 -g -DNOAET -I$(BGQ_INSTALL_DIR) 

LINK=g++ $(LIB_PATH)

utility_library_dir=$(BGQ_INSTALL_DIR)/utility/lib
log4cxx_library_dir=$(BGQ_INSTALL_DIR)/extlib/lib

all:   sdebug install

sdebug:
	make --directory scaledebug

install:  
	mkdir -p $(BGQ_INSTALL_DIR)/coreprocessor/bin
	cp -pf *pm *pl $(BGQ_INSTALL_DIR)/coreprocessor/bin/.
	cp -pf scaledebug/sdebug $(BGQ_INSTALL_DIR)/coreprocessor/bin/.
	cp -pf scaledebug/sdebug_proxy $(BGQ_INSTALL_DIR)/coreprocessor/bin/.
	echo $(BGQ_INSTALL_DIR) > $(BGQ_INSTALL_DIR)/coreprocessor/bin/driver_path

%.pl: 

%.pm:

clean:
	make --directory scaledebug clean
	-rm -rf *.o

distclean: clean
	-rm *.vpwhistu
	-rm *.vtg 
