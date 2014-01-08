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

use strict;
use FileHandle;

# Compare the Punit latches from scandump files. 
#  The file names must include "core#"" in the names to identify
#  which core is significant in this file.
# The files must have been prefiltered to contain only the interested core
#  Thus  grep -P '^PU_[^_]*00' <file> > 'core00-p1.scanout00 
#  to get core 00 info only.
# It only supports differences between 3 cores - and the file names should be
#  appropriate.
# Only lines which do not compare are printed, and the data leading columns indicate
#  how they miscompared.  The core number is listed first, followed by the # 
#  of files which entries for this latch which were '1'.
# The dots indicate successful comparisons - rarer than shown below.
#
# 0# .  .  .   .   .     latch name
# .  1# .  .   .   .     latch name
# .  .  2# .   .   .     latch name
#
# Miscompares between cores is indicated by the next columns
# .  .  .  0-1 0-2 2-3   latch name

sub ExtractCoreNum # filename
{
    my $fileName = $_[0];   
    my $coreNum;
    my $ftail = $fileName;
    $ftail =~ s|.*/||; 
    if ($ftail =~ m/core(\d+)/i) {
       $coreNum = $1;
    }
    else {
        die "ERROR: Cannot find 'core#' in filename '$fileName'\n";
    }
    return $coreNum;
}



my @fileNames = @ARGV;

# sort filename by CoreNum
@fileNames = sort { ExtractCoreNum($a) <=> ExtractCoreNum($b); } @fileNames;



my @fh;
my @fcore;
my $i;
for ($i=0; $i<@fileNames; $i++) {
    $fh[$i] = new FileHandle;
    open ($fh[$i], "< $fileNames[$i]") or die "ERROR: unable to open file '$fileNames[$i]'; $!\n";
    $fcore[$i] = ExtractCoreNum($fileNames[$i]);
} 

# for now - let's assume all files are same size and have same latch entries in 
# an equal order - we'll check to see if that's the case and fail if not.
my $done = 0;
print "@fileNames\n";
while (!$done) {   
   my @lines = {};
   my @latchNames = {};
   my @values = {};
   my $i;
   # read next line from each file
   for ($i=0; $i<@fh; $i++) {
       $lines[$i] = $fh[$i]->getline;
       if (!defined($lines[$i])) { 
           $done = 1;
       }
       else {
           ($latchNames[$i], $values[$i]) = ($lines[$i] =~ m/(\S+)\s+0b(\d+)/);
       }       
   }  
   #print "$lines[0]\n";
   
   if (!$done) {
       my $baseLatch = $latchNames[0];  
       $baseLatch =~ s/\\//g;       
       $baseLatch =~ s/^PU_\S+?\d+//;
       $baseLatch =~ s/([\(\)\.])/\\$1/g;
       #print "$baseLatch\n";
       
       # make comparisons.
       my $diff = 0;
       for ($i=0; $i<@fh-1; $i++) {
           my $pat = "^PU_\\S+?\\d+$baseLatch";
           my $name = $latchNames[$i];
           $name =~ s/\\//g;       
           
           #$name =~ s/([\(\)\.])/\\$1/g;
           #print "$name\n";
           if ($name !~ m/$pat/) {
               print STDERR "ERROR: compare prob in $fileNames[0] vs $fileNames[$i]\n";
               die "ERROR: $latchNames[$i] !~ $pat\n";
           }
           if ($values[$i] != $values[$i+1]) {
               if (   (($values[0]==$values[1]) && ($values[0]==$values[2]))
                    &&(($values[3]==$values[4]) && ($values[3]==$values[5])) ) {
                    $diff = 1;
                }
           }
       }
       if ($diff) {
           foreach my $val (@values) {
               printf "%6.6s ", $val;
           }
           print "$latchNames[0]\n";
       }
   }                 
}
