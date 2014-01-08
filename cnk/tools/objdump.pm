#-*- mode: perl;-*-
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
# (C) Copyright IBM Corp.  2005, 2012                              
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

package objdump;

require Exporter;

@ISA         = qw(Exporter);
@EXPORT      = qw();
@EXPORT_OK   = qw();
%EXPORT_TAGS = ( all => \@EXPORT_OK );

if($^O !~ /^win/)
{
#    $OBJDUMP = "/bgl/BlueLight/ppcfloor/blrts-gnu/bin/powerpc-bgl-blrts-gnu-objdump";
#    if(! -e $OBJDUMP)
#    {
#	print "Unable to locate $OBJDUMP.  Failing over to system's objdump - some opcodes may not be defined\n";
#	$OBJDUMP = "objdump";
#    }
    $OBJDUMP = "/ltmp/giampap/bgq_20080724/bgq/work/crosstools/bin/powerpc64-quattro-linux-objdump";
}

use Socket qw(:all);

sub new
{
  my ($invocant, $exename) = @_;
  my ($class   ) = ref($invocant) || $invocant;
  my ($self) =
  {
      "binary" => $exename
  };
  if($exename !~ /remote:/)
  {
      foreach $executable (split(":", $exename))
      {
	  my $firstline = 1;

	  if(0){
	  print "Reading address map for $executable\n";
	  open(TMP, "$OBJDUMP -t -C $executable 2>&1 |") or die "Unable to start objdump";
	  while($line = <TMP>)
	  {
	      chomp($line);
	      die "Error reading the executable: $line" if(($firstline) && ($line =~ /objdump:/));
	      $firstline = 0;

	      my @tmp = split(/\s+/, $line);
	      my $addr = shift @tmp;
	      shift @tmp;
	      my $type = shift @tmp;
	      if($type eq "F")
	      {
		  shift @tmp;
		  my $length = shift @tmp;
		  my $func = join(" ", @tmp);
		  $self->{"addr"}{"0x$addr"} = $func;
		  $self->{"len"}{$func} = $length;
	      }
	  }
	  close(TMP);
}	
	  $firstline = 1;
	  print "Reading disassembly for $executable\n";
	  open(TMP, "$OBJDUMP -d -C $executable 2>&1 |") or die "Unable to start objdump";
	  while($line = <TMP>)
	  {
	      chomp($line);
	      die "Error reading the executable: $line" if(($firstline) && ($line =~ /objdump:/));
	      $firstline = 0;
	
	      if($line =~ />:$/)
	      {
		  ($iar, $func) = $line =~ /(\S+)\s+\<(\S+)\>:/;
		  $iar = '0x' . uc($iar);
		  $self->{"addr"}{$iar} = $func;
		  $self->{"len"}{$func} = 4;
		
		  push(@{$self->{"TEXT_ADDRESSES"}}, $iar);
	      }
	      ($iar, $opcode) = $line =~ /\s*(\S+)\s+(.*)/;
	
	      $iar = ('0' x (16-length($iar))) . $iar;
	      push(@disasm, sprintf("%8.8x $opcode", hex($iar)));
	  }
	  close(TMP);
      }

      push(@{$self->{"disasm"}}, sort @disasm);
  }
  else
  {
      ($host, $port) = $exename =~ /remote:(\S+):(\d+)/;
      print "Connecting to \"$host\"  port $port\n";
      $exename = "$$ $exename";
      $self->{"socket"} = $exename;

      # Note:  I'm storing the socket outside of the class so that freezeThaw can be used. (avoids GLOB in class)
      $objdump_socket{$exename} = new IO::Socket::INET(PeerAddr => $host,
					   PeerPort => $port,
					   Proto => 'tcp');

      unless ($objdump_socket{$exename}) {  die "Could not connect to $host:$port"  }
      $objdump_socket{$exename}->autoflush();

      if(defined(my $tcp = getprotobyname("tcp")))
      {
          setsockopt($objdump_socket{$exename},$tcp,TCP_NODELAY,1);
#	  my $packed = getsockopt($objdump_socket{$exename}, $tcp, TCP_NODELAY);
#	  my $nodelay = unpack("I", $packed);
#	  print "Nagle's algorithm is turned ", $nodelay ? "off\n" : "on\n";
      }
  }
  bless  ($self,$class);
  return ($self);
}

sub touchdisasm
{
    my($CORE) = @_;
    if(! exist $self->{"disasm"})
    {
	foreach $executable (split(":", $CORE->{"binary"}))
	{
	    print "Reading disassembly for $executable\n";
	    open(TMP, "$OBJDUMP -d $executable |") or die "Unable to disassemble executable";
	    while($line = <TMP>)
	    {
		chomp($line);
		($iar, $opcode) = $line =~ /\s*(\S+)\s+(.*)/;
		while(length($iar) < 8) { $iar = "0$iar"; }
		push(@disasm, "$iar $opcode");
	    }
	    close(TMP);
	}
	push(@{$self->{"disasm"}}, sort @disasm);
    }
}

sub remote
{
    my($CORE) = shift @_;
    my $response = "";
    my $cmd = join(" ", @_);
#    print "send command: $cmd\n";

    return $CORE->{"cache"}{$cmd} if(exists $CORE->{"cache"}{$cmd});

    return "No objdump server connection" if(!exists $CORE->{"socket"});
    $sock = $objdump_socket{$CORE->{"socket"}};
    print $sock "$cmd\n";
    my $line = "";
    while($line = <$sock>)
    {
	chomp($line);
	last if($line =~ /end-of-response/);
#	print "reply: $line\n";
	$response .= "$line\n";
    }
    chomp($response);
    $CORE->{"cache"}{$cmd} = $response;
    return $response;
}

sub location
{
    my($CORE, $iar) = @_;
    return $CORE->remote("location", $iar) if(exists $CORE->{"socket"});
    foreach $executable (split(":", $CORE->{"binary"}))
    {
	$location = `addr2line -e $executable $iar`;
	chomp($location);
	return $location if($location !~ /\?\?/);
    }
    return "???";
}

sub disassemble
{
    my($class, $iar, $plusminus) = @_;
    return $class->remote("disassemble", $iar, $plusminus) if(exists $class->{"socket"});

    my $index = 0;
    $iar =~ s/^0x//;
    foreach $line (@{$class->{"disasm"}})
    {
	($aiar) = $line =~ /^(\S+)\s+/;
	if($aiar eq $iar)
	{
	    last;
	}
	$index++;
    }
    my $min = $index-$plusminus;
    my @tmp;
    for($index=0; $index<$plusminus*2; $index++)
    {
	if($index+$min < 0)
	{
	    push(@tmp, "");
	}
	else
	{
	    push(@tmp, $class->{"disasm"}[$index+$min]);
	}
    }
    return join("\n", @tmp);
}

sub lookup
{
    my($class, $addr) = @_;
    return $class->remote("lookup", $addr) if(exists $class->{"socket"});
    my $lastaddr = "0x0000000000000000";
    foreach $textaddr (@{$class->{"TEXT_ADDRESSES"}})
    {
#	print "$addr   $textaddr\n";
	if(($addr cmp $textaddr) < 0)
	{
	    return $class->{"addr"}{$lastaddr}
	}
	$lastaddr = $textaddr;
    }
    return "$addr";
}

1;
