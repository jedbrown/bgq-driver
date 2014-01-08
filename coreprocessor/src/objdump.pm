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

package objdump;

require Exporter;

@ISA         = qw(Exporter);
@EXPORT      = qw();
@EXPORT_OK   = qw();
%EXPORT_TAGS = ( all => \@EXPORT_OK );

#$NUMADDRDIGITS = 32/4;  # 32 bit addresses w/ 4 bits per hex digit
$NUMADDRDIGITS = 64/4;  # 64 bit addresses w/ 4 bits per hex digit

if($^O !~ /^win/)
{
    $OBJDUMP = "";
    push(@OBJDUMPSEARCHPATH, "/bgsys/drivers/ppcfloor/gnu-linux/bin/powerpc64-bgq-linux-objdump");
    push(@OBJDUMPSEARCHPATH, "/bgsys/bgq/drivers/x86_64.floor/gnu-linux/bin/powerpc64-bgq-linux-objdump");
    push(@OBJDUMPSEARCHPATH, "/auto/BGQ/releases/bgqdev/x86_64.driver/crosstools/bin/powerpc64-quattro-linux-objdump");
    push(@OBJDUMPSEARCHPATH, "/bgsys/drivers/ppcfloor/gnu-linux/bin/powerpc-bgp-linux-objdump");
    push(@OBJDUMPSEARCHPATH, "/bgp/drivers/ppcfloor/gnu-linux/bin/powerpc-bgp-linux-objdump");
    push(@OBJDUMPSEARCHPATH, "/bgl/BlueLight/ppcfloor/blrts-gnu/bin/powerpc-bgl-blrts-gnu-objdump");

    foreach $file (@OBJDUMPSEARCHPATH)
    {
	if(-e $file)
	{
	    $OBJDUMP = $file;
	    last;
	}
    }
    if($OBJDUMP eq "")
    {
	print "Unable to locate a BlueGene-specific objdump utility.  Failing over to the service node's objdump - some opcodes may not be defined\n";
	$OBJDUMP = "objdump";
    }

    $ADDR2LINE = "";
    push(@ADDR2LINESEARCHPATH, "/bgsys/drivers/ppcfloor/gnu-linux/bin/powerpc64-bgq-linux-addr2line");
    push(@ADDR2LINESEARCHPATH, "/bgsys/bgq/drivers/x86_64.floor/gnu-linux/bin/powerpc64-bgq-linux-addr2line");
    push(@ADDR2LINESEARCHPATH, "/auto/BGQ/releases/bgqdev/x86_64.driver/crosstools/bin/powerpc64-quattro-linux-addr2line");
    push(@ADDR2LINESEARCHPATH, "/bgsys/drivers/ppcfloor/gnu-linux/bin/powerpc-bgp-linux-addr2line");
    push(@ADDR2LINESEARCHPATH, "/bgp/drivers/ppcfloor/gnu-linux/bin/powerpc-bgp-linux-addr2line");
    push(@ADDR2LINESEARCHPATH, "/bgl/BlueLight/ppcfloor/blrts-gnu/bin/powerpc-bgl-blrts-gnu-addr2line");
    
    foreach $file (@ADDR2LINESEARCHPATH)
    {
	if(-e $file)
	{
	    $ADDR2LINE = $file;
	    last;
	}
    }
    if($ADDR2LINE eq "")
    {
	print "Unable to locate a BlueGene-specific addr2line  utility.  Failing over to the service node's addr2line\n";
	$ADDR2LINE = "addr2line";
    }
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
  if($exename =~ /remote:/)
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
      bless($self,$class); 
  }
  else
  {
      bless  ($self,$class); 
      $self->touchdisasm();
  }
  return ($self);
}

