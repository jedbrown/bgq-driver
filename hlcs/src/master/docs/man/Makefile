include ../../../../../Make.rules
include ../../../../Make.hlcs.rules

TOP_DIR=hlcs
TOP_INSTALL_DIR=$(BGQ_INSTALL_DIR)/$(TOP_DIR)
MAN_INSTALL_DIR=$(TOP_INSTALL_DIR)/man/man8


XML_SRC = list_agents.xml
XML_SRC += list_clients.xml
XML_SRC += get_errors.xml
XML_SRC += get_history.xml
XML_SRC += bgmaster_server.xml
XML_SRC += master_stop.xml
XML_SRC += alias_wait.xml
XML_SRC += binary_status.xml
XML_SRC += binary_wait.xml
XML_SRC += fail_over.xml
XML_SRC += master_start.xml
XML_SRC += master_status.xml
XML_SRC += bgmaster_server_refresh_config.xml
XML_SRC += bgmaster_server_log_level.xml
XML_SRC += bgagentd.xml
XML_SRC += monitor_master.xml

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

default: all

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

distclean: clean
