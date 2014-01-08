# Licensed Machine Code Source Materials                          #
#                                                                 #
# Product(s):                                                     #
#     Blue Gene/Q Licensed Machine Code                           #
#                                                                 #
# (C) Copyright IBM Corp.  2010, 2011                             #
#                                                                 #
# The Source code for this program is not published  or otherwise #
# divested of its trade secrets,  irrespective of what has been   #
# deposited with the U.S. Copyright Office.                       #
# --------------------------------------------------------------- #

include ../../../../../Make.rules
include ../../../../Make.hlcs.rules

TOP_DIR=hlcs
TOP_INSTALL_DIR=$(BGQ_INSTALL_DIR)/$(TOP_DIR)
MAN_INSTALL_DIR=$(TOP_INSTALL_DIR)/man/man8


XML_SRC = bg_console.xml
XML_SRC += mmcs_server.xml

XML_OBJ = $(XML_SRC:%.xml=%.8)

ifneq ($(DOCBOOK), no)

$(MAN_INSTALL_DIR):
	$(MKDIR) -p $@

$(MAN_INSTALL_DIR)/%.8: %.8 | $(MAN_INSTALL_DIR)
	$(CP) $^ $@

%.8:	%.xml
	@echo "generating man pages"
	xmllint --noout --valid $<
	xsltproc $(DOCBOOK) $<

all:	docs install

docs: $(XML_OBJ)

install: $(addprefix $(MAN_INSTALL_DIR)/, $(XML_OBJ))

uninstall:
	$(RM) -r $(MAN_INSTALL_DIR)

else

all:
install:

endif

clean:
	$(RM) *.8 *~
