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

package bgpcorereader;

use     warnings;

require Exporter;

@ISA         = qw(Exporter);
@EXPORT      = qw();
@EXPORT_OK   = qw();
%EXPORT_TAGS = ( all => \@EXPORT_OK );
$VERSION     = v1.0;
$SVERSION    = '$Id: bgpcorereader.pm,v 1.3 2008/05/19 21:42:26 mtn Exp $';
@DCRLIST=();

sub new 
{
  my ($invocant, $options) = @_;
  my ($class   ) = ref($invocant) || $invocant;
  
  my ($self) = {
      "version" => $VERSION,
      "have_refreshed" => 0,
      "options" => $options
  };
  
  $self->{"options"}{"debugionode"}      = 0;
  $self->{"options"}{"debugcomputenode"} = 1;
  
  @{$self->{"searchpaths"}} = split(":", $$options{"-c"});
  $self->{"mincore"} = 0;  
  $self->{"mincore"} = $$options{"-mincore"}  if(exists $$options{"-mincore"});
  $self->{"maxcore"} = 64*2*1024;  
  $self->{"maxcore"} = $$options{"-maxcore"}  if(exists $$options{"-maxcore"});
  $self->{"coreprefix"} = "core";
  $self->{"coreprefix"} = $ENV{"BG_COREDUMPFILEPREFIX"} if(exists $ENV{"BG_COREDUMPFILEPREFIX"});
  $self->{"coreprefix"} = $$options{"-coreprefix"} if(exists $$options{"-coreprefix"});
  
  bless  ($self,$class);
  return ($self);
}

sub readcore
{
    my($CORE, $filename) = @_;
    $id = $filename;
    $filecnt++;
    print "filename: $filename   (filecount $filecnt)\n";
    
    $oldmode = 0;
    $mode = 0;
    $validCore = 0;
    
    my $corerawdata = "";
    my @ids = ();
    open(TMP, $filename);
    while($line = <TMP>)
    {
	$corerawdata .= $line;
	chomp($line);
	$mode = 1 if($line =~ /^Personality/i);
	if($line =~ /^\+\+\+ID/i)
	{
	    $mode = 2;
	    ($id) = $line =~ /ID\s*(.*)/;
	    $id =~ s/\s+/_/og;
	    $id =~ s/,//og;
	    $id = "core_$id";
	    $corerawdata = $line;
	    $idcount++;
	    if($idcount % 1000 == 0)
	    {
		print "processing id $id   (count=$idcount)\n";
	    }
 	    
	    push(@ids, $id);
	    
	    ($CORE->{$id}{"personality"}{"mpirank"}) = $MPIRank;
	    $CORE->{"mpimap"}[$CORE->{$id}{"personality"}{"mpirank"}] = $id;    
	    $CORE->{$id}{"procstatus"} = "core";
	    $CORE->{$id}{"kernelstatus"} = "core";
	    $CORE->{$id}{"ionode"} = 0;	    
	    $CORE->{$id}{"node_accessible"} = 1;
	    $CORE->{$id}{"stripframes"}     = 0;
	    $CORE->{$id}{"clipthreshold"}   = 0;
	    $CORE->{$id}{"corefilepath"} = $filename;
	    
#	    my($x,$y,$z,$t) = $XYZT =~ /(\d+),\s*(\d+),\s*(\d+),\s*(\d+)/;	    
#	    if(defined $x)
#	    {
#		$CORE->{$id}{"personality"}{"x"} = $x;
#		$CORE->{$id}{"personality"}{"y"} = $y;
#		$CORE->{$id}{"personality"}{"z"} = $z;
#		$CORE->{$id}{"personality"}{"t"} = $t;
#		
#		$CORE->{"maxx"} = $x+1 	    if($x >= $CORE->{"maxx"});
#		$CORE->{"maxy"} = $y+1 	    if($y >= $CORE->{"maxy"}); 
#		$CORE->{"maxz"} = $z+1 	    if($z >= $CORE->{"maxz"});
#		$CORE->{"maxt"} = $t+1 	    if($t >= $CORE->{"maxt"});
#	    }   
	}
	$mode = 5 if($line =~ /General/);
	$mode = 3 if($line =~ /^\+\+\+STACK/i);
	$mode = 4 if($line =~ /Memory:/);
	$mode = 0 if($line =~ /^\-\-\-/);
	if($mode != $oldmode)
	{
	    $oldmode = $mode;
	    $validCore = 1;
	    next;
	}
	
	if($mode == 1)
	{
	    ($XYZT)    = $line =~ /:\s*(.*)/  if($line =~ /XYZT/);
	    ($MPIRank) = $line =~ /:\s*(\d+)/ if($line =~ /Rank/);
	    ($DDRSize) = $line =~ /:\s*(\d+)/ if($line =~ /DDR Size/);
	    ($Mode)    = $line =~ /:\s*(\S+)/ if($line =~ /Mode/);
	}
	if($mode == 2)
	{
	    ($CORE->{$id}{"coreregs"}{"IAR"}) = $line =~ /while executing instruction at\.+(\S+)/ if($line =~ /instruction/i);	    
	}
	if($mode == 3)
	{
	    $line =~ s/^\s*\S+\s+//;
	    unshift(@{$CORE->{$id}{"stacktrace"}}, "0x" . $line) if($line !~ /Address/i);
	    $CORE->{$id}{"corerawdata"}  = $corerawdata;
	}
	if($mode == 4)
	{
#	    ($CORE->{$id}{"stack_top"})    = $line =~ /Stack top\s*:\s*(\S+)/i;
#	    ($CORE->{$id}{"stack_bottom"}) = $line =~ /Stack bottom\*:\s*(\S+)/i;
#	    ($CORE->{$id}{"heap_end"})     = $line =~ /end of heap\.+(\S+)/;
#	    ($CORE->{$id}{"prog_start"})   = $line =~ /start of program\.+(\S+)/;	    
	}
	if($mode == 5)
	{
	    next if($line =~ /Special/);
	    next if($line =~ /Floating/);
	    $line =~ s/^\s*//;
	    my @regvals = split(/\s+/, $line);
	    foreach $regval (@regvals)
	    {
		my ($regname,$regval) = split("=", $regval);
		$regnamehash{$regname} = 1;
		$CORE->{$id}{"coreregs"}{$regname} = $regval;
	    }
	}
    }
    die "Invalid corefile format: $filename"  if($validCore == 0);
    
    print "done reading cores\n";
}


