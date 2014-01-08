#!/usr/bin/perl -w
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

use strict;
use Spreadsheet::ParseExcel;

# For argument parsing
use Getopt::Long;

##########################################
# processClocks()
#
##########################################
sub processClocks
{
    # get arguments
    my $worksheet_name = shift();
    my $worksheet = shift();
    my $outfile = shift();

    my ( $row_min, $row_max ) = $worksheet->row_range();
    my ( $col_min, $col_max ) = $worksheet->col_range();

    # first pass through we are just looking for compute rack clock tree
    my %racks;
    my %midplanes;
    for my $row ( $row_min .. $row_max ) {
        for my $col ( $col_min .. $col_max ) {
            # get source and destination
            my $source = $worksheet->get_cell( $row, $col );
            my $destination = $worksheet->get_cell( $row, $col + 1 );
            next unless $source && $destination;
            my $from = $worksheet->get_cell( 3, $col );
            my $to = $worksheet->get_cell( 3, $col + 1 );
            next unless $from && $to;
            next unless ( $from->value() eq "From" );
            next unless ( $to->value() eq "To" );
                
            if (
                $source->value() =~ /(R[[:xdigit:]]{2}-K)/ && # clock card in a compute rack
                    (
                    $destination->value() =~ /(R[[:xdigit:]]{2}-K)/ ||          # clock card in a compute rack
                    $destination->value() =~ /(R[[:xdigit:]]{2}-M[01])/         # midplane
                    )
            )
            {
                # add source rack to hash
                if ( $source->value() =~ /(R[[:xdigit:]]{2})/ ) {
                    $racks{$1} = undef;
                }
                
                # add destination rack to hash
                if ( $destination->value() =~ /(R[[:xdigit:]]{2})/ ) {
                    $racks{$1} = undef
                }

                # add clock source and destination
                $midplanes{$destination->value()} = $source->value();
            }
        }
    }

    # now we need to find I/O drawer clock tree
    # row 3 always holds number of I/O drawers per rack
    my %columns;
    for my $col ( $col_min .. $col_max ) {
        my $cell = $worksheet->get_cell( 2, $col );
        next unless $cell;
        my $value = $cell->value();
        if ( $cell->value() =~ /^([0124]$)/) {
            $columns{$col} = $1;
        }
    }

    # iterate through each column
    foreach (keys %columns) {
        # get number of drawers
        my $drawers = $columns{$_};

        # print heading
        print $outfile "<$worksheet_name $drawers>\n";

        # output racks
        foreach (keys %racks) {
            print $outfile "$_-M0\n";
            print $outfile "$_-M1\n" if $worksheet_name !~ "mid";
        }

        # output midplane clock tree
        foreach my $key (keys %midplanes) {
            my $value = $midplanes{$key};
            print $outfile "$value,$key\n";
        }

        my $col = $_;
        # iterate through rows and columns
        for my $row ( 4 .. $row_max ) {
            # get source and destination
            my $source = $worksheet->get_cell( $row, $col );
            my $destination = $worksheet->get_cell( $row, $col + 1 );
            next unless $source && $destination;
            
            if (
                $source->value() =~ /([QR][[:alnum:]]{2}-K)/ &&                   # clock card in a compute or I/O rack
                (
                    $destination->value() =~ /(R[[:xdigit:]]{2}-I[CDEF])/ ||      # I/O drawer on top of a rack
                    $destination->value() =~ /(Q[[:alnum:]]{2}-K[01])/ ||         # I/O rack clock card
                    $destination->value() =~ /(Q[[:alnum:]]{2}-I[[:xdigit:]])/    # I/O drawer in an I/O rack
                )
               )
            {
                # matched an I/O cable pair
                print $outfile $source->value(),",",$destination->value(),"\n";
            }
        }

        # add a line feed
        print $outfile "\n";
    }   
} # sub processClocks


