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
# (C) Copyright IBM Corp.  2007, 2011                              
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

package DbPopulate;

# Pass up all errors
use strict;
use warnings;

# Let me use more obvious variable names
use English;

# For debug
use Data::Dumper;

# Use the database module
use DBI;

# use the basename module
use File::Basename;

# pod usage
use Pod::Usage;

# For argument parsing
use Getopt::Long;

# For parsing bg.properties
use Config::IniFiles;

# Globals
use vars
    qw($dbHandle $propHash *INFILE $debug $hours $days);

#initial undef's
my ($propHash, $dbHandle, $hours, $days) = undef;

my $debug = 0;

##########################################
# getDbHandle()
# Returns a handle to the database
##########################################
sub getDbHandle
{
    #connect to the database if we're not already
    if (!defined($dbHandle))
    {
        my $dsn = "dbi:DB2:" . $propHash->{"name"};
        $dbHandle =
            DBI->connect($dsn, $propHash->{"user"}, $propHash->{"password"});
        die if (!defined($dbHandle));

	# set schema based on the properties file
        my $schemaStmt = "SET SCHEMA " . $propHash->{'schema_name'};

	print "\n" . $schemaStmt . "\n" if($debug);
        my $schemaStmtHandle = $dbHandle->prepare($schemaStmt);
        if (!$schemaStmtHandle->execute())
        {
            print "Result is " . getDbHandle()->errstr . ". EXITING\n";
            die "Could not execute $schemaStmtHandle";
        } # if (!$schemaStmtHandle->execute())
    } # if (!defined($dbHandle))

    return $dbHandle;

} # sub getDbHandle


##########################################
# parseProperties()
# Parses the database properties file for schema, uid, password
##########################################
sub parseProperties
{
    my $fileName = shift();
    my $db       = shift();
    my $prop = new Config::IniFiles( -file => $fileName );
    if (!defined($prop))
    {
       print "Could not read $fileName\n";
       foreach (@Config::IniFiles::errors) {
          print "$_\n";
       }
       exit(1);
    }

    # get database section
    my @section = $prop->Parameters( "database" );
    foreach my $key (@section)
    {
        my $val = $prop->val( "database", $key );
        print "adding $key=$val to hash\n" if ($debug);
        $db->{$key} = $val;
    }

    return $db;
} # sub parseProperties



sub listFailedNodes
{
    print "List of locations that have reached failure threshold\n";
  
    if (!defined($days) || ($days < 1))
    {
        print "*)  A positive value for --days was not specified. \n";
        return 1;
    }

    my $sql;
    my $stmt;
    my $loc;
    my $innerloc;
    my $node = 1;

    $sql = qq(select msg_id,thresholdcount,thresholdperiod  from bgqmsgtypes  where thresholdcount > 0);
    $stmt = getDbHandle()->prepare($sql);
    $stmt->execute();

    while ($loc = $stmt->fetchrow_hashref) {
        my $msgid = $loc->{MSG_ID};
        my $thresholdcount = $loc->{THRESHOLDCOUNT};
        my $thresholdperiod = $loc->{THRESHOLDPERIOD};
        if (!defined( $loc->{THRESHOLDPERIOD})) {
            $thresholdperiod = "unspecified";
        }

        print "Checking occurrences of RAS event $msgid, which has threshold count $thresholdcount and period $thresholdperiod\n";

        if (!defined( $loc->{THRESHOLDPERIOD})) {
            $thresholdperiod = "$days DAYS";
        }

        my $innersql = 
            qq(SELECT DISTINCT loc FROM ( select a.recid,  a.location AS loc, a.msg_id, count(*) 
                                          from bgqeventlog a, bgqeventlog b where a.event_time + $days DAYS > current timestamp 
                                          and a.msg_id = '$msgid'  and a.msg_id = b.msg_id  
                                          and a.location = b.location and b.event_time >= a.event_time 
                                          and b.event_time <= a.event_time + $thresholdperiod
                                          group by a.recid,  a.location, a.msg_id HAVING COUNT(*) >  $thresholdcount ) );
        
        ##      print $innersql;
        my $innerstmt = getDbHandle()->prepare($innersql);
        $innerstmt->execute();
        while ($innerloc = $innerstmt->fetchrow_hashref) {
            print "$node)  $innerloc->{LOC}  \n";
            $node++;
        }
    }

    return 0;
} # sub listFailedNodes

