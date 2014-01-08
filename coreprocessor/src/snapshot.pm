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
# (C) Copyright IBM Corp.  2006, 2011                              
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

package snapshot;

use     warnings;
require Exporter;

@ISA         = qw(Exporter);
@EXPORT      = qw();
@EXPORT_OK   = qw(snapshot restore);
%EXPORT_TAGS = ( all => \@EXPORT_OK );
$VERSION     = v1.0;
$SVERSION    = '$Id: snapshot.pm,v 1.3 2008/05/19 22:14:27 mtn Exp $';

use Compress::Zlib;

sub snapshothash
{
    my($item) = @_;
    while(($k,$v) = each(%{$item}))
    {
	&output("/hashkey $k");
	if(ref(\$v) eq "SCALAR")
	{
	    snapshot_internal(\$v);
	}
	else
	{
	    snapshot_internal($v);
	}
    }
}

sub snapshotarray
{
    my($item) = @_;
    foreach $v (@{$item})
    {
	if(ref(\$v) eq "SCALAR")
	{
	    snapshot_internal(\$v);
	}
	else
	{
	    snapshot_internal($v);
	}
    }
}

sub snapshot_internal
{
    $r = ref($_[0]);
    if($r eq "HASH")
    {
	&startsection("HASH");
	snapshothash($_[0]);
	&endsection("HASH");
    }
    elsif($r eq "SCALAR")
    {
	&startsection("SCALAR");
	&output(${$_[0]});
	&endsection("SCALAR");
    }
    elsif($r eq "ARRAY")
    {
	&startsection("ARRAY");
	snapshotarray($_[0]);
	&endsection("ARRAY");
    }
    elsif($r eq "GLOB")
    {
	die "attempted to snapshot a GLOB\n";
    }
    elsif($r eq "REF")
    {
	&startsection("REF");
	snapshot_internal($$_[0]);
	&endsection("REF");
    }
    else
    {
	&startsection("PACKAGE $r");
	snapshothash($_[0]);
	&endsection("PACKAGE $r");
    }
}

sub snapshot
{
    $BUFFER = "";
    $INDENT = 0;
    snapshot_internal(@_);
    $BUFFER = compress($BUFFER);
    return $BUFFER;
}

sub generate
{
    my($parenttype, @lines) = @_;
    my $mytype = "SCALAR";
    my %myhash = ();
    my @myarray = ();
    my $hashkey = "";    
    my $curpackage = $CURRENT_PACKAGE;
    while($#lines >= 0)
    {
	$elem = shift @lines;
	if(ref($elem) eq "ARRAY")
	{
	    my $rawdata;
	    if($mytype eq "SCALAR")
	    {
		my @data = @{$elem};
		$myhash{$hashkey} = join("\n", @data)  if($parenttype eq "HASH");
		push(@myarray, join("\n", @data))      if($parenttype eq "ARRAY");
	    }
	    if($mytype eq "HASH")
	    {
		my %data = generate("HASH", @{$elem});
		$rawdata = {%data};
		$myhash{$hashkey} = {%data}  if($parenttype eq "HASH");
		push(@myarray, {%data})      if($parenttype eq "ARRAY");
	    }
	    if($mytype eq "PACKAGE")
	    {
		my %data = generate("HASH", @{$elem});
		$myhash{$hashkey} = {%data};
		if($parenttype eq "HASH")
		{
		    $myhash{$hashkey} = {%data};
		    bless $myhash{$hashkey}, $curpackage;
		}
		elsif($parenttype eq "ARRAY")
		{
		    push(@myarray, {%data});
		    bless $myarray[$#myarray], $curpackage;
		}
	    }
	    if($mytype eq "ARRAY")
	    {
		my @data = generate("ARRAY", @{$elem});
		push(@{$myhash{$hashkey}}, @data) if($parenttype eq "HASH");
		push(@myarray, @data)             if($parenttype eq "ARRAY");
	    }

	}
	else
	{
	    if($elem =~ /\/begin/)
	    {
		($mytype) = $elem =~ /\/begin(\S+)/;
		if($mytype eq "PACKAGE")
		{
		    ($curpackage) = $elem =~ /beginPACKAGE\s+(\S+)/;
		    $CURRENT_PACKAGE = $curpackage;
		}
	    }
	    if($elem =~ /\/hashkey/)
	    {
		($hashkey) = $elem =~ /\/hashkey\s+(\S+)/;
	    }
	}
    }
    return %myhash if($parenttype eq "HASH");
    return @myarray if($parenttype eq "ARRAY");
}

sub restore
{
    my($buffer) = @_;
    $buffer = uncompress($buffer) or die "Unable to uncompress file.  File is likely corrupt or invalid.";
    &init($buffer);
    shift @LINES;
    $elem = shift @LINES;
    %structure = &generate("HASH", @{$elem});
    return \%structure;
}

sub startsection
{
    my($type) = @_;
    $BUFFER .= '}' x $INDENT . "/begin$type\n";
    $INDENT++;
}

sub endsection
{
    my($type) = @_;
    $INDENT--;
    $BUFFER .= '}' x $INDENT . "/end$type\n";
}

sub output
{
    my($data) = @_;
    return if(!defined $data);
    foreach $line (split("\n", $data))
    {
	$BUFFER .= '}' x $INDENT . $line . "\n";
    }
}

sub processindentation
{
    my($indent) = @_;
    my @array = ();

    do
    {
	($val, $stripped) = $LINES[0] =~ /^(\}+)(.*)/;
	if(!defined $val)
	{
	    $val = 0;
	}
	else
	{
	    $val = length($val);
	}
	if($indent == $val)
	{
	    shift @LINES;
	    push(@array, $stripped);
	}
	elsif($indent+1 == $val)
	{
	    push(@array, [ &processindentation($indent+1) ]);
	}
	elsif($indent-1 == $val)
	{
	    return @array;
	}
	elsif($LINES[0] !~ /\S/)
	{
	    shift @LINES;
	}
	else
	{
	    die "invalid line: $LINES[0]\n";
	}
    }
    while($#LINES >= 0);
    return @array;
}

sub init
{
    my($data) = @_;
    @LINES = split("\n", $data);
    @LINES = processindentation(0);
}
