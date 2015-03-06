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

# This script will allocate the specified block.
# Returns 0 if block was allocated, 2 if it was already allocated, any other value is an error.

# Use the expect perl module to simulate interactive use
use Expect;
use Getopt::Long;

# Perl trim function to remove whitespace from the start and end of the string
sub trim($) {
	my $string = shift();
	$string =~ s/^\s+//;
	$string =~ s/\s+$//;
	return $string;
}

# Parse options
GetOptions("h!"=>\$help,
           "block=s"=>\$block);
if ($help) {
	print	"Allocate and boot a block\n";
	print   "\tallocateBlock.pl [options]\n";
	print	" Options: \n";
	print	"\t-h			Displays help text\n";
	print	"\t-block [block-name]	Specifies the name of the block to boot\n";
	exit 1;
}
if ($block eq "") {
        print "Error: you have to specify a block to allocate\n";
        exit 1;
}

# Start bg_console
$e = Expect->new;
$e->spawn("/bgsys/drivers/ppcfloor/hlcs/bin/bg_console --verbose warn")
	or die "Error: cannot run bg_console\n";
$e->expect(60, "mmcs\$")
	or die "Error: did not get console prompt\n";

# Check if the Block is already booted
$e->log_stdout(0);
$e->send("list_blocks\n");
$e->expect(10, "OK")
	or die "Error: unable to run list_blcoks\n";
$output = $e->exp_after();
$blockStatus = `echo "$output" | grep "$block " | awk '{ print \$2 }'`;
$blockStatus = trim($blockStatus);
if ($blockStatus eq "I") {
	print "Info: block is already initialized\n";
	$e->send("quit\n");
	exit 2;
}
$e->log_stdout(1);

# Allocate the block
$e->send("allocate $block\n");
$e->expect(1200, "OK", ["Block is not free" => sub { print "Info: block already allocated\n"; exit 2; } ],
	["FAIL" => sub { die "Error: unable to allocate block\n"; } ])
	or die "Error: unable to allocate block\n";

# Quit the console
$e->send("quit\n");

# If we get here, we successfully allocated the block
exit 0;
