#!/dfs/eng/bin/perl
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
#-*- mode: perl;-*-

package bagotricks;

use vars qw($VERSION @ISA @EXPORT @EXPORT_OK);

require Exporter;
require AutoLoader;

@ISA = qw(Exporter AutoLoader);
@EXPORT = qw(
	     FINIT
	     FISOPEN
	     FMW
	     FCLOSE
	     
	     FCTRL
	     FSPACE
	     FPOPUP
	     FMENU
	     FADD
	     FCUSTOM
	     FMATCH
	     
	     InitValue
	     HasChanged

	     FIN
	     FOUT
	     FOUTA
	     FCUR
	     FGET
	     FDEPTH
	     FSTACK

	     DoAllEvents
	     Busy
	     
	     GetValue
	     SetValue

             Resolve
	     GetPackage
	     GetPackages
	     GetControls
	     GetRealName
	     GetData
	     SetAlias
	     SetAliasArray
	     ClearSetAlias
	     
	     SelectWindow
	     
	     head
	     tail
	     FBELL
	     Reformat
	     ansort
	     ansort2

	     MAX
	     MIN

	     LoadImage
	     );
$VERSION = '0.01';

BEGIN
{
    eval
    {
        require Tk;
        Tk->import();
	
	require Tk::Adjuster;
	Tk::Adjuster->import();
    };
}

%PACKARGS = (-padx => 1, -pady => 1, -ipadx => 1,-ipady => 1,
	     -expand => 1,-fill => 1,'-side' => 1 );

%FOCUSTYPES = (Entry => 1, Canvas => 0, Listbox => 0, Button => 0);
%WINDOW = ();

sub SelectWindow
{
    my($name) = @_;

    return $name if(!defined $name);
    
    my $index=0;
    my $package = 'bagotricks';
    my @data = ();
    
    while($package eq "bagotricks")
    {
	($package) = caller($index++);
    }
    $WINDOW{$package}=$name;
    return $name;
}

sub GetPackage()
{
    my $index=0;
    my $package = 'bagotricks';

    if((exists $WINDOW{"bagotricks"})&&($WINDOW{"bagotricks"} ne ""))
    {
	return $WINDOW{"bagotricks"};
    }
    
    while($package eq "bagotricks")
    {
	($package) = caller($index++);
    }
    if((exists $WINDOW{$package})&&($WINDOW{$package} ne ""))
    {
	$package = $WINDOW{$package}
    }
    
    return $package;
}

sub FISOPEN
{
    my $package = &GetPackage();
    return 1 if(exists $FRAMES{$package});
    return 0;
}

sub FMW
{
    return $MW;
}

sub FINIT
{
    my ($title, $color) = @_;
    my $package = &GetPackage();
    
    &FCLOSE();
    if(!defined $MW)
    {
	$MW = MainWindow->new;
	@{$FRAMES{$package}} = ($MW);
    }
    else
    {    
	@{$FRAMES{$package}} = ($MW->Toplevel);
	$FRAMES{$package}[0]->OnDestroy([\&WinClosed,$package]);
    }
    
    FCUR()->title($title) if(defined $title);
    FCUR()->setPalette($color) if(defined $color);
}

sub WinClosed
{
    ($package) = @_;
    delete $CTRL{$package};
    delete $TYPE{$package};
    delete $DEFAULT{$package};
    delete $CUSTOMGET{$package};
    delete $CUSTOMPUT{$package};
    delete $PREFIX{$package};
    delete $FRAMES{$package};
}

sub FCLOSE
{
    my $package = &GetPackage();

    delete $CTRL{$package};
    delete $TYPE{$package};
    delete $DEFAULT{$package};
    delete $CUSTOMGET{$package};
    delete $CUSTOMPUT{$package};
    delete $PREFIX{$package};
    
    if(exists $FRAMES{$package})
    {
	$FRAMES{$package}[0]->destroy;
	delete $FRAMES{$package};
    }
}

sub FDEPTH
{
    my $package = &GetPackage();
    return $#{$FRAMES{$package}};
}

sub FIN
{
    @arg = @_;
    my $package = &GetPackage();
    push(@{$FRAMES{$package}}, $FRAMES{$package}[&FDEPTH()]->Frame(@arg));
    return &FCUR();
}

sub FOUT
{
    @arg = @_;
    my $package = &GetPackage();
    pop(@{$FRAMES{$package}})->pack(@arg);
    return &FCUR();
}

