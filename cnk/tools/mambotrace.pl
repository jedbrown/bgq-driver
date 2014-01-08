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

use objdump;
$obj = objdump->new("/ltmp/giampap/bgq_20080724/bgq/cnk/src/bgq_kernel.elf:/ltmp/giampap/bgq_20080724/bgq/cnk/tests/dgemm/dgemm_qpx.elf");

open(TRC, "/ltmp/giampap/bgq_20080724/bgq/cnk/tests/dgemm/d.3");

$indent = 0;
while($line = <TRC>)
{
    next if($line !~ /DEBUG:/);
    next if($line !~ /0:\s+INSTR/);

    $instrcnt++;

    ($PC, $instr) = $line =~ /PC=(\S+?):\s+\S+\s+(\S+)/;
    $func = $obj->lookup($PC);

    $dotrace = 0;
    $dotrace = 1 if($oldfunc ne $func);
    $dotrace = 1 if($instr =~ /^b.*l$/);
    $dotrace = 1 if($instr =~ /^b.*lr$/);
    $dotrace = 1 if($instr eq "sc");

    print sprintf("cnt:%-9d",$instrcnt) . "iar=$PC" . sprintf("%9s  depth:%2d     ", $instr,$indent) . '   ' x $indent . $func . "()\n" if($dotrace);

    if($instr eq "sc")
    {
#	print "syscall at iar=$PC  (cnt:$instrcnt)\n";
    }

    $indent++ if($instr =~ /^b.*l$/);
    $indent-- if($instr =~ /^b.*lr/);

    $oldfunc = $func;
}
close(TRC);
