#!/bin/tcsh
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
# (C) Copyright IBM Corp.  2010, 2012                              
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

# $1 job id

if ($#argv != 1) then
  echo "usage: cancel.sh <job_id>"
  exit
endif

if ($1 == $USER) then
  set res=`llq | grep $USER | awk '{ print $1 }'`
  if ($res != "") then
    llcancel $res
  else
    echo "no running jobs found for $1"
  endif
  
  else
  llcancel $1
endif
