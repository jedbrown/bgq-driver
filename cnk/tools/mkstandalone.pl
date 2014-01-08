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
# (C) Copyright IBM Corp.  2010, 2012                              
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
use Getopt::Long;
use Cwd;

BEGIN
{
    print "$0 @ARGV\n";
    $fn = $0;
    my $startdir = `pwd`;
    chomp($startdir);
    while(($fn =~ /\//)||(-l $fn))
    {
        if($fn =~ /\//)
        {
            ($dir,$fn) = $fn =~ /(\S+)\/(\S+)/;
            chdir($dir);
        }
        if(-l $fn)
        {
            $fn = readlink($fn) if(-l $fn);
        }
    }
    $ENV{CODEPATH} = getcwd;
    unshift(@INC, $ENV{CODEPATH});
    chdir($startdir);
}

configure();
readStructure("$DRIVER/cnk/include/AppState.h", "AppState_t", \@APPSTATE);
readStructure("$DRIVER/cnk/include/AppState.h", "AppSeg_t", \@APPSEG);

initializeELF();

detectDynamic();
populateJobInfo();
populateEnv();
populateProcessConfig();
buildELF();

sub configure
{
    GetOptions("driver=s"    => \$DRIVER,
	       "gcc=s"       => \$GCC,
	       "objdump=s"   => \$OBJDUMP,

	       "exe=s"       => \$PROGRAM,
	       "agentlist=s" => \$AGENTPROGRAM,
	       "agentheap=i" => \$AGENTHEAP,
	       
	       "poffset=i"   => \$POFFSET,

	       "o=s"         => \$OUTPUTELF,

	       "debug!"      => \$DEBUGMODE,

	       "argfile=s"   => \$ARGFILE,
	       "envfile=s"   => \$ENVFILE,
	       "envall!"     => \$ENVALL,
	       "memsize=i"   => \$MEMSIZE,
	       "verbose!"    => \$VERBOSE,
	       "tlbmap!"     => \$EXCLUDE_TLBMAP,
	);
    
    $DEBUGMODE = 0 if(!defined $DEBUGMODE);
    $DRIVER    = "/auto/BGQ/releases/bgqdev/floor-x86_64" if(!defined $DRIVER);
    $GCC       = "$DRIVER/gnu-linux/bin/powerpc64-bgq-linux-gcc" if(!defined $GCC);
    $OBJDUMP   = "$DRIVER/gnu-linux/bin/powerpc64-bgq-linux-objdump" if(!defined $OBJDUMP);
    $PROGRAM   = "./a.out" if(!defined $PROGRAM);
    $OUTPUTELF = "./standaloneloadinfo.elf" if(!defined $OUTPUTELF);
    if(!defined $MEMSIZE)
    {
	if(exists $ENV{"BG_MEMSIZE"})
	{
	    $MEMSIZE   = $ENV{"BG_MEMSIZE"};
	}
	else
	{
	    $MEMSIZE   = 16384;
	}
    }
    $POFFSET   = 0 if(!defined $POFFSET);
    $NUMPROC   = 1;
    $SHMSIZE   = 0;
    $EXCLUDE_TLBMAP = 1 if($ENV{"BG_FORCE_TLBGENERATE"});
}

sub detectDynamic
{
    my $output = `$OBJDUMP -x $PROGRAM`;
    $DYNAMIC = 0;
    $DYNAMIC = 1 if($output =~ /Dynamic Section:/);
    print "dyanmic: $DYNAMIC\n";
}

sub generateBinaryBlob
{
    my($filename) = @_;
    my $str;
    open(BINARY, $filename);
    binmode(BINARY);
    while (read(BINARY, $buff, 4)) 
    {
	$str .= sprintf("0x%08x, ", unpack("N", $buff));
	if(++$newl > 15)
	{
	    $newl = 0;
	    $str .= "\n";
	}
    }
    close(BINARY);
    return $str;
}

sub buildELF
{
    my $numagents = 0;
    open(TMP, ">$OUTPUTELF.c");
    print TMP "#include <cnk/include/AppState.h>\n";
    
    foreach $prog ($PROGRAM, split(",", $AGENTPROGRAM))
    {
	populateArgs($prog);
	populateEntryPt($prog) if($DYNAMIC == 0);
	populateSegments($prog) if($DYNAMIC == 0);
	@out = flatten(\%elf, \@APPSTATE);
	$str = join(",", @out);
	print TMP "AppState_t StandaloneAppState$numagents   = { $str  };\n";
	$numagents++;
    }
    
    if(($EXCLUDE_TLBMAP == 0) && ($DYNAMIC == 0))
    {
	my $phdrs = `$OBJDUMP -p $PROGRAM`;
	($textsize) = $phdrs =~ /memsz (\S+) flags r-x/;
	($textsize) = hex($textsize);
	($textsize) += 4096; # space for kernel_info structure
	($datasize) = $phdrs =~ /memsz (\S+) flags rw-/;
	($datasize) = hex($datasize);
	
	my $kernhdrs = `$OBJDUMP -p $DRIVER/cnk/bin/bgq_kernel.elf`;
	($kernstart, $kernsize) = $kernhdrs =~ /.*vaddr (\S+).*memsz (\S+) flags rw-/s;
	$kernsize = hex($kernstart) + hex($kernsize);
	
	my $agentsizes = "";
	my $x = 0;
	foreach $prog (split(",", $AGENTPROGRAM))
	{
	    print "Calculating agent sizes\n";
	    my $output = `$OBJDUMP -p $prog`;
	    ($agentbase) = $output =~ /LOAD off\s+0x0000000000000000\s+vaddr\s+(\S+)/;
	    ($agenttextsize) = $output =~ /memsz (\S+) flags r-x/;
	    ($agenttextsize) = hex($agenttextsize);
	    ($agenttextsize) += 4096; # space for kernel_info structure
	    ($agentdatasize) = $output =~ /memsz (\S+) flags rw-/;
	    ($agentdatasize) = hex($agentdatasize);
	    $agentsizes .= "-agent$x"."text=$agenttextsize -agent$x"."data=$agentdatasize -agent$x"."heap=$AGENTHEAP -agent$x"."base=$agentbase ";
	    $x++;
	}
	
	cmd("$DRIVER/cnk/tools/staticmapper -kernelsize=$kernsize -text=$textsize -data=$datasize -shared=$SHMSIZE -numprocesses=$NUMPROC -generate=staticmap.bin -memsize=$MEMSIZE -poffset=$POFFSET $agentsizes");
	$str = generateBinaryBlob("staticmap.bin");
	
	print TMP "unsigned char StandalonePad1[ CONFIG_STANDALONE_TLBMAPPER - CONFIG_STANDALONE_APPSTATE - $numagents * sizeof(AppState_t) ] = { 1, };\n";
	print TMP "unsigned int StandaloneTLBMapper[] = { \n$str\n };\n";
    }
    
    print TMP "void func() { }\n";
    close(TMP);
    
    cmd("cat $OUTPUTELF.c") if($VERBOSE);
    
    my $extra = "";
    if($MEMSIZE <= 2048)
    {
	print "Detected low-memory configuration.  Using L2 standalone linker map\n";
	$extra = "_l2";
    }
    cmd("$GCC -g -O2 -m64 -o $OUTPUTELF $OUTPUTELF.c -I$DRIVER -nostdlib -Wl,--script $ENV{CODEPATH}/standaloneldrinfo$extra.lds");
    unlink("$OUTPUTELF.c") if(!$DEBUGMODE);
    unlink("staticmap.bin") if(!$DEBUGMODE);
}

sub cmd
{
    my($c) = @_;
    print "cmd:  $c\n" if($VERBOSE);
    system($c);
}

sub initializeELF
{
    %elf = ();
    $elf{"MAGIC"} = "APP_STANDALONE";
    $elf{"App_NumSegs"}  = 0;
    $elf{"App_Segments"} = "{ 0,0,0,0,0 }";
}

sub populateJobInfo
{
    my $uid = `id -u`;
    my $grp = `id -g`;
    chomp($uid);
    chomp($grp);
    $elf{"UserID"}  = $uid;
    $elf{"GroupID"} = $grp;
    $elf{"JobID"}   = $$;
    
    $elf{"PreloadedApplication"} = 1 - $DYNAMIC;
    if(exists $ENV{"BG_PRELOADEDAPP"})
    {
	$elf{"PreloadedApplication"} = $ENV{"BG_PRELOADEDAPP"};
    }
}

sub populateArgs
{
    my($prog) = @_;
    my $argdata;
    my @lens = ();
    $prog = "./$prog"     if($prog !~ /^\//);
    $argdata = `cat $ARGFILE` if(-e $ARGFILE);
    my @args = ($prog, split("\n", $argdata));
    $elf{"App_Argc"} = $#args+1;
    
    my $CONFIG_STANDALONE_APPSTATE = 0x0F00000;
    my $len = $CONFIG_STANDALONE_APPSTATE + 8;
    foreach $arg (@args)
    {
	push(@lens, "(char*)$len");
	$len += length($arg) + 1;
    }
    foreach $arg (@args)
    {
	print "arg: $arg\n" if($VERBOSE);
    }
    $elf{"App_Argv"} = '{' . join(", ", @lens) . '}';
    $elf{"App_Args"} = '"' . join("\0", @args) . "\0" . '"';
}

sub populateEnv
{
    my $envdata;
    
    $envdata = "";
    $envdata = `cat $ENVFILE` if(-e $ENVFILE);
    if($ENVALL)
    {
	$envdata =~ s/\n/ /g;
	$envdata = `env $envdata`
    }
    $envdata =~ s/\S+\"\S+\n//sog;
    my @envs = split("\n", $envdata);
    
    foreach $env (@envs)
    {
	if($env =~ /BG_PROCESSESPERNODE/)
	{
	    ($NUMPROC) = $env =~ /BG_PROCESSESPERNODE=(\d+)/;
	    $NUMPROC = 1 if($NUMPROC == 0);
	}
	if($env =~ /BG_MEMSIZE/)
	{
	    ($MEMSIZE) = $env =~ /BG_MEMSIZE=(\d+)/;
	}
	if($env =~ /BG_SHAREDMEMSIZE/)
	{
	    ($SHMSIZE) = $env =~ /BG_SHAREDMEMSIZE=(\d+)/;
	    $SHMSIZE = 0 if($SHMSIZE == 0);
	    $SHMSIZE = $SHMSIZE * 1024 * 1024;
	}
	print "env: $env\n" if($VERBOSE);
    }
    $elf{"App_Env"} = '"' . join("\0", @envs) . '"';
}

sub populateProcessConfig
{
    $elf{"Active_Processes"} = 0;
    $elf{"ActiveProcessesInJob"} = 0;
    $elf{"Timeshift"} = time() . "000000LL";
    
    $elf{"NodeName"} = "\"bgqnode\"";
    $elf{"PersistMemIndex"} = 0;
    $elf{"JobStartTime"};
    $elf{"outOfMemDuringLoad"} = 0;
    $elf{"ProcStateIndex"} = '{' . 0 . '}';
    $elf{"SecondaryGroups"} = '{' . 0 . '}';
}

sub populateEntryPt
{
    my($exe) = @_;
    my $output = `$OBJDUMP -f $exe`;
    ($entrypt) = $output =~ /start address\s*(\S+)/;
    $elf{"App_EntryPoint"} = $entrypt . "LL";
}

sub populateSegments
{
    my($exe) = @_;
    my @lines = ();
    my $line;
    my @segs = ();
    my $output = `$OBJDUMP -p $exe`;
    @lines = split("\n", $output);
    while($#lines > 0)
    {
	$line = shift @lines;
	if(($line =~ /LOAD/) || ($line =~ /TLS/))
	{
	    my $type, $vaddr, $paddr, $filesz, $memsz, $permit;
	    ($type, $vaddr, $paddr) = $line =~ /^\s*(\S+)\s.*vaddr\s*(\S+)\s+paddr\s*(\S+)/;
	    $line = shift @lines;
	    ($filesz, $memsz, $permit) = $line =~ /filesz\s*(\S+)\s*memsz\s*(\S+)\s*flags\s*(\S+)/;
	    @flags = ();
	    push(@flags, "APP_FLAGS_LOAD") if($type =~ /LOAD/);
	    push(@flags, "APP_FLAGS_TLS") if($type =~ /TLS/);
	    push(@flags, "APP_FLAGS_R") if($permit =~ /r/i);
	    push(@flags, "APP_FLAGS_W") if($permit =~ /w/i);
	    push(@flags, "APP_FLAGS_X") if($permit =~ /x/i);
	    
	    my %seg = ();
	    $seg{"seg_va"} = "$vaddr" . "LL";
	    $seg{"seg_pa"} = "$paddr" . "LL";
	    $seg{"seg_filesz"} = "$filesz" . "LL";
	    $seg{"seg_memsz"} = "$memsz" . "LL";
	    $seg{"seg_csum"} = 0;
	    $seg{"seg_flags"} = join(" | ", @flags);
	    
	    push(@segs, "{" . join(",", &flatten(\%seg, \@APPSEG)) . "}");
	}
    }
    $elf{"App_NumSegs"}  = $#segs + 1;
    $elf{"App_Segments"} = "{" . join(",", @segs) . "}";
}

sub flatten
{
    my($hash, $order) = @_;
    my @value = ();
    foreach $elem (@{$order})
    {
	if(exists $hash->{"$elem"})
	{
	    push(@value, $hash->{$elem});
	}
	else
	{
	    push(@value, 0);
	}
    }
    return @value;
}

sub readStructure
{
    my($header, $structname, $variable) = @_;
    open(TMP, $header);
    while($line = <TMP>)
    {
	if($line =~ /$structname/)
	{
	    while($line = <TMP>)
	    {
		last if($line =~ /}/);
		$line =~ s/\/\/.*//;
		if($line =~ /;/)
		{
		    ($type, $var) = $line =~ /\s*(\S+)\s+(.*);/;
		    
		    $structure = 0;
		    if($var =~ /\[/)
		    {
			$structure = 1;
			$var =~ s/\[.*//;
		    }
		    $var =~ s/\*//g;
		    $var =~ s/\&//g;
		    push(@{$variable}, $var);
		    $variable{$var} = $type;
		    $struct{$var} = $structure;
		    
		}
	    }
	    last;
	}
    }
    close(TMP);
}
