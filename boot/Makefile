#

include ../Make.rules

TARGETS = $(BGQ_INSTALL_DIR)/boot/cnk \
	  $(BGQ_INSTALL_DIR)/boot/firmware \
	  $(BGQ_INSTALL_DIR)/boot/linux \
	  $(BGQ_INSTALL_DIR)/boot/ramdisk 

# +++
#
# ---

.SUFFIXES:

.PHONY: default all install clean dist_clean fresh $(TARGETS)

default:	install

# make the all also do an install so the top level default make works..
all: install

# +++
#
# ---

install: $(BGQ_INSTALL_DIR)/boot $(TARGETS)

clean: distclean

distclean:
	-@${RM} -rf $(BGQ_INSTALL_DIR)/boot/*

fresh: distclean install

$(BGQ_INSTALL_DIR)/boot:
	-@${MKDIR} -p $(BGQ_INSTALL_DIR)/boot

$(BGQ_INSTALL_DIR)/boot/cnk: $(BGQ_INSTALL_DIR)/cnk/bin/bgq_kernel.elf
	-@ln -snf $< $@ 

$(BGQ_INSTALL_DIR)/boot/firmware: $(BGQ_INSTALL_DIR)/firmware/bin/bgq_firmware.elf
	-@ln -snf $< $@

$(BGQ_INSTALL_DIR)/boot/linux: $(BGQ_INSTALL_DIR)/linux/bin/linux
	-@ln -snf $< $@

$(BGQ_INSTALL_DIR)/boot/ramdisk: $(BGQ_INSTALL_DIR)/ramdisk/bin/ramdisk
	-@ln -snf $< $@

