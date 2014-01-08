# +++
#	Makefile for MU data mover
#
#	$(SRC):	
#		list of files to be linked with kernel to make test
#	$(TEST):
#		name of the test
#                 = name of current directory
#	$(FW_OBJ):
#		object files needed to make the test
#
# ---


#SILENT=@
SILENT=

EXTRA_CFLAGS :=-I$(BGQ_INSTALL_DIR) -I$(BGQ_INSTALL_DIR)/spi/include/kernel/klinux -I$(PWD)/.. -D __LINUX__ 
#EXTRA_CFLAGS += -DDEBUG

obj-m += bgmudev.o

bgmudev-objs := bgmudm_main.o mudm_atomic.o mudm_bcast.o mudm.o mudm_cleanup.o mudm_connect.o mudm_descriptor.o mudm_init.o mudm_ip.o mudm_lock.o mudm_msg.o mudm_personality.o mudm_pkt.o mudm_poll_bcast.o mudm_ras.o mudm_trace.o mudm_utils.o rdma_object.o

default: install

install: modules
	@${MAKE} -C $(BGQ_LINUX_SRC_PATH) SUBDIRS=`pwd` modules_install

modules:
	@${MAKE} -C $(BGQ_LINUX_SRC_PATH) SUBDIRS=`pwd` modules

clean:
	-@${MAKE} -C $(BGQ_LINUX_SRC_PATH) SUBDIRS=`pwd` clean

distclean: clean
	$(shell "[ -r $(INSTALL_MOD_PATH) ] && find $(INSTALL_MOD_PATH) -name bgmudev.ko -exec rm -f '{}' \; ")