##########################################
# processIo()
#
##########################################
sub processIo
{
    # get arguments
    my $worksheet_name = shift();
    my $worksheet = shift();
    my $outfile = shift();

    my ( $row_min, $row_max ) = $worksheet->row_range();
    my ( $col_min, $col_max ) = $worksheet->col_range();

    # row 3 always holds number of I/O drawers per rack
    my %columns;
    for my $col ( $col_min .. $col_max ) {
        my $cell = $worksheet->get_cell( 2, $col );
        next unless $cell;
        my $value = $cell->value();
        if ( $cell->value() =~ /^([0124]$)/) {
            $columns{$col} = $1;
        }
    }

    # iterate through each column
    foreach (keys %columns) {
        # get number of drawers
        my $drawers = $columns{$_};

        # print heading
        print $outfile "<$worksheet_name $drawers>\n";

        my %drawers;
        my %cables;
        my $col = $_;
        # iterate through rows and columns
        for my $row ( 4 .. $row_max ) {
            # get source and destination
            my $source = $worksheet->get_cell( $row, $col );
            my $destination = $worksheet->get_cell( $row, $col + 1 );
            next unless $source && $destination;

            if (
                $source->value() =~ /(R[[:xdigit:]]{2}-M[0-1]-N\d+-T\d+)/ &&    # nodeboard connector
                (
                    $destination->value() =~ /(R[[:xdigit:]]{2}-I[CDEF]-T\d+)/  ||        # I/O drawer on top of a rack
                    $destination->value() =~ /(Q[[:alnum:]]{2}-I[[:xdigit:]]-T\d+)/      # I/O rack
                )
               )
            {
                # matched an I/O cable pair
                $cables{$source->value()} = $destination->value();

                # check if we have this I/O drawer
                if ( $destination->value() =~ /(R[[:xdigit:]]{2}-I[CDEF])-T\d+/ ) {
                    $drawers{$1} = undef;
                }
                
                # check if we have this I/O rack
                if ( $destination->value() =~ /(Q[[:alnum:]]{2}-I[[:xdigit:]])/ ) {
                    $drawers{$1} = undef;
                }
            }
        }

        # output drawers
        foreach (keys %drawers) {
            print $outfile "$_\n";
        }

        # output cables
        foreach (keys %cables) {
            print $outfile "$_,$cables{$_}\n";
        }

        # add a line feed
        print $outfile "\n";
    }   
} # sub processIo


##########################################
# processLinks()
#
##########################################
sub processLinks
{
    # get arguments
    my $worksheet_name = shift();
    my $worksheet = shift();
    my $outfile = shift();

    my ( $row_min, $row_max ) = $worksheet->row_range();
    my ( $col_min, $col_max ) = $worksheet->col_range();

    # array to hold cable results

    my %cables;
    # iterate through rows and columns
    for my $row ( $row_min .. $row_max ) {
        for my $col ( $col_min .. $col_max ) {
            # only look at columns "From" and "To"
            my $from = $worksheet->get_cell( $row_min, $col );
            my $to = $worksheet->get_cell( $row_min, $col + 1 );
            next unless $from;
            next unless $to;
            next unless ($from->value() eq "From" && $to->value() eq "To");

            # get first and second port
            # second port will be in the next column
            my $first_port = $worksheet->get_cell( $row, $col );
            my $second_port = $worksheet->get_cell( $row, $col + 1 );
            next unless $first_port;
            next unless $second_port;

            if (
                $first_port->value() =~ /(R[[:xdigit:]]{2}-M[0-1]-N\d+-T\d+)/ &&
                $second_port->value() =~ /(R[[:xdigit:]]{2}-M[0-1]-N\d+-T\d+)/
                )
            {
                # matched a torus cable pair Rxx-Mx-Nxx-Txx
                $cables{$first_port->value()} = $second_port->value();
            }
        }
    }   

    if ( keys(%cables) ) {
        print $outfile "<$worksheet_name>\n" || die "could not write output";
    }
    foreach my $key (keys %cables) {
        my $value = $cables{$key};
        print $outfile "$key,$value\n";
    }
    if ( keys(%cables) ) {
        print $outfile "\n";
    }
} # sub processLinks

