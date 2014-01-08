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
# (C) Copyright IBM Corp.  2004, 2011                              
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

package mmcs_db;

use     warnings;
require Exporter;
use     Cwd;
use     IO::Socket qw(:all);

@ISA         = qw(Exporter);
@EXPORT      = qw();
@EXPORT_OK   = qw();
%EXPORT_TAGS = ( all => \@EXPORT_OK );
my $szSocket;

sub new {
  my ($invocant) = shift;
  my ($class   ) = ref($invocant) || $invocant;
  my ($self) = {
    version => $VERSION,
    host    => 'localhost',
    port    => 32031,
    socket  => undef,
    user    => $ENV{USER},
    verbose => 1,
    replyformat => 0,
    color   => (-t STDOUT),
    console => "",
    @_,
  };
  bless  ($self,$class);
  return ($self);
}

sub connect()
{
  my ($self, $block, $numthreads) = @_;
  $numthreads = 1 if(! defined $numthreads);
  my $sock; 
  for($x=0; $x<$numthreads; $x++)
  {
      $sockind = $#szSocket+1;
      $sock = $szSocket[$sockind] = IO::Socket::INET->new( Proto => 'tcp', PeerAddr => $self->{host}, PeerPort => $self->{port} );
      unless ($sock) {  die "Could not connect to $self->{host}:$self->{port}"  }
      $sock->autoflush();

      if(defined(my $tcp = getprotobyname("tcp")))
      {
	  setsockopt($sock,$tcp,TCP_NODELAY,1);
      }
      
      $self->{"socket"}[$x] = $sockind;
      my $rc;
      
      print $sock "setusername $self->{user}\n";
      $rc = <$sock>;
      die "Unable to set username.  The username must be a valid username" if($rc =~ /FAIL/i);
      
      print $sock "select_block $block\n";
      $rc = <$sock>;
      die "Unable to select the block.  Must be a valid allocated block.  Additionally, the username of the block owner must match." if($rc =~ /FAIL/i);
      
      print $sock "replyformat 1\n";
      $rc = <$sock>;
      $self->{"replyformat"}=1 if($rc =~ /OK/);
  }
}

sub disconnect()
{
    foreach $sock (@{$self->{"socket"}})
    {
	close $szSocket[$sock];
    }
}

sub log
{
    my($self, $data) = @_;
    $self->{"console"} .= $data . "\n";
}

sub writeCmd()
{
    my($self, $cmd, $thread) = @_;
    $self->log("writeCmd tid=$thread" . $cmd);
    my $sock = $szSocket[$self->{"socket"}[$thread]];
    print $sock $cmd or die "Unable to write to mmcs_db_server.  Did the server die?";
}

sub readCmd()
{
    my($self, $thread) = @_;
    my $sock = $szSocket[$self->{"socket"}[$thread]];
    
    my $szReturn = "";
    if($self->{"replyformat"}==0)
    {
	$szReturn = <$sock>;
	if(!defined $szReturn)
	{
	    $self->log("readCmd tid=$thread failed -- Connection to mmcs_db_server closed");
	    die "readCmd tid=$thread failed -- Connection to mmcs_db_server closed";
	}
	$szReturn =~ s/;/\n/g;
	$szReturn =~ s/\n*$//g;
    }
    else
    {
        while (1)
        {
            my $tmp = <$sock>;
	    die "mmcs protocol error.  Did mmcs_db_server die?" if(!defined $tmp);
            last if ($tmp eq "\0\n");
	    $szReturn .= $tmp;
        } 
    }
    $self->log("readCmd tid=$thread - " . $szReturn);
    return $szReturn;
}

sub sendCmd()
{
    my($self, @cmds) = @_;
    
    local $| = 1;
    my @szReturns = ();
    while($#cmds+1 > 0)
    {
	my @parallelcmds = splice(@cmds, 0, $#{@{$self->{"socket"}}}+1);
	my $tid = 0;
	foreach $cmd (@parallelcmds)
	{
	    return "OK" unless $cmd;
	    
	    $cmd =~ s/\n*$/\n/g;
	    $self->writeCmd($cmd, $tid++);
	}
	$tid = 0;
	foreach $cmd (@parallelcmds)
	{
	    push(@szReturns, $self->readCmd($tid++));
	}
    }
    
    return $szReturns[0] if($#szReturns+1 == 1);
    return @szReturns;
}

1;
