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

#____________________________
#
#  Include needed modules
use strict;
use Getopt::Long;
use DBI;
use DBD::DB2;
use Config::IniFiles;
#____________________________
# Globals
use vars
    qw($dbHandle $dbPropHash $debug);

#____________________________
#
#       MAIN
#
#____________________________
# 
#  Input Variables
my $months=3;
my $verify;
my %options;
my $dbprop=undef;
my $qual=undef;
#____________________________

#____________________________
#
#  Process Command Line Options
#
#
GetOptions(
	\%options,'help','h','v','r','nocount','hist','env','event','diags','perf','security','teal','all','properties=s' => \$dbprop, 'q=s' => \$qual,
	"months|m=i" => \$months) or usage();

if (defined $options{"h"} || defined $options{"help"}) {
  usage();
}

if ( defined $options{"all"} ) {
  $options{"hist"}=1;
  $options{"env"}=1;
  $options{"event"}=1;
  $options{"teal"}=1;
  $options{"diags"}=1;
  $options{"perf"}=1;
  $options{"security"}=1;
}

if ( ! defined $options{"hist"} && ! defined $options{"env"}  && ! defined $options{"diags"} && ! defined $options{"event"} && ! defined $options{"perf"} && ! defined $options{"security"} && !defined $options{"teal"} ) {
  print "\n\tERROR -  Specify --hist, --env, --event, --teal, --diags, --perf, --security, or -all\n";
  usage();
}

#_____________________________________
#
#       CONNECT TO DATABASE
# connect to the database

my $dbp = "/bgsys/local/etc/bg.properties";
if ($dbprop) {
    $dbp = $dbprop;
}
setupDb($dbp);


#
#_____________________________________


if (defined $options{"env"}) {
  purgeTables("ENVIRONMENT","TIME");
  purgeTables("TEMP","TIME");
}
if (defined $options{"hist"}) {
  purgeTables("_HISTORY","ENTRYDATE");
}
if (defined $options{"perf"}) {
  purgeTables("PERF","ENTRYDATE");
}
if (defined $options{"event"}) {
  purgeTables("EVENTLOG","EVENT_TIME");
  cleanupTealEvents();
}
elsif (defined $options{"teal"}) {
  cleanupTealEvents();
}
if (defined $options{"security"}) {
  purgeTables("SECURITYLOG","ENTRYDATE");
}
if (defined $options{"diags"}) {
    purgeTables("DIAGTESTS","ENDTIME");
    purgeTables("DIAGBLOCKS","ENDTIME");
    purgeTables("DIAGRESULTS","ENDTIME");
    purgeTables("DIAGRUNS","ENDTIME");
}
#_______________________

#_______________________
#
#  Usage
#
#  Displays the usage of this script if -h is specified or if the wrong
#  arguments are passed in
#
sub usage 
{
  system("perldoc $0");
  exit(0);
}
#_______________________

##########################################
# getTimestamp()
# Returns a string in the following format:# MM-DD-YYYY HH:MM:SS
##########################################
sub getTimestamp {
  my $now = time;
  my @date = localtime $now;
  $date[5] += 1900;
  $date[4] += 1;
  my $stamp = sprintf("%02d-%02d-%04d %02d:%02d:%02d", $date[4], $date[3], $date[5], $date[2], $date[1], $date[0]);
  return $stamp;
} # sub getTimestamp

##########################################
# setupDb()
# Make connection to the DB
##########################################
sub setupDb 
{
    my $dbPropertiesFile = shift();
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

    #open the db.properties file
    $dbPropHash = parseProperties($dbPropertiesFile, $dbPropHash);

    #check if we have a database name and schema
    die "Please specify --properties\n"
        unless defined $dbPropHash->{"name"} and defined $dbPropHash->{"schema_name"};
}