sub touchdisasm
{
    my($self) = @_;
    if(! defined $self->{"disasm"})
    {
	my @disasm = ();

	foreach $executable (split(":", $self->{"binary"}))
	{
	    my $min = 'f' x $NUMADDRDIGITS;
	    my $max = '0' x $NUMADDRDIGITS;
	    
	    my $firstline = 1;
	    print "Reading disassembly for $executable\n";	  
	    open(TMP, "$OBJDUMP -d -C $executable 2>&1 |") or die "Unable to start objdump";
	    while($line = <TMP>)
	    {
		chomp($line);
		if(($firstline) && ($line =~ /objdump:/))
		{
		    close(TMP);
		    die "Error reading the executable: $line" 
		}
		$firstline = 0;

		($iar, $disasmtext) = $line =~ /\s*(\S+)\s+(.*)/;
		$iar =~ s/://;
		$iar = ('0' x ($NUMADDRDIGITS-length($iar))) . lc($iar);

		if($disasmtext =~ />:$/)
		{
		    ($func) = $disasmtext =~ /\<(.+)\>:/;
		    $self->{"addr"}{$iar} = $func;
		    $self->{"len"}{$func} = 4;
		    
		    push(@{$self->{"TEXT_ADDRESSES"}}, $iar);
		    $sep = "F";
		}
		else
		{
		    $sep = "|";
		}
		$min = $iar if(($iar cmp $min) < 0);
		$max = $iar if(($iar cmp $max) > 0);
		$self->{"minaddr"}{$executable} = $min;
		$self->{"maxaddr"}{$executable} = $max;
		push(@disasm, "$iar $sep  $disasmtext");
	    }
	    close(TMP);
	}
	push(@{$self->{"disasm"}}, split(".", '-.'x1000 . '-'));
	push(@{$self->{"disasm"}}, sort @disasm);
	push(@{$self->{"disasm"}}, split(".", '-.'x1000 . '-'));
	@{$self->{"TEXT_ADDRESSES"}} = sort(@{$self->{"TEXT_ADDRESSES"}});
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
	my $cmd = "$ADDR2LINE -e $executable $iar";
	$location = `$cmd`;
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
    $iar = '0' x ($NUMADDRDIGITS-length($iar)) . lc($iar);
    foreach $line (@{$class->{"disasm"}})
    {
	($aiar) = $line =~ /^(\S+)\s+/;
	if($aiar eq $iar)
	{
	    last;
	}
	$index++;
    }
    @tmp = @{$class->{"disasm"}};
    splice(@tmp, $index+2*$plusminus);
    splice(@tmp, 0, $index-$plusminus);
    return join("\n", @tmp);
}

sub addsymbol
{
    my($self, $addr, $func, $length) = @_;

    $addr =~ s/^0X//i;
    $addr = '0' x ($NUMADDRDIGITS-length($addr)) . lc($addr);
    
    $self->{"addr"}{$addr} = $func;
    $self->{"len"}{$func} = $length;
    $self->{"minaddr"}{"addsymbol"} = 0;
    $self->{"maxaddr"}{"addsymbol"} = 'f' x $NUMADDRDIGITS;
    
    @{$self->{"TEXT_ADDRESSES"}} = ();
    $self->{"addr"}{'0' x $NUMADDRDIGITS} = '0' x $NUMADDRDIGITS;
    my @tmp = sort (keys %{$self->{"addr"}});
    push(@{$self->{"TEXT_ADDRESSES"}}, @tmp);;
}

sub lookup
{
    my($class, $addr) = @_;
    return $class->remote("lookup", $addr) if(exists $class->{"socket"});
    $addr =~ s/^0X//i;
    $addr = '0' x ($NUMADDRDIGITS-length($addr)) . lc($addr);
    
    my $lastaddr = "";
    foreach $textaddr (@{$class->{"TEXT_ADDRESSES"}})
    {
#	print "looking:$addr   comparing:$textaddr\n";
	if(($addr cmp $textaddr) < 0)
	{
	    return "$addr" if($lastaddr eq "");
	    return $class->{"addr"}{$lastaddr};
	}
	$lastaddr = $textaddr;
    }
    return "$addr";
}

sub findexecutable
{
    my($self, $addr) = @_;
    $addr =~ s/^0X//i;
    $addr = '0' x ($NUMADDRDIGITS-length($addr)) . lc($addr);
    foreach $key (keys %{$self->{"minaddr"}})
    {
	return $key if( (($addr cmp $self->{"minaddr"}{$key}) >= 0 &&
		        (($addr cmp $self->{"maxaddr"}{$key}) <= 0)));
    }
    return "";
}

1;
