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

# Use the expect perl module to simulate interactive use
use Expect;
use Getopt::Long;

# Parse options
GetOptions("h!"=>\$help,
           "io!"=>\$io,
           "block=s"=>\$block);
if ($help) {
	print	"Free a block\n";
	print   "\tfreeBlock.pl [options]\n";
	print	" Options: \n";
	print	"\t-h			Displays help text\n";
	print	"\t-block [block-name]	Specifies the name of the block to free\n";
	exit 1;
}
if ($block eq "") {
	print "Error: you have to specify a block to free\n";
	exit 1;
}

# Start bg_console
$e = Expect->new;
$e->spawn("/bgsys/drivers/ppcfloor/hlcs/bin/bg_console --verbose warn")
	or die "Error: cannot run bg_console\n";
$e->expect(60, "mmcs\$")
	or die "Error: did not get console prompt\n";

# Allocate the block
$e->send("free $block\n");
$e->expect(300, "OK", ["block is not allocated" => sub { print "Info: block already free\n"; } ],
	["FAIL" => sub { die "Error: unable to free block\n"; } ])
	or die "Error: unable to free block\n";

# Quit the console
$e->send("quit\n");

