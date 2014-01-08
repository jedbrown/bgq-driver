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


# This is a set of trivial scripting examples for the
# mmcs_console.  There are three types:
# 1) Use perl's expect module to simulate interactive use.
# 2) Execute a single command in the console and have it return
# 3) Have the console batch-process several commands from a list 
#    in a file

BEGIN {

        push @INC,"/bglhome/willstoc/perl/lib/perl5/site_perl/5.8.8";
        push @INC,"/bglhome/willstoc/perl/lib/perl5/site_perl/5.8.8/ppc-linux-thread-multi";

        }

# Use expect perl module to simulate interactive use:
use Expect;
use warnings;

print "\n****Running Expect script examples****\n";
my $e = Expect->new;

# start mmcs_console
$e->spawn("./mmcs_console") or die "Cannot run mmcs_console\n";

# look for mmcs$ prompt
$e->expect(5, "mmcs\$") or die "no prompt\n";

# send the 'list_blocks' command
$e->send("list_blocks\n");

# wait for an 'OK' to see if it worked
$e->expect(5, "OK") or die "Failed to list_blocks\n";
print "Success! list_blocks\n";

# now send a 'list_users'
$e->send("list_users\n");

# and wait for an 'OK' again
$e->expect(5, "OK") or die "Failed to list_users\n";
print "Success! list_users\n";

print "****Expect scripts done!****\n\n";

# Run a single command and check the result
print "****Running a single command and checking the return****\n";
my $result;

# Simple.  Just echo the command to mmcs_console's stdin
$result = `echo \"list_users\" | ./mmcs_console`;

# Now see if we got an 'OK'
if ( $result =~ /OK/ ) 
  {
    print "Success!\n****list_users single command completed****";
  }
else
  {
    print "Fail!\n****list_users did not succeed****";
  }

print "\n\n";

# Run a "batch file" with a list of commands.  In this case
# the file is just the following commands in the file named 'commands':
# help all
# list_blocks
# list_users
print "****Now run a batch command file****\n";
my $batch_result;

# This pipes a list of commands into the console's stdin
# The console then executes them serially
$batch_result = `cat commands | ./mmcs_console`;

$count = 0;
$pos = 0;
$done = 0;

# This just finds all of the 'OK' responses in the
# result.  This isn't really efficient for large batch
# files.  You only want to do this kind of thing for
# single multi-step test cases.
while ( $done == 0 ) {
  $pos = index($batch_result, "OK", $pos);
  if ( $pos == -1 ) {
    $done = 1;
  } 
  else {
    $count++;
  }
  $pos++;
}

if ( $count == 3 ) {
  print "****Success! Batch result OK count: ";
  print $count;
  print "****\n";
}
