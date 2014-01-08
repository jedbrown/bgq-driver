#!/usr/bin/perl -w
# begin_generated_IBM_copyright_prolog                             
#                                                                  
# This is an automatically generated copyright prolog.             
# After initializing,  DO NOT MODIFY OR MOVE                       
# ================================================================ 
#                                                                  
# Licensed Materials - Property of IBM                             
# Blue Gene/Q                                                      
# (C) Copyright IBM Corp. 2012 All Rights Reserved                 
# US Government Users Restricted Rights - Use,                     
# duplication or disclosure restricted by GSA ADP                  
# Schedule contract with IBM Corp.                                 
#                                                                  
# This software is available to you under the                      
# GNU General Public License (GPL).                                
#                                                                  
# ================================================================ 
#                                                                  
# end_generated_IBM_copyright_prolog                               
#
# build-ramdisk
#
# This script is used to regenerate a ramdisk.elf image from a Blue Gene
# driver.  By default this script will use the directories under the driver
# as a source for files to install into the ramdisk.  Note that only a small
# subset of these files will actually be inserted into the ramdisk.
#
# The ramdisk is a compressed cpio image that is trivially built into an elf
# image for loading by the control system.  At boot time the Linux kernel
# extracts the cpio image into a ramfs filesystem.
#
# Running build-ramdisk with no arguments will build a default ramdisk from
# the files in the driver.  If those files are unchanged the resulting ramdisk
# will be identical to the installed ramdisk.elf.  Output of this script is
# "ramdisk.elf" in the current working directory.
#
# The script supports modification of the ramdisk without actually changing
# files under an installed driver.  The --rundist option may be used to
# specify a different "dist" directory to be used at runtime after NFS is
# mounted.  This allows an administrator to create a copy of the driver's "dist"
# directory, make changes in the copy, and then generate a ramdisk that will
# use this dist copy at runtime.  In this case the files to actually go into
# the ramdisk still come from the installed driver's "dist" (not the copy).
# Use the --dist option to specify where the files should come from.  Note that
# --dist will imply --rundist as well.
#
# The script also has a method for adding additional files (or overriding files)
# with the --addtree option.  This option specifies a directory tree that should
# be added to the ramdisk.  This directory will be copied into "/" in the
# ramdisk.  For example, given this directory:
#
#	myroot/
#              etc/
#                  inittab
#                  protocols
#                  services
#              lib/
#                  libxyz.so.1
#              bgfiles/
#
# Now building the ramdisk with "build-ramdisk --addtree myroot" will produce
# a ramdisk with /etc/inittab, /etc/protocols, /etc/services, /bgfiles and /libxyz.so.1
# added.  Note that the installed "dist" already had the files in /etc so they
# will be silently overwritten.  The one exception is if you put whole directories with
# the same names as the base system directories (i.e. /lib , /sbin) into your addtree.
# Due to the way the build-ramdisk script and start-up scripts process system files,
# the version from your addtree directory will be replaced by the default system files.
# To modify/add files under the base system directories you will need to have the individual
# files and/or directories under the system directory in your addtree.
#
# Keep in mind that a larger ramdisk will take longer to load.  Most files can
# be accessed at runtime out of $BG_DIST which is the "dist" directory in the
# driver in NFS.  Only add files that are really needed before NFS is brought
# up.  Otherwise it is far easier (and probably faster) to write a script to
# copy or symlink the files from NFS.
#

my $me="build-ramdisk";

my $rootfs="BG_ROOTFS_PATH";
my $tools="BG_TOOLS_PATH";
my $runos="BG_RUNOS_PATH";
my $site = "/bgsys/local";
my $init = "";
my $saveimg=0;
my $compress=1;
my $classicRamdisk=0;
my $verbose=0;
my $outfile="ramdisk";
my @basefiles;

my $rhel6 = `uname -r | grep "el6" >> /dev/null ; echo \$?`;

sub usage {
    $| = 1;
    print STDERR "USAGE: $me [--rootfs <dir>] [--runos <dir>] [--init <opts>] [--addtree <dir>]\n",
    "-o <outfile>     Name the ramdisk image <outfile> (default ramdisk.elf)\n",
    "--rootfs <dir>   Use files from this directory to build the ramdisk \n",
    "--runos <dir>    Use this directory for the runtime os\n",
    "--site <dir>     Use this directory for sitewide boot config (default $site)\n",
    "--init <opts>    Use this to specify initialization options.\n",
    "--addtree <dir>  Add files from the given tree to the ramdisk\n",
    "--classicRamdisk Build the ramdisk using the full classic ramdisk filelist.\n",
    "--nocompress     Do not compress the ramdisk\n",
    "--saveimg        Do not delete the temporary ramdisk.cpio\n",
    "--verbose        Be verbose about what is being done\n",
    "--help           Show this message\n";
    exit 1;
}


