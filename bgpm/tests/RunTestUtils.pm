
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
# (C) Copyright IBM Corp.  2011, 2012                              
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

package RunTestUtils;

use strict;
use Exporter;
use Time::HiRes qw(gettimeofday tv_interval);
use Carp;

use vars qw( @ISA @EXPORT );

@ISA    = qw(Exporter);
@EXPORT = qw(SetEnvDef GetEnvDef FindLeafFile GetTime ElapsedTimeText 
             MAX MIN SysRC EchoSysRC PrintParsedData CallBashFunc 
             PushDir PopDir);




# $val = SetEnvDef(name, defaultval, echo)
# Get or Set current environment value (set to default if not currently defined)
sub SetEnvDef {
    my $name = shift;
    my $def = shift;
    my $echo = shift;
    if (! defined($ENV{$name})) { $ENV{$name} = $def; }
    if (defined($echo) && $echo) {
        print STDERR "env $name=", $ENV{$name}, "\n";
    }
    return $ENV{$name};
}


# $val = GetEnvDef(name, defaultval, echo)
# Get current environment value or passed default value.
sub GetEnvDef {
    my $name = shift;
    my $def = shift;
    if (defined($ENV{$name})) { $def = $ENV{$name}; }
    return $def;
}




# Look for named file by searching from leaf to parent directories in path.
sub FindLeafFile # filename, filepath, extraPath
{
    my $fname = shift;
    my $fpath = shift;
    my $xtraPath = shift;
    $fpath =~ s|/$||;  # remove trailing '/' if needed.
    
    my $found = "";
    while (($fpath ne "") && ($found eq "")) {
        my $name = "$fpath/$fname";
        #print STDERR "try $name\n";
        if (-f $name) {
            $found = $name;
        }
        else {
            $fpath =~ s|/[^/]*$||o;
        }
    }
    if ((!$found) && (defined($xtraPath))) {
        $xtraPath =~ s|/$||;  # remove trailing '/' if needed.
        my $name = "$xtraPath/$fname";
        if (-f $name) {
            $found = $name;
        }
    }
    
    return $found;
}



# return the shifted system rc
sub SysRC {
    my $rc = ( 0xffff & $? );
    if ( $rc > 0x80 ) { $rc >>= 8; }
    return $rc;
}

# $rc = EchoSysRC( [ $cmdStrg ] )
# get the real system rc value while echoing the command and text describing a failure
sub EchoSysRC {
    my $cmd = shift;
    my $rc  = 0xffff & $?;
    printf STDERR "\ncmd: $cmd; " if defined($cmd);
    printf STDERR "rc %#04x: ", $rc;
    if    ( $rc == 0 )      { print STDERR "normal exit."; }
    elsif ( $rc == 0xff00 ) { print STDERR "failed: $!\n"; }
    elsif ( $rc > 0x80 ) { $rc >>= 8; print STDERR "nonzero exit: $rc\n"; }
    else {

        if ( $rc & 0x80 ) {
            $rc &= ~0x80;
            print STDERR "coredump from ";
        }
        print STDERR "signal $rc\n";
    }
    print STDERR "\n";

    return ($rc);
}

sub MAX {
    return ( $_[0] > $_[1] ? $_[0] : $_[1] );
}

sub MIN {
    return ( $_[0] < $_[1] ? $_[0] : $_[1] );
}


#----------------------------------
# Elapsed Time routines.
# synopsis:
#    $startTime = &GetTime();
#    print &ElapsedTimeText($startTime), "\n";
sub GetTime { return [gettimeofday]; }
sub ElapsedTimeFloatSec { return tv_interval( $_[0], [gettimeofday] ); }

sub ElapsedTimeText {
    my $timesec = &ElapsedTimeFloatSec( $_[0] );
    $timesec = int( $timesec * 1000 ) / 1000;    # ms value.
    my $hrs  = int( $timesec / ( 60 * 60 ) );
    my $min  = int( $timesec / 60 ) - $hrs * 60;
    my $sec  = sprintf( "%0.3f", $timesec - $min * 60 - $hrs * 60 * 60 );
    my $strg = "";
    $strg .= "$hrs hrs, "  if ( $hrs > 0 );
    $strg .= "$min mins, " if ( $min > 0 );
    $strg .= "$sec secs";
    return $strg;
}

