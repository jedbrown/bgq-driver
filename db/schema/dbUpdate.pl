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
"drop trigger linkchip_history_d",
"alter table TBGQCoolantEnvironment ADD COLUMN shutoffcause INTEGER NOT NULL WITH DEFAULT 0",
"INSERT INTO TBGQProductType (productid, description) values('WWD2','BG/Q Compute Card')",
"drop trigger node_history_u",
"
create trigger node_history_u
  after update on tbgqnode
  referencing new as n old as o
  for each row mode db2sql

  begin atomic 

   if ((o.midplanepos = n.midplanepos) and (o.nodecardpos = n.nodecardpos) and (o.position = n.position)) then

   if ((n.status = 'F') or (o.status = 'F' and n.status = 'A') or (o.seqid <> n.seqid)) then
     -- omit insertions for Software Failure transitions
     -- or when sequence ID has changed from another trigger
   else
   insert into tbgqnode_history 
      (serialNumber, productId, ecid, midplanepos,nodecardpos,  position, ipAddress, macaddress, status, memoryModuleSize, memorySize, psro, vpd, voltage)
   values
      (n.serialNumber, n.productId, n.ecid, n.midplanepos, n.nodecardpos, n.position, n.ipAddress, n.macaddress, n.status, n.memoryModuleSize, n.memorySize, n.psro, n.vpd, n.voltage) ;

    if ((o.serialnumber <> n.serialnumber) OR (o.ecid <> n.ecid)) then

     insert into tbgqreplacement_history 
      (type, location, oldserialnumber,newserialnumber, oldecid, newecid, oldstatus, newstatus)
     values
      ( 'Node', n.midplanepos || '-' || n.nodecardpos || '-' || n.position,o.serialnumber,n.serialNumber,o.ecid,n.ecid, o.status, n.status);

     if (n.bitsteering <> -1) then    
      update tbgqnode set bitsteering = -1 where  midplanepos = n.midplanepos and nodecardpos = n.nodecardpos and position = n.position;
     end if;
       
    end if;

   end if;
 
   else

    SIGNAL SQLSTATE '70003' ('Updating positions not permitted');

   end if;
end
",
"
CREATE TABLE TBGQBlockAction_history
(
   blockId               char(32)       NOT NULL,
   status                char(1)        NOT NULL WITH DEFAULT 'F',
   action                char(1)        NOT NULL WITH DEFAULT ' ',
   entrydate             timestamp      DEFAULT current timestamp,
   creationId            integer        NOT NULL
);
CREATE ALIAS BGQBlockAction_history for TBGQBlock_history;
)
",
"
create trigger block_action_history
  after update of action on tbgqblock
  referencing new as n old as o
  for each row mode db2sql
  when (
    n.action <> o.action
  )

  begin atomic
    insert into tbgqblockaction_history (blockid,status,action,creationId) values
    (n.blockid,n.status,n.action,n.creationid);
 end
",
"drop trigger cable_history_u",
"

create trigger cable_history_u
  after update on tbgqcable
  referencing new as n old as o
  for each row mode db2sql

  begin atomic 

   if ((o.fromlocation = n.fromlocation) and (o.tolocation = n.tolocation)) then

     if (o.status <> n.status) then
       insert into tbgqcable_history (fromlocation,tolocation,status,seqid,badwiremask)
       values (n.fromlocation,n.tolocation,n.status,n.seqid,n.badwiremask);
     end if;
   else

     SIGNAL SQLSTATE '70003' ('Updating positions not permitted');

   end if;

  end
",
"
create trigger cable_history_d
  before delete on tbgqcable
  referencing old as n
  for each row mode db2sql

  begin atomic 

    SIGNAL SQLSTATE '70003' ('Deletion not permitted');
  
 end
"
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
    system($dbImport); 

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

    print "\n" . $stmt . "\n";
  
    if (substr($stmt,0,9) eq "IMPORTANT") { return; }
    $stmtHandle->execute();

    my $errno = $stmtHandle->err;  
   
    if  (($errno) &&  (($errno == -601) || ($errno == -803) || ($errno == -612) || ($errno == 605))) {
        print "Schema already updated \n";
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
               my $dbPropertiesFile = "./bg.properties";    #location of bg.properties file, in CWD by default
               my $schema;
               my $database;

# Parse the command line
               GetOptions(
                          'properties=s' => \$dbPropertiesFile,
                          'help'           => \$optHelp
                          )
               or printUsage();

               printUsage() if ($optHelp);
 

setupDb($dbPropertiesFile);

updateStatements();

system("mkdir -p --mode=777 /bgsys/logs/BGQ/diags/bringup/");

exit(255);

__END__

=head1 NAME

dbUpdate.pl (utility to update the Blue Gene database)

=head1 SYNOPSIS

dbUpdate.pl [options]

Options:

--properties [properties-file-name] |  properties file

--help | Prints this help text


=head1 DESCRIPTION

This utility should be used to update the Blue Gene/Q database tables when installing a new driver. 

NOTE: This utility requires the perl DBI module and DB2 database driver.  

=head1 BUGS

None.

=head1 AUTHOR

International Business Machines (IBM)

=cut

