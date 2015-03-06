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
#
# This is an example of how to access the schedule info as dumped by
# "sched_test -V". It simply loads the info into this perl program
# and then traverses the schedule portion of it and displays it.

require sched;

sched_load();

# If you want to dump the raw Perl structure, use this:
#dump_sched(\$SCHED);

# The top-level structure is a reference to a hash. One entry
# exists for each rank in the block. All keys are of the form "rank%d".
my $nranks = scalar(@{$SCHED});
print "Number of ranks = $nranks\n";
for (my $n = 0; $n < $nranks; ++$n) {
	my $h = ${$SCHED}[$n];
	my $nph = scalar(@{${h}->{phases}});
	my $nphases = ${h}->{nphases};
	my $startph = ${h}->{startph};
	print "$n: Number of phases = $nphases ($nph), start = $startph\n";
	# Note: there is also mapping information in the rank hash
	# entry, e.g. ${h}->{x} is the X-coordinate.
	for (my $p = $startph; $p < $startph + $nphases; ++$p) {
		my $a = ${${h}->{phases}}[$p];
		# Each phase is a hash entry keyed with "phase%d",
		# The phase is stored as an array, one element for
		# each step in the phase.
		my $nsteps = scalar(@{$a});
		print "$n: $p: Number of steps = $nsteps\n";
		for (my $s = 0; $s < $nsteps; ++$s) {
			# Each step is a hash (ref). If the key 'dst'
			# is defined, then this step is a SEND.
			# Key 'src' will be defined if it is a RECV.
			# 'op' contains the operation (task) string.
			my $h = ${$a}[$s];
			if (defined($h->{dst})) {
				print "$n: $p: $s: -> ".$h->{dst}." ".$h->{op}."\n";
			} else {
				print "$n: $p: $s: <- ".$h->{src}." ".$h->{op}."\n";
			}
		}
	}
}
