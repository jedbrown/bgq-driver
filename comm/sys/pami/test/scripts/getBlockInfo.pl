#!/usr/bin/perl
# begin_generated_IBM_copyright_prolog                             
#                                                                  
# This is an automatically generated copyright prolog.             
# After initializing,  DO NOT MODIFY OR MOVE                       
#  --------------------------------------------------------------- 
# Licensed Materials - Property of IBM                             
# Blue Gene/Q 5765-PER 5765-PRP                                    
#                                                                  
# (C) Copyright IBM Corp. 2011, 2012 All Rights Reserved           
# US Government Users Restricted Rights -                          
# Use, duplication, or disclosure restricted                       
# by GSA ADP Schedule Contract with IBM Corp.                      
#                                                                  
#  --------------------------------------------------------------- 
#                                                                  
# end_generated_IBM_copyright_prolog                               

# Use the expect perl module to simulate interactive use
use Expect;
use Getopt::Long;

#===============================================================================
# Start up a new bg_console
sub startConsole {
	# Start bg_console
	$e->spawn("/bgsys/drivers/ppcfloor/hlcs/bin/bg_console --verbose warn")
		or die "Error: cannot run bg_console\n";
	$e->expect(60, "mmcs\$")
		or die "Error: did not get console prompt\n";
}

#===============================================================================
# Quit the bg_console
sub quitConsole {
	# Quit the console
	$e->send("quit\n");
}

#===============================================================================
# Start up a new DB/2 session
sub startDB2 {
	$dbName = "BGDB0";
	$dbUserName = "bgqsysdb";
	$dbPassword = "db24bgq";

	# Start DB/2
	$e->spawn("db2")
		or die "Error: cannot start db2\n";
	$e->expect(5, "db2 =\>")
		or die "Error: did not get db2 prompt\n";
	$e->send("connect to $dbName user $dbUserName using $dbPassword\n");
	$e->expect(3, "Database Connection Information")
		or die "Error: unable to connec to $dbName\n";
}

#===============================================================================
# Quite the DB/2 session
sub quitDB2 {
	# Quit DB/2
	$e->send("quit\n");
}

# Perl trim function to remove whitespace from the start and end of the string
sub trim($) {
	my $string = shift();
	$string =~ s/^\s+//;
	$string =~ s/\s+$//;
	return $string;
}

#===============================================================================
# Start the main program

# Parse options
GetOptions("h!"=>\$help,
           "isBlock=s"=>\$isBlock_blockName,
           "numNodes=s"=>\$numNodes_blockName,
           "locate=s"=>\$locate_blockName,
           "ioLinks=s"=>\$ioLinks_blockName,
           "ioBlocks=s"=>\$ioBlocks_blockName,
           "blockShape=s"=>\$blockShape_blockName,
           "midplaneCorners=s"=>\$midplaneCorners_blockName,
           "hasJobs=s"=>\$hasJobs_blockName);
if ($help) {
	print	"Retrieve information about a block\n";
	print   "\tgetBlockInfo.pl [options]\n";
	print	" Options: \n";
	print	"\t-h					Displays help text\n";
	print	"\t-isBlock [block-name]		Returns 0 if the block exists, 1 if it does not\n";
	print	"\t-numNodes [block-name]		Prints out the number of nodes in the block\n";
	print	"\t-locate [block-name]			Prints out the 'locate' information for the block\n";
	print	"\t-ioLinks [block-name]		Prints out a list of the IO links used by the block\n";
	print	"\t-ioBlocks [block-name]		Prints out a list of the currently booted IO blocks used by the CN block\n";
	print	"\t-blockShape [block-name]		Prints out the shape of the block (AxBxCxDxE)\n";
	print	"\t-midplaneCorners [block-name]	Prints out the midplanes for the block with associated corner nodes\n";
	print	"\t-hasJobs [block-name]		Returns 0 if the block has any running jobs, 1 if it does not\n";
	exit 1;
}

# Create our expect instance
$e = Expect->new;

# Check if the block exists
$rc = 0;
if ($isBlock_blockName ne "") {
	$e->log_stdout(0);
	startConsole();
	$e->send("get_block_info $isBlock_blockName\n");
	$e->expect(3, "OK", ["block not found" => sub { $rc = 1; } ])
		or die "Error: unable to run get_block_info\n";
	quitConsole();
	if ($rc == 0) {
		print "true\n";
	} else {
		print "false\n";
	}
}