my $tmpscript = "/tmp/ramdisk.gencpio.script.$$";
my $tmpcpiolog = "/tmp/ramdisk.cpio.log.$$";
my $tmpldscript = "/tmp/ramdisk.lds.$$";
my $tmpinit = "/tmp/ramdisk.init.$$";

# Process the arguments ...
while (defined(my $arg = shift @ARGV)) {
    if ($arg eq "--rootfs") {
	$rootfs = shift @ARGV;
    } elsif ($arg eq "--runos") {
	$runos = shift @ARGV;
    } elsif ($arg eq "--site") {
	$site = shift @ARGV;
    } elsif ($arg eq "--init") {
	$init = shift @ARGV;
    } elsif ($arg eq "--addtree") {
	push(@addtree, shift @ARGV);
    } elsif ($arg eq "--saveimg") {
	$saveimg=1;
    } elsif ($arg eq "--nocompress") {
	$compress=0;
    } elsif ($arg eq "--classicRamdisk") {
	$classicRamdisk=1;
    } elsif ($arg eq "-o") {
	$outfile = shift @ARGV;
    } elsif ($arg eq "--verbose") {
	$verbose=1;
    } elsif ($arg eq "--help") {
	usage();
    } else {
	# no additional args for now.
	print STDERR "$me: unknown arg $arg\n";
	usage();
    }
}

# List of files from the runos that are required in the ramdisk
# Order is not important and dirs need only be listed if they are empty.
#
# Symlinks are listed as link_name -> dest_name
# Use a trailing slash to indicate empty directories that must exist.
# Use "/dev/xyz c # #" to indicate devices 

