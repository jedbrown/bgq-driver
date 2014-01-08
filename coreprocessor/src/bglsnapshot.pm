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

package bglsnapshot;

use warnings;
use mmcs_db;
use snapshot qw(snapshot restore);

require Exporter;

@ISA         = qw(Exporter);
@EXPORT      = qw();
@EXPORT_OK   = qw();
%EXPORT_TAGS = ( all => \@EXPORT_OK );
$VERSION     = v1.0;
$SVERSION    = '$Id: bglsnapshot.pm,v 1.5 2008/05/19 21:39:24 mtn Exp $';
sub clone
{
  my ($invocant, $copyclass) = @_;
  my ($class   ) = ref($invocant) || $invocant;
  
  $self = restore(snapshot($copyclass));
  bless  ($self,$class);
  return ($self);
}

sub mkfreeze
{
    my($invocant, $copyclass) = @_;
    $output = snapshot($copyclass);
    return $output;
}

sub mkthaw
{
  my ($invocant, $filename) = @_;
  my ($class   ) = ref($invocant) || $invocant;
  my ($self) = 
  {
      "version" => $VERSION,
  };
  my $input = "";
  open(TMP, $filename) or die "Unable to open snapshot file: \"$filename\"";
  $line = "";
  while($line = <TMP>)
  {
      $input .= $line;
  }
  close(TMP);
  
  $self = restore($input);
  bless  ($self,$class);

  $self->{"options"}{"stripframes"}      = 1;
  return ($self);    
}

sub touchlocation
{
    my($CORE) = @_;
}

sub touchiar
{
}

sub touchstackdata
{
    my($CORE) = @_;
}

sub touchkernelstatus
{	
    my($CORE) = @_;
}

sub touchregisters
{
    my($CORE) = @_;
}

sub touchdcr
{	
    my($CORE, $dcr) = @_;
}

sub touchmemory
{
    my($CORE, $address) = @_;
}

sub is_refresh_inprogress
{
    my($CORE) = @_;
    return 1 if($#{@{$CORE->{"corefiles"}}}+1 > 0);
    return 0;
}

sub setaccessmask
{
    my($CORE, $value, @nodes) = @_;
    foreach $node (keys %{$CORE})
    {
        if(($node =~ /^Q/) || ($node =~ /^R/))
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
    return 0 if((exists $CORE->{$key}{"node_accessible"}) && (! $CORE->{$key}{"node_accessible"}));
    return 1;
}

sub getnodenames
{
    my($CORE, $accessible) = @_;
    my @list = ();
    foreach $key (keys %{$CORE})
    {
#	print "getnodenames.  key=$key\n";
	push(@list, $key) if(($key =~ /mmcsnode_\d+/) && ((!defined $accessible) || isaccessible($CORE, $key)));
	push(@list, $key) if(($key =~ /^core/) && ((!defined $accessible) || isaccessible($CORE, $key)));
	push(@list, $key) if(($key =~ /^rank/) && ((!defined $accessible) || isaccessible($CORE, $key)));
        if(($key =~ /^Q/) || ($key =~ /^R/))
	{
            if((!defined $accessible) || isaccessible($CORE, $key))
            {
                push(@list, $key);
            }
	}
    }
    return sort @list;
}

sub getmpiname
{
    my($CORE, $index) = @_;
    return $CORE->{"mmcsmap"}[$index];
}

sub getconsole
{
    my($CORE) = @_;
}

sub getnameddcrs
{
    my($CORE) = @_;
    return "";
}

sub getnamedregisters
{
    my($CORE) = @_;
    foreach $reg (0..31)
    {
	push(@regnames, sprintf("GPR%02d", $reg));
    }
    return @regnames;
}

sub setstate
{
    my($CORE, $node, $newstate) = @_;
    my $cmd = "";
}
