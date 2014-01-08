#-*- mode: perl;-*-
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
# (C) Copyright IBM Corp.  2005, 2011                              
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

package gui;

BEGIN
{
    eval
    {
	$TK_LIBRARIES_LOADED = 0;
	require Tk;
	Tk->import();
	$TK_LIBRARIES_LOADED = 1;
    };
    if($TK_LIBRARIES_LOADED == 0)
    {
	print '
Sorry, the Tk module for Perl has not been installed on this machine.  Tk is a graphics package and is needed for coreprocessor to run in GUI mode.

You can either:
1) Install the Tk module.  It can be found at www.cpan.org.
2) Run coreprocessor in -nogui mode
';
	exit(-1);
    }
}
use bagotricks;
use objdump;
use bglsnapshot;
use POSIX qw(SIGCHLD);

require "utilities.pl";
require "3dmap.pl";
require "memtemplate.pl";

@COREMAP = ();

#sub REAPER
#{
#    print "reaper @_\n";
#    while(($child = waitpid(-1, WNOHANG)) > 0)
#    {
#	print "child $child\n";
#    }
#}
#$SIG{CHLD} = &\REAPER;
#$sigset = POSIX::SigSet->new(SIGCHLD);
#$sigact = POSIX::SigAction->new( \&REAPER);
#$sigact->flags(&POSIX::SA_NOCLDSTOP);
#POSIX::sigaction(SIGCHLD, $sigact);
#print "sigaction: $sigact\n";

$SIG{CHLD} = 'IGNORE';
$SIG{PIPE} = 'IGNORE';