if ( $classicRamdisk == 0 ) {
	@basefiles = (
		"/bgusr/",
		"/bin/[ -> busybox",
		"/bin/[[ -> busybox",
		"/bin/ar -> busybox",
		"/bin/ash -> busybox",
		"/bin/awk -> busybox",
		"/bin/basename -> busybox",
		#"/bin/bash -> busybox",
		"/bin/bunzip2 -> busybox",
		"/bin/cat -> busybox",
		"/bin/chgrp -> busybox",
		"/bin/chmod -> busybox",
		"/bin/chown -> busybox",
		"/bin/clear -> busybox",
		"/bin/cp -> busybox",
		"/bin/cut -> busybox",
		"/bin/date -> busybox",
		"/bin/dd -> busybox",
		"/bin/df -> busybox",
		"/bin/diff -> busybox",
		"/bin/dirname -> busybox",
		"/bin/dmesg -> busybox",
		"/bin/du -> busybox",
		"/bin/echo -> busybox",
		"/bin/env -> busybox",
		"/bin/expr -> busybox",
		"/bin/false -> busybox",
		"/bin/fgrep -> busybox",
		"/bin/find -> busybox",
		"/bin/free -> busybox",
		"/bin/grep -> busybox",
		"/bin/gunzip -> busybox",
		"/bin/gzip -> busybox",
		"/bin/head -> busybox",
		"/bin/hostname -> busybox",
		"/bin/id -> busybox",
		"/bin/kill -> busybox",
		"/bin/killall -> busybox",
		"/bin/less -> busybox",
		"/bin/ln -> busybox",
		"/bin/logger -> busybox",
		"/bin/login -> busybox",
		"/bin/ls -> busybox",
		"/bin/md5sum -> busybox",
		"/bin/mesg -> busybox",
		"/bin/mkdir -> busybox",
		"/bin/mkfifo -> busybox",
		"/bin/mknod -> busybox",
		"/bin/more -> busybox",
		"/bin/mv -> busybox",
		"/bin/netstat -> busybox",
		"/bin/nice -> busybox",
		"/bin/passwd -> busybox",
		"/bin/ping -> busybox",
		"/bin/ping6 -> busybox",
		"/bin/printenv -> busybox",
		"/bin/printf -> busybox",
		"/bin/ps -> busybox",
		"/bin/pwd -> busybox",
		"/bin/renice -> busybox",
		"/bin/reset -> busybox",
		"/bin/rm -> busybox",
		"/bin/rmdir -> busybox",
		"/bin/sed -> busybox",
		"/bin/sh -> bash",
		"/bin/sleep -> busybox",
		"/bin/sort -> busybox",
		"/bin/su -> busybox",
		"/bin/sync -> busybox",
		"/bin/tail -> busybox",
		"/bin/tar -> busybox",
		"/bin/tee -> busybox",
		"/bin/test -> busybox",
		"/bin/tftp -> busybox",
		"/bin/time -> busybox",
		"/bin/top -> busybox",
		"/bin/touch -> busybox",
		"/bin/tr -> busybox",
		"/bin/traceroute -> busybox",
		"/bin/true -> busybox",
		"/bin/tty -> busybox",
		"/bin/umount -> busybox",
		"/bin/uname -> busybox",
		"/bin/uniq -> busybox",
		"/bin/uptime -> busybox",
		"/bin/usleep -> busybox",
		"/bin/vi -> busybox",
		"/bin/watch -> busybox",
		"/bin/wc -> busybox",
		"/bin/wget -> busybox",
		"/bin/which -> busybox",
		"/bin/whoami -> busybox",
		"/bin/xargs -> busybox",
		"/bin/yes -> busybox",
		"/bin/zcat -> busybox",


		"/dev/pts/",
		"/dev/shm/",
		# Need to continue to create the personality device manually until we are no longer using
		# simulators.
		"/dev/stderr -> /proc/self/fd/2",
		"/dev/stdin -> /proc/self/fd/0",
		"/dev/stdout -> /proc/self/fd/1",

		"/etc/init.d -> /etc/rc.d/init.d",
		
		"/lib64/ld64.so.1 -> ld-2.12.so",
		"/lib64/ld-lsb-ppc64.so.3 -> ld64.so.1",
		"/lib64/libc.so.6 -> libc-2.12.so",
		"/lib64/libblkid.so.1 -> libblkid.so.1.1.0",
		"/lib64/libdl.so.2 -> libdl-2.12.so",
		"/lib64/libnsl.so.1 -> libnsl-2.12.so",
		"/lib64/libnss_files.so.2 -> libnss_files-2.12.so",
		"/lib64/libpci.so.3 -> libpci.so.3.1.4",
		"/lib64/libpthread.so.0 -> libpthread-2.12.so",
		"/lib64/libresolv.so.2 -> libresolv-2.12.so",
		"/lib64/libuuid.so.1 -> libuuid.so.1.3.0",
		"/lib64/libtinfo.so.5 -> libtinfo.so.5.7",

		"/sbin/arp -> ../bin/busybox",
		"/sbin/chroot -> ../bin/busybox",
		"/sbin/crond -> ../bin/busybox",
		"/sbin/getty -> ../bin/busybox",
		"/sbin/halt -> ../etc/rc.shutdown",
		"/sbin/hwclock -> ../bin/busybox",
		"/sbin/ifconfig -> ../bin/busybox",
		"/sbin/ifdown -> ../bin/busybox",
		"/sbin/ifup -> ../bin/busybox",
		"/sbin/inetd -> ../bin/busybox",
		#"/sbin/init -> ../bin/busybox",
		"/sbin/insmod -> ../bin/busybox",
		"/sbin/lsmod -> ../bin/busybox",
		"/sbin/modprobe -> ../bin/busybox",
		"/sbin/pidof -> ./killall5",
		#"/sbin/pivot_root -> ../bin/busybox",
		"/sbin/poweroff -> ../bin/busybox",
		"/sbin/reboot -> ../bin/busybox",
		"/sbin/rmmod -> ../bin/busybox",
		"/sbin/route -> ../bin/busybox",
		"/sbin/runlevel -> ../bin/busybox",
		#"/sbin/switch_root -> ../bin/busybox",
		"/sbin/telnetd -> ../bin/busybox",
		"/sbin/telnet -> ../bin/busybox",
		"/sbin/ftpput -> ../bin/busybox",
		"/sbin/ftpget -> ../bin/busybox",

		"/tmp/",
		"/proc/",
		"/selinux/",	# Added to avoid a bug with RHEL's busybox/selinux libs ...try removing some day.
		"/sys/",

#		"/usr/lib64/libgssglue.so.1 -> libgssglue.so.1.0.0",
		"/usr/lib64/libresolv.so -> /lib64/libresolv.so.2",
#		"/usr/lib64/libstdc++.so.6 -> libstdc++.so.6.0.13",
#		"/usr/lib64/libtirpc.so.1 -> libtirpc.so.1.0.10",

		"/var/",
		"/var/empty/",
		"/var/empty/sshd/",
		"/var/lib/",
		"/var/lib/nfs/",
		"/var/lib/nfs/rpc_pipefs/"

	);

} else {

	#Classic Ramdisk Files
	@basefiles = (
                "/bgusr/",
                "/bin/[ -> busybox",
                "/bin/[[ -> busybox",
                "/bin/ar -> busybox",
                "/bin/ash -> busybox",
                "/bin/awk -> busybox",
                "/bin/basename -> busybox",
                "/bin/bunzip2 -> busybox", 
                "/bin/cat -> busybox",     
                "/bin/chgrp -> busybox",   
                "/bin/chmod -> busybox",   
                "/bin/chown -> busybox",   
                "/bin/clear -> busybox",   
                "/bin/cp -> busybox",      
                "/bin/cut -> busybox",     
                "/bin/date -> busybox",    
                "/bin/dd -> busybox",      
                "/bin/df -> busybox",      
                "/bin/diff -> busybox",    
                "/bin/dirname -> busybox", 
                "/bin/dmesg -> busybox",   
                "/bin/du -> busybox",      
                "/bin/echo -> busybox",    
                "/bin/env -> busybox",     
                "/bin/expr -> busybox",    
                "/bin/false -> busybox",   
                "/bin/fgrep -> busybox",   
                "/bin/find -> busybox",    
                "/bin/free -> busybox",    
                "/bin/grep -> busybox",    
                "/bin/gunzip -> busybox",  
                "/bin/gzip -> busybox",    
                "/bin/head -> busybox",    
                "/bin/hostname -> busybox",
                "/bin/id -> busybox",      
                "/bin/kill -> busybox",    
                "/bin/killall -> busybox", 
                "/bin/less -> busybox",    
                "/bin/ln -> busybox",      
                "/bin/logger -> busybox",  
                "/bin/login -> busybox",   
                "/bin/ls -> busybox",      
                "/bin/md5sum -> busybox",  
                "/bin/mesg -> busybox",    
                "/bin/mkdir -> busybox",   
                "/bin/mkfifo -> busybox",  
                "/bin/mknod -> busybox",   
                "/bin/more -> busybox",    
                "/bin/mv -> busybox",      
                "/bin/netstat -> busybox", 
                "/bin/nice -> busybox",    
                "/bin/passwd -> busybox",  
                "/bin/ping -> busybox",    
                "/bin/ping6 -> busybox",   
                "/bin/printenv -> busybox",
                "/bin/printf -> busybox",  
                "/bin/pwd -> busybox",     
                "/bin/renice -> busybox",  
                "/bin/reset -> busybox",   
                "/bin/rm -> busybox",      
                "/bin/rmdir -> busybox",   
                "/bin/sed -> busybox",     
                "/bin/sh -> bash",         
                "/bin/sleep -> busybox",   
                "/bin/sort -> busybox",    
                "/bin/su -> busybox",      
                "/bin/sync -> busybox",    
                "/bin/tail -> busybox",    
                "/bin/tar -> busybox",     
                "/bin/test -> busybox",    
                "/bin/tftp -> busybox",    
                "/bin/time -> busybox",    
                "/bin/top -> busybox",     
                "/bin/touch -> busybox",   
                "/bin/traceroute -> busybox",
                "/bin/true -> busybox",      
                "/bin/tty -> busybox",       
                "/bin/umount -> busybox",    
                "/bin/uname -> busybox",     
                "/bin/uniq -> busybox",      
                "/bin/uptime -> busybox",    
                "/bin/usleep -> busybox",    
                "/bin/vi -> busybox",        
                "/bin/watch -> busybox",     
                "/bin/wget -> busybox",      
                "/bin/which -> busybox",     
                "/bin/whoami -> busybox",    
                "/bin/xargs -> busybox",     
                "/bin/yes -> busybox",       
                "/bin/zcat -> busybox",      


                "/dev/pts/",
                "/dev/shm/",
                # Need to continue to create the personality device manually until we are no longer using
                # simulators.                                                                            
                "/dev/bgpers c 10 111",                                                                  

                "/etc/init.d -> /etc/rc.d/init.d",

                "/lib/libbgcios.so -> /lib/libbgcios.so.1.0.0",
                "/lib/libbgcios.so.1 -> /lib/libbgcios.so.1.0.0",
                "/lib/libbgutility.so -> /lib/libbgutility.so.1.0.0",
                "/lib/libbgutility.so.1 -> /lib/libbgutility.so.1.0.0",

                "/lib64/ld64.so.1 -> ld-2.12.so",
                "/lib64/ld-lsb-ppc64.so.3 -> ld64.so.1",
                "/lib64/libattr.so.1 -> libattr.so.1.1.0",
                "/lib64/libaudit.so.1 -> libaudit.so.1.0.0",
                "/lib64/libblkid.so.1 -> libblkid.so.1.1.0",
                "/lib64/libc.so.6 -> libc-2.12.so",         
                "/lib64/libcap.so.2 -> libcap.so.2.16",     
                "/lib64/libcom_err.so.2 -> libcom_err.so.2.1",
                "/lib64/libcrypt.so.1 -> libcrypt-2.12.so",   
                "/lib64/libdl.so.2 -> libdl-2.12.so",         
                "/lib64/libexpat.so.1 -> libexpat.so.1.5.2",  
                "/lib64/libgcc_s.so.1 -> libgcc_s-4.4.4-20100726.so.1",
                "/lib64/libgssapi_krb5.so.2 -> libgssapi_krb5.so.2.2", 
                "/lib64/libk5crypto.so.3 -> libk5crypto.so.3.1",       
                "/lib64/libkeyutils.so.1 -> libkeyutils.so.1.3",       
                "/lib64/libkrb5.so.3 -> libkrb5.so.3.3",               
                "/lib64/libkrb5support.so.0 -> libkrb5support.so.0.1", 
#               "/lib64/liblog4cxx.so -> /lib/liblog4cxx.so.10.0.0",   
#               "/lib64/liblog4cxx.so.10 -> /lib/liblog4cxx.so.10.0.0",
                "/lib64/libm.so.6 -> libm-2.12.so",                    
                "/lib64/libnsl.so.1 -> libnsl-2.12.so",                
                "/lib64/libnss_files.so.2 -> libnss_files-2.12.so",    
                "/lib64/libpam.so.0 -> libpam.so.0.82.2",              
                "/lib64/libpci.so.3 -> libpci.so.3.1.4",               
                "/lib64/libpthread.so.0 -> libpthread-2.12.so",        
                "/lib64/libresolv.so.2 -> libresolv-2.12.so",          
                "/lib64/librt.so.1 -> librt-2.12.so",                  
                "/lib64/libtinfo.so.5 -> libtinfo.so.5.7",             
                "/lib64/libuuid.so.1 -> libuuid.so.1.3.0",             
                "/lib64/libutil.so.1 -> libutil-2.12.so",              
                "/lib64/libwrap.so.0 -> libwrap.so.0.7.6",             
                "/lib64/libz.so.1 -> libz.so.1.2.3",                   


                "/sbin/arp -> ../bin/busybox",
                "/sbin/chroot -> ../bin/busybox",
                "/sbin/crond -> ../bin/busybox", 
                "/sbin/getty -> ../bin/busybox", 
                "/sbin/halt -> ../etc/rc.shutdown",
                "/sbin/hwclock -> ../bin/busybox", 
                "/sbin/ifconfig -> ../bin/busybox",
                "/sbin/ifdown -> ../bin/busybox",  
                "/sbin/ifup -> ../bin/busybox",    
                "/sbin/inetd -> ../bin/busybox",   
                #"/sbin/init -> ../bin/busybox",    
                "/sbin/insmod -> ../bin/busybox",  
                "/sbin/lsmod -> ../bin/busybox",   
                "/sbin/modprobe -> ../bin/busybox",
                "/sbin/pidof -> ./killall5",       
                "/sbin/pivot_root -> ../bin/busybox",
                "/sbin/poweroff -> ../bin/busybox",  
                "/sbin/reboot -> ../bin/busybox",    
                "/sbin/rmmod -> ../bin/busybox",     
                "/sbin/route -> ../bin/busybox",     
                "/sbin/runlevel -> ../bin/busybox",  
                "/sbin/telnetd -> ../bin/busybox",   
                "/sbin/telnet -> ../bin/busybox",    
                "/sbin/ftpput -> ../bin/busybox",    
                "/sbin/ftpget -> ../bin/busybox",    

                "/tmp/",
                "/proc/",
                "/selinux/",    # Added to avoid a bug with RHEL's busybox/selinux libs ...try removing some day.                                                                                                       
                "/sys/",                                                                                    

                "/usr/lib64/autofs/lookup_files.so -> lookup_file.so",
                "/usr/lib64/autofs/lookup_ldaps.so -> lookup_ldap.so",
                "/usr/lib64/autofs/lookup_nis.so -> lookup_yp.so",    
                "/usr/lib64/autofs/mount_ext3.so -> mount_ext2.so",   
                "/usr/lib64/autofs/mount_ext4.so -> mount_ext2.so",   
                "/usr/lib64/autofs/mount_nfs4.so -? mount_nfs.so",    
                "/usr/lib64/libapr-1.so -> libapr-1.so.0.3.9",        
                "/usr/lib64/libapr-1.so.0 -> libapr-1.so.0.3.9",      
                "/usr/lib64/libaprutil-1.so -> libaprutil-1.so.0.3.9",
                "/usr/lib64/libaprutil-1.so.0 -> libaprutil-1.so.0.3.9",
# libboost disabled until we switch to using the distro's boost         
#               "/usr/lib64/libboost_system.so.5 -> libboost_system.so.1.39.0",
#               "/usr/lib64/libboost_program_options.so.5 -> libboost_program_options.so.1.39.0",
#               "/usr/lib64/libboost_regex.so.5 -> libboost_regex.so.1.39.0",                    
#               "/usr/lib64/libboost_serialization.so.5 -> libboost_serialization.so.1.39.0",    
#               "/usr/lib64/libboost_thread-mt.so.5 -> libboost_thread-mt.so.1.39.0",            
                "/usr/lib64/libcrack.so.2 -> libcrack.so.2.8.0",                                 
                "/usr/lib64/libcrypto.so.10 -> libcrypto.so.1.0.0",                              
                "/usr/lib64/libdb-4.7.so -> /lib64/libdb-4.7.so",                                
                "/usr/lib64/libfipscheck.so.1 -> ./libfipscheck.so.1.1.0",                       
                "/usr/lib64/libgssapi_krb5.so.2 -> /lib64/libgssapi_krb5.so.2.2",                
                "/usr/lib64/libgssglue.so.1 -> libgssglue.so.1.0.0",                             
                "/usr/lib64/libicudata.so.42 -> libicudata.so.42.1",                             
                "/usr/lib64/libicui18n.so.42 -> libicui18n.so.42.1",                             
                "/usr/lib64/libicuuc.so.42 -> libicuuc.so.42.1",                                 
                "/usr/lib64/libk5crypto.so -> /lib64/libk5crypto.so.3.1",                        
                "/usr/lib64/liblber.so -> liblber-2.4.so.2.5.2",                                 
                "/usr/lib64/liblber-2.4.so.2 -> liblber-2.4.so.2.5.2",                           
                "/usr/lib64/libldap.so -> libldap-2.4.so.2.5.2",                                 
                "/usr/lib64/libldap-2.4.so.2 -> libldap-2.4.so.2.5.2",                           
                "/usr/lib64/libibverbs.so -> libibverbs.so.1.0.0",                               
                "/usr/lib64/libibverbs.so.1 -> libibverbs.so.1.0.0",                             
                "/usr/lib64/libresolv.so -> /lib64/libresolv.so.2",                              
                "/usr/lib64/librdmacm.so -> librdmacm.so.1.0.0",                                 
                "/usr/lib64/librdmacm.so.1 -> librdmacm.so.1.0.0",                               
                "/usr/lib64/libsasl2.so -> libsasl2.so.2.0.23",                                  
                "/usr/lib64/libsasl2.so.2 -> libsasl2.so.2.0.23",                                
                "/usr/lib64/libssl.so.10 -> libssl.so.1.0.0",                                    
                "/usr/lib64/libssl.so -> libssl.so.1.0.0",                                       
                "/usr/lib64/libstdc++.so.6 -> libstdc++.so.6.0.13",                              
                "/usr/lib64/libtirpc.so.1 -> libtirpc.so.1.0.10",                                
                "/usr/lib64/libxml2.so.2 -> libxml2.so.2.7.6",                                   

                "/var/",
                "/var/empty/",
                "/var/empty/sshd/"

        );

}

