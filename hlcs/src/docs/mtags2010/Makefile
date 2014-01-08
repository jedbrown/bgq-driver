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
# (C) Copyright IBM Corp.  2011, 2011                              
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
.PHONY: paper all

default all: paper.pdf
paper: paper.dvi

#we have to run latex twice then bibtex, then latex three more times
#to generate the bibliography references correctly
paper.pdf: *.tex *.bib
	latex paper
	latex paper
	bibtex paper
	latex paper
	latex paper
	latex paper

clean:
	$(RM) *.aux
	$(RM) *.log
	$(RM) *.dvi
	$(RM) *.ps
	$(RM) paper.pdf
	$(RM) *.bbl
	$(RM) *.blg