##########################################
# getDbHandle()
# Returns a handle to the database
##########################################
sub getDbHandle
{
    #connect to the database if we're not already
    if (!defined($dbHandle))
    {
        my $dsn = "dbi:DB2:" . $dbPropHash->{"name"};
        $dbHandle =
            DBI->connect($dsn, $dbPropHash->{"user"}, $dbPropHash->{"password"});

	# set schema based on the properties file
        my $schemaStmt = "SET SCHEMA " . $dbPropHash->{'schema_name'};

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


sub cleanupTealEvents
{

# If events were purged from the TBGQEVENTLOG table then the corresponding event records from the TEAL tables
# should also be deleted to maintain data consistency.
#
   my @stmts = (
     "DELETE X_TEALALERT2EVENT WHERE \"t_event_recid\" NOT IN (SELECT RECID FROM TBGQEVENTLOG)",
     "DELETE X_TEALCHECKPOINT  WHERE \"chkpt_id\" NOT IN (SELECT RECID FROM TBGQEVENTLOG)",
     "DELETE X_TEALEVENTLOGEXT WHERE REC_ID NOT IN (SELECT RECID FROM TBGQEVENTLOG)"
   );

   foreach my $stmt (@stmts) {
      
       my $stmtHandle = getDbHandle()->prepare($stmt);

       print "\n" . $stmt . "\n" if($debug);
                
       $stmtHandle->execute();

       my $errno = $stmtHandle->err;  
          
       if  (($errno) &&  (($errno == -601) || ($errno == -803) || ($errno == -612) || ($errno == 605))) {
            print "No TEAL records to delete \n" if($debug)
       }
   } 
}

##########################################
# purgeTables()
# Purge the specified database tables.
##########################################
sub purgeTables {

  my ($class, $time) = @_;

  # print "Class - $class   Time - $time\n";

  my $db_schema = $dbPropHash->{'schema_name'};

  my $sql = qq(select tabname from syscat.tables where tabname like 'TBGQ%$class' and tabname <> 'TBGQREPLACEMENT_HISTORY' and tabschema=ucase('$db_schema'));

  if ($qual) {
      $sql = $sql . " and tabname like ucase('%" . $qual . "%')";     
  }

  my $sth_tables = getDbHandle()->prepare($sql);
  $sth_tables->execute();

  #  print $sql;
  my $tables;
  my $timestamp;

  while ($tables = $sth_tables->fetchrow_hashref) {

    my $tablename = $tables->{TABNAME};
    my $rows;
    
    #######################
    # If the -v option is specified, or the -nocount option is not specified, then get the count of
    # rows that will be deleted.
    #######################
    if ( defined $options{"v"} || !defined $options{"nocount"} ) {

      $sql = qq(select count\(\*\) from $db_schema.$tablename where date\($time\) < CURRENT DATE - $months MONTHS with UR);
       #    print "  $sql \n";
      my $sth_purge = getDbHandle()->prepare($sql);

      # Run the SQL against the db engine
      $sth_purge->execute();
      
      #######################
      # If -v option specified, then just need to print out the number of rows that would be deleted.
      #######################
      if ( defined $options{"v"} ) {
          $timestamp = getTimestamp();
          $rows = $sth_purge->fetchrow;
          print "$timestamp $tablename - \t$rows rows over $months months old\n";
      } else {
          #######################
          # Actually need to delete the rows.
          #######################
          while ($rows = $sth_purge->fetchrow_hashref) {

              my $numrows = $rows->{1};
              my $counter = 0;
              $timestamp = getTimestamp();
 
              print "\n$timestamp Deleting $numrows rows from $tablename\n";

              # Only process the delete if there are rows to delete.
              if ($numrows > 0 ) {
                  my $xactSize = 25000;
                  if ( $tablename eq "TBGQBLOCK_HISTORY" ) {
                      $xactSize = 1;
                  }

                  my $sql = qq( delete from \( select 1 from $db_schema.$tablename where date\($time\) < CURRENT DATE - $months MONTHS FETCH FIRST $xactSize ROWS ONLY\) AS D);
                  my $sth_delete = getDbHandle()->prepare($sql);

                  while ( $counter <= $numrows ) {
                      $timestamp = getTimestamp();
                      if ( $xactSize != 1 && $counter != 0 ) {
                          print "$timestamp\t$counter rows deleted out of $numrows for table - $tablename\n";
                      } elsif ( $xactSize == 1 && $counter != 0 && ($counter % 100 == 0) ) {
                          print "$timestamp\t$counter rows deleted out of $numrows for table - $tablename\n";
                      }
                      $sth_delete->execute();
                      $counter = $counter + $xactSize;
                  }
              }
              $timestamp = getTimestamp();
              print "$timestamp COMPLETED deleting $numrows rows for table - $tablename\n";
          } # end processing rows
      } 
    } else {
        #######################
        # nocount option specified (and -v option not specified), delete rows until done.
        #######################
        $timestamp = getTimestamp();
 
        print "\n$timestamp Deleting from $tablename\n";

        my $counter = 0;
        my $xactSize = 25000;
        my $rowsDeleted = 25000;
        if ( $tablename eq "TBGQBLOCK_HISTORY" ) {
            $xactSize = 1;
            $rowsDeleted = 1;
        }

        my $sql = qq( delete from \( select 1 from $db_schema.$tablename where date\($time\) < CURRENT DATE - $months MONTHS FETCH FIRST $xactSize ROWS ONLY\) AS D);
        my $sth_delete = getDbHandle()->prepare($sql);

        # Keep deleting X number of rows until the rows that were deleted is not equal to the number of rows to 
        # to be deleted each time. If less rows were deleted, then deleted the last rows that meet the criteria.
        while ( $rowsDeleted == $xactSize ) {
            $timestamp = getTimestamp();
            $sth_delete->execute();
            $rowsDeleted = $sth_delete->rows;
            if ( $rowsDeleted > 0 ) {
                $counter = $counter + $rowsDeleted;
                if ( $xactSize != 1 ) {
                    print "$timestamp\t$counter rows deleted for table - $tablename\n";
                } elsif ( $xactSize == 1  && ($counter % 100 == 0) ) {
                    print "$timestamp\t$counter rows deleted for table - $tablename\n";
                }
            }
        }
        $timestamp = getTimestamp();
        print "$timestamp COMPLETED deleting $counter rows for table - $tablename\n";
    }
  }

}
__END__


=pod

=head1 NAME

dbPurge.pl  (Purges the BG/Q database tables of rows beyond a certain timeframe)

=head1 SYNOPSIS

B<dbPurge.pl> [B<-help|-h>] [B<-months|-m> I<months>] [B<-v>] [B<-nocount>] [B<-env>] [B<-hist>] [B<-event>] [B<-diags>] [B<-perf>] [B<-all>]

See below for more description of the switches.

=head1 DESCRIPTION

dbPurge.pl will connect to the BG/Q database and delete all rows beyond a certain number of months. The default is to delete rows that are beyond 3 months old, but the -m flag can be passed to change this to any number of months

=head1 OPTIONS

=over 5

=item B<-h, --help>

This help text

=item B<-properties> <properties-file-name>

DB properties file, defaults to /bgsys/local/etc/bg.properties

=item B<-months, -m> I<number_of_months>

Number of months that determines which rows to delete. Rows older then the number of months passed in from the current date are purged from the tables

=item B<-v>

Will not actually purge anything. Instead it just reports on which tables it would purge, and how many rows it would purge in each.
If this option is specified, the '-nocount' option will be ignored

=item B<-nocount>

Will not determine the number of rows to be purged before purging the rows. Not determing the count of rows to purge 
will improve the performance of the purge.
If the '-v' option is specified, this option will be ignored

=item B<-env>

Will only purge rows from the TBGQ*ENVIRONMENT and TBGQ*TEMP tables

=item B<-hist>

Will only purge rows from the TBGQ*_HISTORY tables

=item B<-event>

Will only purge rows from the TBGQ*EVENTLOG tables

=item B<-perf>

Will only purge rows from the TBGQ*PERF tables

=item B<-security>

Will only purge rows from the TBGQ*SECURITYLOG tables

=item B<-diags>

Will only purge rows from the TBGQDIAG* tables

=item B<-all>

Will purge rows from the ENVIRONMENT, PERF, DIAGS, and HISTORY tables along with the EVENTLOG tables

=item B<-q> <text>

Further qualify the table(s) to purge using text which must be part of the table name
 
=back

NOTE: This utility requires the perl DBI module and DB2 database driver.  


=head1 AUTHOR

International Business Machines (IBM)

=cut  