# special modes (default 755 for dirs, 644 for devices and use existing mode for files)
my %modes = (  
    "/tmp" => "777",
);


sub verbose {
    $verbose && print STDERR "$me: ", @_, "\n";
}

sub error {
    cleanup();
    die "$me: ERROR: ", @_, "\n";
}

sub cleanup {
    unlink $tmpscript if defined $tmpscript;
    unlink $tmpcpiolog if defined $tmpcpiolog;
    unlink $tmpldscript if defined $tmpldscript;
    unlink $tmpinit if defined $tmpinit;
}

# Do some checks to make sure the dirs exist

#-r $dist || error "dist directory $dist not readable";
#-d $dist || error "dist directory $dist not a directory";
#-d "$dist/etc" || error "dist directory $dist does not appear to be a dist directory";
#-r "$rundist" || error "rundist directory $rundist not readable";
#-d "$rundist" || error "rundist directory $rundist not a directory";
#-d "$rundist/etc" || error "rundist directory $rundist does not appear to be a dist directory";
#-r $runos || error "OS directory $runos not readable";
#-d $runos || error "OS directory $runos not a directory";

verbose "root file system: $rootfs";
verbose "runos: $runos";
verbose "init: $init";

# Build a file list and weed out duplicates
my %allfiles;	# rel-filename => base dir 
foreach my $f (@basefiles) {
    $allfiles{$f} = $rootfs;
}

