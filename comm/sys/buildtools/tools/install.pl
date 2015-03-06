#!/usr/bin/perl
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

use strict;
use warnings;

use File::Basename;
use File::Copy;
use File::Path;
use Getopt::Long;

my %o = ("m" => "0755");
Getopt::Long::Configure(qw"bundling require_order");
GetOptions(\%o, qw(d v m=s p))
    or die "Incorrect argument specification: $!";
$o{m} = oct("0".$o{m});

my $dest = pop(@ARGV).'/';
mkpath($dest, $o{v}, 0755) if $o{d};

for (@ARGV) {
    my $d = $dest.basename($_);
    print "`$_' -> `$d'\n" if $o{v};
    copy($_, $d)     or die $!;
    chmod($o{m}, $d) or die $!;
    utime((stat($_))[8,9], $d) or die $! if $o{p};
}
