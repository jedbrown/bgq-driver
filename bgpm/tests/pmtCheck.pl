#!/usr/bin/perl -w
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
 
use strict;

my $cwd = `pwd -P`; chomp $cwd;
my $cfgName = shift @ARGV;
my $inFile = "$cfgName.out";

print "cwd=$cwd, cfgName=$cfgName, infile=$inFile\n";

my $curFiles=`ls`;
print $curFiles;

open FILE, "$inFile" or die "ERROR: Unable to open $inFile; $!\n";
# we could slurp file, but since they can get large, let's do one line at a time.
my $line;
my $status = 2; # retry test - didn't find a status
while (defined($line = <FILE>)) {
    if ($line =~ m/^\s*EXIT_STATUS:\s+(\d+)/io) {
        $status = ($1 == 0) ? 0 : 1;
        last;
    }
    elsif ($line =~ m/\s*Job\s+\d+\s+Exit\s+status\s+(\d+)/io) {
        $status = ($1 == 0) ? 0 : 1;
        last;
    }
}
close FILE;

print "status is $status\n";
exit $status;
