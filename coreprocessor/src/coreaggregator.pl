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
#-*- mode: perl;-*-
$_IBMID_ = "Copyright IBM CORP 2005, 2006 LICENSED MATERIAL - PROGRAM PROPERTY OF IBM";

use Getopt::Long;
use Pod::Usage;

sub parseArgs();
sub getCorefilesExeJobid();
sub processCorefiles();
sub getStackInfo($$);
sub printer($);
sub printError($);
sub trim($);

# Global variables
my $exe = "";
my $outfile = "";
my $jobid = "-1";
my @corefiles = ();
my $numCorefiles = 0;
my %stackInfo = ();
my $removeOutfile = 1;

#------------------------------------------------------------------------------------------------------
# Begin main execution

print "\n\n###############################################################";
print "\n# BlueGene/Q Core Aggregator";
print "\n###############################################################\n";

# Parse program arguments ...
parseArgs();

# Need to setup our OUTFILE at this point if one was defined
if ( ! $outfile eq "" ) {
    print "\n\tUsing reqested output file: $outfile";
    if ( -e $outfile ) {
	print "\n\t\tWARNING; Output file $outfile already exists.  Core aggregation will be appended to the existing file.";
        $removeOutfile = 0;
    }
    open OUTFILE, ">>$outfile" or printError("Failure opening $outfile: $!");
} else {
    print "\n\tNo output file specified.  Using standard out ...";
}

# Get the corefiles to be analyzed.
getCorefilesJobid();
# Process the core files
processCorefiles();

close(OUTFILE);


print "\n\n###############################################################";
print "\n# Aggregation Complete";
print "\n###############################################################\n";

print "\n\n";

#-------------------------------------------------------------------------------------------------
# Start subroutine definitions

sub parseArgs() {

    GetOptions('h'         => sub { pod2usage(-exitstatus => 1, -verbose => 1) },
	       'help'      => sub { pod2usage(-exitstatus => 1, -verbose => 2) },
	       'outfile=s' => \$outfile,
	) or pod2usage(-exitstatus => 1);

    # If we don't have at least file to process we need to print the usage and exit.
    if ( $#ARGV < 0 ) {
	pod2usage(-exitstatus => 1, -verbose =>1);
    }

} # End parseArgs()

sub getCorefilesJobid() {
    
    # parseArgs will have taken care of any additional arguments.  The rest of the arguments should be
    # one or more corefiles to be analyzed.

    my @lines = ();
    my @tmpJobid = ();

    for ($i=0; $i<$#ARGV+1; $i++) {
	$corefiles[$i]=$ARGV[$i];
	if (( -e $corefiles[$i] ) && ( -r $corefiles[$i] )) {
	    $numCorefiles++;
	} else {
	    if ( ! -e $corefiles[$i] ) {
		printError( "Cannot find Lightweight Corefile $corefiles[$i] ." );
	    } else {
		printError ( "Cannot read from Lightweight Corefile $corefiles[$i] ." );
	    }
	}
    }
    
    open COREFILE, $corefiles[0] or printError($!);

    @lines = <COREFILE>;
    for ($i=0; $i<$#lines; $i++) {
	if ( $lines[$i] =~ m"^Job ID" ) {
	    @tmpJobid = split(/:/, $lines[$i]);
	    if ( $jobid eq "-1" ) {
		$jobid = trim($tmpJobid[1]);
		print "\n\tAnalyzing cores for Job ID: $jobid";
	    } else {
		if ( $jobid eq "" ) {
		    printError( "Improperly formatted Lightwight Corefile! \n\tUnable to find 'Job ID' line in corefile $corefiles[$i] .");
		}
	    }
	}
    }
    close(COREFILE);

} # End getCorefilesJobid()