sub start_refresh
{
    my($CORE) = @_;
    $CORE->{"maxx"} = 0;
    $CORE->{"maxy"} = 0;
    $CORE->{"maxz"} = 0;
    $CORE->{"maxt"} = 0;
    @{$CORE->{"files"}} = ();
    my $numcorefiles = 0;
    my $coreprefix = $CORE->{"coreprefix"};
    
    foreach $coredir (@{$CORE->{"searchpaths"}})
    {
	if(-f $coredir)
	{
	    push(@{$CORE->{"files"}}, $coredir);
	    $numcorefiles++;
	}
	else
	{
	    opendir(DIR, $coredir) or die "Unable to open corefile directory \"$coredir\"";
	    my @files = readdir(DIR);
	    closedir(DIR);
	    
	    foreach $file (@files)
	    {
		if($file =~ /^$coreprefix\.\d+$/)
		{
		    ($tag) = $file =~ /^$coreprefix\.(\d+)$/;
		    if(($tag >= $CORE->{"mincore"})&&($tag <= $CORE->{"maxcore"}))
		    {
			$numcorefiles++;
			push(@{$CORE->{"files"}}, "$coredir/$file");
		    }
		}
	    }
	}
    }
    if($numcorefiles == 0)
    {
	my $dirs = join(", ", @{$CORE->{"searchpaths"}});
	die "Could not find any core files in the specified directories \"$dirs\" within the specified range";
    }
}

sub is_refresh_inprogress
{
    my($CORE) = @_;
    my @files = @{$CORE->{"files"}};
    return 1 if($#files >= 0);
    return 0;
}

sub refresh
{
    my($CORE) = @_;
    start_refresh($CORE);
    while(is_refresh_inprogress($CORE))
    {
	readcore($CORE, shift @{$CORE->{"files"}});
    }
    $CORE->{"have_refreshed"} = 1;
}

sub touchlocation
{
    my($CORE) = @_;
    refresh($CORE) if($CORE->{"have_refreshed"} != 1);
}
sub touchiar
{
    my($CORE) = @_;
    refresh($CORE) if($CORE->{"have_refreshed"} != 1);
}
sub touchstackdata
{
    my($CORE) = @_;
    refresh($CORE) if($CORE->{"have_refreshed"} != 1);
}
sub touchdcr
{
    my($CORE, $dcr) = @_;
    refresh($CORE) if($CORE->{"have_refreshed"} != 1);
}
sub touchkernelstatus
{
    my($CORE) = @_;
    refresh($CORE) if($CORE->{"have_refreshed"} != 1);
}
sub touchregisters
{
    my($CORE) = @_;
    refresh($CORE) if($CORE->{"have_refreshed"} != 1);
}
sub touchmemory
{
    my($CORE, $address) = @_;
    refresh($CORE) if($CORE->{"have_refreshed"} != 1);
}
sub setaccessmask
{
    my($CORE, $value, @nodes) = @_;
    my $coreprefix = $CORE->{"coreprefix"};
    
    foreach $node (keys %{$CORE})
    {
        if(($node =~ /mmcsnode_\d+/) || ($node =~ /^$coreprefix/))
	{
            $CORE->{$node}{"node_accessible"} = 1-$value;
	}
    }
    foreach $node (@nodes)
    {
	$CORE->{$node}{"node_accessible"} = $value;
    }
}


sub isaccessible
{
    my($CORE, $key) = @_;

#    return 0 if(($CORE->{"options"}{"debugionode"}==0) && ($CORE->{$key}{"ionode"}==1));
#    return 0 if(($CORE->{"options"}{"debugcomputenode"}==0) && ($CORE->{$key}{"ionode"}==0));
#    return 0 if(! $CORE->{$key}{"node_accessible"});
    return 1;
}

sub getnodenames
{
    my($CORE, $accessible) = @_;
    my @list = ();
    foreach $key (keys %{$CORE})
    {
	if($key =~ /^core/)
	{
	    if((!defined $accessible) || isaccessible($CORE, $key))
	    {
		push(@list, $key);
	    }
	}
    }
    return @list;
}

sub getmpiname
{
    my($CORE, $index) = @_;
    return $CORE->{"mpimap"}[$index];
}

sub getnameddcrs
{
    my($CORE) = @_;
    return @DCRLIST;
}

sub getnamedregisters
{
    my($CORE) = @_;
    return sort(keys %regnamehash);
}

1;
