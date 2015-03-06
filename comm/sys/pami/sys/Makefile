# begin_generated_IBM_copyright_prolog                             
#                                                                  
# This is an automatically generated copyright prolog.             
# After initializing,  DO NOT MODIFY OR MOVE                       
#  --------------------------------------------------------------- 
# Licensed Materials - Property of IBM                             
# Blue Gene/Q 5765-PER 5765-PRP                                    
#                                                                  
# (C) Copyright IBM Corp. 2011, 2012 All Rights Reserved           
# US Government Users Restricted Rights -                          
# Use, duplication, or disclosure restricted                       
# by GSA ADP Schedule Contract with IBM Corp.                      
#                                                                  
#  --------------------------------------------------------------- 
#                                                                  
# end_generated_IBM_copyright_prolog                               
HEADERS   = ${shell echo *.h}
HEADERS_O = ${patsubst %.h,  %.h.o, ${HEADERS}}
INSTALL_DIR ?= .
.PHONY: docs clean check ${SUBDIRS} default all install

default all install: check docs

docs:
	${RM} -fr ${INSTALL_DIR}/share/doc/pami
	mkdir -p ${INSTALL_DIR}/share/doc/pami
	( cat ./Doxyfile; echo "OUTPUT_DIRECTORY=${INSTALL_DIR}/share/doc/pami" ) | doxygen - >${INSTALL_DIR}/doc.out 2>${INSTALL_DIR}/doc.err

check: ${HEADERS_O} pami_checkall.h.o
pami_checkall.h.o: ${HEADERS}
	@perl cmp ${CC} $@ ${HEADERS}
	@echo All of {${HEADERS}} parse together
${SUBDIRS}:
	${MAKE} -C $@

clean:
	${RM} -r ${INSTALL_DIR}/doc *~ *.h.o
	for i in ${SUBDIRS} ; do ${MAKE} -C $$i clean ; done

%.h.o: %.h
	@perl cmp ${CC} $@ $^
	@echo "$^" parses
%.txt: %.pod
	podchecker $^
	pod2text $^ > $@
