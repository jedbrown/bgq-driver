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

=head1 COPYRIGHT

(C)Copyright IBM Corp.  2009, 2010
IBM CPL License

=cut

$_name = "Data";	# What sort of test am I?

use Getopt::Long;
require sched;

Getopt::Long::Configure("no_ignore_case", "bundling");
GetOptions("v" => \$verb,
	"verbose" => \$verb,
	"p" => \$pos,
	"positive" => \$pos);

sched_load();

#dump_sched(\@SCHED);

if (scalar(@{$SCHED}) == 0) {
	print STDERR "${_title} ${_name}: Empty schedule\n";
	exit 1;
}

# Each entry represents that node's "buffer" at the point in time.
my @data = ();
for (my $n = 0; $n < get_nranks(); ++$n) {
	$data[$n] = "$n";
}

# Each entry represents that node's "buffer" at the "next" point in time.
# This data is copied to @data at the end of each phase.
my @pre = ();

# Assume recvs/sends have previously been verified.
# Uses receives (SrcPeList) for verification.
for (my $p = 0; ; ++$p) {
	my $did = 0;
	@pre = @data;
	# First, process all TREE_* sends
	my @barriers = ();
	my $dat;
	for (my $n = 0; $n < get_nranks(); ++$n) {
		my $rh = get_rank($n);
		if ($p >= get_nphases($rh)) { next; }
		my $pref = get_phase($rh, $p);
		if (!defined($pref)) { next; }
		for (my $t = 0; $t < get_nsteps($pref); ++$t) {
			my $th = get_step($pref, $t);
			if (!defined($th) || !defined($th->{dst})) { next; }
			if ($th->{op} eq "LOCKBOX_BARRIER") {
				my $peers = get_peers($rh->{rank});
				foreach my $peer (@{$peers}) {
					$pre[$n] .= ",".$data[$peer];
				}
			} else {
				# For the tree ops we're interested in,
				# the dst will be the root node.
				my $i;
				if ($th->{op} eq "TREE_BCAST") {
					# BCAST: only root contributes data
					$i = ($rh->{rank} == $ROOT);
				} elsif ($th->{op} eq "TREE_ALLREDUCE") {
					# ALLREDUCE: all contribute data
					$i = 1;
				} elsif ($th->{op} eq "TREE_BARRIER") {
					# BARRIER: all contribute "data"
					$i = 1;
					push(@barriers, $n);
				} else {
					next;
				}
				if ($i) {
					if (!defined($dat)) {
						$dat = $data[$n];
					} else {
						$dat .= ",".$data[$n];
					}
				}
			}
		}
	}
	# TREE_BARRIER: There will be no recvs...
	foreach my $n (@barriers) {
		$pre[$n] = $dat;
	}
	# $dat, if defined(), is the data to use for this phase for *_RECV_STORE
	for (my $n = 0; $n < get_nranks(); ++$n) {
		my $rh = get_rank($n);
		if ($p >= get_nphases($rh)) {
			next;
		}
		++$did;
		my $pref = get_phase($rh, $p);
		if (!defined($pref)) {
			print "Undefined phase: $p\n";
			next;
		}
		for (my $t = 0; $t < get_nsteps($pref); ++$t) {
			my $th = get_step($pref, $t);
			if (!defined($th)) {
				# Error?
				next;
			}
			if (defined($th->{dst})) {
				# send - assume already verified matching recvs
				# exception: barriers need processing since there
				# are no recvs in that case. barriers are handled above.
			} elsif (defined($th->{src})) {
				# recv
				if ($th->{op} eq "COMBINE_SUBTASK" ||
						$OP eq "Barrier") {
					my $s = get_index_rank($th->{src});
					$pre[$n] .= ",".$data[$s];
				} elsif ($th->{op} =~ m/_RECV_STORE/) {
					$pre[$n] = $dat;
				} elsif ($th->{op} !~ m/_RECV_NOSTORE/) {
					my $s = get_index_rank($th->{src});
					$pre[$n] = $data[$s];
				}
			} else {
				# bogus?
				print STDERR "Unrecognized step: $p $n ...\n";
			}
		}
	}
	if ($did eq 0 ) {
		last;
	}
	@data = @pre;
}
if ($verb) {
	print Dumper(\@data);
}

my $rootx = get_index_rank($ROOT);
if ($OP eq "Reduce") {
	$ret = chk_node_data($rootx, $data[$rootx]);
} elsif ($OP eq "Allreduce") {
	$ret = chk_all_data(\@data);
} elsif ($OP eq "Broadcast") {
	$ret = chk_all_bcast($rootx, \@data);
} elsif ($OP eq "Barrier") {
	# print STDERR "Barrier data verify not supported (yet?)\n";
	$ret = chk_all_data(\@data);
}
if (($pos || $verb) && $ret == 0) { print "${_title} ${_name}: OK\n"; }
exit $ret;
