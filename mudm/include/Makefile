#
# Make rules to install include files....
#
docs_install_dir=$(BGQ_INSTALL_DIR)/mudm/docs/external

include ../../Make.rules

$(eval $(call bgq_header_make, mudm/include))

TARGETS_ALL		= $(addsuffix .all,$(DIRS))
TARGETS_INSTALL		= $(addsuffix .install,$(DIRS))
TARGETS_CLEAN		= $(addsuffix .clean,$(DIRS))
TARGETS_DISTCLEAN	= $(addsuffix .distclean,$(DIRS))
TARGETS_DOCS		= $(addsuffix .docs,$(DIRS))

TARGETS = $(TARGETS_ALL)       \
	  $(TARGETS_INSTALL)   \
	  $(TARGETS_CLEAN)     \
	  $(TARGETS_DISTCLEAN)

default:	install

all:	this.all $(TARGETS_ALL)

this.all: install

install: this.install $(TARGETS_INSTALL) docs
	@mkdir -p $(BGQ_INSTALL_DIR)/mudm/src
	@cp -u ../src/*.[ch] $(BGQ_INSTALL_DIR)/mudm/src/.

this.install:	$(TARGET_HDRS)

clean:	this.clean $(TARGETS_CLEAN)

this.clean:	
	       

distclean:	this.distclean $(TARGETS_DISTCLEAN)

this.distclean:	clean

this.docs: docs

docs: Doxyfile
	$(MKDIR) -p $(docs_install_dir)
	sed -e s^@OUTPUT_DIR@^$(docs_install_dir)^ Doxyfile | doxygen - &> doxygen.out


# +++
#	Each element in "$TARGETS" is of the form:
#		subdirectory.target
#	"$(basename $@)" picks off "subdirectory"
#	"$(subst .,,$(suffix $@))" picks off "target"
# ---

$(TARGETS)::
	$(MAKE) --directory $(basename $@) $(subst .,,$(suffix $@))