sub addsession
{
    my($name, $CORE) = @_;
    die "There is already a session called '$name'" if(exists $SNAPSHOTS{$name});
    
    $SNAPSHOTS{$name} = $CORE;
    setsnapshot($name);
    &mkmenu();
}
sub setsnapshot
{
    my($val) = @_;
    $ACTIVECORE = $SNAPSHOTS{$val};
    FGET("mode")->configure(-state => "normal");
    if(ref($ACTIVECORE) eq "bgqjtageader")
    {
	SetValue("sessionlabel", "$val (JTAG)");
    }
    if(ref($ACTIVECORE) eq "bglsnapshot")
    {
	SetValue("sessionlabel", "$val (SNAP)");
    }
    if(ref($ACTIVECORE) eq "bglcorereader")
    {
	SetValue("sessionlabel", "$val (CORE)");
    }
    if(ref($ACTIVECORE) eq "bgpcorereader")
    {
	SetValue("sessionlabel", "$val (CORE)");
    }
    if(ref($ACTIVECORE) eq "rrcorereader")
    {
	SetValue("sessionlabel", "$val (CORE)");
    }
    if(ref($ACTIVECORE) eq "sdbgreader")
    {
	SetValue("sessionlabel", "$val (SDBG)");
    }
    &changemode("dummyclass", "NOCHANGE");
    mkmenu();
}
sub mksnapshot
{
    my $core = bglsnapshot->clone($ACTIVECORE);
    $core->{"cnkbin"} = objdump->new($core->{"options"}{"-b"});
    &addsession("Session " . ++$::SESSIONID, $core);
}
sub changehaltstate
{
    my($newstate) = @_;
    my @sel = FGET("coremap")->curselection();
    my @nodes = ();
    foreach $key (@sel) { push(@nodes, @{$COREMAP[$key]{"nodes"}}); }
    
    if($#nodes == -1)
    {
	@nodes = $ACTIVECORE->getnodenames();
    }
    @nodes = ansort(@nodes);
    
    $ACTIVECORE->setstate($newstate, @nodes);
    if($newstate =~ /step/i)
    {
	&changemode("mode", GetValue("mode"));
    }
    if($newstate =~ /run/i)
    {
	&changemode("mode", "Processor Status");
    }
}

sub analyze
{
    my @nodes = ansort(map { @{$COREMAP[$_]{"nodes"}}; } FGET("coremap")->curselection());
    my $output = "";
    
    $output .= "Common Neighbors\n";
    $output .= "----------------\n";
    my (%hash) = &utilities::FindCommonNeighbors($ACTIVECORE, "X+X-Y+Y-Z+Z-T+SET", @nodes);
    
    foreach $node (reverse sort { $hash{$a}{"neighcount"} <=> $hash{$b}{"neighcount"}} keys %hash)
    {
        if($hash{$node}{"inset"} == 0)
        {
            $output .= sprintf("%20s has %d neighbors (node was outside the set of selected nodes)\n", $node, $hash{$node}{"neighcount"});
        }
        else
        {
            $output .= sprintf("%20s has %d neighbors (node was in the set of selected nodes)\n", $node, $hash{$node}{"neighcount"});
        }
    }
    SetValue("data", $output);
}

sub draw3dmap
{
    my @sel = FGET("coremap")->curselection();
    my @nodes = ();
    foreach $key (@sel) { push(@nodes, @{$COREMAP[$key]{"nodes"}}); }
    @nodes = ansort(@nodes);
    &map3d::build($ACTIVECORE, @nodes);
}

sub updatetext
{
    my $node = GetValue("nodes");
    if($node =~ /disasm/i)
    {
        ($iar) = $node =~ /disasm\s+(\S+)/i;
        SetValue("corelabel", "Disassembly at $iar");
        SetValue("data", &utilities::disassemble($ACTIVECORE, $key, $iar, 100));
        FGET("data")->see("101.0");
        FGET("data")->tagAdd("sel", "101.0 linestart", "101.0 lineend");
    }
    else
    {
        SetValue("corelabel", "Corefile: " . $ACTIVECORE->{$node}{"corefilepath"}) if(exists $ACTIVECORE->{$node}{"corefilepath"});
        SetValue("data", $ACTIVECORE->{$node}{"corerawdata"});
    }
    mkmenu();
}

sub updatenodes
{
    my @sel = FGET("coremap")->curselection();
    my @nodes = ();
    foreach $key (@sel) { push(@nodes, @{$COREMAP[$key]{"nodes"}}); }
    @nodes = ansort(@nodes);
    
    FGET("nodes")->delete(0, 'end');
    
    my $iar = $COREMAP[$sel[0]]{"signature"};
    if($iar =~ /^0x/)
    {
        $iar =~ s/(0x.*?)\s.*/$1/;
        FGET("nodes")->insert(0, "disasm $iar");
        SetValue("locationlabel", "Location: " . &utilities::location($ACTIVECORE, $nodes[0], $iar));
    }
    else
    {
        SetValue("locationlabel", "Location: ???");
    }
    SetValue("ctrlselector", ($#nodes+1) . " nodes");
    FGET("nodes")->insert('end', @nodes);
    mkmenu();
}

sub drawmap
{
    my(@COREMAP) = @_;
    
    $lastcoremap = "";
    FGET("coremap")->delete(0, 'end');
    for($x=0; $x<$#COREMAP+1; $x++)
    {
        $sig = $COREMAP[$x]{"signature"};
        $depth = sprintf("%-2d:", $COREMAP[$x]{"indent"});
        if(GetValue("mode") =~ /condensed|detailed|survey|trace/i)
        {
            if($sig =~ /^0x/)
            {
                $sig = &utilities::lookup($ACTIVECORE, $COREMAP[$x]{"nodes"}[0], $sig);
            }
            if(GetValue("mode") =~ /detailed/i)
            {
                $depth .= " (IAR=".$COREMAP[$x]{"signature"}.")" if(($COREMAP[$x]{"signature"} ne "IO Node") && 
                                                                    ($COREMAP[$x]{"signature"} ne "Compute Node") &&
								    ($COREMAP[$x]{"signature"} ne "<traceback not fetched>") &&
								    ($COREMAP[$x]{"signature"} ne "<over clip depth threshold>"));
            }
        }
        my @nodes = @{$COREMAP[$x]{"nodes"}};
        FGET("coremap")->insert('end', $depth . ' ' x ($COREMAP[$x]{"indent"}*4) . $sig . " (" . ($#nodes+1) . ")");
        $lastcoremap .= $depth . ' ' x ($COREMAP[$x]{"indent"}*4) . $sig . " (" . ($#nodes+1) . ")\n";
    }
}

sub changeoption
{
    &changemode("dummyclass", "NOCHANGE");
}

sub opentemplate
{
    my $node = GetValue("nodes");
    &template::build($ACTIVECORE, $node);
}

sub quitcoreprocessor
{
    foreach $snap (keys %SNAPSHOTS)
    {
	my $core = $SNAPSHOTS{$snap};
	if(ref($core) eq "bgqjtagreader")
	{
	    foreach $key ($core->getnodenames())
	    {
		if($core->{$key}{"procstatus"} !~ /run/i)
		{
		    &stoppedjobs::build();
		    return;
		}
	    }    
	}
    }
    
    Tk::exit(0);
}

sub resumeall
{
    foreach $snap (keys %SNAPSHOTS)
    {
        my $core = $SNAPSHOTS{$snap};
        if(ref($core) eq "bgqjtagreader")
        {
	    my @nodes = $core->getnodenames();
	    $core->setstate("run", @nodes);
        }
    }
}

sub getfilters
{
    return ansort(keys %{$ACTIVECORE->{"filters"}});
}

sub setfilter
{
    my $core = shift @_;
    my $name = shift @_;
    my @nodes = @_;
    $core->{"filters"}{$name}{"active"} = 0;
    @{$core->{"filters"}{$name}{"nodes"}} = @nodes;
    mkmenu();
}

sub mkmenu
{    
    my @orig = FSTACK();
    FSTACK(@orig, FGET("menucanvas"));
    $menuframe = FIN();
    
    my @disabled = ();             if(!defined $ACTIVECORE) { @disabled = ("disabled"); }
    my @nommcs   = ("disabled");   if(ref($ACTIVECORE) eq "bgqjtagreader") { @nommcs = (); }
    my @livesystem = ("disabled"); if(ref($ACTIVECORE) eq "bgqjtagreader") { @livesystem = (); }
    @tmp = FGET("coremap")->curselection();
    my @nogroup  = ();           if($#tmp==-1) {@nogroup = ("disabled"); }
    @tmp = FGET("nodes")->curselection();
    my @nonode   = ();           if(($#tmp==-1)||(GetValue("nodes") =~ /disasm/)) {@nonode  = ("disabled"); }
    my @sdbg     = ();           if(ref($ACTIVECORE) eq "sdbgreader") { @sdbg = ("disabled"); }
    
    FCTRL("file", Menubutton, -text => "File", -side => 'left');
    FMENU("file", [-tearoff => 0], [ "Attach to Block" => ['attachdebugger::build'],
				     "Attach to Job ID"   => ['attachsdbg::build'],
				     "Attach to runjob PID"   => ['attachsdbgpid::build'],
				     "Load Core"       => ['loadcore::build'],
				     "Load Snapshot"   => ['loadsnapshot::build'],
				     "sep",
				     "Load symbol table" => ['loadsymbols::build', $ACTIVECORE], @disabled,
				     "sep",
				     "Take Snapshot" => ['gui::mksnapshot'], @disabled,
				     "sep",
				     "Save Traceback" => ['gui::savedata', "Traceback"], @disabled,
#				     "Save MMCS Console" => ['gui::savedata', "Console"], @disabled, @nommcs,
				     "Save Snapshot" => ['gui::savedata', "Snapshot"], @disabled,
				     ,"sep", "Quit" => ['gui::quitcoreprocessor']]);
    
    FCTRL("control", Menubutton, -text => "Control", -side => 'left');
    FMENU("control", [-tearoff => 0], [ "Run" => ['gui::changehaltstate',  "run" ], @disabled, @livesystem,
					"Step" => ['gui::changehaltstate', "step" ], @disabled, @livesystem,
					"Halt" => ['gui::changehaltstate', "halt" ], @disabled, @livesystem]);
    
    FCTRL("analyze", Menubutton, -text => "Analyze", -side => 'left');
    FMENU("analyze", [-tearoff => 0], [
                                       "Pop stack frame" => sub { foreach $key (map { @{$COREMAP[$_]{"nodes"}};} FGET("coremap")->curselection()) { $ACTIVECORE->{$key}{"stripframes"}++; } &changemode("dummyclass", "NOCHANGE"); }, @disabled, @nogroup,
                                        "Increase stack clip threshold" => sub { foreach $key (map { @{$COREMAP[$_]{"nodes"}};} FGET("coremap")->curselection()) { $ACTIVECORE->{$key}{"clipthreshold"}++; } &changemode("dummyclass", "NOCHANGE"); }, @disabled, @nogroup,
                                       "Show full stack" => sub { foreach $key (map { @{$COREMAP[$_]{"nodes"}};} FGET("coremap")->curselection()) { $ACTIVECORE->{$key}{"stripframes"}=0; $ACTIVECORE->{$key}{"clipthreshold"}=0;} &changemode("dummyclass", "NOCHANGE"); }, @disabled, @nogroup,
                                        "sep",
                                       "Find Missing Neighbors" => ['gui::analyze'], @disabled, @nogroup, @sdbg
#				       ,
#				       "3D Map" => ['gui::draw3dmap'], @disabled, @nogroup, @sdbg,
#	                               "sep",
#	                               "Memory Template" => ['gui::opentemplate'], @disabled, @nommcs, @nogroup, @nonode, @sdbg,
#				       "Start GDB" => ['gdb::build', $ACTIVECORE], @disabled, @nommcs, @nonode, @sdbg
				       ]);
    
    my @filters = ();
    @filters = map { ("checkbutton", "Filter: $_", [\&changeoption], "variable", \$gui::ACTIVECORE->{"filters"}{$_}{"active"}); } &getfilters() if(defined $ACTIVECORE); 
    my @nofilters   = ();           if($#filters==-1) { @filters = ("command", "No Filters" => undef, "disabled"); @nofilters = ("disabled"); }
    my @debugnode = ("checkbutton", "Debug IONodes" => [\&changeoption], @disabled, "checkbutton", "Debug Compute Nodes" => [\&changeoption], @disabled);
    if(defined $ACTIVECORE)
    {
        @debugnode =  ("checkbutton", "Debug IONodes" => [\&changeoption],       "variable", \$gui::ACTIVECORE->{"options"}{"debugionode"},
                       "checkbutton", "Debug Compute Nodes" => [\&changeoption], "variable", \$gui::ACTIVECORE->{"options"}{"debugcomputenode"});

    }
    FCTRL("filter", Menubutton, -text => "Filter", -side => 'left');
    FMENU("filter", [-tearoff => 0], [
                                        "command", "Create Filter from Group Selection" => sub { &mkfilter::build($ACTIVECORE, map { @{$COREMAP[$_]{"nodes"}}; } FGET("coremap")->curselection())}, @nogroup,
                                        "command", "Create Filter from Node Selection"  => sub { &mkfilter::build($ACTIVECORE, map { FGET("nodes")->get($_); }   FGET("nodes")->curselection())}, @nonode,
                                        "sep",
                                        "command", "Calculate New Filter from Group Selection" => sub { &calcfilter::build($ACTIVECORE, map { @{$COREMAP[$_]{"nodes"}}; } FGET("coremap")->curselection())}, @nogroup,
                                        "command", "Calculate New Filter from Node Selection"  => sub { &calcfilter::build($ACTIVECORE, map { FGET("nodes")->get($_); }   FGET("nodes")->curselection())}, @nonode,
                                        "sep",
                                        "command", "Delete Filter" => [\&delfilter::build, $ACTIVECORE], @nofilters, "sep",
                                        "checkbutton", "UNION selected filters" => [\&changeoption], "variable", \$gui::orfilters,
                                        "sep", @debugnode,
                                        "sep", @filters, "sep", "command", "Refilter" => sub { &changemode("dummyclass", "NOCHANGE"); }, @disabled
                                        ]);
    
    FCTRL("snapshot", Menubutton, -side => 'left', -text => "Sessions");
    FMENU("snapshot", [-tearoff => 0], [ map { $_ => [\&gui::setsnapshot, $_] } (keys %SNAPSHOTS) ]);
    
    FOUT(-side => 'left', -fill => 'x');

    FGET('menucanvas')->delete('all');
    FGET("menucanvas")->createWindow(4, 4, -anchor => 'nw', -window => $menuframe, -tags => ["menu"]);
    FSTACK(@orig);
}

sub build
{
    FINIT("Core Processor", "wheat3");
    
    FMW()->protocol("WM_DELETE_WINDOW" => [\&gui::quitcoreprocessor]);
    FIN();
    FCTRL("menucanvas", Canvas, -width => '4i', -height => 28, -relief => 'raised', -bd => 2, -fill => 'x');
    
    FIN();
    
    $ppccore = "??";
    $ppccore = "PPC"    if($::machinetype eq "BGQ");
    $ppccore = "PPC450" if($::machinetype eq "BGP");
    $ppccore = "??"     if($::machinetype eq "RR");
    
    FIN();
    FCTRL(undef, Label, -text => "Group Mode: ", -side => 'left');
    FCTRL("mode", Menubutton, -text => "** Select Grouping Mode **", -relief => 'raised', -side => 'left', -state => 'disabled');
    FPOPUP("mode", [-tearoff => 0], ["Ungrouped", "Ungrouped w/ Traceback", "sep", "Stack Traceback (condensed)", "Stack Traceback (detailed)", "Stack Traceback (survey)", "sep", "Instruction Address", "$ppccore Register", "DCR", "disabled", "sep", "Processor Status", "Kernel Status", "disabled", "Neighbor Count", "Filters"], 'gui::changemode');
    FSPACE(-width => '2c', -side => 'left');
    FCTRL("dcr", Label, -side => 'left');
    
    FCTRL("sessionlabel", Label, -text => "", -side => 'right');
    
    FOUT(-fill => 'x');
    
    FCTRL("coremap", Listbox, -width => 80, -height => 20, orient => 'xy', -fill => 'both', -selectmode => 'extended', -expand => 'yes', -exportselection => 'no', -font => 'Courier 10 bold');

    FOUTA(-side => 'left', -fill => 'both', -expand => 'yes', -side => 'left');
    FIN();
	FIN();
    FCTRL(undef, Label, -text => "Common nodes:", -side => 'left');
	FOUT(-fill => 'x');
    FCTRL("nodes", Listbox, -width => 20, -height => 20, orient => 'xy', -selectmode => 'extended', -exportselection => 'no', -font => 'Courier 10 bold', -expand => 'yes', -fill => 'both');
    FOUT(-side => 'left', -fill => 'both', -expand => 'yes');
    FOUTA(-fill => 'both');
    
    FGET("coremap")->bind('<ButtonPress>', sub { &updatenodes(); });
    FGET("nodes")->bind('<ButtonPress>', sub { &updatetext(); });
    
    FCTRL("locationlabel", Label, -text => "Location: ???", -side => 'left', frame => 1, -fill => 'x');
    FCTRL("corelabel", Label, -text => "Corefile: n/a", -side => 'left', frame => 1, -fill => 'x');
    
    FIN();
    FIN();
    FCTRL("data", Text, -width => 64, -height => 12, orient => 'xy', -fill => 'both', -expand => 'yes');
    FOUT(-fill => 'both', -expand => 'yes', -side => 'left');
    FOUT(-fill => 'both', -expand => 'yes');
    
    &mkmenu();
    
    FMW()->after(500, sub { &changemode(); });
    FMW()->after(500, sub { &attachdebugger::build() })    if(exists $::OPTIONS{"-a"});
    FMW()->after(500, sub { &loadcore::build() })          if(exists $::OPTIONS{"-c"});    
    FMW()->after(500, sub { &loadsnapshot::build() })      if(exists $::OPTIONS{"-s"});
    FMW()->after(500, sub { &attachsdbg::build() })        if(exists $::OPTIONS{"-j"});
    FMW()->after(500, sub { &attachsdbgpid::build() })     if(exists $::OPTIONS{"-pid"});
    
    MainLoop;
}

sub savedata
{
    my($val) = @_;
    if($val =~ /trace/i)
    {
	&savedialog::build($val, $lastcoremap);
    }
    elsif($val =~ /console/i)
    {
	&savedialog::build($val, $ACTIVECORE->getconsole());
    }
    elsif($val =~ /snapshot/i)
    {
	&savedialog::build($val, bglsnapshot->mkfreeze($ACTIVECORE));
    }
}

sub changemode
{
    if(($#_>0)&&($_[1] =~ /dcr/i))
    {
        if($#_+1 != 3)
        {
            setdcr::build($ACTIVECORE, "DCR");
            return;
        }
        else
        {
            SetValue("dcr", $_[2]);
        }
    }
    elsif(($#_>0)&&($_[1] =~ /PPC/i))
    {
        if($#_+1 != 3)
        {
            setdcr::build($ACTIVECORE, "PPC register");
            return;
        }
        else
        {
            SetValue("dcr", $_[2]);
        }
    }
    elsif(($#_>0)&&($_[1] =~ /NOCHANGE/))
    {
    }
    else
    {
        SetValue("dcr", "");
    }
    SetValue($_[0], $_[1]) if($#_ > 0);
    
    my @nodes = ();
    my $first = 1;
    my $filtered = 0;
    if(defined $ACTIVECORE)
    {
        foreach $filter (keys %{$ACTIVECORE->{"filters"}})
        {
            if($ACTIVECORE->{"filters"}{$filter}{"active"} == 1)
            {
                $filtered = 1;
                if($gui::orfilters == 1)
                {
                    push(@nodes, @{$ACTIVECORE->{"filters"}{$filter}{"nodes"}});
                }
                else
                {
                    if($first == 1)
                    {                    
                        push(@nodes, @{$ACTIVECORE->{"filters"}{$filter}{"nodes"}});
                        $first = 0;
                    }
                    else
                    {
                        my %nodeset = map { $_ => 1 } @nodes;
                        @nodes = ();
                        foreach $key (@{$ACTIVECORE->{"filters"}{$filter}{"nodes"}})
                        {
                            push(@nodes, $key) if(exists $nodeset{$key});
                        }
                    }
                }
            }
        }
        $ACTIVECORE->setaccessmask(1, @nodes) if($filtered == 1);
    }
    eval
    {
	@COREMAP = &utilities::calculatemap($ACTIVECORE, GetValue("mode"), GetValue("dcr"));
    };
    if($@)
    {
	&errordialog::build("Unable to gather requested data", $@);
    }
    else
    {
	&drawmap(@COREMAP);
    }
    $ACTIVECORE->setaccessmask(0) if(defined $ACTIVECORE);
    mkmenu();
}

package stoppedjobs;
use Tk;
use bagotricks;

sub build
{
    FINIT("Warning:  Halted processors");
    FCTRL(undef, Label, -text => "There are halted processors, what do you wish to do?");
    FCTRL(undef, Button, -text => "Resume Processors & Quit", -command => [\&resume]);
    FCTRL(undef, Button, -text => "Quit without resuming procesors", -command => [\&justquit]);
    FCTRL(undef, Button, -text => "Cancel Quit", -command => [\&cancel]);
}

sub resume
{
    &gui::resumeall();
    Tk::exit(0);
}

sub justquit
{
    FCLOSE();
    Tk::exit(0);
}

sub cancel
{
    FCLOSE();
}

package loadsnapshot;
use Tk;
use bagotricks;

sub build
{
    FINIT("Load Snapshot...");
    FIN();
    
    FIN();
    FCTRL(undef, Label, -text => "Session Name: ", -side => 'left');
    FCTRL("name", Entry, -width => 16, -side => 'left');
    FOUT(-fill => 'x');
    
    FIN();
    FCTRL(undef, Label, -text => "Path/Filename: ", -side => 'left');
    FCTRL("path", Entry, -width => 80, -side => 'left');
    FOUT(-fill => 'x');
    FIN();
    FCTRL(undef, Button, -text => "Load", -side => 'left', -command => [\&Load]);
    FSPACE(-width => '2c', -side => 'left');
    FCTRL(undef, Button, -text => "Close", -side => 'left', -command => [\&Close]);
    FOUT();
    FOUT();
    
    SetValue("path", $::OPTIONS{"-s"});
    SetValue("name", "Session " . ++$::SESSIONID);
    FGET("name")->focus();
}

sub Load
{
    my $path = GetValue("path");
    eval
    {
	my $core = bglsnapshot->mkthaw($path);
	&gui::addsession(GetValue("name"), $core);
	FCLOSE();
    };
    if($@)
    {
	&errordialog::build("Unable to load snapshot",  $@);
    }
}

sub Close
{
    FCLOSE();
}


package savedialog;
use Tk;
use bagotricks;

sub xlate
{
    my($char) = @_;
    return "\n" if($char =~ /\n/);
    return "#";
}

sub build
{
    my($type, $data) = @_;
    FINIT("Save $type...");
    FIN();
    
    if($type !~ /snapshot/i)
    {
	FIN();
	FCTRL(undef, Label, -text => "Preview:", frame => 1, -side => 'left', -fill => 'x');
	FCTRL("data", Text, -width => 80, -height => 15, orient => 'y', -fill => 'both', -expand => 'yes', -exportselection => 'no');
	FOUT(-fill => 'both', -expand => 'yes');
	
	($pdata = $data) =~ s/([[:^print:]])/&xlate($1)/oge;
	print "$pdata\n";
	SetValue("data", $pdata);
	FGET("data")->configure(-state => "disabled");
	
	FSPACE(-height => '.5c');
    }

    FIN();
    FCTRL(undef, Label, -text => "Path: ", -side => 'left');
    FCTRL("path", Entry, -width => 80, -side => 'left');
    FOUT();
    FIN();
    FCTRL(undef, Button, -text => "Save", -side => 'left', -command => [\&Save, $data]);
    FCTRL(undef, Button, -text => "Close", -side => 'left', -command => [\&Close]);
    FOUT();
    FOUT(-fill => 'both', -expand => 'yes');
    
    FGET("path")->focus();
}

sub Save
{
    my($data) = @_;
    my $path = GetValue("path");
    
    my $rc = open(TMP, ">$path");
    if($rc == 0)
    {
	&errordialog::build("Unable to write to file", "Unable to open file \"$path\" for writing.  Doublecheck for the proper directory and access permissions.");
    }
    else
    {
	print TMP $data;
	close(TMP);
	FCLOSE();
    }
}

sub Close
{
    FCLOSE();
}

package errordialog;
use Tk;
use bagotricks;

sub build
{
    my($title, $string) = @_;
    
    $string =~ s/\sat\s\S+\sline\s\d+.*$//;
    
    FINIT("ERROR: $title");
    FIN();
    FCTRL("desc", Text, -width => 60, -height => 5, orient => 'y', -wrap => 'word', frame => 1, -fill => 'both', -expand => 'yes');
    FCTRL(undef, Button, -text => "Close", -command => sub { FCLOSE(); });
    FOUT(-fill => 'both', -expand => 'yes');
    SetValue("desc", $string);
}

package setdcr;
use Tk;
use bagotricks;

sub build
{
    my ($CORE, $type) = @_;
    my @dcrlist = ();
    
    print "type: $type\n";
    if($type =~ /dcr/i)
    {
	@dcrlist = $CORE->getnameddcrs();
    }
    else
    {
	@dcrlist = $CORE->getnamedregisters();
    }
    
    FINIT("Set $type");
    FIN();
    FIN();
    FCTRL("registers", Listbox, -width => 20, -height => 20, orient => 'xy', -fill => 'both', -selectmode => 'extended', -expand => 'yes', -exportselection => 'no', -font => 'Courier 10 bold');
    FOUT();
    
    FIN();
    FCTRL(undef, Button, -text => "Set", -command => [\&set, $type], -side => 'left');
    FCTRL(undef, Button, -text => "Cancel", -command => [\&cancel], -side => 'left');
    FOUT();
    FOUT();
    FGET("registers")->insert(0, @dcrlist);
}

sub set
{
    my($type) = @_;
    
    my $value = GetValue("registers");
    print "value: $value\n";
    gui::changemode("mode", $type, $value);
    FCLOSE();
}

sub cancel
{
    FCLOSE();
}

package attachdebugger;
use Tk;
use bagotricks;

sub build
{
    FINIT("Attach to JTAG debugger");
    FIN();
    FIN();
    FCTRL(undef, Label, -text => "Session Name: ", -side => 'left');
    FCTRL("name", Entry, -width => 20, -side => 'left');
    FSPACE(-width => '2c');
    FCTRL(undef, Label, -text => "Block: ", -side => 'left');
    FCTRL("block", Entry, -width => 25, -side => 'left');
    FOUT();
    
    FIN();
    FCTRL(undef, Label, -text => "ELF Image(s): ", -side => 'left');
    FCTRL("cnkbin", Entry, -width => 80, -side => 'left');
    FOUT();
    FIN();
    FCTRL(undef, Label, -text => "User: ", -side => 'left');
    FCTRL("user", Entry, -width => 10,-side => 'left');
#    FCTRL(undef, Label, -text => "Host: ", -side => 'left');
#    FCTRL("host", Entry, -width => 40,-side => 'left');
#    FCTRL(undef, Label, -text => "   Port: ", -side => 'left');
#    FCTRL("port", Entry, -width => 6,-side => 'left');
    FOUT();
    
    FIN();
    FCTRL(undef, Button, -text => "Attach", -command => [\&Attach], -side => 'left');
    FCTRL(undef, Button, -text => "Close", -command => sub { FCLOSE(); }, -side => 'left');
    FOUT();
    FOUT();

    $::OPTIONS{"-b"}="remote:localhost:1200" if(($^O =~ /^win/i)&&(!exists $::OPTIONS{"-b"}));
    $::OPTIONS{"-user"} = $ENV{"USER"} if(!exists $::OPTIONS{"-user"});
#    $::OPTIONS{"-host"} = "localhost" if(!exists $::OPTIONS{"-host"});
#    $::OPTIONS{"-port"} = "32031"     if(!exists $::OPTIONS{"-port"});
    
    SetValue("name", "Session " . ++$::SESSIONID);
    SetValue("cnkbin", $::OPTIONS{"-b"});
#    SetValue("host", $::OPTIONS{"-host"});
#    SetValue("port", $::OPTIONS{"-port"});
    SetValue("user", $::OPTIONS{"-user"});
    SetValue("block", $::OPTIONS{"-a"});
    FGET("name")->focus();
}
sub Attach
{
#    $opt{"-host"} = GetValue("host");
#    $opt{"-port"} = GetValue("port");
    $opt{"-a"} = GetValue("block");
    $opt{"-user"} = GetValue("user");
    $opt{"-numbadframes"} = $::OPTIONS{"-numbadframes"} if(exists $::OPTIONS{"-numbadframes"});
    
    eval
    {
	my $core = bgqjtagreader->new(\%opt);
	$core->{"cnkbin"} = objdump->new(GetValue("cnkbin"));
	$core->{"options"}{"-b"} = GetValue("cnkbin");
	
	&gui::addsession(GetValue("name"), $core);
	FCLOSE();
    };
    
    if($@)
    {
	&errordialog::build("Unable to attach to block", $@);
    }
}

package attachsdbg;
use Tk;
use bagotricks;

sub build
{
    FINIT("Attach to Scalable Debugger");
    FIN();
    FIN();
    FCTRL(undef, Label, -text => "Session Name: ", -side => 'left');
    FCTRL("name", Entry, -width => 20, -side => 'left');
    FSPACE(-width => '2c');
    FCTRL(undef, Label, -text => "JobID: ", -side => 'left');
    FCTRL("jobid", Entry, -width => 25, -side => 'left');
    FOUT();
    FIN();
    FCTRL(undef, Label, -text => "ELF Image(s): ", -side => 'left');
    FCTRL("cnkbin", Entry, -width => 80, -side => 'left');
    FOUT();
    
    FIN();
    FCTRL(undef, Button, -text => "Attach", -command => [\&Attach], -side => 'left');
    FCTRL(undef, Button, -text => "Close", -command => sub { FCLOSE(); }, -side => 'left');
    FOUT();
    FOUT();

    $::OPTIONS{"-b"}="remote:localhost:1200" if(($^O =~ /^win/i)&&(!exists $::OPTIONS{"-b"}));
    $::OPTIONS{"-lb"}="remote:localhost:1201" if(($^O =~ /^win/i)&&(!exists $::OPTIONS{"-lb"}));
    
    SetValue("name", "Session " . ++$::SESSIONID);
    SetValue("cnkbin", $::OPTIONS{"-b"});
    SetValue("jobid", $::OPTIONS{"-j"});
    
    if(($::OPTIONS{"-b"} !~ /\S/) && ($::OPTIONS{"-j"} =~ /\d/))
    {
	open(TMP, "/bgsys/drivers/ppcfloor/bin/list_jobs |");
	while($line = <TMP>)
	{
	    ($jobid,$executable) = $line =~ /^\s*(\d+)\s+\S+\s+(\S+)/;
	    if($jobid == $::OPTIONS{"-j"})
	    {
		SetValue("cnkbin", $executable);
	    }
	}
	close(TMP);
    }
    
    FGET("name")->focus();
}
sub Attach
{
    $opt{"-j"} = GetValue("jobid");
    $opt{"-numbadframes"} = $::OPTIONS{"-numbadframes"} if(exists $::OPTIONS{"-numbadframes"});
    
    eval
    {
	my $core = sdbgreader->new(\%opt);
	$core->{"cnkbin"} = objdump->new(GetValue("cnkbin"));
	$core->{"options"}{"-b"} = GetValue("cnkbin");
	
	&gui::addsession(GetValue("name"), $core);
	FCLOSE();
    };
    
    if($@)
    {
	&errordialog::build("Unable to attach to block", $@);
    }
}

package attachsdbgpid;
use Tk;
use bagotricks;

sub build
{
    FINIT("Attach to Scalable Debugger");
    FIN();
    FIN();
    FCTRL(undef, Label, -text => "Session Name: ", -side => 'left');
    FCTRL("name", Entry, -width => 20, -side => 'left');
    FSPACE(-width => '2c');
    FCTRL(undef, Label, -text => "Runjob PID: ", -side => 'left');
    FCTRL("pidid", Entry, -width => 25, -side => 'left');
    FOUT();
    FIN();
    FCTRL(undef, Label, -text => "ELF Image(s): ", -side => 'left');
    FCTRL("cnkbin", Entry, -width => 80, -side => 'left');
    FOUT();
    
    FIN();
    FCTRL(undef, Button, -text => "Attach", -command => [\&Attach], -side => 'left');
    FCTRL(undef, Button, -text => "Close", -command => sub { FCLOSE(); }, -side => 'left');
    FOUT();
    FOUT();

    $::OPTIONS{"-b"}="remote:localhost:1200" if(($^O =~ /^win/i)&&(!exists $::OPTIONS{"-b"}));
    $::OPTIONS{"-lb"}="remote:localhost:1201" if(($^O =~ /^win/i)&&(!exists $::OPTIONS{"-lb"}));
    
    SetValue("name", "Session " . ++$::SESSIONID);
    SetValue("cnkbin", $::OPTIONS{"-b"});
    SetValue("pidid", $::OPTIONS{"-pid"});
    
    FGET("name")->focus();
}
sub Attach
{
    $opt{"-pid"} = GetValue("pidid");
    $opt{"-numbadframes"} = $::OPTIONS{"-numbadframes"} if(exists $::OPTIONS{"-numbadframes"});
    
    eval
    {
	my $core = sdbgreader->new(\%opt);
	$core->{"cnkbin"} = objdump->new(GetValue("cnkbin"));
	$core->{"options"}{"-b"} = GetValue("cnkbin");
	
	&gui::addsession(GetValue("name"), $core);
	FCLOSE();
    };
    
    if($@)
    {
	&errordialog::build("Unable to attach to block", $@);
    }
}

package loadcore;
use Tk;
use bagotricks;

sub build
{
    FINIT("Load Core Files");
    FIN();

    FIN();
    FCTRL(undef, Label, -text => "Session Name: ", -side => 'left');
    FCTRL("name", Entry, -width => 20, -side => 'left');
    FOUT();

    FIN();
    FCTRL(undef, Label, -text => "Path to Cores: ", -side => 'left');
    FCTRL("path", Entry, -width => 60, -side => 'left');
    FSPACE(-width => '1c');
    FCTRL(undef, Label, -text => "Node Range: ", -side => 'left');
    FCTRL("min", Entry, -width => 6, -side => 'left');
    FCTRL(undef, Label, -text => "-", -side => 'left');
    FCTRL("max", Entry, -width => 6, -side => 'left');
    FOUT();
    
    FIN();
    FCTRL(undef, Label, -text => "ELF Image(s): ", -side => 'left');
    FCTRL("cnkbin", Entry, -width => 80, -side => 'left');
    FOUT();


    FIN();
    FCTRL(undef, Button, -text => "Load Cores", -command => [\&Attach], -side => 'left');
    FCTRL(undef, Button, -text => "Close", -command => sub { FCLOSE(); }, -side => 'left');
    FOUT();

    FOUT();
    
    $::OPTIONS{"-b"}="remote:localhost:1200" if(($^O =~ /^win/i)&&(!exists $::OPTIONS{"-b"}));
    $::OPTIONS{"-mincore"} = 0 if(!exists $::OPTIONS{"-mincore"});
    $::OPTIONS{"-maxcore"} = 64*1024*2 if(!exists $::OPTIONS{"-maxcore"});
    
    SetValue("name", "Session " . ++$::SESSIONID);
    SetValue("cnkbin", $::OPTIONS{"-b"});
    SetValue("path", $::OPTIONS{"-c"});
    SetValue("min", $::OPTIONS{"-mincore"});
    SetValue("max", $::OPTIONS{"-maxcore"});
    FGET("name")->focus();
}

sub Attach
{
    $opt{"-c"} = GetValue("path");
    $opt{"-mincore"} = GetValue("min");
    $opt{"-maxcore"} = GetValue("max");
    
    eval
    {
	my $core;
	if(($::machinetype eq "BGP") || ($::machinetype eq "BGQ"))
	{
	    $core = bgpcorereader->new(\%opt);
	}
	elsif($::machinetype eq "RR")
	{
	    $core = rrcorereader->new(\%opt);
	}
	else
	{
	    $core = bglcorereader->new(\%opt);
	}
	
	$core->refresh();
	$core->{"cnkbin"} = objdump->new(GetValue("cnkbin")) if(! exists $core->{"cnkbin"});
	$core->{"options"}{"-b"} = GetValue("cnkbin");   
	
	&gui::addsession(GetValue("name"), $core);
	FCLOSE();
    };
    if($@)
    {
	&errordialog::build("Unable to load core files", $@);
    }
}

package loadsymbols;
use Tk;
use bagotricks;

sub build
{
    my($core) = @_;
    FINIT("Load Symbol Table");
    FIN();
    
    FIN();
    FCTRL(undef, Label, -text => "ELF Image(s): ", -side => 'left');
    FCTRL("cnkbin", Entry, -width => 80, -side => 'left');
    FOUT();

    FIN();
    FCTRL(undef, Button, -text => "Load Symbol Table", -command => [\&Attach, $core], -side => 'left');
    FCTRL(undef, Button, -text => "Close", -command => sub { FCLOSE(); }, -side => 'left');
    FOUT();

    FOUT();
    
    $::OPTIONS{"-b"}="remote:localhost:1200" if(($^O =~ /^win/i)&&(!exists $::OPTIONS{"-b"}));
    
    SetValue("cnkbin", $::OPTIONS{"-b"});
}

sub Attach
{
    my($core) = @_;
	
    eval
    {
	$core->{"cnkbin"} = objdump->new(GetValue("cnkbin"));
	$core->{"options"}{"-b"} = GetValue("cnkbin");   
	FCLOSE();

	&gui::changemode("dummyclass", "NOCHANGE");
    };
    if($@)
    {
	&errordialog::build("Unable to load symbols",  $@);
    }    
}

package mkfilter;
use bagotricks;
use Tk;

sub build
{
    my($core, @nodes) = @_;
    FINIT("Create/Replace Filter");
    FIN();
    FCTRL(undef, Label, -text => "Filter Name: ", -side => 'left');
    FCTRL("name", Entry, -width => 16, -side => 'left');
    FOUT();
    FIN();
    FCTRL(undef, Button, -text => "Create", -side => 'left', -command => sub { gui::setfilter($core, GetValue("name"), @nodes); FCLOSE(); });
    FCTRL(undef, Button, -text => "Close", -side => 'left', -command => sub { FCLOSE(); });
    FOUT();
    SetValue("name", "New Filter");
    FGET("name")->focus();
}

package delfilter;
use bagotricks;
use Tk;

sub build
{
    my($core) = @_;
    FINIT("Delete Filter");
    FIN();
    FIN();
    FCTRL("filters", Listbox, -width => 32, -height => 16);
    FOUT();
    FIN();
    FCTRL(undef, Button, -text => "Delete", -side => 'left', -command => sub { delete $core->{"filters"}{GetValue("filters")}; gui::mkmenu(); FCLOSE(); });
    FCTRL(undef, Button, -text => "Close", -side => 'left', -command => sub { FCLOSE(); });
    FOUT();
    FOUT();
    FGET("filters")->insert(0, &gui::getfilters());
}

package gdb;
use bagotricks;
use Tk;

sub build
{
    my($core) = @_;
    my $node = GetValue("gui::nodes");    
    my $executable = "";    
    $executable = $core->{"cnkbin"}->findexecutable($core->{$node}{"coreregs"}{"IAR"});
    
    FINIT("Start GDB");
    FIN();
    
    FIN();
    FCTRL(undef, Label, -text => "Executable: ", -side => 'left');
    FCTRL("executable", Entry, -width => 80, -side => 'left');
    FOUT();
    
    FIN();
    FCTRL(undef, Label, -text => "GDB Server Port: ", -side => 'left');
    FCTRL("port", Entry, -width => 40, -side => 'left');
    FOUT();
    
    FIN();
    FCTRL(undef, Label, -text => "JTAG GDB Server: ", -side => 'left');
    FCTRL("gdbserver", Entry, -width => 40, -side => 'left');
    FOUT();
    
    FIN();
    FCTRL(undef, Label, -text => "MCServer Address: ", -side => 'left');
    FCTRL("mcserver", Entry, -width => 40, -side => 'left');
    FOUT();
    
    FIN();
    FCTRL(undef, Button, -text => "Start GDB", -side => 'left', -command => [\&startgdb, $core, $node]);
    FCTRL(undef, Button, -text => "Close", -side => 'left', -command => sub {FCLOSE();});
    FOUT();
    
    FOUT();
    
    SetValue("executable", $executable);
    SetValue("mcserver", "localhost:1206");
    SetValue("port", "localhost:8001");
    SetValue("gdbserver", "/bgsys/drivers/ppcfloor/bin/gdbserver-bgp");
}

sub startgdb
{
    my($core, $node) = @_;
    $nodelocation = $core->{$node}{"personality"}{"location"};
    $mcserver = GetValue("mcserver");
    $gdbserveripport = GetValue("port");    
    $executable = GetValue("executable");    
    $gdbserver = GetValue("gdbserver");
    
    return -1 if(!-e $gdbserver);
    
    $pid = fork();
    if($pid == 0)
    {
	exec("$gdbserver $gdbserveripport $mcserver $nodelocation");
	print "exec failed?!?\n";
	exit(0);
    }
    sleep(1);
    
    $gdbpid = fork();
    if($gdbpid == 0)
    {
	my $gdbcommand;
#       $gdbcommand = "$floor/gnu-linux/bin/gdb -l 9999 \\\"--eval-command=target remote $gdbserveripport\\\" $executable";
	$gdbcommand = "gdb -l 9999 \\\"--eval-command=target remote $gdbserveripport\\\" $executable";
	
	print "GDB Command: $gdbcommand\n";
	exec("xterm -T \"gdb to $nodelocation on port $gdbserveripport\" -e \"$gdbcommand;kill $pid\"");
	print "exec failed?!?\n";
	exit(0);
    }
    FCLOSE();
}

package calcfilter;
use bagotricks;
use Tk;

sub build
{
    my($core, @nodes) = @_;
    FINIT("Calculate Filter");
    FIN();
    
    FIN();
    FCTRL(undef, Label, -text => "Filter Name: ", -side => 'left');
    FCTRL("name", Entry, -width => 16, -side => 'left');
    FOUT(-fill => 'x');
    FIN();
    FIN();
    FCTRL("xm", Checkbutton, -text => "Nodes in X- are neighbors", frame => 1, -side => 'left', -fill => 'x');
    FCTRL("ym", Checkbutton, -text => "Nodes in Y- are neighbors", frame => 1, -side => 'left', -fill => 'x');
    FCTRL("zm", Checkbutton, -text => "Nodes in Z- are neighbors", frame => 1, -side => 'left', -fill => 'x');
    FOUT(-side => 'left');
    FIN();
    FCTRL("xp", Checkbutton, -text => "Nodes in X+ are neighbors", frame => 1, -side => 'left', -fill => 'x');
    FCTRL("yp", Checkbutton, -text => "Nodes in Y+ are neighbors", frame => 1, -side => 'left', -fill => 'x');
    FCTRL("zp", Checkbutton, -text => "Nodes in Z+ are neighbors", frame => 1, -side => 'left', -fill => 'x');
    FOUT(-side => 'left');    
    FOUT(-fill => 'x');
    
    FIN();
    FCTRL("t", Checkbutton, -text  => "Nodes in T are neighbors",  frame => 1, -side => 'left', -fill => 'x');
    FCTRL("inset", Checkbutton, -text => "Include nodes contained in selection", frame => 1, -side => 'left', -fill => 'x');
    FCTRL("outset",Checkbutton, -text => "Include neighboring nodes that are outside existing selection", frame => 1, -side => 'left', -fill => 'x');
    FIN();
    FCTRL(undef, Label, -text => "Only include node if it has at least ", -side => 'left');
    FCTRL("minneigh", Entry, -width => 5, -side => 'left');
    FCTRL(undef, Label, -text => "neighbors", -side => 'left');
    FOUT();
    FIN();
    FCTRL(undef, Label, -text => "Only include node if it has at most ", -side => 'left');
    FCTRL("maxneigh", Entry, -width => 5, -side => 'left');
    FCTRL(undef, Label, -text => "neighbors", -side => 'left');
    FOUT();
    
    FIN();
    FCTRL(undef, Label, -text => "Location regex Qualifier (eg, Rxx-Mx-Nx):", -side => 'left');
    FCTRL("regex", Entry, -width => 20, -side => 'left');
    FOUT(-fill => 'x');
    FCTRL("core0", Checkbutton, -text => "Exclude node if it is on core 0", frame => 1, -side => 'left', -fill => 'x');
    FCTRL("core1", Checkbutton, -text => "Exclude node if it is on core 1", frame => 1, -side => 'left', -fill => 'x');
    if($::machinetype eq "BGP")
    {
	FCTRL("core2", Checkbutton, -text => "Exclude node if it is on core 2", frame => 1, -side => 'left', -fill => 'x');
	FCTRL("core3", Checkbutton, -text => "Exclude node if it is on core 3", frame => 1, -side => 'left', -fill => 'x');
    }
    FCTRL("inverse", Checkbutton, -text => "After calculation, invert results", frame => 1, -side => 'left', -fill => 'x');
    FOUT(-fill => 'x');
    
    FIN();
    FCTRL(undef, Button, -text => "Create", -side => 'left', -command => [\&create, $core, @nodes]);
    FCTRL(undef, Button, -text => "Close", -side => 'left', -command => sub { FCLOSE(); });
    FOUT();
    
    FOUT();
    
    SetValue("minneigh", 0);
    SetValue("maxneigh", 7);
    SetValue("core0", 0);
    SetValue("core1", 0);
    SetValue("core2", 0);
    SetValue("core3", 0);
    SetValue("inset", 1);
    SetValue("name", "Last Calculation");
    SetValue("xp", 1);
    SetValue("xm", 1);
    SetValue("yp", 1);
    SetValue("ym", 1);
    SetValue("zp", 1);
    SetValue("zm", 1);
    FGET("name")->focus();
}

sub create
{
    my($core, @nodes) = @_;
    my @calcnodes = ();
    my @filter = ();
    my $locfilt = GetValue("regex");
    
    push(@filter, "X+") if(GetValue("xp"));
    push(@filter, "X-") if(GetValue("xm"));
    push(@filter, "Y+") if(GetValue("yp"));
    push(@filter, "Y-") if(GetValue("ym"));
    push(@filter, "Z+") if(GetValue("zp"));
    push(@filter, "Z-") if(GetValue("zm"));
    push(@filter, "T+") if(GetValue("t"));
    push(@filter, "SET") if(GetValue("inset"));

    my (%hash) = &utilities::FindCommonNeighbors($core, join(", ", @filter), @nodes);
    foreach $node (keys %hash)
    {
        $proc = $core->{$node}{"mmcsprocid"};
        if(!GetValue("core$proc"))
        {
            if(($hash{$node}{"neighcount"} >= GetValue("minneigh"))&&
                ($hash{$node}{"neighcount"} <= GetValue("maxneigh")))
            {
                if($hash{$node}{"inset"} || GetValue("outset"))
                {
                    if(($locfilt !~ /\S+/)||($core->{$node}{"personality"}{"location"} =~ /$locfilt/))
                    {
                        push(@calcnodes, $node);
                    }
                }
            }
        }
    }
    if(GetValue("inverse"))
    {
        %calchash = map { $_, 1 } @calcnodes;
        @calcnodes = ();
        foreach $node ($core->getnodenames(0))
        {
            push(@calcnodes, $node) if(!exists $calchash{$node});
        }
    }
    &gui::setfilter($core, GetValue("name"), @calcnodes);
    FCLOSE();
}

1;
