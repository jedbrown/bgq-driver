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

# For argument parsing
use Getopt::Long;

# For parsing bg.properties
use Config::IniFiles;

# Globals
use vars
    qw(%portMap $size $doneLinks $doneSwitch $doneClock $dbHandle $dbPropHash @bgpMidplanes *INFILE $debug $macAddress $ipAddress);

# For dirname
use File::Basename;

sub updateStatements
{

# the statements to execute, separate by commas    
my @insertStmts = (

"ALTER TABLE tbgqdiagseventlog data capture none",
"ALTER TABLE tbgqdiagseventlog alter column ctlaction set data type varchar(256) ",
"ALTER TABLE tbgqdiagseventlog data capture changes",
"DROP trigger midplane_history_u",
"CREATE INDEX EventLogQ on  Tbgqeventlog (qualifier, recid desc)",
"CREATE INDEX TEALTHRESHOLDEVENTLOG ON TBGQEVENTLOG (MSG_ID ASC, LOCATION ASC, SERIALNUMBER ASC, EVENT_TIME ASC)",
"DROP VIEW X_BGQ_1_1",
"CREATE VIEW X_BGQ_1_1 (
    \"rec_id\",
    \"category\",
    \"severity\",
    \"jobid\",
    \"block\",
    \"serialnumber\",
    \"ecid\",
    \"cpu\",
    \"ctlaction\",
    \"message\",
    \"rawdata\",
    \"diags\",
    \"qualifier\") AS
    SELECT BGQEVENTLOG.RECID AS \"rec_id\",
           BGQEVENTLOG.CATEGORY AS \"category\",
           BGQEVENTLOG.SEVERITY AS \"severity\",
           BGQEVENTLOG.JOBID AS \"jobid\",
           BGQEVENTLOG.BLOCK AS \"block\",
           BGQEVENTLOG.SERIALNUMBER AS \"serialnumber\",
           BGQEVENTLOG.ECID AS \"ecid\",
           BGQEVENTLOG.CPU AS \"cpu\",
           BGQEVENTLOG.CTLACTION AS \"ctlaction\",
           BGQEVENTLOG.MESSAGE AS \"message\",
           BGQEVENTLOG.RAWDATA AS \"rawdata\",
           BGQEVENTLOG.DIAGS AS \"diags\",
           BGQEVENTLOG.QUALIFIER AS \"qualifier\"
   FROM TBGQEVENTLOG AS BGQEVENTLOG;",
"
create trigger midplane_history_u
  after update on tbgqmidplane
  referencing new as n old as o
  for each row mode db2sql

  begin atomic 

    if (o.posinmachine = n.posinmachine) then

    if ((n.status = 'F') or (o.status = 'F' and n.status = 'A') or (o.seqid <> n.seqid)) then
      -- omit insertions for Software Failure transitions
      -- or when sequence ID has changed from another trigger
    else
    insert into tbgqmidplane_history 
      (serialNumber, productId, machineSerialNumber, posInMachine, status, ismaster, vpd)
     values
      (n.serialNumber, n.productId, n.machineSerialNumber, n.posInMachine, n.status, n.ismaster, n.vpd);

     if (o.serialnumber <> n.serialnumber) then

     insert into tbgqreplacement_history 
      (type, location, oldserialnumber,newserialnumber, oldstatus, newstatus)
     values
      ('Midplane', n.posInMachine,o.serialnumber,n.serialNumber, o.status, n.status);
        
     end if;
     end if;

    else

     SIGNAL SQLSTATE '70003' ('Updating positions not permitted');

    end if;

   end
",
"ALTER TABLE tbgqnetconfig data capture none",
"ALTER TABLE tbgqnetconfig alter column itemname set data type varchar(128) ",
"ALTER TABLE tbgqnetconfig data capture changes",
"CREATE OR REPLACE ALIAS BGQBlockAction_history for TBGQBlockAction_history"
);


    foreach my $stmt (@insertStmts)
    {
        processSQL($stmt);
    }

    # Update tbgqmsgtypes table
    my $exe_dir = dirname( $0 );
    my $eventFile = $exe_dir . "/ddl/event_code_table.txt";
    my $user = $dbPropHash->{"user"};
    my $pwd = $dbPropHash->{"password"};
    my $dsn = $dbPropHash->{"name"};
    my $dbConnect = "connect to " . $dsn;
    if (defined($user) && defined($pwd)) {
        $dbConnect .= " user " . $user . " using " . $pwd;
    }
    my $dbImport = "db2 $dbConnect; db2 import from " . $eventFile . " of DEL replace into " . $dbPropHash->{"schema_name"} . ".tbgqmsgtypes";
    my $result = `$dbImport`;

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


##########################################
# processSQL()
# Deals with SQL commands and provides a common interface
##########################################
sub processSQL
{
    my $stmt = shift();
  
    my $stmtHandle = getDbHandle()->prepare($stmt);

    print "\n" . $stmt . "\n" if($debug);
  
    #if (substr($stmt,0,9) eq "IMPORTANT") { return; }
    $stmtHandle->execute();

    my $errno = $stmtHandle->err;  
   
    if  (($errno) &&  (($errno == -601) || ($errno == -803) || ($errno == -612) || ($errno == 605))) {
        print "Schema already updated \n" if($debug)
    } 
    
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
# main()
##########################################

# Define local variables
               my $optHelp          = undef;
               my $bgPropertiesFile = $ENV{BG_PROPERTIES_FILE};
               my $schema;
               my $database;

# Parse the command line
               GetOptions(
                          'properties=s' => \$bgPropertiesFile,
                          'help'           => \$optHelp,
                          'debug'          => \$debug
                          )
               or printUsage();

               printUsage() if ($optHelp);
               
               if (!defined($bgPropertiesFile)) {
                  $$bgPropertiesFile = "/bgsys/local/etc/bg.properties";
               }
 

setupDb($bgPropertiesFile);

close(STDERR) unless($debug);

updateStatements();

print "dbUpdate completed successfully\n";

exit(255);

__END__

=head1 NAME

dbUpdate.pl (utility to update the Blue Gene database)

=head1 SYNOPSIS

dbUpdate.pl [options]

Options:

--properties [properties-file-name] |  properties file

--help | Prints this help text

--debug | dump extra output


=head1 DESCRIPTION

This utility should be used to update the Blue Gene/Q database tables when installing a new driver. 

NOTE: This utility requires the perl DBI module and DB2 database driver. 

NOTE: Database connection information will be obtained from the bg.properties file specified by
BG_PROPERTIES_FILE environment variable. If that environment is not defined, it will use 
/bgsys/local/etc/bg.properties by default.

=head1 AUTHOR

International Business Machines (IBM)

=cut