# Add all files in rootfs to the cpio script.
system("$tools/gen_initramfs_list.sh -u squash -g squash -d $rootfs > $tmpscript");

foreach my $dir (@addtree) {
    $dir =~ s,/$,,;
    -r "$dir" || error "addtree directory $dir not readable";
    -d "$dir" || error "addtree directory $dir not a directory";
    my @newfiles = getfiles($dir);
#    verbose "new files from $dir:\n	", join("\n	", @newfiles);
    verbose "new files/directories from $dir:\n ", join("\n     ", @newfiles);
    foreach my $f (@newfiles) {
	$allfiles{$f} = $dir;
    }
}
my @filelist = sort keys %allfiles;

# verify that /etc/sysconfig/sysinit was not overridden
if (defined $allfiles{'/etc/sysconfig/sysinit'}) {
    error "/etc/sysconfig/sysinit must be generated and cannot be added manually to the ramdisk";
}

# Create a list of dirs to create
my %ramdiskdirs;
my %rootfiles;
foreach my $f (sort keys %allfiles) {
    my $path = $f;
    $path =~ s, ->.*$,,;	# remove symlink dest
    $path =~ s,/[^/]*$,,;
    if ( $path eq "" ) {
	#ooops, the isn't a directory, it is a file or link.
	$rootfiles{$f} = 0;
    } else {
	$ramdiskdirs{$path} = 0;
    }
}
verbose "final dirs in ramdisk:\n	", join("\n	", sort keys %ramdiskdirs);


