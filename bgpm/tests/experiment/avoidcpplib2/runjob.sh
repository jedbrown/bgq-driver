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

# $1 block name
# $2 number of nodes
# $3 ppn: processes per node (1)
# $4 service host file (../mmu.svchost)
# $5 program name 
# $6 args to program, for multiple args, place inside ".. " (optional)
# pass "-" for argument to use default 

if (@ARGV < 1) {
  print "usage: runjob.sh <block> [<ppn> <svchost>] <prgrm> <args>]\n";
  exit -1;
}

my $nodes   = 1;
my $ppn     = 1;
my $svcfile = "../../mmu.svchost";
my $prog    = `ls *.elf`; chomp($prog);
my $args    = "";

$ENV{'PAMI_EAGER'} = 65536;  # keep operations eager
#$ENV{'OBJDUMP'} = 1;


my $blk = shift @ARGV;


sub GetArg
{
    my $arg = $_[0];
    if (@ARGV > 0) { 
        $arg = shift @ARGV;
        if ($arg eq '-') {
            $arg = $_[0];
        }
    }
    return $arg;     
}

$nodes = GetArg($nodes);
$ppn = GetArg($ppn);
$svcfile = GetArg($svcfile);
$prog = GetArg($prog);
if (@ARGV > 0) {
    $args = "@ARGV";
}
 
my $cmd = "../../subjob.sh $blk $nodes $ppn $svcfile $prog \"$args\"";
print "$cmd\n";
system($cmd);

