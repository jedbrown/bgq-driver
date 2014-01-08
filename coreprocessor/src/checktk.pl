#!/usr/bin/perl
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
# (C) Copyright IBM Corp.  2010, 2011                              
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

BEGIN
{
    eval
    {
        $TK_LIBRARIES_LOADED = 0;
        require Tk;
        Tk->import();
        $TK_LIBRARIES_LOADED = 1;
    };
    if($TK_LIBRARIES_LOADED == 0)
    {
	print "***********\n";
	print "** Warning: perl/Tk libraries do not appear to be installed on this system.\n";
	print "**          The Tk perl package can be found at www.cpan.org\n";
	print "**          Users will have to use coreprocessor's -nogui option until these packages have been installed\n";
	print "***********\n";
    }
}

1;