sub parseWorksheet
{
    my $name = shift();
    my $worksheet = shift();

    # Makes sure we don't churn on any useless tables
    return if($name !~ m/((\d\s*x\d)|mid)\s*/);

    print "processing sheet <$name>\n";
  
    # If we have something that matches the right format...then dig it up!
    if ($name =~ /((\d+\s*[xX]\s*\d+)|mid).*/)
    {
        # construct filename
        my $filename = $name;
        $filename =~ s/[[:blank:]].*/.txt/;

        # open file
        open( OUTFILE, ">>$filename") || die "Unable to open $filename for writing";
        my $outfile = *OUTFILE;

        if ($name =~ /(\d+\s*[xX]\s*\d+\s+A|B|C|D)/)
        {
            processLinks( $name, $worksheet, $outfile);
        }
        elsif ($name =~ /(((\d+\s*[xX]\s*\d+)|mid)\s+IO)/)
        {
            processIo( $1, $worksheet, $outfile)
        }
        elsif ($name =~ /(((\d+\s*[xX]\s*\d+)|mid)\s+K)/)
        {
            processClocks( $1, $worksheet, $outfile);

            # clocks are always last
            print $outfile "<END>\n";
        }
        else
        {
            # some other worksheet we do'nt know about
            print "skipping worksheet\n";
        }
    } else {
        print "invalid sheet\n";
    }
}

##########################################
# printUsage()
# Prints the help text for this program
##########################################
sub printUsage()
{
    print "\nUsage: PROGRAM_NAME [OPTIONS]\n
        --input=[input XLS file]   Specifies source XLS (default is Cables.xls)
        --help\t                   Prints this help text
	
        Examples:
        PROGRAM_NAME --input=Cables.xls\n\n";
    exit(255);
} # sub printUsage()

##########################################
# main()
##########################################

# Define local variables
my $inFile           = "Cables.xls";
my $optHelp          = undef;

# Parse the command line
GetOptions(
        'input=s'        => \$inFile,
        'help'           => \$optHelp
        )
or printUsage();

printUsage() if ($optHelp);

# create spreadsheet parser
my $parser   = Spreadsheet::ParseExcel->new();
print "opening $inFile\n";
my $workbook = $parser->parse( $inFile );
if ( !defined $workbook ) {
    die $parser->error(), ".\n";
}
print "opened spreadsheet\n";

for my $worksheet ( $workbook->worksheets() ) {
    my $name = $worksheet->get_name();
    parseWorksheet( $name, $worksheet );
}

exit(0);

__END__

=head1 NAME

cable_spreadsheet_parser.pl (utility to generate configuration files used by dbPopulate.pl)

=head1 SYNOPSIS

cable_spreadsheet_parser.pl [options]

Options:

--input=[input .xls file] | Specifies source .xls spreadsheet

--help | Prints this help text
=back

=head1 DESCRIPTION

TThis utility should be used to generate the configuration file used by dbPopulate.pl.  The input for this program is the Excel document (in .xls format) that contains the cable configurations for various configs.  This document is owned by Mark Megerian and can be found in the BG/Q teamroom under his name.

NOTE: This utility requires the perl Spreadsheet::ParseExcel module.  This can be obtained from your local CPAN mirror. You can check for this module's presence by executing the following command: "perl -e "use Spreadsheet::ParseExcel;".  If it prints out anything, your Spreadsheet::ParseExcel installation is not working and you should re-install it correctly.  

To install Spreadsheet::ParseExcel, run (as root) "perl -MCPAN -e shell".  At the cpan> prompt, type "force install Spreadsheet::ParseExcel".

=head1 BUGS

None.

=head1 AUTHOR

Sam Miller

=cut

