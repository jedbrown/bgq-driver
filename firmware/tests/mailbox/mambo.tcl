# use mambo floor
# begin_generated_IBM_copyright_prolog                             
#                                                                  
# This is an automatically generated copyright prolog.             
# After initializing,  DO NOT MODIFY OR MOVE                       
#  --------------------------------------------------------------- 
#                                                                  
# Licensed Materials - Property of IBM                             
# Blue Gene/Q                                                      
# (C) Copyright IBM Corp.  2010, 2012                              
# US Government Users Restricted Rights - Use, duplication or      
#   disclosure restricted by GSA ADP Schedule Contract with IBM    
#   Corp.                                                          
#                                                                  
# This software is available to you under the Eclipse Public       
# License (EPL).                                                   
#                                                                  
#  --------------------------------------------------------------- 
#                                                                  
# end_generated_IBM_copyright_prolog                               

set my_mambo_dir /bgsys/bgq/drivers/x86_64.mambo
set my_app_floor $env(BGQHOME)/bgq/work
set my_app       mailbox.elf
set my_pers      mailbox.elf.t1/mailbox.elf.t1.pers
set my_ttype     fwddr
set my_num_cores 1
# path where tcl script is located
# configure BGQ chip
set my_mambo_tcls_dir ${my_mambo_dir}/run/bgq/tcls

source ./debug_tools.tcl
source ${my_mambo_tcls_dir}/bgq_cores.tcl

set my_num_of_cores $my_num_cores
set my_test ""
# default 'my_trace' is off
set my_trace "no"

define dup bgq my_conf

# Configures BGQ with default configuration
bgq_default_machine my_conf $my_num_of_cores

# Force it to run with 1 thread only
my_conf config processor/number_of_threads 4



if {1} {

# Create a new instance of simulator

define machine my_conf my_sim

}


#
# load booter (relative path to executable)
#


#
# Load the Firmware, CNK, and app personality
#
set my_booter  $my_app_floor/firmware/bin/bgq_firmware.elf
set my_kernel  $my_app_floor/cnk/bin/bgq_kernel.elf

################################################################################
## Procedure to load the booter and the application images on one core        ##
################################################################################
proc bgq_load_images { my_sim cpu_num num_of_cpus my_app my_booter my_ttype my_pers my_kernel } {

$my_sim cpu $cpu_num load elf ${my_app}

# capture program info from registers
# Mambo sets the pc and toc registers from the elf file and points
# gpr1 to the top of physical memory (for use as program stack).
#
set my_app_iar [my_sim cpu $cpu_num display spr pc]
set my_mem_top [my_sim cpu $cpu_num display gpr  1]
set my_app_toc [my_sim cpu $cpu_num display gpr  2]

#
# load booter into cpu cpu_num
#
if {${my_ttype}=="cnk"} {
$my_sim cpu $cpu_num load elf $my_booter
$my_sim cpu $cpu_num load elf $my_pers
$my_sim cpu $cpu_num load elf $my_kernel
}
if {${my_ttype}=="fwddr"} {
$my_sim cpu $cpu_num load elf $my_booter
}

#set my_booter_iar [my_sim cpu $cpu_num display spr pc]
# reset entry: top of 32-bit physical address
set my_booter_reset_entry 0x0fffffffc

#
# set program counter and pass information to booter via parameter registers
#
$my_sim cpu $cpu_num thread 0 set spr pc $my_booter_reset_entry
$my_sim cpu $cpu_num thread 0 set gpr  3 $my_mem_top
$my_sim cpu $cpu_num thread 0 set gpr  4 $my_app_iar
$my_sim cpu $cpu_num thread 0 set gpr  5 $my_app_toc
$my_sim cpu $cpu_num thread 0 set gpr  6 $num_of_cpus

# set Processor Identification Register - PIR
set pir_value [expr {$cpu_num*4} ]

$my_sim cpu $cpu_num set spr pir $pir_value
}

#################################################################################
# Loop over all cores
for {set i 0} {$i < $my_num_of_cores} {incr i} {

        puts "About to load elf images (app, kernel, booter)"

        bgq_load_images my_sim $i $my_num_of_cores $my_app $my_booter $my_ttype $my_pers $my_kernel

        puts "About to reset shadow tlb"

        # Resets Shadow TLB cpu i
        #bgq_configure_reset_shadow_tlb my_sim $i

        bgq_configure_boot_edram_shadow_tlb my_sim $i
}

# Enable debug info:
#simdebug set "INSTRUCTION" 1
#simdebug set "INSTRUCTION_W_REGS" 1
#simdebug set "EXCEPTIONS" 1
#simdebug set "XLATE" 1
#simdebug set "QPX" 1
#simdebug set "MEM_REFS" 1;

#my_sim mode warmup
#my_sim mode cycle

puts "About to initialize THREAD_ACTIVE registers"

# Initializes THREAD_ACTIVE registers

set_thread_active_registers my_sim $my_num_of_cores



#if {${my_ttype}=="cnk"} {
#set my_kernel  $my_app_floor/cnk/bin/bgq_kernel.elf
#set my_test $my_app
#my_sim load elf $my_pers
#my_sim load elf $my_kernel
#my_sim load elf $my_booter
#}
#if {${my_ttype}=="fwddr"} {
#set my_test $my_app
#my_sim load elf $my_booter
#}
#if {${my_ttype}=="fwext"} {
#set my_test $my_app
#}


# Enable debug info:
#simdebug set "INSTRUCTION" 1
#simdebug set "INSTRUCTION_W_REGS" 1
#simdebug set "EXCEPTIONS" 1
#simdebug set "XLATE" 1
#simdebug set "QPX" 1


#
# launch instruction tracer, if desired
#
if ($my_trace=="yes") {
   if [catch { exec ${my_mambo_dir}/bin/emitter/reader [pid] & }] {
      puts "Can't start tracer"
      exit -1
      }

   ereader expect 1
   simemit set instructions 1
   simemit start
   }



#
# launch L1 Cache emitter reader
# if(1) enables reader
#
if (0) {
   if [catch { exec  ${my_mambo_dir}/bin/emitter/l1cache_reader [pid] ${my_app}_l1trace & }] {
      puts "Cannot start tracer"
      exit -1
   }

   ereader expect 1

   # Lists all instructions with cycle mode
   #simemit set "Instructions" 1

   # These work with warmup mode
   #simemit set "Memory_Write" 1
   #simemit set "Memory_Read" 1

   # These work with cycle mode
   simemit set "L1_DCache_Miss" 1
   #simemit set "L1_ICache_Miss" 1

   simemit start
}


#simdebug set "INSTRUCTION" 1;
#simdebug set "INSTRUCTION_W_REGS" 1;
#simdebug set "MEM_REFS" 1;

#
# enable PERCS cache model
#

#my_sim mode warmup

#my_sim mode cycle

#
# launch simulation
#  
#
#my_sim go
#quit
