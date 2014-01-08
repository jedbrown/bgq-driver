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
# (C) Copyright IBM Corp.  2005, 2011                              
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
#-*- mode: perl;-*-

use IO::Socket qw(:all);
use objdump;

if($#ARGV+1 != 2)
{
    print "Usage: $0 <binary> <port>\n";
    exit(-1);
}

$object = objdump->new($ARGV[0]);
$sock = new IO::Socket::INET(LocalPort => $ARGV[1],
			     Proto     => 'tcp',
			     Listen    => 5,
			     Reuse => 1);

die "Could not connect: $!" unless $sock;

print "objdump server running\n";

while($new_sock = $sock->accept())
{
    $new_sock->autoflush();
    if(defined(my $tcp = getprotobyname("tcp")))
    {
	setsockopt($new_sock,$tcp,TCP_NODELAY,1);
#	my $packed = getsockopt($sock, $tcp, TCP_NODELAY);
#	my $nodelay = unpack("I", $packed);
#	print "Nagle's algorithm is $nodelay\n";
    }
    
    while(defined ($line = <$new_sock>))
    {
	chomp($line);
#	print "input: $line\n";
	@parms = split(/\s+/, $line);
	if($parms[0] eq "location")
	{
	    $response = $object->location($parms[1]);
	}
	elsif($parms[0] eq "lookup")
	{
	    $response = $object->lookup($parms[1]);
	}
	elsif($parms[0] eq "disassemble")
	{
	    $response = $object->disassemble($parms[1], $parms[2]);
	}
	elsif($parms[0] eq "ping")
	{
	    $response = "pong";
	}
	else
	{
	    print "Unknown opcode\n";
	    $response = "protocol error";
	}
	chomp($response);
#	print "response: $response\n";
	print $new_sock "$response\nend-of-response\n";
    }
}
close($sock);