#----------------------------------------
# PrintParsedData \@array, [\*FILE], $separator=" ", $indent=0, $inclLabels=1
#----------------------------------------
sub PrintParsedData {
    my ( $ref, $fileref, $sep, $indent, $inclLabels ) = @_;
    if ( !defined($fileref) ) { $fileref = \*STDOUT; }
    if ( !defined($sep) )     { $sep     = " "; }
    if   ( !defined($indent) || ( $indent <= 0 ) ) { $indent = ""; }
    else                                           { $indent = sprintf "%${indent}s", " "; }
    if ( !defined($inclLabels) ) { $inclLabels = 1; }

    my $firstRow = $inclLabels ? 1 : 0;

    # print STDERR Dumper($ref);

    # 1st row has labels (split label on blanks and set defult sizes)
    my @sizes;
    my @align;
    for ( my $i = 0 ; $i < 25 ; $i++ ) { $sizes[$i] = 0; $align[$i] = ''; }    # prevent undef warnings on odd alignments

    my @labels;
    my $fieldNum = 0;
    if ($inclLabels) {
        foreach my $field ( @{ $$ref[0] } ) {
            my @words = split( ' ', $field );
            for ( my $i = 0 ; $i <= $#words ; $i++ ) {
                $labels[$i][$fieldNum] = $words[$i];
                $sizes[$fieldNum] = length( $words[$i] ) if ( length( $words[$i] ) > $sizes[$fieldNum] );
            }
            $fieldNum++;
        }

        # print $fileref Dumper(@labels);
    }

    # figure out the maximum field widths to pretty print from other rows
    foreach my $row ( @$ref[ $firstRow .. $#$ref ] ) {
        my $fieldNum = 0;
        foreach my $field (@$row) {

            # set alignment to left if not number or space
            if ( !defined( $align[$fieldNum] ) ) { $align[$fieldNum] = ''; }    # init value
            if ( ( length($field) > 0 ) && ( $field !~ m/^[\d\s\-]/ ) ) { $align[$fieldNum] = '-'; }

            $sizes[$fieldNum] = length($field) if ( length($field) > $sizes[$fieldNum] );
            $fieldNum++;
        }
    }

    if ($inclLabels) {

        # print labels
        foreach my $row (@labels) {
            my $fieldNum = 0;
            my $line     = $indent;    # output must all be in same print statement to be line buffered.
            foreach my $field (@$row) {

                # print STDERR "field: $field, afieldNum:$fieldNum, align:$align[$fieldNum], sizes:$sizes[$fieldNum]\n";
                if ( defined $field ) { $line .= sprintf "%$align[$fieldNum]$sizes[$fieldNum]s%s", $field, $sep; }
                else                  { $line .= sprintf " %$sizes[$fieldNum]s %s", " ", $sep; }
                $fieldNum++;
            }
            print $fileref $line, "\n";
        }
    }

    # print other rows
    foreach my $row ( @$ref[ $firstRow .. $#$ref ] ) {
        my $fieldNum = 0;
        my $line     = $indent;    # output must all be in same print statement to be line bufferred.
        foreach my $field (@$row) {
            $line .= sprintf "%$align[$fieldNum]$sizes[$fieldNum]s%s", $field, $sep;
            $fieldNum++;
        }
        print $fileref $line, "\n";
    }
}


sub CallBashFunc # $file, $func, @args
{
    my $file = shift;
    my $func = shift;
    my @fargs;
    foreach my $arg (@_) {
        my $farg = $arg;  # allow modifications.
        $farg =~ s|(['"\\])|\\$1|g;
        push @fargs, $farg;
    }
    #system(bash => ("--rcfile" => "$file", "-i", "-c" => "$func", "--", @fargs));
    print STDERR "CallBashFunc: $file, $func, #args=", scalar(@fargs), ", @fargs\n";
    my $rc = system("bash", "--rcfile", $file, "-i", "-c", "$func @fargs"); 
    return $rc;
}


my @pushDirList = ();
sub PushDir # $newDir
{
    my $newDir = shift;
    my $cwd = `pwd -P`; chomp($cwd);
    if ($cwd eq "") { die "ERROR: pwd -P failed in PushDir subroutine; $!\n"; }
    push @pushDirList, $cwd;
    chdir $newDir or die "ERROR: Can't cd to $newDir in PushDir; $!\n";
    return $newDir;
}
sub PopDir
{
    my $lastDir = pop @pushDirList;
    if (defined($lastDir)) {
        chdir($lastDir) or die "ERROR: Can't cd to $lastDir in PopDir subroutine; $!\n";
    }
    return $lastDir;
}



1;