# Build the gen_init_cpio script in dir order
verbose "writing $tmpscript for gen_init_cpio";
open(TMPSCRIPT, ">>$tmpscript") or error "cannot create $tmpscript: $!\n";
# Iterate over all known dirs.
foreach my $d (sort keys %ramdiskdirs) {
    my @thislist = grep(m%^$d/[^/]*$|^$d/[^/]* %, @filelist);
    my $dirmode = $modes{$d};
    $dirmode = "755" if !$dirmode;
    print TMPSCRIPT "dir $d $dirmode 0 0\n";
    for my $f (@thislist) {
	next if $f eq "$d/";	# already did the dir itself
	my ($name, $a, $b, $c) = split(/ /, $f);
	if (defined($a)) {
	    if ($a eq "->") {
		print TMPSCRIPT "slink $name $b 0755 0 0\n";
	    } elsif ($a eq "c" or $a eq "b") {
		my $mode = $modes{$name};
		if ($a eq "c") {
			$mode = "0666" if !$mode;
		} else {
			$mode = "0644" if !$mode;
		}
		print TMPSCRIPT "nod $name $mode 0 0 $a $b $c \n";
	    }
	} else {
	    my $mode = $modes{$name};
	    my $fpath = $allfiles{$name} . $name;
	    -r $fpath or error "cannot open $fpath: $!\n";
	    my ($dev, $ino, $fmode, $nlink) = stat(_);	# only want the mode
	    $mode = sprintf("%o", $fmode & 0777) if !$mode;
	    print TMPSCRIPT "file $name $fpath 0$mode 0 0\n";
	}
    }
}
foreach my $file (sort keys %rootfiles) {
    my ($name, $a, $b, $c) = split(/ /, $file);
    if (defined($a)) {
        if ($a eq "->") {
                print TMPSCRIPT "slink $name $b 0755 0 0\n";
        }
    } else {
        my $mode = $modes{$name};
        my $fpath = $allfiles{$name} . $name;
        -r $fpath or error "cannot open $fpath: $!\n";
        my ($dev, $ino, $fmode, $nlink) = stat(_);  # only want the mode
        $mode = sprintf("%o", $fmode & 0777) if !$mode;
        print TMPSCRIPT "file $name $fpath 0$mode 0 0\n";
   }
}