sub FOUTA
{
    @arg = @_;
    my $package = &GetPackage();
    pop(@{$FRAMES{$package}})->packAdjust(@arg);
    return &FCUR();
}

sub FCUR
{
    my $package = &GetPackage();
    return $FRAMES{$package}[&FDEPTH()];
}

sub FSTACK
{
    my $package = &GetPackage();
    if($#_>=0)
    {
	@{$FRAMES{$package}} = @_;
    }
    else
    {
	return @{$FRAMES{$package}};
    }
}

sub GetValue
{
    my($var) = @_;
    my $package = &GetPackage();
    ($package,$var) = $var =~ /(\S+)\|(\S+)/ if($var =~ /\|/);
    ($package,$var) = $var =~ /(\S+)::(\S+)/ if($var =~ /::/);
    $var = uc($var);
    return $DATA{$package}{$var} if(! exists $TYPE{$package}{$var});
    
    if(exists $CUSTOMGET{$package}{$TYPE{$package}{$var}})
    {
	$WINDOW{bagotricks} = $package;
	my $rc = &{$CUSTOMGET{$package}{$TYPE{$package}{$var}}}($var);
	$WINDOW{bagotricks} = "";
	return $rc;
    }

    return $CTRL{$package}{$var}->get if($TYPE{$package}{$var} eq 'Scale');
    return $CTRL{$package}{$var}->get if($TYPE{$package}{$var} eq 'Entry');
    return $CTRL{$package}{$var}->cget('-text') if(($TYPE{$package}{$var} eq 'Menubutton')||($TYPE{$package}{$var} eq 'Label'));
    return $CTRL{$package}{$var}->get('0.0','end') if($TYPE{$package}{$var} eq 'Text');
    
    if($TYPE{$package}{$var} eq 'Checkbutton')
    {
	my $data = ${$CTRL{$package}{$var}->cget('-variable')};
	$data = 0 if(!defined $data);
	return $data;
    }
    if($TYPE{$package}{$var} eq 'Listbox')
    {
	my @ids = $CTRL{$package}{$var}->curselection();
	foreach $id (@ids)
	{
	    $id = $CTRL{$package}{$var}->get($id);
	}
	return @ids if(wantarray);
	return $ids[0];
    }
    return () if(wantarray);
    return undef;
}

sub InitValue
{
    my($var,$value) = @_;
    &SetValue($var,$value);
    
    my $package = &GetPackage();
    ($package,$var) = $var =~ /(\S+)\|(\S+)/ if($var =~ /\|/);
    ($package,$var) = $var =~ /(\S+)::(\S+)/ if($var =~ /::/);
    $DEFAULT{$package}{uc($var)} = $value;
}

sub GetPackages
{
    return keys %CTRL;
}

sub GetRealName
{
    my($var) = @_;

    my $package = &GetPackage();
    ($package,$var) = $var =~ /(\S+)\|(\S+)/ if($var =~ /\|/);
    ($package,$var) = $var =~ /(\S+)::(\S+)/ if($var =~ /::/);

    return $REALNAME{$package}{uc($var)};
}

sub GetControls
{
    my($package) = @_;
    $package = GetPackage() if(!defined $package);
    return keys %{$CTRL{$package}};
}

sub GetData
{
    my($var) = @_;
    my $package = &GetPackage();
    ($package,$var) = $var =~ /(\S+)\|(\S+)/ if($var =~ /\|/);
    ($package,$var) = $var =~ /(\S+)::(\S+)/ if($var =~ /::/);
    $var = uc($var);
    return $DATA{$package}{$var};
}

sub FMATCH
{
    my($pattern) = @_;
    $pattern = uc($pattern);
    
    my @ctrls = ();
    foreach $ctrl (&GetControls(&GetPackage()))
    {
	push(@ctrls, $ctrl) if($ctrl =~ $pattern);
    }
    return @ctrls;
}

sub compare
{
    my($package, $str, $match) = @_;
    my($index,$prefix,$postfix, $elem, $ind);
    my @array = split('{', $str);
    
    $index = $#array;
    $elem = $array[$index];
    while($index > 0)
    {
	$prefix  = "$package\::" . substr($elem, 0, $ind = index($elem,'}'));
	if(!exists $SETALIAS{$prefix})
	{
	    $SETALIAS{$prefix} = &GetValue($prefix);
	}
	$postfix = substr($elem, $ind+1);
	$elem = $array[--$index] . $SETALIAS{$prefix} . $postfix;
    }
    return 1 if($elem eq $match);
    return 0;
}

sub Resolve
{
    my($package, $str) = @_;
    $str =~ s/\{(.*?)\}/&GetCValue("$package\::$1")/eg;
    return $str;
}

sub compare2
{
    my($package, $str, $match) = @_;
    $str =~ s/\{(.*?)\}/&GetCValue("$package\::$1")/eg;
    return 1 if($str eq $match);
    return 0;
}

sub GetCValue
{
    my($var) = @_;
    $CACHE{$var} = &GetValue($var) if(!exists $CACHE{$var});
    return $CACHE{$var}
}

sub ClearSetAlias
{
    undef %CACHE;
}

sub GetPrefix
{
    my($name) = @_;
    $name =~ s/\(.*//o;
    return $name;
}

sub SetAlias
{
    my ($package, $var, $value) = @_;
    $var = uc($var);
    foreach $name (keys %{$CTRL{$package}})
    {
        if(&compare2($package, $name, $var))
	{
	    InitValue("$package\::$name", $value);
	}
    }
}

sub SetAliasArray
{
    my $package = shift @_;
    my %hash = @_;
    foreach $name (keys %{$CTRL{$package}})
    {
	my $rname = Resolve($package, $name);
	if(exists $hash{$rname})
	{
	    InitValue("$package\::$name", $hash{$rname});
	}
    }
}

sub HasChanged
{
    my($var) = @_;
    my $package = &GetPackage();
    ($package,$var) = $var =~ /(\S+)\|(\S+)/ if($var =~ /\|/);
    ($package,$var) = $var =~ /(\S+)::(\S+)/ if($var =~ /::/);
    return 0 if($DEFAULT{$package}{$var} eq &GetValue($var));
    return 1;
}

sub SetValue
{
    my($var,$value) = @_;

    my $package = &GetPackage();
    ($package,$var) = $var =~ /(\S+)\|(\S+)/ if($var =~ /\|/);
    ($package,$var) = $var =~ /(\S+)::(\S+)/ if($var =~ /::/);
    $var = uc($var);

    $DATA{$package}{$var} = $value;

    return if(! exists $TYPE{$package}{$var});
    
    if(exists $CUSTOMPUT{$package}{$TYPE{$package}{$var}})
    {
	$WINDOW{bagotricks} = $package;
	&{$CUSTOMPUT{$package}{$TYPE{$package}{$var}}}($var, $value);
	$WINDOW{bagotricks} = "";
	return;
    }
    
    if(($TYPE{$package}{$var} eq 'Menubutton')||($TYPE{$package}{$var} eq 'Label'))
    {
	$CTRL{$package}{$var}->configure(-text => $value);
    }
    if($TYPE{$package}{$var} eq 'Entry')
    {
	my $state = $CTRL{$package}{$var}->cget(-state);
	$CTRL{$package}{$var}->configure(-state => 'normal');
	my $cursor = $CTRL{$package}{$var}->index('insert');
	$CTRL{$package}{$var}->delete(0,'end');
	$CTRL{$package}{$var}->insert(0,$value);
	$CTRL{$package}{$var}->icursor($cursor);
	$CTRL{$package}{$var}->xview(moveto => $cursor);
	$CTRL{$package}{$var}->configure(-state => $state);
    }
    if($TYPE{$package}{$var} eq 'Scale')
    {
	$CTRL{$package}{$var}->set($value);
    }
    if($TYPE{$package}{$var} eq 'Checkbutton')
    {
	if(($value ne "")&&($value ne "0"))
	{
	    $CTRL{$package}{$var}->select();
	}
	else
	{
	    $CTRL{$package}{$var}->deselect();
	}
    }
    if($TYPE{$package}{$var} eq 'Text')
    {
	$CTRL{$package}{$var}->delete('0.0','end');
	$CTRL{$package}{$var}->insert('0.0',$value);
    }
    if($TYPE{$package}{$var} eq 'Listbox')
    {
	my @items = $CTRL{$package}{$var}->get(0,'end');
	my $index = 0;
	foreach $item (@items)
	{
	    foreach $value (@_)
	    {
		$CTRL{$package}{$var}->selectionSet($index) if($value =~ "$item\$");
	    }
	    $index++;
	}
	my @sel = $CTRL{$package}{$var}->curselection();
	$CTRL{$package}{$var}->see($sel[0]) if($#sel >= 0);
    }
}

sub head
{
    my ($fn) = @_;
    return(substr($fn,0,rindex($fn,"/"))) if ($fn =~ /\//);
    return ".";
}

sub tail
{
    my ($fn) = @_;
    $fn =~ s/.*\///og;
    return $fn;
}

sub FCTRL
{
    my $realname = shift @_;
    my $name = uc($realname);
    
    my $type = shift @_;
    my @ctrlargs = ();
    my @packargs = ();
    my @FRAMEPARMS = ();
    my $orient = "";
    my $xscroll;
    my $yscroll;
    my $focus = $FOCUSTYPES{$type};
    my $frame = 0;
    my $tab = 0;
    my @tf = ();
    my $nopack = 0;
    my $package = GetPackage();
    
    while(@_)
    {
	my $var = shift @_;
	my $value = shift @_;
	if($PACKARGS{$var})
	{
	    push(@packargs,($var,$value));
	}
	elsif($var eq "orient")
	{
	    $orient = $value;
	}
	elsif($var eq "frame")
	{
	    $frame = $value;
	}
	elsif($var eq "tab")
	{
	    $tab = $value;
	}
	elsif($var eq "nopack")
	{
	    $nopack = $value;
	}
	else
	{
	    $focus = 1 if($var eq "-takefocus");
	    push(@ctrlargs,($var,$value));
	}
	if($var eq "-fill")
	{
	    push(@FRAMEPARMS, $var, $value);
	}
	if($var eq "-expand")
	{
	    push(@FRAMEPARMS, $var, $value);
	}
    }

    if(exists $FOCUSTYPES{$type})
    {
	@tf = (-takefocus,0) if($focus == 0);
	push(@ctrlargs, @tf);
    }
    if($frame)
    {
	FIN();
    }
    if($orient =~ "y")
    {
	$yscroll = FCUR()->Scrollbar(@tf)->pack(-side => 'right', -fill => 'y');
    }

    my $w = FCUR()->$type(@ctrlargs);
    if(!$nopack)
    {
	$w->pack(@packargs);
    }

    if($orient =~ "x")
    {
	$xscroll = FCUR()->Scrollbar(-orient => 'horizontal', @tf)->pack(-side => 'bottom', -fill => 'x');
    }
    
    if(defined $yscroll)
    {
	$w->configure(-yscrollcommand => [$yscroll => 'set']); 
	$yscroll->configure(-command => [$w => 'yview']);
    }
    
    if(defined $xscroll)
    {
	$w->configure(-xscrollcommand => [$xscroll => 'set']); 
        $xscroll->configure(-command => [$w => 'xview']);
    }
    if(defined $name)
    {
	$WINDOW{bagotricks} = $package;
	FADD($name,$type,$w);
	$REALNAME{$package}{$name} = $realname;
	$WINDOW{bagotricks} = "";
    }
    if($frame)
    {
	FOUT(@FRAMEPARMS);
    }
    return $w;
}

sub FSPACE
{
    my @args = @_;
    FCTRL(undef, Canvas, -height => 1, -takefocus => 0, @args);
}

sub FPOPUP
{
    my ($name, $b, $c, $cmd, $menu) = @_;
    my @menuargs = @{$b};
    my @entries = @{$c};
    
    my $package = &GetPackage();
    my $command = "command";
    $name = uc($name);
    
    %ma = @menuargs;
    
    my @options = ();
    my @bg = ();
    @bg = (-background => $ma{'-bg'}) if(exists $ma{'-bg'});

    my $count = 0;
    if(!defined $MAXSCREENHEIGHT)
    {
	($MAXSCREENWIDTH,$MAXSCREENHEIGHT) = $MW->maxsize();
    }
    
    if(!defined $cmd)
    {
	$cmd = 'SetValue';
    }
    if(! defined $menu)
    {
	$menu = $CTRL{$package}{$name}->cget(-menu);
	if(!defined $menu)
	{
	    $menu = $CTRL{$package}{$name}->Menu(@menuargs);
	    $CTRL{$package}{$name}->configure(-menu => $menu);
	}
	else
	{
	    $menu->delete(0,'end');
	}
    }
    while(@entries)
    {
	my $item = shift @entries;
	if($#{$item} == -1)
	{
	    if($item eq "sep")
	    {
		$menu->add('separator');
	    }
            elsif($item =~ /disabled/)
            {
                $menu->entryconfigure('end', -state => 'disabled');
            }
	    elsif($item eq "command")
	    {
		$command = "command";
	    }
            elsif($item eq "checkbutton")
            {
                $command = "checkbutton";
            }
	    elsif($item eq "variable")
	    {
		my $var = shift @entries;
		push(@options, -variable, $var);
	    }
	    elsif($item =~ /key/)
	    {
		$key = shift @entries;
		
		$func = $menu->entrycget('end', -command);

		$key = lc($key);
		$MW->bind("<Control-Key-$key>"	=> $func);
		$MW->bind("<Alt-Key-$key>"	=> $func);
		$key = uc($key);
		$MW->bind("<Control-Key-$key>"	=> $func);
		$MW->bind("<Alt-Key-$key>"	=> $func);
		
		$text = "    Ctrl+$key";
		$menu->entryconfigure('end', -accelerator => $text);
	    }
	    elsif($item eq "-font")
	    {
		$font = shift @entries;
		$menu->entryconfigure('end', -font => $font);
	    }
	    else
	    {
		if(++$count > $MAXSCREENHEIGHT / 24)
		{
		    my $newmenu = $menu->Menu(@menuargs);
		    $menu->cascade(-label => "--- MORE ---");
		    $menu->entryconfigure('end', -menu => $newmenu);
		    FPOPUP($name, $b, [$item, @entries], $cmd, $newmenu);
		    return;
		}
		else
		{
		    $menu->add($command, -label     => $item,
				   -command   => [\&$cmd,"$package|$name",$item],
				   @bg, @options);
		    @options = ();
		}
	    }
	}
	else
	{
	    my $newmenu = $menu->Menu(@menuargs);
	    my @e = @{$item};
	    $menu->cascade(-label => shift @e, -underline => 0);
	    $menu->entryconfigure('end', -menu => $newmenu);
	    &FPOPUP($name,$b,[@{$item}],$cmd,$newmenu);
	}
    }
}

sub FMENU
{
    my ($name, $b, $c, $menu) = @_;
    my @menuargs = @{$b};
    my @entries = @{$c};
    $name = uc($name);
    my $command = "command";
    my @options = ();
    
    my $count = 0;
    if(!defined $MAXSCREENHEIGHT)
    {
	($MAXSCREENWIDTH,$MAXSCREENHEIGHT) = $MW->maxsize();
    }
    
    my $package = &GetPackage();
    if(!defined $menu)
    {
	$menu = $CTRL{$package}{$name}->cget(-menu);
	if(!defined $menu)
        {
            $menu = $CTRL{$package}{$name}->Menu(@menuargs);
            $CTRL{$package}{$name}->configure(-menu => $menu);
        }
        else
        {
            $menu->delete(0,'end');
        }
    }
    while(@entries)
    {
	$item = shift @entries;
	if($#{$item} == -1)
	{
	    if($item eq "sep")
	    {
		$menu->add('separator');
	    }
	    elsif($item =~ /checkbutton/)
	    {
		$command = "checkbutton";
	    }
	    elsif($item =~ /command/)
	    {
		$command = "command";
	    }
	    elsif($item =~ /variable/)
	    {
		$menu->entryconfigure('end', "-variable" => $entries[0]); shift @entries;
	    }
	    elsif($item =~ /disabled/)
	    {
		$menu->entryconfigure('end', -state => 'disabled');
	    }
	    elsif($item =~ /key/)
	    {
		$key = shift @entries;
		
		$func = $menu->entrycget('end', -command);
		
		$MW->bind("<Control-Key-$key>"	=> $func);
		$MW->bind("<Alt-Key-$key>"	=> $func);
		$key = uc($key);
		$MW->bind("<Control-Key-$key>"	=> $func);
		$MW->bind("<Alt-Key-$key>"	=> $func);
		
		$text = "    Ctrl+$key";
		$menu->entryconfigure('end', -accelerator => $text);
	    }
	    else
	    {
		if(++$count > $MAXSCREENHEIGHT / 24)
		{
		    my $newmenu = $menu->Menu(@menuargs);
		    $menu->cascade(-label => "--- MORE ---");
		    $menu->entryconfigure('end', -menu => $newmenu);
		    FMENU($name, $b, [$item, @entries], $newmenu);
		    return;
		}
		else
		{
                    $menu->add($command, -label => $item);
		    $func = shift @entries;
		    $menu->entryconfigure('end', -command => $func) if(defined $func);
		}
	    }
	    
	}
	else
	{
	    my $newmenu = $menu->Menu(@menuargs);
	    my @e = @{$item};
	    $menu->cascade(-label => shift @e, -underline => 0);
	    $menu->entryconfigure('end', -menu => $newmenu);
	    &FMENU($name,$b,[@e],$newmenu);
	}
    }
}

sub FADD
{
    my ($name, $type, $ctrl) = @_;
    $name = uc($name);

    my $package = &GetPackage();
    $TYPE{$package}{$name} = $type;
    $CTRL{$package}{$name} = $ctrl;
    $DATA{$package}{$name} = "";
    my $prefix = &GetPrefix($name);
    
    push(@{$PREFIX{$package}{$prefix}}, $name);
}

sub FCUSTOM
{
    my($type, $getfunc, $putfunc) = @_;
    my $package = &GetPackage();
    $CUSTOMGET{$package}{$type} = $getfunc;
    $CUSTOMPUT{$package}{$type} = $putfunc;
}

sub FGET
{
    my ($var) = @_;
    my $package = &GetPackage();
    ($package,$var) = $var =~ /(\S+)\|(\S+)/ if($var =~ /\|/);
    ($package,$var) = $var =~ /(\S+)::(\S+)/ if($var =~ /::/);
    $var = uc($var);
    return $CTRL{$package}{$var};
}

sub FBELL
{
    my $package = &GetPackage();
    $FRAMES{$package}[0]->bell();
}

sub ansort
{
    return map
    {
	$_->[0]
    }
    sort
    {
	my $rc;
	my $i=1;
	my $numeric = 0;
	my $cnt = $a->[1];
	$cnt = $b->[1] if($cnt > $b->[1]);
	$numeric = 1 if(($a->[2] !~ /^\d/)||($b->[2] !~ /^\d/));	
	while(++$i != $cnt)
	{
	    if($i%2 == $numeric)
	    {
		return $rc if($rc = ($a->[$i] <=> $b->[$i]));
	    }
	    else
	    {
		return $rc if($rc = ($a->[$i] cmp $b->[$i]));
	    }
	}
	return 1 if(defined $a->[$i]);
	return -1 if(defined $b->[$i]);
	return 0;
    } 
    map
    {
	$a=[ $_, 0, /^(\d+)/o, /(\D+)(\d+)/go, /(\D+)$/o];
	$a->[1]=$#{@{$a}}+1;
	$a;
    } @_;
}

sub ansort2
{
    my ($a) = @_;
    $a =~ s/(\d+)/'0' x (5-length($1)) . $1/ge;
    return $a;
}

sub Reformat
{
    my @lines = @_;
    my @max = ();

    foreach $line (@lines)
    {
	my @args = split(" ",$line);
	my $index = 0;
	foreach $arg (@args)
	{
	    my $len = length($arg);
	    $max[$index] = $len if($len > $max[$index]);
	    $index++;
	}
    }
    my $index=0;
    foreach $m (@max)
    {
	$m ++;
    }
    
    foreach $line (@lines)
    {
	my @args = split(" ",$line);
	my $index = 0;
	foreach $arg (@args)
	{
	    my $len = $max[$index++] - length($arg);
	    $arg .= ' ' x $len;
	}
	$line = join(" ",@args);
    }
    return @lines;
}

sub DoAllEvents
{
    my $package = &GetPackage();
    $FRAMES{$package}[0]->updateWidgets();
}

sub Busy
{
    my($busy) = @_;
    my $curs;
    
    if($busy == 1)
    {
	push(@CURSORS, $MW->cget(-cursor));
	push(@CURSORS, 'watch');
    }
    $curs = pop @CURSORS;
    foreach $key (keys %FRAMES)
    {
	$FRAMES{$key}[0]->configure(-cursor => $curs);
    }
    &DoAllEvents();
}

sub MAX
{
    my($a,$b) = @_;
    return $a if($a > $b);
    return $b;
}

sub MIN
{
    my($a,$b) = @_;
    return $a if($a < $b);
    return $b;
}

sub LoadImage
{
    my($file) = @_;
    if(!exists $IMAGES{$file})
    {
	if($file =~ /\.xbm/)
	{
	    $IMAGES{$file} = FMW()->Bitmap($file, -file => $file, -maskfile => $file);
	}
	else
	{
	    $IMAGES{$file} = FMW()->Photo($file, -file => $file);
	}
    }
    return $IMAGES{$file};    
}

1;