sub listFailingNodes
{
    print "List of locations that may reach failure threshold in provided number of hours\n";

    if (!defined($hours) || ($hours < 1))
    {
        print "*)  A positive value for --hours was not specified. \n";
        return 1;
    }
  
    my $sql;
    my $stmt;
    my $loc;
    my $innerloc;
    my $node = 1;

    $sql = qq(select msg_id,thresholdcount,thresholdperiod  from bgqmsgtypes  where thresholdcount > 0 and thresholdperiod is not null);
    $stmt = getDbHandle()->prepare($sql);
    $stmt->execute();

    while ($loc = $stmt->fetchrow_hashref) {
        my $msgid = $loc->{MSG_ID};
        my $thresholdcount = $loc->{THRESHOLDCOUNT};
        my $thresholdperiod = $loc->{THRESHOLDPERIOD};

        print "Checking occurrences of RAS event $msgid, which has threshold count $thresholdcount and period $thresholdperiod\n";

        my $innersql =
            qq( select decimal(timestampdiff(2,  
                char((current timestamp + $thresholdperiod - $hours hours)- current timestamp ))) / 
                       decimal(timestampdiff(2,  char((current timestamp + $thresholdperiod)- current timestamp ))) as perc
                from sysibm.sysdummy1);

        my $innerstmt = getDbHandle()->prepare($innersql);
        $innerstmt->execute();
        $innerloc = $innerstmt->fetchrow_hashref;
        my $percentage = $innerloc->{PERC};
        
        $innersql = 
            qq (SELECT DISTINCT loc FROM (select a.recid,  a.location AS loc, a.msg_id, count(*)  
                                          from bgqeventlog a, bgqeventlog b where a.event_time + $thresholdperiod - $hours hours  > current timestamp 
                                          and a.msg_id = '$msgid'  and a.msg_id = b.msg_id  
                                          and a.location = b.location and b.event_time >= a.event_time   
                                          group by a.recid,  a.location, a.msg_id HAVING COUNT(*) > $thresholdcount * $percentage ) ); 

        
        $innerstmt = getDbHandle()->prepare($innersql);
        $innerstmt->execute();
        while ($innerloc = $innerstmt->fetchrow_hashref) {
            print "$node)  $innerloc->{LOC}  \n";
            $node++;
        }
    }

    return 0;
} # sub listFailingNodes




##########################################
# processSQL()
# Deals with SQL commands and provides a common interface
##########################################
sub processSQL
{
    my $stmt = shift();
    print "\n" . $stmt . "\n" if($debug);
    my $stmtHandle = getDbHandle()->prepare($stmt);
    !$stmtHandle->execute();
  
}


##########################################
# printUsage()
# Prints the help text for this program
##########################################
sub printUsage()
{
    system("perldoc $0"); 
    exit(0);
} # sub printUsage()


##########################################
# setupDb()
# Make connection to the DB
##########################################
sub setupDb
{
    my $bgPropertiesFile = shift();
    #make sure we have the DB2 driver
    my @drivers = DBI->available_drivers();
    die "No perl DBI drivers found!\n" unless @drivers;

    #check if the DB2 driver is available
    my $foundDB2Driver = 0;
    foreach my $driver (@drivers)
    {
        if ($driver eq "DB2")
        {
	    $foundDB2Driver = 1;
	    last;
        } # if ($driver eq "DB2")
    } # foreach my $driver (@drivers)

    die "Could not find DBI::DB2 database driver, make sure it's installed!"
        if ($foundDB2Driver == 0);

    #open the properties file
    if (defined($bgPropertiesFile))
    {
        $propHash = parseProperties($bgPropertiesFile, $propHash);
    }

    #check if we have a database name and schema
    die "Please specify --properties\n"
        unless defined $propHash->{"name"} and defined $propHash->{"schema_name"};
}

  
##########################################
# main()
##########################################


# Define local variables
my $optHelp          = undef;
my $bgPropertiesFile = $ENV{BG_PROPERTIES_FILE};
         

# Parse the command line
GetOptions(
           'hours=i'        => \$hours,
           'days=i'         => \$days,
           'properties=s'   => \$bgPropertiesFile,
           'help'           => \$optHelp
           
           )
    or printUsage();

printUsage() unless (!defined($optHelp));

if (!defined($bgPropertiesFile)) {
    $bgPropertiesFile = "/bgsys/local/etc/bg.properties";
}


setupDb($bgPropertiesFile);

if ( !defined($days) && !defined($hours) ) {
    print "please specify --hours or --days.\n";
    exit(1);
}

if ( defined($days) && listFailedNodes() != 0 ) {
    exit(1);
}

if ( defined($hours) && listFailingNodes() != 0 ) {
    exit(1);
}

exit(0);
               
__END__
               
=head1 NAME

nodeFailures.pl (utility to find nodes either at or approaching the threshold for failure)

=head1 SYNOPSIS

nodeFailures.pl [options]

Options:

=over

--properties [properties-file-name] | properties file, used to obtain database name, schema, username, and
password.  If --properties is not given, the BG_PROPERTIES_FILE environment variable will be used.

--hours [number-of-hours] | Number of hours into the future to look for predicting failures.

--days  [number-of-days]  | Number of days in the past to look for failures.

--help | Prints this help text


=back

=head1 DESCRIPTION

This utility looks at accumulated RAS events, and shows nodes that have either reached the threshold for failure, or may reach
the threshold in the number of hours specified.  This does not guarantee that the node will fail in the timespan provided.  Also,
the past rate of RAS events may not be an accurate predictor of the future rate of RAS events, due to changes in machine usage.

NOTE: This utility requires the perl DBI module and DB2 database driver. 

=head1 BUGS

None.

=head1 AUTHOR

International Business Machines (IBM)

=cut