open(INIT, ">$tmpinit") or error "cannot write to $tmpinit: $!";
print INIT "export BG_OSDIR=\"$runos\"\n";
if ($init ne "") {
	print INIT "$init\n";
}
close(INIT);
print TMPSCRIPT "file /etc/sysconfig/sysinit $tmpinit 0755 0 0\n";
close(TMPSCRIPT);

# Now use gen_init_cpio (from Linux kernel build) to write our files as a cpio
# image.  We could use cpio itself, but then we'd need to deal with device
# files.  An alternative is to use an existing image with device files and
# add additional files.
$cpiofile = "$outfile.cpio";
$rc = system("$tools/gen_init_cpio $tmpscript > $cpiofile 2> $tmpcpiolog");
if ($verbose || $rc) {
    system("cat $tmpcpiolog");
    # Can we diagnose a failure here better?
    error "gen_init_cpio failed" if $rc;
}

if($compress)
{
    verbose "compress $cpiofile";
    $rc = system("gzip -f9 $cpiofile");
    $rc == 0 or error "gzip $cpiofile failed";
    $cpiofile = "$outfile.cpio.gz";
}

verbose "build $outfile from $cpiofile";
open(LDSCRIPT, ">$tmpldscript") or error "cannot create $tmpldscript: $!";
print LDSCRIPT "OUTPUT_ARCH(powerpc)\n",
  "SECTIONS\n",
  "{\n",
  "	.data 0x1000000: {\n",
  "		LONG(0xf0e1d2c3);\n",
  "		LONG(ramdisk_end - ramdisk_start);\n",
  "		ramdisk_start = .;\n",
  "		$outfile.cpio.elf\n",
  "		ramdisk_end = .;\n",
  "	 }\n",
  "}\n";