# Get the number of nodes in the block
if ($numNodes_blockName ne "") {
	$e->log_stdout(0);
	startConsole();
	$e->send("list bgqblock $numNodes_blockName\n");
	$e->expect(3, "OK")
		or die "Error: unable to run list bgqblock\n";
	$output = $e->exp_after();
	$numNodes = `echo "$output" | grep "_numcnodes" | awk '{ print \$NF }'`;
	print "$numNodes";
	quitConsole();
}

# Get the locate information for the block
if ($locate_blockName ne "") {
	$e->log_stdout(0);
	startConsole();
	$e->send("select_block $locate_blockName\n");
	$e->expect(3, "OK")
		or die "Error: unable to run select_block\n";
	$e->send("locate \n");
	$e->expect(3, "OK")
		or die "Error: unable to run locate\n";
	$output = $e->exp_after();
	print "$output";
	quitConsole();
}

# Get the IO link information for the block
if ($ioLinks_blockName ne "") {
	$e->log_stdout(0);
	startConsole();
	$e->send("list_io_links $ioLinks_blockName\n");
	$e->expect(3, "OK")
		or die "Error: unable to run list_io_links\n";
	$output = $e->exp_after();
	print "$output";
	quitConsole();
}

# Get the size of the block
if ($blockShape_blockName ne "") {
	$e->log_stdout(0);
	startConsole();
	$e->send("list bgqblock $blockShape_blockName\n");
	$e->expect(3, "OK")
		or die "Error: unable to run list bgqblock\n";
	$output = $e->exp_after();
	$aSize = trim(`echo "$output" | grep "_sizea" | awk '{ print \$3 }'`);
	$bSize = trim(`echo "$output" | grep "_sizeb" | awk '{ print \$3 }'`);
	$cSize = trim(`echo "$output" | grep "_sizec" | awk '{ print \$3 }'`);
	$dSize = trim(`echo "$output" | grep "_sized" | awk '{ print \$3 }'`);
	$eSize = trim(`echo "$output" | grep "_sizee" | awk '{ print \$3 }'`);
	print "${aSize}x${bSize}x${cSize}x${dSize}x${eSize}\n";
	quitConsole();
}

# Get the midplanes and associated corner nodes of the block
# Output will be in the form:
#     "midplane1 midplane2 ..." "corner-node"
#     "R00-M0 R00-M1" "N00"
if ($midplaneCorners_blockName ne "") {
	$e->log_stdout(0);
	startDB2();
	$e->send("select bpid from bgqbpblockmap where blockid = '$midplaneCorners_blockName'\n");
	$e->expect(3, "BPID")
		or die "Error: unable to query bgqbpblockmap table\n";
	$output = $e->exp_after();
        if ($output =~ /0 record\(s\) selected/) {
		# This is a small block - query the small block table
		$e->send("select posinmachine, nodecardpos from bgqsmallblock where blockid = '$midplaneCorners_blockName' order by nodecardpos asc\n");
		$e->expect(3, "POSINMACHINE")
			or die "Error: unable to query bgqsmallblock table\n";
		$output = $e->exp_after();
        	if ($output =~ /0 record\(s\) selected/) {
			die "Error: small block does not exist";
		}
		@lines = `echo "$output" | grep -v "POSINMACHINE" | grep -v "NODECARDPOS" | grep -v "record\(s\) selected" | grep -v "db2"`;
		@midplanes = ();
		@cornerNodes = ();
		foreach $line (@lines) {
			$line = trim($line);
			if ($line ne "" && $line !~ "---") {
				$midplanesSize = $#midplanes + 1;
				if ($midplanesSize eq 0) {
					# The first entry will be the corner since this is a small block
					@midplanes[0] = trim(`echo "$line" | awk '{ print \$1 }'`);
					@cornerNodes[0] = trim(`echo "$line" | awk '{ print \$2 }'`);
					break;
				}
			}
		}
	} else {
		# This is a base partition or large block
		@lines = `echo "$output" | grep -v "BPID" | grep -v "record\(s\) selected" | grep -v "db2"`;
		@midplanes = ();
		@cornerNodes = ("N00");		# All midplanes have a corner at N00
		$i = 0;
		foreach $line (@lines) {
			$line = trim($line);
			if ($line ne "" && $line !~ "---") {
				@midplanes[$i] = $line;
				$i++;
			}
		}
	}

	print "\"@midplanes\" \"@cornerNodes\"\n";
	quitDB2();
}

