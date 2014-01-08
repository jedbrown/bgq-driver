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
set my_mambo_dir /auto/BGQ/releases/floor-mambo

# path where tcl script is located
# configure BGQ chip
set my_mambo_tcls_dir ${my_mambo_dir}/tcls

#source ../../../work/bin/tcls/bgq.tcl

source ${my_mambo_tcls_dir}/bgq.tcl

# default 'my_num_of_cores' 1
set my_num_of_cores 1

# default 'my_trace' is off
set my_trace "no"

# relative path for executable
#set my_app hello


define dup bgq my_conf

# Configures BGQ with default configuration
bgq_default_machine my_conf

# Force it to run with 1 thread only
my_conf config processor/number_of_threads 4



if {1} {

# Create a new instance of simulator

define machine my_conf my_sim

}


# display cache settings
#my_sim cache display setting


#
# Load the Firmware - no CNK
#
set my_firmware ./hello_cores.elf

my_sim load elf $my_firmware

# Set the entry point (coming out of reset)
set my_booter_reset_entry 0xFFFFFFFC

set my_booter_iar [my_sim display spr pc]
set my_booter_toc [my_sim display gpr  2]

#
# set program counter and pass information to booter via parameter registers
#
my_sim cpu 0:0 set spr pc $my_booter_reset_entry

#my_sim cpu 0:0 set gpr  3 $my_mem_top
#my_sim cpu 0:0 set gpr  4 $my_app_iar
#my_sim cpu 0:0 set gpr  5 $my_app_toc
#my_sim cpu 0:0 set gpr  6 $my_num_of_cores




if {1} {

# Initializes Shadow TLB
#bgq_configure_shadow_tlb my_sim

# Resets Shadow TLB
#bgq_configure_reset_shadow_tlb my_sim


# Add map to Boot eDRAM area in the Shadow TLB
# Maps top of 32-bit virtual address space to
# top of 42-bit physical address space
# Note: where the Boot eDRAM is located
bgq_configure_boot_edram_shadow_tlb my_sim

}


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


# Enable debug info:
#simdebug set "INSTRUCTION" 1
#simdebug set "INSTRUCTION_W_REGS" 1
simdebug set "EXCEPTIONS" 1
#simdebug set "XLATE" 1
#simdebug set "QPX" 1

#
# launch simulation
#
puts "pc   for thread 0:" ; puts [my_sim cpu 0:0 display spr pc ]
# puts "gprs for thread 0:" ; puts [my_sim cpu 0:0 display gprs ]
# puts "gprs for thread 1:" ; puts [my_sim cpu 0:1 display gprs ]
# puts [my_conf display]
my_sim go