close(LDSCRIPT);
$rc = system("ld -b binary -e 0 -o $outfile.cpio.elf $cpiofile && ld -T $tmpldscript -o $outfile");
$rc == 0 or error "build $outfile from $cpiofile failed";
unlink "$outfile.cpio.elf";
$saveimg or unlink "$cpiofile";

cleanup();

exit 0;


# Get a list of files under the given dir
sub getfiles {
    my ($dir) = @_;
    my @ret;
    my $link;
    # Add directories
    open(F, "find \"$dir\" -type d -print |") or error "cannot read dir $dir: $!";
    while (defined(my $line = <F>)) {
        chomp($line);
        if ("$line" ne "$dir") {
            $line =~ s,^$dir/,,;
            # Exclude trash
            next if $line =~ m,\.svn,;
            next if $line =~ m,~$,;
            push(@ret, '/' . $line . '/');
        }
    }
    close(F);
    # Add links
    open(F, "find \"$dir\" -type l -print |") or error "cannot read dir $dir: $!";
    while (defined(my $line = <F>)) {
        chomp($line);
        $line =~ s,^$dir/,,;
        # Exclude trash
        next if $line =~ m,\.svn,;
        next if $line =~ m,~$,;
        $link = readlink("$dir/$line");
        $line = $line . " -> " . $link;
        push(@ret, '/' . $line);
    }
    close(F);
    # Add files
    open(F, "find \"$dir\" -type f -print |") or error "cannot read dir $dir: $!";
    while (defined(my $line = <F>)) {
	chomp($line);
	$line =~ s,^$dir/,,;
	# Exclude trash
	next if $line =~ m,\.svn,;
	next if $line =~ m,~$,;
	push(@ret, '/' . $line);
    }
    close(F);
    return @ret;
}