# Get the IO link information for the block
if ($ioBlocks_blockName ne "") {
	$e->log_stdout(0);
	startConsole();

        # Get the IO nodes used by the CN block
	$e->send("list_io_links $ioBlocks_blockName\n");
	$e->expect(3, "OK")
		or die "Error: unable to run list_io_links\n";
	$output = $e->exp_after();
	@ioNodes = `echo "$output" | grep "...-..-J.." | awk '{ print \$1 }'`;

        # Find out which booted IO blocks use the IO nodes.  Sample command output below:
        #
	# ID      STATUS SUBDIVIDED BLOCK                            BLOCKSTATUS NODEPOS NUMNODES
	# Q01-I5  A      Y          Q01-I5-N01                       I           J00     2
	#                           Q01-I5-N00                       I           J02     2
	#                           Q01-I5-N03                       I           J04     2
	#                           Q01-I5-N02                       I           J06     2
	# Q01-I6  A      N
	# Q01-I7  A      N          Q01-I6                           I
	$e->send("list_io\n");
	$e->expect(3, "OK")
		or die "Error: unable to run list_io\n";
	$output = $e->exp_after();
	@lines = `echo "$output" | grep -v "SUBDIVIDED"`;
	shift(@lines);	# remove the first line
	pop(@lines);	# remove the last line
	$ioDrawer = "";
	@ioBlocks = ();
	foreach $line (@lines) {
		$isSubdivided = trim(`echo "$line" | awk '{ print \$3 }'`);
		if ($isSubdivided eq "Y" || $isSubdivided eq "N") {
			# New set of lines for an IO drawer
			$ioDrawer = trim(`echo "$line" | awk '{ print \$1 }'`);
			$ioBlock = trim(`echo "$line" | awk '{ print \$4 }'`);
			if ($isSubdivided eq "N") {
				# Entire IO drawer is booted together
				foreach $ioNode (@ioNodes) {
					$tmpIoDrawer = substr($ioNode, 0, 6);
					if ($ioDrawer eq $tmpIoDrawer) {
						push(@ioBlocks, $ioBlock);
					}
				}
			} else {
				# IO drawer is divided up into smaller blocks
				$ioBlock = trim(`echo "$line" | awk '{ print \$4 }'`);
				$startNode = trim(`echo "$line" | awk '{ print \$6 }'`);
				foreach $ioNode (@ioNodes) {
					$tmpIoDrawer = substr($ioNode, 0, 6);
					if ($ioDrawer eq $tmpIoDrawer) {
						$tmpStartNode = substr($ioNode, 7, 3);
						if ($startNode eq $tmpStartNode) {
							push(@ioBlocks, $ioBlock);
						}
					}
				}
			}
		} else {
			# Continuation of the previous IO drawer (and is subdivided up)
			$ioBlock = trim(`echo "$line" | awk '{ print \$1 }'`);
			$startNode = trim(`echo "$line" | awk '{ print \$3 }'`);
			foreach $ioNode (@ioNodes) {
				$tmpIoDrawer = substr($ioNode, 0, 6);
				if ($ioDrawer eq $tmpIoDrawer) {
					$tmpStartNode = substr($ioNode, 7, 3);
					if ($startNode eq $tmpStartNode) {
						push(@ioBlocks, $ioBlock);
					}
				}
			}
		}
	}
	%tmpHash = map { $_ => 1 } @ioBlocks;
	@ioBlocks = keys %tmpHash;
	$numBlocks = @ioBlocks;
        for ($i = 0; $i < $numBlocks; $i++) {
		if ($i != 0) {
			print ",";
		}
		print $ioBlocks[$i];
	}
	print "\n";

	quitConsole();
}

# Check if the block has any jobs on it or not
$rc = 0;
if ($hasJobs_blockName ne "") {
	$e->log_stdout(0);
	startConsole();
	$e->send("list_jobs --block $hasJobs_blockName\n");
	$e->expect(3, ["No jobs returned." => sub { $rc = 1; } ],
		["OK" => sub { $rc = 0; } ])
		or die "Error: unable to run list_jobs\n";
	quitConsole();
	if ($rc == 0) {
		print "true\n";
	} else {
		print "false\n";
	}
}

# Done
exit $rc