sub processCorefiles() {

    # Go through each corefile, make sure it is for the same exe and jobid,
    # lookup the stack information if necessary.

    my $coreExe="";

    print "\n\n\tProcessing $numCorefiles corefiles...\n\n";

    for($i=0; $i<$numCorefiles; $i++) {
	# Only print status message if sendout output to a file.
	if ( ! $outfile eq "" ) {
	    print "\n\t\t$corefiles[$i] ... ";
	}
	open COREFILE, $corefiles[$i] or printError($!);
	@lines = <COREFILE>;
	for($j=0; $j<$#lines; $j++) {
	    if ( $lines[$j] =~ m"^Program" ) { 
		@tmpExe = split(/:/, $lines[$j]);
		$coreExe = trim($tmpExe[1]);
		if ( $coreExe eq "" ) {
		    printError( "Improperly formatted Lightwight Corefile! \n\tUnable to find 'Program' line in corefile $corefiles[$i] .");
		}
		
		printer($lines[$j]);

	    } elsif ( $lines[$j] =~ m"^Job ID" ) {
		@tmpJobid = split(/:/, $lines[$j]);
		my $coreJobid = trim( $tmpJobid[1] );
		if ( $coreJobid eq "" ) {
		    printError( "Improperly formatted Lightwight Corefile! \n\tUnable to find 'Job ID' line in corefile $corefiles[$i] .");
		}
		if ( $jobid != $coreJobid ) {
		    printError( "$corefiles[$i] is not from the same job as the other core files. Expected: $jobid - Found: $coreJobid .");
		}
		printer($lines[$j]);
	    } elsif ( $lines[$j] =~ m"^[0-9a-f]{16}\s\s[0-9a-f]{16}" ) {
		my @stackLine = split(/\s\s/, $lines[$j]);
		my $address = trim( $stackLine[1] );
		my $fileInfo = getStackInfo($address, $coreExe);
		chomp($lines[$j]);
		printer("$lines[$j] ==> $fileInfo");
	    } else {
		printer($lines[$j]);
	    }
	}
	close(COREFILE);
    }

} #End sub processCorefiles()

# Takes an address and checks to see if it is in our hash, if not it
# looks up the address, adds it to the hash and returns the info.  
sub getStackInfo($$) {

    my $address = $_[0];
    my $exe = $_[1];
    my $file = $stackInfo{$exe}{$address};

    # Need to make sure we can find the executable:
    $exe = trim($exe);

    if ( ! -e $exe ) {
	printError( "Cannot find executable $exe ." );
    } elsif ( ! -r $exe ) {
	printError( "Cannot read executable $exe ." );
    }

    if ( $file eq "" ) {
	$stackInfo{$exe}{$address} = `addr2line -e "$exe" "$address"`;
	$file = $stackInfo{$exe}{$address};
	if ($file eq "") {
	    $file = "\n";
	}
    }

    return $file;

} # End sub getStackInfo($)

sub printer($) {

    if ( $outfile eq "" ) {
	print( $_[0] );
    } else {
	print OUTFILE $_[0];
    }

} # End sub printer($)

sub printError($) {

    print "\n\n###############################################################";
    print "\n# coreaggregator.pl error";
    print "\n###############################################################";

    print "\n\n\t$_[0]\n\n";

    # Remove output file if it exists.
    if ( ! $outfile eq "" ) {
	close(OUTFILE);
	if (( -e $outfile ) && ( $removeOutfile eq "1" )) {
	    system("rm -f $outfile");
	}
    }

    close(COREFILE);

    exit 1;
     
} # End printError($)

# Remove white space from the beginning or end of a string.
sub trim($) {

    my $string = shift;
    $string =~ s/^\s+//;
    $string =~ s/\s+$//;
    
    return $string;

} # End trim


#-----------------------------------------------------------------------------------------

__END__

=head1 NAME

coreaggregator.pl ( Utility for analyzing BlueGene/Q Lightweight Corefiles )

=head1 SYNOPSIS

coreaggregator.pl [--outfile=<output file>] [--h] [--help] <one or more Lightweight Corefiles>

=head1 DESCRIPTION

This command line utility takes one or more Lightweight Corefiles (LCFs) and analyzes them.  Stack traces
are analyzed and addresses are decoded to a source routine and line number using the executable that
created the corefile.  The resulting output is the full LCF plus decoded stacks.
The resulting analysis may either be printed to the console or placed in an output file
specified via the command line.  If multiple LCF's are placed on the command line the contents of all of the
LCF's are aggregated into one file/output stream.

coreaggregator.pl verifies that all of the Lightweight Corefiles being analyzed come from the same job ID.
If a corefile is found with a different job ID the analysis will terminate and report the offending file.

coreaggregator.pl may be used with Lightweight Corefiles containing multiple stack traces as well as corefiles from
Multiple Program Multiple Data (MPMD) runs.

It is assumed that the executables associated with the corefiles will be at the same path indicated by the 'Program' 
line in the Lightweight Corefiles.  If a relative path is found in the corefile, it is assumed that coreaggregator will
be executed from the same location as the original job invocation so that the executables may be found by addr2line.

At least one Lightweight Corefile must be sent to the coreaggregator.pl command line.

The arguments for coreaggregator.pl are:

[B<--outfile=>I<output file>]
    I<output file> This optional argument specifies the path to the file in which to put the results of the analysis.  If no output file is specified, the results will be printed to standard out.

[B<--h>] Display brief help text.

[B<--help>] Display full help text.

=head1 AUTHOR

IBM

=cut
