/* begin_generated_IBM_copyright_prolog                             */
/*                                                                  */
/* This is an automatically generated copyright prolog.             */
/* After initializing,  DO NOT MODIFY OR MOVE                       */
/* ================================================================ */
/*                                                                  */
/* Licensed Materials - Property of IBM                             */
/*                                                                  */
/* Blue Gene/Q                                                      */
/*                                                                  */
/* (C) Copyright IBM Corp.  2010, 2012                              */
/*                                                                  */
/* US Government Users Restricted Rights -                          */
/* Use, duplication or disclosure restricted                        */
/* by GSA ADP Schedule Contract with IBM Corp.                      */
/*                                                                  */
/* This software is available to you under the                      */
/* Eclipse Public License (EPL).                                    */
/*                                                                  */
/* ================================================================ */
/*                                                                  */
/* end_generated_IBM_copyright_prolog                               */
/*!
    
    


//************************************************************************
// WARNING: This is an script generated file - do not manually change.
//   The file is generated from bgpm/events/Events-CrtDoxy.xsl
//   based on the data in Events.xml and Events-map.xml and bgpm_event_tips.htpl
//
// build.pl
//   generates the bgpm_events* files and copies to appropriate directories for
//   manual SVN merge.
//
//************************************************************************


/**
 *  
 * \page bgpm_events BGPM Event Tables
 *
 \htmlonly
 <script xmlns="http://www.w3.org/1999/xhtml" type="text/javascript">
    function toggleDetail(e) {
      var evt=window.event || e;
      var targ;
      if (evt.target) { targ=e.target; }
      else if (evt.srcElement) { targ=e.srcElement; }
      if (targ.nodeType==3) { // workaround Safari bug
          targ = targ.parentNode;
      }
	      
      var curIdx = targ.cellIndex;
      var curData = targ.innerHTML;
      
      var hiddenIdx = curIdx + 1;
      var parNode = targ.parentNode;
      var cells = parNode.cells;

      cells[curIdx].innerHTML = cells[hiddenIdx].innerHTML;
      cells[hiddenIdx].innerHTML = curData;
    }
 </script>
 
 \endhtmlonly
 
\htmlonly


<div class="toc">
\endhtmlonly

<h2>Table Of Contents</h2>

- \ref index "Return to Main Page"
- \ref event_table_legend


 *
 *
- \ref punit_events  
\htmlonly
<ul xmlns="http://www.w3.org/1999/xhtml"><li><a class="el" href="bgpm_events.html#a_undefined_">Undefined</a></li><li><a class="el" href="bgpm_events.html#a_axu_execution">AXU Execution Unit Events (Quad Floating Point Unit)</a></li><li><a class="el" href="bgpm_events.html#a_instruction_unit">Instruction Unit Events</a></li><li><a class="el" href="bgpm_events.html#a_execution_unit">Execution Unit Events</a></li><li><a class="el" href="bgpm_events.html#a_load_store">Load Store unit events.</a></li><li><a class="el" href="bgpm_events.html#a_memory_management">Memory Management Unit Events</a></li><li><a class="el" href="bgpm_events.html#a_l1p_base">L1p Base Events</a></li><li><a class="el" href="bgpm_events.html#a_l1p_switch">L1p Switch Event</a></li><li><a class="el" href="bgpm_events.html#a_l1p_stream">L1p Stream Event</a></li><li><a class="el" href="bgpm_events.html#a_l1p_list">L1p List Event</a></li><li><a class="el" href="bgpm_events.html#a_wakeup_unit">Wakeup Unit Event</a></li><li><a class="el" href="bgpm_events.html#a_xu_instruction">XU Instruction Opcode Counting</a></li><li><a class="el" href="bgpm_events.html#a_qfpu_instruction">QFPU Instruction Opcode Counting</a></li><li><a class="el" href="bgpm_events.html#a_qfpu_floating">QFPU Floating Point Operations Counting Groups</a></li><li><a class="el" href="bgpm_events.html#a_qfpu_floating">QFPU Floating Point Instructions Counting Groups</a></li><li><a class="el" href="bgpm_events.html#a_all_instruction">All Instruction Opcode Counting</a></li></ul>
\endhtmlonly
 

 *
 *
- \ref l2unit_events  
\htmlonly
<ul xmlns="http://www.w3.org/1999/xhtml"><li><a class="el" href="bgpm_events.html#a_l2_combined">L2 Combined Event (all slice counts combined)</a></li><li><a class="el" href="bgpm_events.html#a_l2_slice">L2 Slice Event (counters for each slice)</a></li></ul>
\endhtmlonly
 

 *
 *
- \ref iounit_events  
\htmlonly
<ul xmlns="http://www.w3.org/1999/xhtml"><li><a class="el" href="bgpm_events.html#a_message_unit">Message Unit Events</a></li><li><a class="el" href="bgpm_events.html#a_pcie_io">PCIe I/O Unit Events</a></li><li><a class="el" href="bgpm_events.html#a_devbus_io">DevBus I/O Unit Events</a></li></ul>
\endhtmlonly
 

 *
 *
- \ref nwunit_events  
\htmlonly
<ul xmlns="http://www.w3.org/1999/xhtml"><li><a class="el" href="bgpm_events.html#a_network_unit">Network Unit Events</a></li></ul>
\endhtmlonly
 

 *
 *
- \ref cnkunit_events  
\htmlonly
<ul xmlns="http://www.w3.org/1999/xhtml"><li><a class="el" href="bgpm_events.html#a_compute_node">Compute Node Kernel Node-Wide Events</a></li><li><a class="el" href="bgpm_events.html#a_compute_node">Compute Node Kernel Process Events</a></li><li><a class="el" href="bgpm_events.html#a_compute_node">Compute Node Kernel Hardware Thread Events</a></li></ul>
\endhtmlonly
 


\htmlonly


</div>
<div class="contents-right">


\endhtmlonly



* <p xmlns="http://www.w3.org/1999/xhtml">
 * \section event_table_legend Column Descriptions
 * The following tables describe the available events by unit.
 * The tables headers have the following meaning: 
 * </p>
 * - EventID is the unique event number
 * - Label is the event label and equivalent to the BGPM event enum values (see bgpm_events.h)
 * - Description - short description of event.
 * - Source refers to the unit and subunit for an event
     <ul xmlns="http://www.w3.org/1999/xhtml"><li>undef = Undefined</li><li>a2_iu = Instruction Unit Events</li><li>a2_xu = Execution Unit Events</li><li>a2_mmu = Memory Management Unit Events</li><li>a2_axu = AXU Execution Unit Events (Quad Floating Point Unit)</li><li>a2_lsu = Load Store unit events.</li><li>l1p_base = L1p Base Events</li><li>l1p_switch = L1p Switch Event</li><li>l1p_stream = L1p Stream Event</li><li>l1p_list = L1p List Event</li><li>wakeup = Wakeup Unit Event</li><li>l2 = L2 Combined Event (all slice counts combined)</li><li>l2_slice = L2 Slice Event (counters for each slice)</li><li>op_xu = XU Instruction Opcode Counting</li><li>op_axu = QFPU Instruction Opcode Counting</li><li>op_fp_ops = QFPU Floating Point Operations Counting Groups</li><li>op_fp_instr = QFPU Floating Point Instructions Counting Groups</li><li>op_all = All Instruction Opcode Counting</li><li>pcie = PCIe I/O Unit Events</li><li>devbus = DevBus I/O Unit Events</li><li>mu = Message Unit Events</li><li>nw = Network Unit Events</li><li>cnknode = Compute Node Kernel Node-Wide Events</li><li>cnkproc = Compute Node Kernel Process Events</li><li>cnkhwt = Compute Node Kernel Hardware Thread Events</li></ul>
 * - Scope
     <ul xmlns="http://www.w3.org/1999/xhtml"><li>thread = events attributable to current sw/hw thread</li><li>core = events are core-wide and events can't directly be attributed to single thread</li><li>core-shared = events are core-wide, and only one thread may count on the core when this event is used</li><li>node-shared = events are node-wide with shared counters - each thread can start/stop events of this type for all threads.</li><li>node = events are node-wide but counting is independently controllable by each thread (that is, each thread sees a personal delta value between the start and stop of the event)</li><li>exclusive = events are node-wide but counter resource is controllable by a single sw thread at a time</li><li>sampled = node wide events which are sampled and cannot be started or stopped (no current instances of this type of event)</li></ul>

 * - Features - BGPM support features
     <ul xmlns="http://www.w3.org/1999/xhtml"><li>o = fast overflow supported in BGPM Distributed Mode</li><li>s = slow overflow supported in BGPM Distributed Mode</li><li>l = low latency access method supported in BGPM Island Mode</li><li>m = multiplexing supported in BGPM Distributed Mode</li><li>M = multiplexing supported within BGPM Distributed Mode only within single L1p mode</li><li>c = kernel or user context sensitive (via core-wide setting)</li></ul>
 * - Tag indicates the suitability of the fundamental hardware event. 
 *   (This is informational only, the event list includes separate events when the 
 *   fundamental event may count cycles or events. 
     <ul xmlns="http://www.w3.org/1999/xhtml"><li>b = Suitable for counting both cycles and number of events</li><li>be = Configured to count number of event by default</li><li>bc = Configured to count cycles by default</li><li>c = Only suitable for counting cycles</li><li>e = Only suitable for counting numbers of events</li><li>s = Single cycle event, cycles and events are synonymous</li><li>v = Single cycle event which can occur on consecutive cycles; cycles and events are synonymous</li></ul>
 * - Status indicates the state of testing the event 
      <ul xmlns="http://www.w3.org/1999/xhtml"><li>v = Bgpm validated - counts confidently verified in controlled bgpm test.</li><li>c = Caveots - discovered some conditions which seem to alter this event from the designed intent</li><li>b = Broken - this event appears broken and may not be useful.</li></ul>
 *   
 *  
 * - PAPI = anticipated equivalent PAPI preset event (see /ref papi_presets for full list).
 * 
 *   
 * - Detail gives a longer description or details about the event. In a browser, click on contents to expand further.
 *
    
 *
 *
 * \section punit_events Punit Events
 * \htmlonly
 * <TABLE xmlns="http://www.w3.org/1999/xhtml" class="evt">
 * <TR bgcolor="AliceBlue">
 * <TH title="Unique integer value of BGPM event">EventId</TH>
 * <TH title="bgpm enum name for event (see bgpm_events.h)">Label</TH>
 * <TH title="Simple Description of event">Description</TH>
 * <TH title="Scope of the event (thread, core, core-shared, node-shared, node, exclusive, or sampled)">Scope</TH>
 


 * <TH title="Types of features supported by event (e.g. Overflow,LLAccess, multiplexing, etc)">Features</TH>
 * <TH title="Indicates the suitability of the fundamental hardware event">Tag</TH>
 * <TH title="Anticipated PAPI Preset event mapping to this event (see PAPI Presets for whole list)">PAPI</TH>
 * <TH title="Validation Testing level of this event">Status</TH>
 * <TH style="width:20em;" title="Detailed description of event">Detail</TH>
 * <TH style="display:none;">Hidden Detail</TH>
 * </TR><TR><TD colspan="11" style="text-align:left"><a class="anchor" id="a_undefined_"></a><B><I>Undefined</I></B></TD></TR>
 * <TR>
 * <TD>0</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_UNDEF"></a>PEVT_UNDEF</TD>   
   
 * <TD style="text-align:left">undefined</TD>
      
 * <TD title="events attributable to current sw/hw thread">thread</TD>
 
 
 
 * <TD title=""></TD>
 * <TD title="Single cycle event, cycles and events are synonymous">s</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title=""></TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">Undefined event should not occ...</TD> 
 * <TD style="display:none">Undefined event should not occur </TD>
 
 * </TR><TR><TD colspan="11" style="text-align:left"><a class="anchor" id="a_axu_execution"></a><B><I>AXU Execution Unit Events (Quad Floating Point Unit)</I></B></TD></TR>
 * <TR>
 * <TD>1</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_AXU_INSTR_COMMIT"></a>PEVT_AXU_INSTR_COMMIT</TD>   
   
 * <TD style="text-align:left">AXU Instruction Committed</TD>
      
 * <TD title="events attributable to current sw/hw thread">thread</TD>
 
 
 
 * <TD title="low latency access method supported in BGPM Island Mode;  multiplexing supported in BGPM Distributed Mode;  ">olmc</TD>
 * <TD title="Single cycle event, cycles and events are synonymous">s</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title="Bgpm validated - counts confidently verified in controlled bgpm test.">v</TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">A valid AXU (non-load/store) i...</TD> 
 * <TD style="display:none">A valid AXU (non-load/store) instruction has completed (past the last flush stage).<br></br>in EX6 pipeline stage.<br></br>-AXU uCode sub-operations are excluded </TD>
 
 * </TR>
 * <TR>
 * <TD>2</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_AXU_CR_COMMIT"></a>PEVT_AXU_CR_COMMIT</TD>   
   
 * <TD style="text-align:left">AXU CR Instruction Committed</TD>
      
 * <TD title="events attributable to current sw/hw thread">thread</TD>
 
 
 
 * <TD title="low latency access method supported in BGPM Island Mode;  multiplexing supported in BGPM Distributed Mode;  ">olmc</TD>
 * <TD title="Single cycle event, cycles and events are synonymous">s</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title=""></TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">A valid AXU CR updater instruc...</TD> 
 * <TD style="display:none">A valid AXU CR updater instruction has completed, past the last flush point.<br></br>in EX6 in pipeline stage </TD>
 
 * </TR>
 * <TR>
 * <TD>3</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_AXU_IDLE"></a>PEVT_AXU_IDLE</TD>   
   
 * <TD style="text-align:left">AXU Idle</TD>
      
 * <TD title="events attributable to current sw/hw thread">thread</TD>
 
 
 
 * <TD title="low latency access method supported in BGPM Island Mode;  multiplexing supported in BGPM Distributed Mode;  ">olmc</TD>
 * <TD title="Single cycle event, cycles and events are synonymous">s</TD>
 
 * <TD style="text-align:left">PAPI_FXU_IDL</TD>
   
 * <TD title=""></TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">No valid AXU instruction is in...</TD> 
 * <TD style="display:none">No valid AXU instruction is in the last stage of the floating point unit.<br></br>EX6 stage is empty </TD>
 
 * </TR>
 * <TR>
 * <TD>4</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_AXU_FP_DS_ACTIVE"></a>PEVT_AXU_FP_DS_ACTIVE</TD>   
   
 * <TD style="text-align:left">AXU FP Divide or Square root in progress</TD>
      
 * <TD title="events attributable to current sw/hw thread">thread</TD>
 
 
 
 * <TD title="low latency access method supported in BGPM Island Mode;  multiplexing supported in BGPM Distributed Mode;  ">olmc</TD>
 * <TD title="Configured to count number of event by default">be</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title=""></TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">A Floating-Point Divide or Squ...</TD> 
 * <TD style="display:none">A Floating-Point Divide or Square Root sequence is in progress (incl single-precision). </TD>
 
 * </TR>
 * <TR>
 * <TD>5</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_AXU_FP_DS_ACTIVE_CYC"></a>PEVT_AXU_FP_DS_ACTIVE_CYC</TD>   
   
 * <TD style="text-align:left">AXU FP Divide or Square root in progress cycles</TD>
      
 * <TD title="events attributable to current sw/hw thread">thread</TD>
 
 
 
 * <TD title="low latency access method supported in BGPM Island Mode;  multiplexing supported in BGPM Distributed Mode;  ">olmc</TD>
 * <TD title="Configured to count cycles by default">bc</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title=""></TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">Number of Cycles for a Floatin...</TD> 
 * <TD style="display:none">Number of Cycles for a Floating-Point Divide or Square Root sequence is in progress (incl single-precision). </TD>
 
 * </TR>
 * <TR>
 * <TD>6</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_AXU_DENORM_FLUSH"></a>PEVT_AXU_DENORM_FLUSH</TD>   
   
 * <TD style="text-align:left">AXU Denormal Operand flush</TD>
      
 * <TD title="events attributable to current sw/hw thread">thread</TD>
 
 
 
 * <TD title="low latency access method supported in BGPM Island Mode;  multiplexing supported in BGPM Distributed Mode;  ">olmc</TD>
 * <TD title="Single cycle event, cycles and events are synonymous">s</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title=""></TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">A B operand of a Floating Poin...</TD> 
 * <TD style="display:none">A B operand of a Floating Point instruction caused a Denormal Operand flush2ucode. Microcode prenormalization sequence will follow.  </TD>
 
 * </TR>
 * <TR>
 * <TD>7</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_AXU_UCODE_OPS_COMMIT"></a>PEVT_AXU_UCODE_OPS_COMMIT</TD>   
   
 * <TD style="text-align:left">AXU uCode Operations Committed</TD>
      
 * <TD title="events attributable to current sw/hw thread">thread</TD>
 
 
 
 * <TD title="low latency access method supported in BGPM Island Mode;  multiplexing supported in BGPM Distributed Mode;  ">olmc</TD>
 * <TD title="Single cycle event, cycles and events are synonymous">s</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title="Bgpm validated - counts confidently verified in controlled bgpm test.">v</TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">A valid AXU ucode operation is...</TD> 
 * <TD style="display:none">A valid AXU ucode operation is committed past the last flush point. This does not include the original AXU instruction, but only the operations from the expanded ucode sequence </TD>
 
 * </TR>
 * <TR>
 * <TD>8</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_AXU_FP_EXCEPT"></a>PEVT_AXU_FP_EXCEPT</TD>   
   
 * <TD style="text-align:left">AXU Floating Point Exception</TD>
      
 * <TD title="events attributable to current sw/hw thread">thread</TD>
 
 
 
 * <TD title="low latency access method supported in BGPM Island Mode;  multiplexing supported in BGPM Distributed Mode;  ">olmc</TD>
 * <TD title="Only suitable for counting numbers of events">e</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title=""></TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">FP Exception - FX bit of the F...</TD> 
 * <TD style="display:none">FP Exception - FX bit of the FPSCR </TD>
 
 * </TR>
 * <TR>
 * <TD>9</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_AXU_FP_ENAB_EXCEPT"></a>PEVT_AXU_FP_ENAB_EXCEPT</TD>   
   
 * <TD style="text-align:left">AXU Floating Point Enabled Exception</TD>
      
 * <TD title="events attributable to current sw/hw thread">thread</TD>
 
 
 
 * <TD title="low latency access method supported in BGPM Island Mode;  multiplexing supported in BGPM Distributed Mode;  ">olmc</TD>
 * <TD title="Only suitable for counting numbers of events">e</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title=""></TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">FP Enabled Exception - FEX bit...</TD> 
 * <TD style="display:none">FP Enabled Exception - FEX bit of the FPSCR </TD>
 
 * </TR><TR><TD colspan="11" style="text-align:left"><a class="anchor" id="a_instruction_unit"></a><B><I>Instruction Unit Events</I></B></TD></TR>
 * <TR>
 * <TD>10</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_IU_IL1_MISS"></a>PEVT_IU_IL1_MISS</TD>   
   
 * <TD style="text-align:left">IL1 Miss</TD>
      
 * <TD title="events attributable to current sw/hw thread">thread</TD>
 
 
 
 * <TD title="low latency access method supported in BGPM Island Mode;  multiplexing supported in BGPM Distributed Mode;  ">olmc</TD>
 * <TD title="Configured to count number of event by default">be</TD>
 
 * <TD style="text-align:left">PAPI_L1_ICM</TD>
   
 * <TD title="Bgpm validated - counts confidently verified in controlled bgpm test.">v</TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">A thread is waiting for a relo...</TD> 
 * <TD style="display:none">A thread is waiting for a reload from the L2. <br></br>-Not when cache inhibited.<br></br>-Not when thread held off for a reload that another thread is waiting for. <br></br>-Still counts even if flush has occurred. </TD>
 
 * </TR>
 * <TR>
 * <TD>11</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_IU_IL1_MISS_CYC"></a>PEVT_IU_IL1_MISS_CYC</TD>   
   
 * <TD style="text-align:left">IL1 Miss cycles</TD>
      
 * <TD title="events attributable to current sw/hw thread">thread</TD>
 
 
 
 * <TD title="low latency access method supported in BGPM Island Mode;  multiplexing supported in BGPM Distributed Mode;  ">olmc</TD>
 * <TD title="Configured to count cycles by default">bc</TD>
 
 * <TD style="text-align:left">PAPI_L1_ICM</TD>
   
 * <TD title="Bgpm validated - counts confidently verified in controlled bgpm test.">v</TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">Number of Cycles for a thread ...</TD> 
 * <TD style="display:none">Number of Cycles for a thread is waiting for a reload from the L2. <br></br>-Not when cache inhibited.<br></br>-Not when thread held off for a reload that another thread is waiting for. <br></br>-Still counts even if flush has occurred. </TD>
 
 * </TR>
 * <TR>
 * <TD>12</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_IU_IL1_RELOADS_DROPPED"></a>PEVT_IU_IL1_RELOADS_DROPPED</TD>   
   
 * <TD style="text-align:left">IL1 Reloads Dropped</TD>
      
 * <TD title="events attributable to current sw/hw thread">thread</TD>
 
 
 
 * <TD title="low latency access method supported in BGPM Island Mode;  multiplexing supported in BGPM Distributed Mode;  ">olmc</TD>
 * <TD title="Only suitable for counting numbers of events">e</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title=""></TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">Number of times a reload from ...</TD> 
 * <TD style="display:none">Number of times a reload from the L2 is dropped, per thread <br></br>-Not when cache inhibited <br></br>-Does not count when not loading cache due to a back invalidate to that address </TD>
 
 * </TR>
 * <TR>
 * <TD>13</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_IU_RELOAD_COLLISIONS"></a>PEVT_IU_RELOAD_COLLISIONS</TD>   
   
 * <TD style="text-align:left">Reload Collisions</TD>
      
 * <TD title="events attributable to current sw/hw thread">thread</TD>
 
 
 
 * <TD title="low latency access method supported in BGPM Island Mode;  multiplexing supported in BGPM Distributed Mode;  ">olmc</TD>
 * <TD title="Configured to count number of event by default">be</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title=""></TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">A ready thread is held off due...</TD> 
 * <TD style="display:none">A ready thread is held off due to the L1 Cache being reloaded <br></br>-Could occur on multiple threads per cycle </TD>
 
 * </TR>
 * <TR>
 * <TD>14</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_IU_RELOAD_COLLISIONS_CYC"></a>PEVT_IU_RELOAD_COLLISIONS_CYC</TD>   
   
 * <TD style="text-align:left">Reload Collisions cycles</TD>
      
 * <TD title="events attributable to current sw/hw thread">thread</TD>
 
 
 
 * <TD title="low latency access method supported in BGPM Island Mode;  multiplexing supported in BGPM Distributed Mode;  ">olmc</TD>
 * <TD title="Configured to count cycles by default">bc</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title=""></TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">Number of Cycles for a ready t...</TD> 
 * <TD style="display:none">Number of Cycles for a ready thread is held off due to the L1 Cache being reloaded <br></br>-Could occur on multiple threads per cycle </TD>
 
 * </TR>
 * <TR>
 * <TD>15</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_IU_IU0_REDIR_CYC"></a>PEVT_IU_IU0_REDIR_CYC</TD>   
   
 * <TD style="text-align:left">IU0 Redirected cycles</TD>
      
 * <TD title="events attributable to current sw/hw thread">thread</TD>
 
 
 
 * <TD title="low latency access method supported in BGPM Island Mode;  multiplexing supported in BGPM Distributed Mode;  ">olmc</TD>
 * <TD title="Only suitable for counting cycles">c</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title=""></TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">Cycles IU0 is flushed for any ...</TD> 
 * <TD style="display:none">Cycles IU0 is flushed for any reason (XU, UC, BP, etc.) </TD>
 
 * </TR>
 * <TR>
 * <TD>16</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_IU_IERAT_MISS"></a>PEVT_IU_IERAT_MISS</TD>   
   
 * <TD style="text-align:left">IERAT Miss</TD>
      
 * <TD title="events attributable to current sw/hw thread">thread</TD>
 
 
 
 * <TD title="low latency access method supported in BGPM Island Mode;  multiplexing supported in BGPM Distributed Mode;  ">olmc</TD>
 * <TD title="Configured to count number of event by default">be</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title=""></TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">IERAT Miss occurrence...</TD> 
 * <TD style="display:none">IERAT Miss occurrence<br></br>-Can only occur on one thread per cycle </TD>
 
 * </TR>
 * <TR>
 * <TD>17</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_IU_IERAT_MISS_CYC"></a>PEVT_IU_IERAT_MISS_CYC</TD>   
   
 * <TD style="text-align:left">IERAT Miss cycles</TD>
      
 * <TD title="events attributable to current sw/hw thread">thread</TD>
 
 
 
 * <TD title="low latency access method supported in BGPM Island Mode;  multiplexing supported in BGPM Distributed Mode;  ">olmc</TD>
 * <TD title="Configured to count cycles by default">bc</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title=""></TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">Number of Cycles for IERAT Mis...</TD> 
 * <TD style="display:none">Number of Cycles for IERAT Miss occurrence<br></br>-Can only occur on one thread per cycle </TD>
 
 * </TR>
 * <TR>
 * <TD>18</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_IU_ICACHE_FETCH"></a>PEVT_IU_ICACHE_FETCH</TD>   
   
 * <TD style="text-align:left">ICache Fetch</TD>
      
 * <TD title="events attributable to current sw/hw thread">thread</TD>
 
 
 
 * <TD title="low latency access method supported in BGPM Island Mode;  multiplexing supported in BGPM Distributed Mode;  ">olmc</TD>
 * <TD title="Configured to count number of event by default">be</TD>
 
 * <TD style="text-align:left">PAPI_L1_ICR</TD>
   
 * <TD title=""></TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">ICache read completes for inst...</TD> 
 * <TD style="display:none">ICache read completes for instruction <br></br>-Does not count if flushed before IU2 <br></br>-Counts whether cache hit or miss <br></br>-Can only occur on one thread per cycle </TD>
 
 * </TR>
 * <TR>
 * <TD>19</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_IU_ICACHE_FETCH_CYC"></a>PEVT_IU_ICACHE_FETCH_CYC</TD>   
   
 * <TD style="text-align:left">ICache Fetch cycles</TD>
      
 * <TD title="events attributable to current sw/hw thread">thread</TD>
 
 
 
 * <TD title="low latency access method supported in BGPM Island Mode;  multiplexing supported in BGPM Distributed Mode;  ">olmc</TD>
 * <TD title="Configured to count cycles by default">bc</TD>
 
 * <TD style="text-align:left">PAPI_L1_ICR</TD>
   
 * <TD title=""></TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">Number of Cycles for ICache re...</TD> 
 * <TD style="display:none">Number of Cycles for ICache read completes for instruction <br></br>-Does not count if flushed before IU2 <br></br>-Counts whether cache hit or miss <br></br>-Can only occur on one thread per cycle </TD>
 
 * </TR>
 * <TR>
 * <TD>20</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_IU_INSTR_FETCHED"></a>PEVT_IU_INSTR_FETCHED</TD>   
   
 * <TD style="text-align:left">Instructions Fetched</TD>
      
 * <TD title="events attributable to current sw/hw thread">thread</TD>
 
 
 
 * <TD title="low latency access method supported in BGPM Island Mode;  multiplexing supported in BGPM Distributed Mode;  ">olmc</TD>
 * <TD title="Configured to count number of event by default">be</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title=""></TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">Instructions fetched, divided ...</TD> 
 * <TD style="display:none">Instructions fetched, divided by 4 (only counts every 4 instructions)<br></br>-Uses a counter so fetches of 1, 2, or 3 instructions are not lost<br></br>-Includes CI=0 or 1, hit or miss (any instruction that comes through IU2) </TD>
 
 * </TR>
 * <TR>
 * <TD>21</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_IU_INSTR_FETCHED_CYC"></a>PEVT_IU_INSTR_FETCHED_CYC</TD>   
   
 * <TD style="text-align:left">Instructions Fetched cycles</TD>
      
 * <TD title="events attributable to current sw/hw thread">thread</TD>
 
 
 
 * <TD title="low latency access method supported in BGPM Island Mode;  multiplexing supported in BGPM Distributed Mode;  ">olmc</TD>
 * <TD title="Configured to count cycles by default">bc</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title=""></TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">Number of Cycles for instructi...</TD> 
 * <TD style="display:none">Number of Cycles for instructions fetched, divided by 4 (only counts every 4 instructions)<br></br>-Uses a counter so fetches of 1, 2, or 3 instructions are not lost<br></br>-Includes CI=0 or 1, hit or miss (any instruction that comes through IU2) </TD>
 
 * </TR>
 * <TR>
 * <TD>22</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_IU_RSV_ANY_L2_BACK_INV"></a>PEVT_IU_RSV_ANY_L2_BACK_INV</TD>   
   
 * <TD style="text-align:left">reserved any L2 Back Invalidates</TD>
      
 * <TD title="events attributable to current sw/hw thread">thread</TD>
 
 
 
 * <TD title="low latency access method supported in BGPM Island Mode;  multiplexing supported in BGPM Distributed Mode;  ">olmc</TD>
 * <TD title="Configured to count number of event by default">be</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title=""></TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">Back invalidate from L2...</TD> 
 * <TD style="display:none">Back invalidate from L2<br></br>-Per core, not per thread </TD>
 
 * </TR>
 * <TR>
 * <TD>23</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_IU_RSV_ANY_L2_BACK_INV_CYC"></a>PEVT_IU_RSV_ANY_L2_BACK_INV_CYC</TD>   
   
 * <TD style="text-align:left">reserved any L2 Back Invalidates cycles</TD>
      
 * <TD title="events attributable to current sw/hw thread">thread</TD>
 
 
 
 * <TD title="low latency access method supported in BGPM Island Mode;  multiplexing supported in BGPM Distributed Mode;  ">olmc</TD>
 * <TD title="Configured to count cycles by default">bc</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title=""></TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">Number of Cycles for back inva...</TD> 
 * <TD style="display:none">Number of Cycles for back invalidate from L2<br></br>-Per core, not per thread </TD>
 
 * </TR>
 * <TR>
 * <TD>24</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_IU_L2_BACK_INV_HITS"></a>PEVT_IU_L2_BACK_INV_HITS</TD>   
   
 * <TD style="text-align:left">L2 Back Invalidates - Hits</TD>
      
 * <TD title="events attributable to current sw/hw thread">thread</TD>
 
 
 
 * <TD title="low latency access method supported in BGPM Island Mode;  multiplexing supported in BGPM Distributed Mode;  ">olmc</TD>
 * <TD title="Configured to count number of event by default">be</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title=""></TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">Back invalidate from L2, and d...</TD> 
 * <TD style="display:none">Back invalidate from L2, and data was contained within the instruction cache.<br></br>-Per core, not per thread<br></br>-Does not count if hits cacheline for which we are waiting for a reload </TD>
 
 * </TR>
 * <TR>
 * <TD>25</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_IU_L2_BACK_INV_HITS_CYC"></a>PEVT_IU_L2_BACK_INV_HITS_CYC</TD>   
   
 * <TD style="text-align:left">L2 Back Invalidates - Hits cycles</TD>
      
 * <TD title="events attributable to current sw/hw thread">thread</TD>
 
 
 
 * <TD title="low latency access method supported in BGPM Island Mode;  multiplexing supported in BGPM Distributed Mode;  ">olmc</TD>
 * <TD title="Configured to count cycles by default">bc</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title=""></TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">Number of Cycles for back inva...</TD> 
 * <TD style="display:none">Number of Cycles for back invalidate from L2, and data was contained within the instruction cache.<br></br>-Per core, not per thread<br></br>-Does not count if hits cacheline for which we are waiting for a reload </TD>
 
 * </TR>
 * <TR>
 * <TD>26</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_IU_IBUFF_EMPTY"></a>PEVT_IU_IBUFF_EMPTY</TD>   
   
 * <TD style="text-align:left">IBuff Empty</TD>
      
 * <TD title="events attributable to current sw/hw thread">thread</TD>
 
 
 
 * <TD title="low latency access method supported in BGPM Island Mode;  multiplexing supported in BGPM Distributed Mode;  ">olmc</TD>
 * <TD title="Configured to count number of event by default">be</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title="Bgpm validated - counts confidently verified in controlled bgpm test.">v</TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">Instruction buffers are empty ...</TD> 
 * <TD style="display:none">Instruction buffers are empty </TD>
 
 * </TR>
 * <TR>
 * <TD>27</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_IU_IBUFF_EMPTY_CYC"></a>PEVT_IU_IBUFF_EMPTY_CYC</TD>   
   
 * <TD style="text-align:left">IBuff Empty cycles</TD>
      
 * <TD title="events attributable to current sw/hw thread">thread</TD>
 
 
 
 * <TD title="low latency access method supported in BGPM Island Mode;  multiplexing supported in BGPM Distributed Mode;  ">olmc</TD>
 * <TD title="Configured to count cycles by default">bc</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title="Bgpm validated - counts confidently verified in controlled bgpm test.">v</TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">Number of Cycles for instructi...</TD> 
 * <TD style="display:none">Number of Cycles for instruction buffers are empty </TD>
 
 * </TR>
 * <TR>
 * <TD>28</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_IU_IBUFF_FLUSH"></a>PEVT_IU_IBUFF_FLUSH</TD>   
   
 * <TD style="text-align:left">IBuff Flush</TD>
      
 * <TD title="events attributable to current sw/hw thread">thread</TD>
 
 
 
 * <TD title="low latency access method supported in BGPM Island Mode;  multiplexing supported in BGPM Distributed Mode;  ">olmc</TD>
 * <TD title="Configured to count number of event by default">be</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title=""></TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">Instruction buffer address ran...</TD> 
 * <TD style="display:none">Instruction buffer address range mismatch and flush </TD>
 
 * </TR>
 * <TR>
 * <TD>29</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_IU_IBUFF_FLUSH_CYC"></a>PEVT_IU_IBUFF_FLUSH_CYC</TD>   
   
 * <TD style="text-align:left">IBuff Flush cycles</TD>
      
 * <TD title="events attributable to current sw/hw thread">thread</TD>
 
 
 
 * <TD title="low latency access method supported in BGPM Island Mode;  multiplexing supported in BGPM Distributed Mode;  ">olmc</TD>
 * <TD title="Configured to count cycles by default">bc</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title=""></TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">Number of Cycles for instructi...</TD> 
 * <TD style="display:none">Number of Cycles for instruction buffer address range mismatch and flush </TD>
 
 * </TR>
 * <TR>
 * <TD>30</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_IU_IS1_STALL_CYC"></a>PEVT_IU_IS1_STALL_CYC</TD>   
   
 * <TD style="text-align:left">IU5 Stage Register Dependency Stall</TD>
      
 * <TD title="events attributable to current sw/hw thread">thread</TD>
 
 
 
 * <TD title="low latency access method supported in BGPM Island Mode;  multiplexing supported in BGPM Distributed Mode;  ">olmc</TD>
 * <TD title="Only suitable for counting cycles">c</TD>
 
 * <TD style="text-align:left">PAPI_RES_STL</TD>
   
 * <TD title="Bgpm validated - counts confidently verified in controlled bgpm test.">v</TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">Register Dependency Stall ...</TD> 
 * <TD style="display:none">Register Dependency Stall <br></br>-any stall due to dependencies between instructions (IU5 stage).<br></br>-includes Issue Stage Stalls (IU6) </TD>
 
 * </TR>
 * <TR>
 * <TD>31</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_IU_IS2_STALL_CYC"></a>PEVT_IU_IS2_STALL_CYC</TD>   
   
 * <TD style="text-align:left">IU6 Instruction Issue Stall</TD>
      
 * <TD title="events attributable to current sw/hw thread">thread</TD>
 
 
 
 * <TD title="low latency access method supported in BGPM Island Mode;  multiplexing supported in BGPM Distributed Mode;  ">olmc</TD>
 * <TD title="Only suitable for counting cycles">c</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title="Bgpm validated - counts confidently verified in controlled bgpm test.">v</TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">Instruction Issue Stall ...</TD> 
 * <TD style="display:none">Instruction Issue Stall <br></br>Each execution unig (XU or AXU) can receive one instruction from any of the 4 threads each cycle. This stall occurs while an instruction is ready, but another ready thread takes a turn (Stage IU6) </TD>
 
 * </TR>
 * <TR>
 * <TD>32</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_IU_BARRIER_OP_STALL_CYC"></a>PEVT_IU_BARRIER_OP_STALL_CYC</TD>   
   
 * <TD style="text-align:left">Barrier Op Stall</TD>
      
 * <TD title="events attributable to current sw/hw thread">thread</TD>
 
 
 
 * <TD title="low latency access method supported in BGPM Island Mode;  multiplexing supported in BGPM Distributed Mode;  ">olmc</TD>
 * <TD title="Only suitable for counting cycles">c</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title="Caveots - discovered some conditions which seem to alter this event from the designed intent">c</TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">Stalled pending barrier op res...</TD> 
 * <TD style="display:none">Stalled pending barrier op resolution (each uCoded instruction will also cause a barrier operation).<br></br>-Caveot. The results from this event may be suspect: The barrier stall cycles include event from sync instruction, pls stall due to AIX and XU uCoded instructions.  </TD>
 
 * </TR>
 * <TR>
 * <TD>33</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_IU_SLOW_SPR_ACCESS_CYC"></a>PEVT_IU_SLOW_SPR_ACCESS_CYC</TD>   
   
 * <TD style="text-align:left">Slow SPR Access</TD>
      
 * <TD title="events attributable to current sw/hw thread">thread</TD>
 
 
 
 * <TD title="low latency access method supported in BGPM Island Mode;  multiplexing supported in BGPM Distributed Mode;  ">olmc</TD>
 * <TD title="Only suitable for counting cycles">c</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title=""></TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">Stalled for MFSPR/MTSPR ops th...</TD> 
 * <TD style="display:none">Stalled for MFSPR/MTSPR ops that move across the slow SPR bus. </TD>
 
 * </TR>
 * <TR>
 * <TD>34</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_IU_RAW_DEP_HIT_CYC"></a>PEVT_IU_RAW_DEP_HIT_CYC</TD>   
   
 * <TD style="text-align:left">RAW Dep Hit</TD>
      
 * <TD title="events attributable to current sw/hw thread">thread</TD>
 
 
 
 * <TD title="low latency access method supported in BGPM Island Mode;  multiplexing supported in BGPM Distributed Mode;  ">olmc</TD>
 * <TD title="Only suitable for counting cycles">c</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title=""></TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">Stalled for Read-After-Write d...</TD> 
 * <TD style="display:none">Stalled for Read-After-Write dependency </TD>
 
 * </TR>
 * <TR>
 * <TD>35</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_IU_WAW_DEP_HIT_CYC"></a>PEVT_IU_WAW_DEP_HIT_CYC</TD>   
   
 * <TD style="text-align:left">WAW Dep Hit</TD>
      
 * <TD title="events attributable to current sw/hw thread">thread</TD>
 
 
 
 * <TD title="low latency access method supported in BGPM Island Mode;  multiplexing supported in BGPM Distributed Mode;  ">olmc</TD>
 * <TD title="Only suitable for counting cycles">c</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title=""></TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">Stalled for Write-After-Write ...</TD> 
 * <TD style="display:none">Stalled for Write-After-Write dependency </TD>
 
 * </TR>
 * <TR>
 * <TD>36</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_IU_SYNC_DEP_HIT_CYC"></a>PEVT_IU_SYNC_DEP_HIT_CYC</TD>   
   
 * <TD style="text-align:left">Sync Dep Hit</TD>
      
 * <TD title="events attributable to current sw/hw thread">thread</TD>
 
 
 
 * <TD title="low latency access method supported in BGPM Island Mode;  multiplexing supported in BGPM Distributed Mode;  ">olmc</TD>
 * <TD title="Only suitable for counting cycles">c</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title=""></TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">Stalled for SYNC/ISYNC/TLBSYNC...</TD> 
 * <TD style="display:none">Stalled for SYNC/ISYNC/TLBSYNC instructions </TD>
 
 * </TR>
 * <TR>
 * <TD>37</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_IU_SPR_DEP_HIT_CYC"></a>PEVT_IU_SPR_DEP_HIT_CYC</TD>   
   
 * <TD style="text-align:left">SPR Dep Hit</TD>
      
 * <TD title="events attributable to current sw/hw thread">thread</TD>
 
 
 
 * <TD title="low latency access method supported in BGPM Island Mode;  multiplexing supported in BGPM Distributed Mode;  ">olmc</TD>
 * <TD title="Only suitable for counting cycles">c</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title=""></TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">Stalled for SPR use or update ...</TD> 
 * <TD style="display:none">Stalled for SPR use or update dependency </TD>
 
 * </TR>
 * <TR>
 * <TD>38</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_IU_AXU_DEP_HIT_CYC"></a>PEVT_IU_AXU_DEP_HIT_CYC</TD>   
   
 * <TD style="text-align:left">AXU Dep Hit</TD>
      
 * <TD title="events attributable to current sw/hw thread">thread</TD>
 
 
 
 * <TD title="low latency access method supported in BGPM Island Mode;  multiplexing supported in BGPM Distributed Mode;  ">olmc</TD>
 * <TD title="Only suitable for counting cycles">c</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title="Bgpm validated - counts confidently verified in controlled bgpm test.">v</TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">Stalled for any AXU dependency...</TD> 
 * <TD style="display:none">Stalled for any AXU dependency (excludes IS2 stall) </TD>
 
 * </TR>
 * <TR>
 * <TD>39</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_IU_FXU_DEP_HIT_CYC"></a>PEVT_IU_FXU_DEP_HIT_CYC</TD>   
   
 * <TD style="text-align:left">FXU Dep Hit</TD>
      
 * <TD title="events attributable to current sw/hw thread">thread</TD>
 
 
 
 * <TD title="low latency access method supported in BGPM Island Mode;  multiplexing supported in BGPM Distributed Mode;  ">olmc</TD>
 * <TD title="Only suitable for counting cycles">c</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title="Caveots - discovered some conditions which seem to alter this event from the designed intent">c</TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">Stalled for any FXU dependency...</TD> 
 * <TD style="display:none">Stalled for any FXU dependency (excludes IS2 stall)<br></br>Caveat: AXU ucode Barrier stall cycles included, but XU uCode barrier stall cycles are not. Must subtract 2*PEVT_INST_QFPU_UCODE to get more accurate representation of FXU stalls. </TD>
 
 * </TR>
 * <TR>
 * <TD>40</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_IU_AXU_FXU_DEP_HIT_CYC"></a>PEVT_IU_AXU_FXU_DEP_HIT_CYC</TD>   
   
 * <TD style="text-align:left">Cycles the FP unit is stalled</TD>
      
 * <TD title="events attributable to current sw/hw thread">thread</TD>
 
 
 
 * <TD title="low latency access method supported in BGPM Island Mode;  multiplexing supported in BGPM Distributed Mode;  ">olmc</TD>
 * <TD title="Only suitable for counting cycles">c</TD>
 
 * <TD style="text-align:left">PAPI_FP_STAL</TD>
   
 * <TD title="Bgpm validated - counts confidently verified in controlled bgpm test.">v</TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">Stalled for any AXU/FXU depend...</TD> 
 * <TD style="display:none">Stalled for any AXU/FXU dependency (excludes IS2 stall) </TD>
 
 * </TR>
 * <TR>
 * <TD>41</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_IU_AXU_ISSUE_PRI_LOSS_CYC"></a>PEVT_IU_AXU_ISSUE_PRI_LOSS_CYC</TD>   
   
 * <TD style="text-align:left">AXU Issue Priority Loss</TD>
      
 * <TD title="events attributable to current sw/hw thread">thread</TD>
 
 
 
 * <TD title="low latency access method supported in BGPM Island Mode;  multiplexing supported in BGPM Distributed Mode;  ">olmc</TD>
 * <TD title="Only suitable for counting cycles">c</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title=""></TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">AXU instruction that is valid ...</TD> 
 * <TD style="display:none">AXU instruction that is valid in issue and another thread issues because it has priority (see IS2 Stall for combined AXU/FXU issue priority loss) </TD>
 
 * </TR>
 * <TR>
 * <TD>42</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_IU_FXU_ISSUE_PRI_LOSS_CYC"></a>PEVT_IU_FXU_ISSUE_PRI_LOSS_CYC</TD>   
   
 * <TD style="text-align:left">FXU Issue Priority Loss</TD>
      
 * <TD title="events attributable to current sw/hw thread">thread</TD>
 
 
 
 * <TD title="low latency access method supported in BGPM Island Mode;  multiplexing supported in BGPM Distributed Mode;  ">olmc</TD>
 * <TD title="Only suitable for counting cycles">c</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title=""></TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">FXU instruction that is valid ...</TD> 
 * <TD style="display:none">FXU instruction that is valid in issue and another thread issues because it has priority (see IS2 Stall for combined AXU/FXU issue priority loss) </TD>
 
 * </TR>
 * <TR>
 * <TD>43</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_IU_FXU_ISSUE_COUNT"></a>PEVT_IU_FXU_ISSUE_COUNT</TD>   
   
 * <TD style="text-align:left">FXU Issue Count</TD>
      
 * <TD title="events attributable to current sw/hw thread">thread</TD>
 
 
 
 * <TD title="low latency access method supported in BGPM Island Mode;  multiplexing supported in BGPM Distributed Mode;  ">olmc</TD>
 * <TD title="Single cycle event which can occur on consecutive cycles; cycles and events are synonymous">v</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title=""></TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">FXU instructions issued per th...</TD> 
 * <TD style="display:none">FXU instructions issued per thread. AXU Issues is broken; Instead use axu issues = total issues - fxu issues </TD>
 
 * </TR>
 * <TR>
 * <TD>44</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_IU_TOT_ISSUE_COUNT"></a>PEVT_IU_TOT_ISSUE_COUNT</TD>   
   
 * <TD style="text-align:left">Total Issue Count</TD>
      
 * <TD title="events attributable to current sw/hw thread">thread</TD>
 
 
 
 * <TD title="low latency access method supported in BGPM Island Mode;  multiplexing supported in BGPM Distributed Mode;  ">olmc</TD>
 * <TD title="Single cycle event which can occur on consecutive cycles; cycles and events are synonymous">v</TD>
 
 * <TD style="text-align:left">PAPI_TOT_IIS</TD>
   
 * <TD title=""></TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">all instructions issued per th...</TD> 
 * <TD style="display:none">all instructions issued per thread </TD>
 
 * </TR><TR><TD colspan="11" style="text-align:left"><a class="anchor" id="a_execution_unit"></a><B><I>Execution Unit Events</I></B></TD></TR>
 * <TR>
 * <TD>45</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_XU_PROC_BUSY"></a>PEVT_XU_PROC_BUSY</TD>   
   
 * <TD style="text-align:left">Processor Busy</TD>
      
 * <TD title="events are core-wide, and only one thread may count on the core when this event is used">core-shared</TD>
 
 
 
 * <TD title="low latency access method supported in BGPM Island Mode;  multiplexing supported in BGPM Distributed Mode;  ">olmc</TD>
 * <TD title="Only suitable for counting cycles">c</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title=""></TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">Cycles that any thread is runn...</TD> 
 * <TD style="display:none">Cycles that any thread is running on the core.  </TD>
 
 * </TR>
 * <TR>
 * <TD>46</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_XU_BR_COMMIT_CORE"></a>PEVT_XU_BR_COMMIT_CORE</TD>   
   
 * <TD style="text-align:left">Branch Commit</TD>
      
 * <TD title="events are core-wide, and only one thread may count on the core when this event is used">core-shared</TD>
 
 
 
 * <TD title="low latency access method supported in BGPM Island Mode;  multiplexing supported in BGPM Distributed Mode;  ">olmc</TD>
 * <TD title="Only suitable for counting cycles">c</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title=""></TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">Number of Branches committed  ...</TD> 
 * <TD style="display:none">Number of Branches committed  </TD>
 
 * </TR>
 * <TR>
 * <TD>47</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_XU_BR_MISPRED_COMMIT_CORE"></a>PEVT_XU_BR_MISPRED_COMMIT_CORE</TD>   
   
 * <TD style="text-align:left">Branch Mispredict Commit</TD>
      
 * <TD title="events are core-wide, and only one thread may count on the core when this event is used">core-shared</TD>
 
 
 
 * <TD title="low latency access method supported in BGPM Island Mode;  multiplexing supported in BGPM Distributed Mode;  ">olmc</TD>
 * <TD title="Single cycle event, cycles and events are synonymous">s</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title=""></TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">Number of mispredicted Branche...</TD> 
 * <TD style="display:none">Number of mispredicted Branches committed (does not include target address mispredicted)  </TD>
 
 * </TR>
 * <TR>
 * <TD>48</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_XU_BR_TARG_ADDR_MISPRED_COMMIT_CORE"></a>PEVT_XU_BR_TARG_ADDR_MISPRED_COMMIT_CORE</TD>   
   
 * <TD style="text-align:left">Branch Target Address Mispredict Commit</TD>
      
 * <TD title="events are core-wide, and only one thread may count on the core when this event is used">core-shared</TD>
 
 
 
 * <TD title="low latency access method supported in BGPM Island Mode;  multiplexing supported in BGPM Distributed Mode;  ">olmc</TD>
 * <TD title="Single cycle event, cycles and events are synonymous">s</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title=""></TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">Number of Branch Target addres...</TD> 
 * <TD style="display:none">Number of Branch Target addresses mispredicted committed  </TD>
 
 * </TR>
 * <TR>
 * <TD>49</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_XU_THRD_RUNNING"></a>PEVT_XU_THRD_RUNNING</TD>   
   
 * <TD style="text-align:left">Thread Running</TD>
      
 * <TD title="events attributable to current sw/hw thread">thread</TD>
 
 
 
 * <TD title="low latency access method supported in BGPM Island Mode;  multiplexing supported in BGPM Distributed Mode;  ">olmc</TD>
 * <TD title="Only suitable for counting cycles">c</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title="Bgpm validated - counts confidently verified in controlled bgpm test.">v</TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">Number of cycles that thread i...</TD> 
 * <TD style="display:none">Number of cycles that thread is in run state.<br></br>only meaningful in BGPM_MODE_HWDISTRIB, since otherwise counters are stopped if SW thread is swapped out </TD>
 
 * </TR>
 * <TR>
 * <TD>50</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_XU_TIMEBASE_TICK"></a>PEVT_XU_TIMEBASE_TICK</TD>   
   
 * <TD style="text-align:left">Timebase Tick</TD>
      
 * <TD title="events attributable to current sw/hw thread">thread</TD>
 
 
 
 * <TD title="low latency access method supported in BGPM Island Mode;  multiplexing supported in BGPM Distributed Mode;  ">olmc</TD>
 * <TD title="Only suitable for counting cycles">c</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title="Bgpm validated - counts confidently verified in controlled bgpm test.">v</TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">Number of times the timebase h...</TD> 
 * <TD style="display:none">Number of times the timebase has incremented (same as GetBaseTime()) </TD>
 
 * </TR>
 * <TR>
 * <TD>51</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_XU_SPR_READ_COMMIT"></a>PEVT_XU_SPR_READ_COMMIT</TD>   
   
 * <TD style="text-align:left">SPR Read Commit</TD>
      
 * <TD title="events attributable to current sw/hw thread">thread</TD>
 
 
 
 * <TD title="low latency access method supported in BGPM Island Mode;  multiplexing supported in BGPM Distributed Mode;  ">olmc</TD>
 * <TD title="Only suitable for counting cycles">c</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title=""></TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">Number of mfspr, mftb, mfmsr o...</TD> 
 * <TD style="display:none">Number of mfspr, mftb, mfmsr or mfcr instructions committed </TD>
 
 * </TR>
 * <TR>
 * <TD>52</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_XU_SPR_WRITE_COMMIT"></a>PEVT_XU_SPR_WRITE_COMMIT</TD>   
   
 * <TD style="text-align:left">SPR Write Commit</TD>
      
 * <TD title="events attributable to current sw/hw thread">thread</TD>
 
 
 
 * <TD title="low latency access method supported in BGPM Island Mode;  multiplexing supported in BGPM Distributed Mode;  ">olmc</TD>
 * <TD title="Only suitable for counting cycles">c</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title=""></TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">Number of mtspr, mtmsr, mtcrf,...</TD> 
 * <TD style="display:none">Number of mtspr, mtmsr, mtcrf, wrtee, wrteei instructions committed </TD>
 
 * </TR>
 * <TR>
 * <TD>53</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_XU_STALLED_ON_WAITRSV"></a>PEVT_XU_STALLED_ON_WAITRSV</TD>   
   
 * <TD style="text-align:left">Cycles stalled on waitrsv</TD>
      
 * <TD title="events attributable to current sw/hw thread">thread</TD>
 
 
 
 * <TD title="low latency access method supported in BGPM Island Mode;  multiplexing supported in BGPM Distributed Mode;  ">olmc</TD>
 * <TD title="Configured to count number of event by default">be</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title=""></TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">Between commit of waitrsv and ...</TD> 
 * <TD style="display:none">Between commit of waitrsv and wakeup by lost reservation.  </TD>
 
 * </TR>
 * <TR>
 * <TD>54</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_XU_STALLED_ON_WAITRSV_CYC"></a>PEVT_XU_STALLED_ON_WAITRSV_CYC</TD>   
   
 * <TD style="text-align:left">Cycles stalled on waitrsv cycles</TD>
      
 * <TD title="events attributable to current sw/hw thread">thread</TD>
 
 
 
 * <TD title="low latency access method supported in BGPM Island Mode;  multiplexing supported in BGPM Distributed Mode;  ">olmc</TD>
 * <TD title="Configured to count cycles by default">bc</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title=""></TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">Number of Cycles for between c...</TD> 
 * <TD style="display:none">Number of Cycles for between commit of waitrsv and wakeup by lost reservation.  </TD>
 
 * </TR>
 * <TR>
 * <TD>55</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_XU_EXT_INT_ASSERT"></a>PEVT_XU_EXT_INT_ASSERT</TD>   
   
 * <TD style="text-align:left">External Interrupt Asserted</TD>
      
 * <TD title="events attributable to current sw/hw thread">thread</TD>
 
 
 
 * <TD title="low latency access method supported in BGPM Island Mode;  multiplexing supported in BGPM Distributed Mode;  ">olmc</TD>
 * <TD title="Only suitable for counting cycles">c</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title=""></TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">Number of cycles the external ...</TD> 
 * <TD style="display:none">Number of cycles the external interrupt signal is asserted  </TD>
 
 * </TR>
 * <TR>
 * <TD>56</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_XU_CRIT_EXT_INT_ASSERT"></a>PEVT_XU_CRIT_EXT_INT_ASSERT</TD>   
   
 * <TD style="text-align:left">Critical External Interrupt Asserted</TD>
      
 * <TD title="events attributable to current sw/hw thread">thread</TD>
 
 
 
 * <TD title="low latency access method supported in BGPM Island Mode;  multiplexing supported in BGPM Distributed Mode;  ">olmc</TD>
 * <TD title="Only suitable for counting cycles">c</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title=""></TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">Number of cycles the critical ...</TD> 
 * <TD style="display:none">Number of cycles the critical external interrupt signal is asserted  </TD>
 
 * </TR>
 * <TR>
 * <TD>57</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_XU_PERF_MON_INT_ASSERT"></a>PEVT_XU_PERF_MON_INT_ASSERT</TD>   
   
 * <TD style="text-align:left">Performance Monitor Interrupt Asserted</TD>
      
 * <TD title="events attributable to current sw/hw thread">thread</TD>
 
 
 
 * <TD title="low latency access method supported in BGPM Island Mode;  multiplexing supported in BGPM Distributed Mode;  ">olmc</TD>
 * <TD title="Only suitable for counting cycles">c</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title=""></TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">Number of cycles the performan...</TD> 
 * <TD style="display:none">Number of cycles the performance monitor interrupt signal is asserted  </TD>
 
 * </TR>
 * <TR>
 * <TD>58</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_XU_PPC_COMMIT"></a>PEVT_XU_PPC_COMMIT</TD>   
   
 * <TD style="text-align:left">PPC Commit</TD>
      
 * <TD title="events attributable to current sw/hw thread">thread</TD>
 
 
 
 * <TD title="low latency access method supported in BGPM Island Mode;  multiplexing supported in BGPM Distributed Mode;  ">olmc</TD>
 * <TD title="Only suitable for counting cycles">c</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title="Bgpm validated - counts confidently verified in controlled bgpm test.">v</TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">Number of instructions committ...</TD> 
 * <TD style="display:none">Number of instructions committed. uCode sequences count as one instruction. <br></br>- Similar to PEVT_INSTR_ALL except that it may be context sensitive. Includes AXU instructions. </TD>
 
 * </TR>
 * <TR>
 * <TD>59</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_XU_COMMIT"></a>PEVT_XU_COMMIT</TD>   
   
 * <TD style="text-align:left">XU Operations Committed</TD>
      
 * <TD title="events attributable to current sw/hw thread">thread</TD>
 
 
 
 * <TD title="low latency access method supported in BGPM Island Mode;  multiplexing supported in BGPM Distributed Mode;  ">olmc</TD>
 * <TD title="Only suitable for counting cycles">c</TD>
 
 * <TD style="text-align:left">PAPI_TOT_INS</TD>
   
 * <TD title="Bgpm validated - counts confidently verified in controlled bgpm test.">v</TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">Number of XU operations commit...</TD> 
 * <TD style="display:none">Number of XU operations committed.<br></br>-Includes every committed sub-operation of an XU uCode sequence.<br></br>-Includes one committed operation per AXU uCode sequence;<br></br>-Includes all XU committed instructions </TD>
 
 * </TR>
 * <TR>
 * <TD>60</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_XU_UCODE_COMMIT"></a>PEVT_XU_UCODE_COMMIT</TD>   
   
 * <TD style="text-align:left">uCode Sequence Commit</TD>
      
 * <TD title="events attributable to current sw/hw thread">thread</TD>
 
 
 
 * <TD title="low latency access method supported in BGPM Island Mode;  multiplexing supported in BGPM Distributed Mode;  ">olmc</TD>
 * <TD title="Single cycle event, cycles and events are synonymous">s</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title="Bgpm validated - counts confidently verified in controlled bgpm test.">v</TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">Number of uCode instructions c...</TD> 
 * <TD style="display:none">Number of uCode instructions committed (includes QFPU ucode instructions committed). Does not include uCode sub-operations </TD>
 
 * </TR>
 * <TR>
 * <TD>61</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_XU_ANY_FLUSH"></a>PEVT_XU_ANY_FLUSH</TD>   
   
 * <TD style="text-align:left">Any Flush</TD>
      
 * <TD title="events attributable to current sw/hw thread">thread</TD>
 
 
 
 * <TD title="low latency access method supported in BGPM Island Mode;  multiplexing supported in BGPM Distributed Mode;  ">olmc</TD>
 * <TD title="Only suitable for counting cycles">c</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title=""></TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">Number of cycles flush is asse...</TD> 
 * <TD style="display:none">Number of cycles flush is asserted to the IU  </TD>
 
 * </TR>
 * <TR>
 * <TD>62</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_XU_BR_COMMIT"></a>PEVT_XU_BR_COMMIT</TD>   
   
 * <TD style="text-align:left">Branch Commit</TD>
      
 * <TD title="events attributable to current sw/hw thread">thread</TD>
 
 
 
 * <TD title="low latency access method supported in BGPM Island Mode;  multiplexing supported in BGPM Distributed Mode;  ">olmc</TD>
 * <TD title="Only suitable for counting cycles">c</TD>
 
 * <TD style="text-align:left">PAPI_BR_INS</TD>
   
 * <TD title=""></TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">Number of Branches committed  ...</TD> 
 * <TD style="display:none">Number of Branches committed  </TD>
 
 * </TR>
 * <TR>
 * <TD>63</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_XU_BR_MISPRED_COMMIT"></a>PEVT_XU_BR_MISPRED_COMMIT</TD>   
   
 * <TD style="text-align:left">Branch Mispredict Commit</TD>
      
 * <TD title="events attributable to current sw/hw thread">thread</TD>
 
 
 
 * <TD title="low latency access method supported in BGPM Island Mode;  multiplexing supported in BGPM Distributed Mode;  ">olmc</TD>
 * <TD title="Single cycle event, cycles and events are synonymous">s</TD>
 
 * <TD style="text-align:left">PAPI_BR_MSP</TD>
   
 * <TD title=""></TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">Number of mispredicted Branche...</TD> 
 * <TD style="display:none">Number of mispredicted Branches committed (does not include target address mispredicted)  </TD>
 
 * </TR>
 * <TR>
 * <TD>64</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_XU_BR_TAKEN_COMMIT"></a>PEVT_XU_BR_TAKEN_COMMIT</TD>   
   
 * <TD style="text-align:left">Branch Taken Commit</TD>
      
 * <TD title="events attributable to current sw/hw thread">thread</TD>
 
 
 
 * <TD title="low latency access method supported in BGPM Island Mode;  multiplexing supported in BGPM Distributed Mode;  ">olmc</TD>
 * <TD title="Only suitable for counting cycles">c</TD>
 
 * <TD style="text-align:left">PAPI_BR_TKN</TD>
   
 * <TD title=""></TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">Number of taken branches commi...</TD> 
 * <TD style="display:none">Number of taken branches committed </TD>
 
 * </TR>
 * <TR>
 * <TD>65</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_XU_BR_TARG_ADDR_MISPRED_COMMIT"></a>PEVT_XU_BR_TARG_ADDR_MISPRED_COMMIT</TD>   
   
 * <TD style="text-align:left">Branch Target Address Mispredict Commit</TD>
      
 * <TD title="events attributable to current sw/hw thread">thread</TD>
 
 
 
 * <TD title="low latency access method supported in BGPM Island Mode;  multiplexing supported in BGPM Distributed Mode;  ">olmc</TD>
 * <TD title="Single cycle event, cycles and events are synonymous">s</TD>
 
 * <TD style="text-align:left">PAPI_BTAC_M</TD>
   
 * <TD title=""></TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">Number of Branch Target addres...</TD> 
 * <TD style="display:none">Number of Branch Target addresses mispredicted committed  </TD>
 
 * </TR>
 * <TR>
 * <TD>66</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_XU_MULT_DIV_COLLISION"></a>PEVT_XU_MULT_DIV_COLLISION</TD>   
   
 * <TD style="text-align:left">Mult/Div Collision</TD>
      
 * <TD title="events attributable to current sw/hw thread">thread</TD>
 
 
 
 * <TD title="low latency access method supported in BGPM Island Mode;  multiplexing supported in BGPM Distributed Mode;  ">olmc</TD>
 * <TD title="Only suitable for counting cycles">c</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title=""></TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">Number of Multiply/Divide reso...</TD> 
 * <TD style="display:none">Number of Multiply/Divide resource collisions  </TD>
 
 * </TR>
 * <TR>
 * <TD>67</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_XU_EXT_INT_PEND"></a>PEVT_XU_EXT_INT_PEND</TD>   
   
 * <TD style="text-align:left">External Interrupt Pending</TD>
      
 * <TD title="events attributable to current sw/hw thread">thread</TD>
 
 
 
 * <TD title="low latency access method supported in BGPM Island Mode;  multiplexing supported in BGPM Distributed Mode;  ">olmc</TD>
 * <TD title="Only suitable for counting cycles">c</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title=""></TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">Count number of cycles the int...</TD> 
 * <TD style="display:none">Count number of cycles the interrupt signal into the processor is asserted before the completion logic redirects program flow to the interrupt vector  </TD>
 
 * </TR>
 * <TR>
 * <TD>68</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_XU_CRIT_EXT_INT_PEND"></a>PEVT_XU_CRIT_EXT_INT_PEND</TD>   
   
 * <TD style="text-align:left">Critical External Interrupt Pending</TD>
      
 * <TD title="events attributable to current sw/hw thread">thread</TD>
 
 
 
 * <TD title="low latency access method supported in BGPM Island Mode;  multiplexing supported in BGPM Distributed Mode;  ">olmc</TD>
 * <TD title="Only suitable for counting cycles">c</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title=""></TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">Count number of cycles the int...</TD> 
 * <TD style="display:none">Count number of cycles the interrupt signal into the processor is asserted before the completion logic redirects program flow to the interrupt vector  </TD>
 
 * </TR>
 * <TR>
 * <TD>69</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_XU_PERF_MON_INT_PEND"></a>PEVT_XU_PERF_MON_INT_PEND</TD>   
   
 * <TD style="text-align:left">Performance Monitor Interrupt Pending</TD>
      
 * <TD title="events attributable to current sw/hw thread">thread</TD>
 
 
 
 * <TD title="low latency access method supported in BGPM Island Mode;  multiplexing supported in BGPM Distributed Mode;  ">olmc</TD>
 * <TD title="Only suitable for counting cycles">c</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title=""></TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">Count number of cycles the int...</TD> 
 * <TD style="display:none">Count number of cycles the interrupt signal into the processor is asserted before the completion logic redirects program flow to the interrupt vector  </TD>
 
 * </TR>
 * <TR>
 * <TD>70</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_XU_RUN_INSTR"></a>PEVT_XU_RUN_INSTR</TD>   
   
 * <TD style="text-align:left">Run Instructions</TD>
      
 * <TD title="events attributable to current sw/hw thread">thread</TD>
 
 
 
 * <TD title="low latency access method supported in BGPM Island Mode;  multiplexing supported in BGPM Distributed Mode;  ">olmc</TD>
 * <TD title="Only suitable for counting cycles">c</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title=""></TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">Number of PPC instruction comm...</TD> 
 * <TD style="display:none">Number of PPC instruction commits while any thread is in run state  </TD>
 
 * </TR>
 * <TR>
 * <TD>71</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_XU_INTS_TAKEN"></a>PEVT_XU_INTS_TAKEN</TD>   
   
 * <TD style="text-align:left">Interrupts Taken</TD>
      
 * <TD title="events are core-wide and events can't directly be attributed to single thread">core</TD>
 
 
 
 * <TD title="low latency access method supported in BGPM Island Mode;  multiplexing supported in BGPM Distributed Mode;  ">olmc</TD>
 * <TD title="Only suitable for counting cycles">c</TD>
 
 * <TD style="text-align:left">PAPI_HW_INT</TD>
   
 * <TD title=""></TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">Number of external, critical e...</TD> 
 * <TD style="display:none">Number of external, critical external, or performance mintor interrupt taken across all threads.<br></br>Does not count more than one per cycle, although up to 4 can occur simultaneously. </TD>
 
 * </TR>
 * <TR>
 * <TD>72</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_XU_EXT_INT_TAKEN"></a>PEVT_XU_EXT_INT_TAKEN</TD>   
   
 * <TD style="text-align:left">External Interrupt Taken</TD>
      
 * <TD title="events attributable to current sw/hw thread">thread</TD>
 
 
 
 * <TD title="low latency access method supported in BGPM Island Mode;  multiplexing supported in BGPM Distributed Mode;  ">olmc</TD>
 * <TD title="Single cycle event, cycles and events are synonymous">s</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title=""></TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">Number of external interrupts ...</TD> 
 * <TD style="display:none">Number of external interrupts taken  </TD>
 
 * </TR>
 * <TR>
 * <TD>73</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_XU_CRIT_EXT_INT_TAKEN"></a>PEVT_XU_CRIT_EXT_INT_TAKEN</TD>   
   
 * <TD style="text-align:left">Critical External Interrupt Taken</TD>
      
 * <TD title="events attributable to current sw/hw thread">thread</TD>
 
 
 
 * <TD title="low latency access method supported in BGPM Island Mode;  multiplexing supported in BGPM Distributed Mode;  ">olmc</TD>
 * <TD title="Single cycle event, cycles and events are synonymous">s</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title=""></TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">Number of critical external in...</TD> 
 * <TD style="display:none">Number of critical external interrupts taken  </TD>
 
 * </TR>
 * <TR>
 * <TD>74</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_XU_PERF_MON_INT_TAKEN"></a>PEVT_XU_PERF_MON_INT_TAKEN</TD>   
   
 * <TD style="text-align:left">Performance Monitor Interrupt Taken</TD>
      
 * <TD title="events attributable to current sw/hw thread">thread</TD>
 
 
 
 * <TD title="low latency access method supported in BGPM Island Mode;  multiplexing supported in BGPM Distributed Mode;  ">olmc</TD>
 * <TD title="Single cycle event, cycles and events are synonymous">s</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title=""></TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">Number of performance monitor ...</TD> 
 * <TD style="display:none">Number of performance monitor interrupts taken  </TD>
 
 * </TR>
 * <TR>
 * <TD>75</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_XU_DOORBELL_INT_TAKEN"></a>PEVT_XU_DOORBELL_INT_TAKEN</TD>   
   
 * <TD style="text-align:left">Doorbell Interrupt Taken</TD>
      
 * <TD title="events attributable to current sw/hw thread">thread</TD>
 
 
 
 * <TD title="low latency access method supported in BGPM Island Mode;  multiplexing supported in BGPM Distributed Mode;  ">olmc</TD>
 * <TD title="Single cycle event, cycles and events are synonymous">s</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title=""></TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">Number of doorbell or doorbell...</TD> 
 * <TD style="display:none">Number of doorbell or doorbell critical interrupts taken  </TD>
 
 * </TR>
 * <TR>
 * <TD>76</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_XU_STCX_FAIL"></a>PEVT_XU_STCX_FAIL</TD>   
   
 * <TD style="text-align:left">stcx fail</TD>
      
 * <TD title="events attributable to current sw/hw thread">thread</TD>
 
 
 
 * <TD title="low latency access method supported in BGPM Island Mode;  multiplexing supported in BGPM Distributed Mode;  ">olmc</TD>
 * <TD title="Single cycle event, cycles and events are synonymous">s</TD>
 
 * <TD style="text-align:left">PAPI_CSR_FAL</TD>
   
 * <TD title=""></TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">Number of failing stwcx./stdcx...</TD> 
 * <TD style="display:none">Number of failing stwcx./stdcx. instructions  </TD>
 
 * </TR>
 * <TR>
 * <TD>77</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_XU_ICSWX_FAILED"></a>PEVT_XU_ICSWX_FAILED</TD>   
   
 * <TD style="text-align:left">icswx failed</TD>
      
 * <TD title="events attributable to current sw/hw thread">thread</TD>
 
 
 
 * <TD title="low latency access method supported in BGPM Island Mode;  multiplexing supported in BGPM Distributed Mode;  ">olmc</TD>
 * <TD title="Single cycle event, cycles and events are synonymous">s</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title=""></TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">Number of failing icswx/icswep...</TD> 
 * <TD style="display:none">Number of failing icswx/icswepx instructions  </TD>
 
 * </TR>
 * <TR>
 * <TD>78</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_XU_ICSWX_COMMIT"></a>PEVT_XU_ICSWX_COMMIT</TD>   
   
 * <TD style="text-align:left">icswx commit</TD>
      
 * <TD title="events attributable to current sw/hw thread">thread</TD>
 
 
 
 * <TD title="low latency access method supported in BGPM Island Mode;  multiplexing supported in BGPM Distributed Mode;  ">olmc</TD>
 * <TD title="Single cycle event, cycles and events are synonymous">s</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title=""></TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">Number of icswx/icswepx instru...</TD> 
 * <TD style="display:none">Number of icswx/icswepx instructions committed  </TD>
 
 * </TR>
 * <TR>
 * <TD>79</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_XU_MULT_DIV_BUSY"></a>PEVT_XU_MULT_DIV_BUSY</TD>   
   
 * <TD style="text-align:left">Mult/Div Busy</TD>
      
 * <TD title="events attributable to current sw/hw thread">thread</TD>
 
 
 
 * <TD title="low latency access method supported in BGPM Island Mode;  multiplexing supported in BGPM Distributed Mode;  ">olmc</TD>
 * <TD title="Only suitable for counting cycles">c</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title=""></TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">Number of cycles the multiplie...</TD> 
 * <TD style="display:none">Number of cycles the multiplier or divider is in use.  </TD>
 
 * </TR><TR><TD colspan="11" style="text-align:left"><a class="anchor" id="a_load_store"></a><B><I>Load Store unit events.</I></B></TD></TR>
 * <TR>
 * <TD>80</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_LSU_COMMIT_STS"></a>PEVT_LSU_COMMIT_STS</TD>   
   
 * <TD style="text-align:left">Committed Stores</TD>
      
 * <TD title="events attributable to current sw/hw thread">thread</TD>
 
 
 
 * <TD title="low latency access method supported in BGPM Island Mode;  multiplexing supported in BGPM Distributed Mode;  ">olmc</TD>
 * <TD title="Single cycle event which can occur on consecutive cycles; cycles and events are synonymous">v</TD>
 
 * <TD style="text-align:left">PAPI_SR_INS</TD>
   
 * <TD title=""></TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">Number of completed store comm...</TD> 
 * <TD style="display:none">Number of completed store commands.<br></br>-Microcoded instructions will count more than once.<br></br>-Does not count syncs,tlb ops,dcbz,icswx, or data cache management instructions.<br></br>-Includes stcx, but does not wait for stcx complete response from the L2.<br></br>-Includes cache-inhibited stores. </TD>
 
 * </TR>
 * <TR>
 * <TD>81</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_LSU_COMMIT_ST_MISSES"></a>PEVT_LSU_COMMIT_ST_MISSES</TD>   
   
 * <TD style="text-align:left">Committed Store Misses</TD>
      
 * <TD title="events attributable to current sw/hw thread">thread</TD>
 
 
 
 * <TD title="low latency access method supported in BGPM Island Mode;  multiplexing supported in BGPM Distributed Mode;  ">olmc</TD>
 * <TD title="Single cycle event which can occur on consecutive cycles; cycles and events are synonymous">v</TD>
 
 * <TD style="text-align:left">PAPI_L1_STM</TD>
   
 * <TD title=""></TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">Number of completed store comm...</TD> 
 * <TD style="display:none">Number of completed store commands that missed the L1 Data Cache. Note that store misses are pipelined and write through to the L2, so the store time typically has less impact on performance than load misses. <br></br>-Microcoded instructions may be counted more than once.<br></br>-Does not count syncs,tlb ops,dcbz,icswx, or data cache management instructions.<br></br>-Includes stcx, but does not wait for stcx complete response from the L2.<br></br>-Does not includes cache-inhibited stores. </TD>
 
 * </TR>
 * <TR>
 * <TD>82</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_LSU_COMMIT_LD_MISSES"></a>PEVT_LSU_COMMIT_LD_MISSES</TD>   
   
 * <TD style="text-align:left">Committed Load Misses</TD>
      
 * <TD title="events attributable to current sw/hw thread">thread</TD>
 
 
 
 * <TD title="low latency access method supported in BGPM Island Mode;  multiplexing supported in BGPM Distributed Mode;  ">olmc</TD>
 * <TD title="Single cycle event which can occur on consecutive cycles; cycles and events are synonymous">v</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title=""></TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">Number of completed load comma...</TD> 
 * <TD style="display:none">Number of completed load commands that missed the L1 Data Cache.<br></br>-Microcoded instructions may be counted more than once.<br></br>-Does not count dcbt[st][ls][ep].<br></br>-Include larx.<br></br>-Does not includes cache-inhibited loads. </TD>
 
 * </TR>
 * <TR>
 * <TD>83</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_LSU_COMMIT_CACHE_INHIB_LD_MISSES"></a>PEVT_LSU_COMMIT_CACHE_INHIB_LD_MISSES</TD>   
   
 * <TD style="text-align:left">Committed Cache-Inhibited Load Misses</TD>
      
 * <TD title="events attributable to current sw/hw thread">thread</TD>
 
 
 
 * <TD title="low latency access method supported in BGPM Island Mode;  multiplexing supported in BGPM Distributed Mode;  ">olmc</TD>
 * <TD title="Single cycle event which can occur on consecutive cycles; cycles and events are synonymous">v</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title=""></TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">Number of completed cache-inhi...</TD> 
 * <TD style="display:none">Number of completed cache-inhibited load commands.<br></br>-Microcoded instructions may be counted more than once.<br></br>-Does not count dcbt[st][ls][ep].<br></br>-Does not includes cacheable loads. </TD>
 
 * </TR>
 * <TR>
 * <TD>84</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_LSU_COMMIT_CACHEABLE_LDS"></a>PEVT_LSU_COMMIT_CACHEABLE_LDS</TD>   
   
 * <TD style="text-align:left">Committed Cacheable Loads</TD>
      
 * <TD title="events attributable to current sw/hw thread">thread</TD>
 
 
 
 * <TD title="low latency access method supported in BGPM Island Mode;  multiplexing supported in BGPM Distributed Mode;  ">olmc</TD>
 * <TD title="Single cycle event which can occur on consecutive cycles; cycles and events are synonymous">v</TD>
 
 * <TD style="text-align:left">PAPI_L1_DCR</TD>
   
 * <TD title=""></TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">Number of completed cache-able...</TD> 
 * <TD style="display:none">Number of completed cache-able load commands.<br></br>-Microcoded instructions may be counted more than once.<br></br>-Does not count dcbt[st][ls][ep].<br></br>-Include larx.<br></br>-Does not includes cache-inhibited loads. </TD>
 
 * </TR>
 * <TR>
 * <TD>85</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_LSU_COMMIT_DCBT_MISSES"></a>PEVT_LSU_COMMIT_DCBT_MISSES</TD>   
   
 * <TD style="text-align:left">Committed DCBT Misses</TD>
      
 * <TD title="events attributable to current sw/hw thread">thread</TD>
 
 
 
 * <TD title="low latency access method supported in BGPM Island Mode;  multiplexing supported in BGPM Distributed Mode;  ">olmc</TD>
 * <TD title="Single cycle event which can occur on consecutive cycles; cycles and events are synonymous">v</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title=""></TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">Number of completed dcbt[st][l...</TD> 
 * <TD style="display:none">Number of completed dcbt[st][ls][ep] commands that missed the L1 Data Cache.<br></br>-Does not include touch ops that were dropped due to the following:<br></br>1) Unsupported TH(CT) fields.<br></br>2) Translated to cache-inhibited.<br></br>3) Exception detected on dcbt[st][ep]. </TD>
 
 * </TR>
 * <TR>
 * <TD>86</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_LSU_COMMIT_DCBT_HITS"></a>PEVT_LSU_COMMIT_DCBT_HITS</TD>   
   
 * <TD style="text-align:left">Committed DCBT Hits</TD>
      
 * <TD title="events attributable to current sw/hw thread">thread</TD>
 
 
 
 * <TD title="low latency access method supported in BGPM Island Mode;  multiplexing supported in BGPM Distributed Mode;  ">olmc</TD>
 * <TD title="Single cycle event which can occur on consecutive cycles; cycles and events are synonymous">v</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title=""></TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">Number of completed dcbt[st][l...</TD> 
 * <TD style="display:none">Number of completed dcbt[st][ls][ep] commands that hit the L1 Data Cache.<br></br>-Does not include touch ops that were dropped due to the following:<br></br>1) Unsupported TH(CT) fields.<br></br>2) Translated to cache-inhibited.<br></br>3) Exception detected on dcbt[st][ep]. </TD>
 
 * </TR>
 * <TR>
 * <TD>87</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_LSU_COMMIT_AXU_LDS"></a>PEVT_LSU_COMMIT_AXU_LDS</TD>   
   
 * <TD style="text-align:left">Committed AXU Loads</TD>
      
 * <TD title="events attributable to current sw/hw thread">thread</TD>
 
 
 
 * <TD title="low latency access method supported in BGPM Island Mode;  multiplexing supported in BGPM Distributed Mode;  ">olmc</TD>
 * <TD title="Single cycle event which can occur on consecutive cycles; cycles and events are synonymous">v</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title=""></TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">Number of completed AXU loads....</TD> 
 * <TD style="display:none">Number of completed AXU loads. AXU refers to the unit attached on the AXU interface (i.e a floating point unit).<br></br>-Cacheable and cache-inhibited loads are counted. </TD>
 
 * </TR>
 * <TR>
 * <TD>88</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_LSU_COMMIT_AXU_STS"></a>PEVT_LSU_COMMIT_AXU_STS</TD>   
   
 * <TD style="text-align:left">Committed AXU Stores</TD>
      
 * <TD title="events attributable to current sw/hw thread">thread</TD>
 
 
 
 * <TD title="low latency access method supported in BGPM Island Mode;  multiplexing supported in BGPM Distributed Mode;  ">olmc</TD>
 * <TD title="Single cycle event which can occur on consecutive cycles; cycles and events are synonymous">v</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title=""></TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">Number of completed AXU stores...</TD> 
 * <TD style="display:none">Number of completed AXU stores. AXU refers to the unit attached on the AXU interface (i.e a floating point unit).<br></br>-Cacheable and cache-inhibited stores are counted. </TD>
 
 * </TR>
 * <TR>
 * <TD>89</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_LSU_COMMIT_STCX"></a>PEVT_LSU_COMMIT_STCX</TD>   
   
 * <TD style="text-align:left">Committed STCX</TD>
      
 * <TD title="events attributable to current sw/hw thread">thread</TD>
 
 
 
 * <TD title="low latency access method supported in BGPM Island Mode;  multiplexing supported in BGPM Distributed Mode;  ">olmc</TD>
 * <TD title="Single cycle event which can occur on consecutive cycles; cycles and events are synonymous">v</TD>
 
 * <TD style="text-align:left">PAPI_CSR_TOT</TD>
   
 * <TD title=""></TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">Number of completed STCX instr...</TD> 
 * <TD style="display:none">Number of completed STCX instructions. Does not wait for the stcx complete response from the L2. </TD>
 
 * </TR>
 * <TR>
 * <TD>90</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_LSU_COMMIT_WCLR"></a>PEVT_LSU_COMMIT_WCLR</TD>   
   
 * <TD style="text-align:left">Committed WCLR</TD>
      
 * <TD title="events attributable to current sw/hw thread">thread</TD>
 
 
 
 * <TD title="low latency access method supported in BGPM Island Mode;  multiplexing supported in BGPM Distributed Mode;  ">olmc</TD>
 * <TD title="Single cycle event which can occur on consecutive cycles; cycles and events are synonymous">v</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title=""></TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">Number of completed WCLR instr...</TD> 
 * <TD style="display:none">Number of completed WCLR instructions.  </TD>
 
 * </TR>
 * <TR>
 * <TD>91</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_LSU_COMMIT_WCLR_WL"></a>PEVT_LSU_COMMIT_WCLR_WL</TD>   
   
 * <TD style="text-align:left">Committed WCLR Watchlost</TD>
      
 * <TD title="events attributable to current sw/hw thread">thread</TD>
 
 
 
 * <TD title="low latency access method supported in BGPM Island Mode;  multiplexing supported in BGPM Distributed Mode;  ">olmc</TD>
 * <TD title="Single cycle event which can occur on consecutive cycles; cycles and events are synonymous">v</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title=""></TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">Number of completed WCLR instr...</TD> 
 * <TD style="display:none">Number of completed WCLR instructions that set the Watchlost indicator. </TD>
 
 * </TR>
 * <TR>
 * <TD>92</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_LSU_COMMIT_LDAWX"></a>PEVT_LSU_COMMIT_LDAWX</TD>   
   
 * <TD style="text-align:left">Committed LDAWX</TD>
      
 * <TD title="events attributable to current sw/hw thread">thread</TD>
 
 
 
 * <TD title="low latency access method supported in BGPM Island Mode;  multiplexing supported in BGPM Distributed Mode;  ">olmc</TD>
 * <TD title="Single cycle event which can occur on consecutive cycles; cycles and events are synonymous">v</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title=""></TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">Number of completed LDAWX inst...</TD> 
 * <TD style="display:none">Number of completed LDAWX instructions.  </TD>
 
 * </TR>
 * <TR>
 * <TD>93</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_LSU_UNSUPPORTED_ALIGNMENT_FLUSH"></a>PEVT_LSU_UNSUPPORTED_ALIGNMENT_FLUSH</TD>   
   
 * <TD style="text-align:left">Unsupported Alignment Flush</TD>
      
 * <TD title="events attributable to current sw/hw thread">thread</TD>
 
 
 
 * <TD title="low latency access method supported in BGPM Island Mode;  multiplexing supported in BGPM Distributed Mode;  ">olmc</TD>
 * <TD title="Single cycle event which can occur on consecutive cycles; cycles and events are synonymous">v</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title=""></TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">Number of flushes due to an un...</TD> 
 * <TD style="display:none">Number of flushes due to an unsupported alignment.<br></br>-This is a speculative count.<br></br>-Includes speculative flushes to microcode.<br></br>-Includes speculative flushes to the alignment interrupt due to<br></br>unaligned larx,stcx,icswx,ldawx or XUCR0[FLSTA]=1 or XUCR0[AFLSTA]=1. </TD>
 
 * </TR>
 * <TR>
 * <TD>94</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_LSU_RELOAD_RESRC_CONFLICT_FLUSH"></a>PEVT_LSU_RELOAD_RESRC_CONFLICT_FLUSH</TD>   
   
 * <TD style="text-align:left">Reload Resource Conflict Flush</TD>
      
 * <TD title="events attributable to current sw/hw thread">thread</TD>
 
 
 
 * <TD title="low latency access method supported in BGPM Island Mode;  multiplexing supported in BGPM Distributed Mode;  ">olmc</TD>
 * <TD title="Single cycle event which can occur on consecutive cycles; cycles and events are synonymous">v</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title=""></TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">Number of flushes due to a res...</TD> 
 * <TD style="display:none">Number of flushes due to a resource conflict on a reload.<br></br>1)Cache-Inhibited Reload colliding with store,icswx,mftgpr,mffgpr, mtdp,mfdp instructions valid in EX2 pipe stage.<br></br>2)Cache-Inhibited Reload targeting AXU colliding with AXU load instruction.<br></br>3)1st half of Cacheable Reload colliding with dcbt[st]ls or ldawx. </TD>
 
 * </TR>
 * <TR>
 * <TD>95</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_LSU_COMMIT_DUPLICATE_LDAWX"></a>PEVT_LSU_COMMIT_DUPLICATE_LDAWX</TD>   
   
 * <TD style="text-align:left">Committed Duplicate LDAWX</TD>
      
 * <TD title="events attributable to current sw/hw thread">thread</TD>
 
 
 
 * <TD title="low latency access method supported in BGPM Island Mode;  multiplexing supported in BGPM Distributed Mode;  ">olmc</TD>
 * <TD title="Single cycle event which can occur on consecutive cycles; cycles and events are synonymous">v</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title=""></TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">Number of completed LDAWX whic...</TD> 
 * <TD style="display:none">Number of completed LDAWX which set CR=001||XER[SO].  </TD>
 
 * </TR>
 * <TR>
 * <TD>96</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_LSU_INTER_THRD_DIR_ACCESS_FLUSH"></a>PEVT_LSU_INTER_THRD_DIR_ACCESS_FLUSH</TD>   
   
 * <TD style="text-align:left">Inter-Thread Directory Access Flush</TD>
      
 * <TD title="events attributable to current sw/hw thread">thread</TD>
 
 
 
 * <TD title="low latency access method supported in BGPM Island Mode;  multiplexing supported in BGPM Distributed Mode;  ">olmc</TD>
 * <TD title="Single cycle event which can occur on consecutive cycles; cycles and events are synonymous">v</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title=""></TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">Number of flushes due to a thr...</TD> 
 * <TD style="display:none">Number of flushes due to a thread setting/clearing cacheline directory contents (i.e. valid,lock,thread watch bits) and different thread<br></br>accesses same cacheline. Also, count of non-committed WCLR L[0]=0 in pipe and different thread has a directory access in EX3.<br></br>-This is a speculative count. </TD>
 
 * </TR>
 * <TR>
 * <TD>97</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_LSU_LMQ_DEPENDENCY_FLUSH"></a>PEVT_LSU_LMQ_DEPENDENCY_FLUSH</TD>   
   
 * <TD style="text-align:left">Load Miss Queue Dependency Flush</TD>
      
 * <TD title="events attributable to current sw/hw thread">thread</TD>
 
 
 
 * <TD title="low latency access method supported in BGPM Island Mode;  multiplexing supported in BGPM Distributed Mode;  ">olmc</TD>
 * <TD title="Single cycle event which can occur on consecutive cycles; cycles and events are synonymous">v</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title=""></TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">Number of flushes due to a RAW...</TD> 
 * <TD style="display:none">Number of flushes due to a RAW/WAW hazard detected against the Load Miss Queue.<br></br>-This is a speculative count. </TD>
 
 * </TR>
 * <TR>
 * <TD>98</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_LSU_COMMIT_WCHKALL"></a>PEVT_LSU_COMMIT_WCHKALL</TD>   
   
 * <TD style="text-align:left">Committed WCHKALL</TD>
      
 * <TD title="events attributable to current sw/hw thread">thread</TD>
 
 
 
 * <TD title="low latency access method supported in BGPM Island Mode;  multiplexing supported in BGPM Distributed Mode;  ">olmc</TD>
 * <TD title="Single cycle event which can occur on consecutive cycles; cycles and events are synonymous">v</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title=""></TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">Number of completed WCHKALL in...</TD> 
 * <TD style="display:none">Number of completed WCHKALL instructions.  </TD>
 
 * </TR>
 * <TR>
 * <TD>99</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_LSU_COMMIT_SUCC_WCHKALL"></a>PEVT_LSU_COMMIT_SUCC_WCHKALL</TD>   
   
 * <TD style="text-align:left">Committed Successful WCHKALL</TD>
      
 * <TD title="events attributable to current sw/hw thread">thread</TD>
 
 
 
 * <TD title="low latency access method supported in BGPM Island Mode;  multiplexing supported in BGPM Distributed Mode;  ">olmc</TD>
 * <TD title="Single cycle event which can occur on consecutive cycles; cycles and events are synonymous">v</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title=""></TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">Number of completed WCHKALL in...</TD> 
 * <TD style="display:none">Number of completed WCHKALL instructions that returned CR=000||XER[SO].  </TD>
 
 * </TR>
 * <TR>
 * <TD>100</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_LSU_LD_MISS_Q_FULL_FLUSH"></a>PEVT_LSU_LD_MISS_Q_FULL_FLUSH</TD>   
   
 * <TD style="text-align:left">Load Miss Queue Full Flush</TD>
      
 * <TD title="events attributable to current sw/hw thread">thread</TD>
 
 
 
 * <TD title="low latency access method supported in BGPM Island Mode;  multiplexing supported in BGPM Distributed Mode;  ">olmc</TD>
 * <TD title="Single cycle event which can occur on consecutive cycles; cycles and events are synonymous">v</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title=""></TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">Number of flushes due to the L...</TD> 
 * <TD style="display:none">Number of flushes due to the Load Miss Queue being full. Load Miss Queue Full is determined when all 8 entries are in use and new load<br></br>miss is flushed. Also, count of load miss command sequence wrapped flushes.<br></br>-This is a speculative count. </TD>
 
 * </TR>
 * <TR>
 * <TD>101</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_LSU_ST_Q_FULL_FLUSH"></a>PEVT_LSU_ST_Q_FULL_FLUSH</TD>   
   
 * <TD style="text-align:left">Store Queue Full Flush</TD>
      
 * <TD title="events attributable to current sw/hw thread">thread</TD>
 
 
 
 * <TD title="low latency access method supported in BGPM Island Mode;  multiplexing supported in BGPM Distributed Mode;  ">olmc</TD>
 * <TD title="Single cycle event which can occur on consecutive cycles; cycles and events are synonymous">v</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title=""></TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">Number of flushes due to the S...</TD> 
 * <TD style="display:none">Number of flushes due to the Store Queue being full or a sync,mbar,tlbsync instruction hits against outstanding load for issuing thread.<br></br>-This is a speculative count. </TD>
 
 * </TR>
 * <TR>
 * <TD>102</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_LSU_HIT_LD_FLUSH"></a>PEVT_LSU_HIT_LD_FLUSH</TD>   
   
 * <TD style="text-align:left">Hit Against Outstanding Load Flush</TD>
      
 * <TD title="events attributable to current sw/hw thread">thread</TD>
 
 
 
 * <TD title="low latency access method supported in BGPM Island Mode;  multiplexing supported in BGPM Distributed Mode;  ">olmc</TD>
 * <TD title="Single cycle event which can occur on consecutive cycles; cycles and events are synonymous">v</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title=""></TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">Number of flushes due to a cac...</TD> 
 * <TD style="display:none">Number of flushes due to a cache instruction (i.e load,store, or cache management) hit against an outstanding load miss.<br></br>-XUCR0[CLS]=0Cacheline check is down to the 64Byte boundary, else check is down to the 128Byte boundary.<br></br>-This is a speculative count. </TD>
 
 * </TR>
 * <TR>
 * <TD>103</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_LSU_HIT_IG1_REQ_FLUSH"></a>PEVT_LSU_HIT_IG1_REQ_FLUSH</TD>   
   
 * <TD style="text-align:left">Hit Against Outstanding I=G=1 Request Flush</TD>
      
 * <TD title="events attributable to current sw/hw thread">thread</TD>
 
 
 
 * <TD title="low latency access method supported in BGPM Island Mode;  multiplexing supported in BGPM Distributed Mode;  ">olmc</TD>
 * <TD title="Single cycle event which can occur on consecutive cycles; cycles and events are synonymous">v</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title=""></TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">Number of flushes due to a cac...</TD> 
 * <TD style="display:none">Number of flushes due to a cache instruction (i.e load,store, or cache management) hit against an outstanding guarded cache-inhibited<br></br>request in the load miss queue or in the store queue.<br></br>-This is a speculative count. </TD>
 
 * </TR>
 * <TR>
 * <TD>104</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_LSU_LARX_FINISHED"></a>PEVT_LSU_LARX_FINISHED</TD>   
   
 * <TD style="text-align:left">LARX Finished</TD>
      
 * <TD title="events attributable to current sw/hw thread">thread</TD>
 
 
 
 * <TD title="low latency access method supported in BGPM Island Mode;  multiplexing supported in BGPM Distributed Mode;  ">olmc</TD>
 * <TD title="Single cycle event which can occur on consecutive cycles; cycles and events are synonymous">v</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title=""></TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">Number of completed LARX instr...</TD> 
 * <TD style="display:none">Number of completed LARX instructions.<br></br>-Waits for reload from the L2 </TD>
 
 * </TR>
 * <TR>
 * <TD>105</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_LSU_INTER_THRD_ST_WATCH_LOST"></a>PEVT_LSU_INTER_THRD_ST_WATCH_LOST</TD>   
   
 * <TD style="text-align:left">Inter-Thread Store Set Watch Lost Indicator</TD>
      
 * <TD title="events attributable to current sw/hw thread">thread</TD>
 
 
 
 * <TD title="low latency access method supported in BGPM Island Mode;  multiplexing supported in BGPM Distributed Mode;  ">olmc</TD>
 * <TD title="Single cycle event which can occur on consecutive cycles; cycles and events are synonymous">v</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title=""></TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">Number of Watch Lost indicator...</TD> 
 * <TD style="display:none">Number of Watch Lost indicator sets due to a different thread storing to a watched line by another thread.  </TD>
 
 * </TR>
 * <TR>
 * <TD>106</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_LSU_RELOAD_WATCH_LOST"></a>PEVT_LSU_RELOAD_WATCH_LOST</TD>   
   
 * <TD style="text-align:left">Reload Set Watch Lost Indicator</TD>
      
 * <TD title="events attributable to current sw/hw thread">thread</TD>
 
 
 
 * <TD title="low latency access method supported in BGPM Island Mode;  multiplexing supported in BGPM Distributed Mode;  ">olmc</TD>
 * <TD title="Single cycle event which can occur on consecutive cycles; cycles and events are synonymous">v</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title=""></TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">Number of Watch Lost indicator...</TD> 
 * <TD style="display:none">Number of Watch Lost indicator sets due to a reload evicting watched line.  </TD>
 
 * </TR>
 * <TR>
 * <TD>107</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_LSU_BACK_INV_WATCH_LOST"></a>PEVT_LSU_BACK_INV_WATCH_LOST</TD>   
   
 * <TD style="text-align:left">Back-Invalidate Set Watch Lost Indicator</TD>
      
 * <TD title="events attributable to current sw/hw thread">thread</TD>
 
 
 
 * <TD title="low latency access method supported in BGPM Island Mode;  multiplexing supported in BGPM Distributed Mode;  ">olmc</TD>
 * <TD title="Single cycle event which can occur on consecutive cycles; cycles and events are synonymous">v</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title=""></TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">Number of Watch Lost indicator...</TD> 
 * <TD style="display:none">Number of Watch Lost indicator sets due to a back invalidate to a watched line. </TD>
 
 * </TR>
 * <TR>
 * <TD>108</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_LSU_L1_DCACHE_BACK_INVAL"></a>PEVT_LSU_L1_DCACHE_BACK_INVAL</TD>   
   
 * <TD style="text-align:left">L1 Data Cache Back-Invalidate</TD>
      
 * <TD title="events attributable to current sw/hw thread">thread</TD>
 
 
 
 * <TD title="low latency access method supported in BGPM Island Mode;  multiplexing supported in BGPM Distributed Mode;  ">olmc</TD>
 * <TD title="Single cycle event which can occur on consecutive cycles; cycles and events are synonymous">v</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title=""></TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">Number of back-invalidates sen...</TD> 
 * <TD style="display:none">Number of back-invalidates sent to the L1 Data Cache.  </TD>
 
 * </TR>
 * <TR>
 * <TD>109</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_LSU_L1_DCACHE_BACK_INVAL_HITS"></a>PEVT_LSU_L1_DCACHE_BACK_INVAL_HITS</TD>   
   
 * <TD style="text-align:left">L1 Data Cache Back-Invalidate Hits</TD>
      
 * <TD title="events attributable to current sw/hw thread">thread</TD>
 
 
 
 * <TD title="low latency access method supported in BGPM Island Mode;  multiplexing supported in BGPM Distributed Mode;  ">olmc</TD>
 * <TD title="Single cycle event which can occur on consecutive cycles; cycles and events are synonymous">v</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title=""></TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">Number of back-invalidates sen...</TD> 
 * <TD style="display:none">Number of back-invalidates sent to the L1 Data Cache that invalidated a line.  </TD>
 
 * </TR>
 * <TR>
 * <TD>110</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_LSU_L1_CACHE_PTYERR_DETECTED"></a>PEVT_LSU_L1_CACHE_PTYERR_DETECTED</TD>   
   
 * <TD style="text-align:left">L1 Cache Parity Error Detected</TD>
      
 * <TD title="events attributable to current sw/hw thread">thread</TD>
 
 
 
 * <TD title="low latency access method supported in BGPM Island Mode;  multiplexing supported in BGPM Distributed Mode;  ">olmc</TD>
 * <TD title="Single cycle event which can occur on consecutive cycles; cycles and events are synonymous">v</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title=""></TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">Number of parity errors detect...</TD> 
 * <TD style="display:none">Number of parity errors detected in the L1 Directories and Caches.<br></br>-Includes both Instruction and Data Directories and Caches.<br></br>-Does not count more than one per cycle, although up to 4 may occur simultaneously. </TD>
 
 * </TR>
 * <TR>
 * <TD>111</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_LSU_LD_LAT_MEM_SUBSYS_CYC"></a>PEVT_LSU_LD_LAT_MEM_SUBSYS_CYC</TD>   
   
 * <TD style="text-align:left">(event is broken) Load Latency memory subsystem</TD>
      
 * <TD title="events attributable to current sw/hw thread">thread</TD>
 
 
 
 * <TD title="low latency access method supported in BGPM Island Mode;  multiplexing supported in BGPM Distributed Mode;  ">olmc</TD>
 * <TD title="Only suitable for counting cycles">c</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title="">Broken</TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">load miss queue entry 0 is in ...</TD> 
 * <TD style="display:none">load miss queue entry 0 is in use was the intent - but is broken. Doesn't indicate anything useful.  </TD>
 
 * </TR><TR><TD colspan="11" style="text-align:left"><a class="anchor" id="a_memory_management"></a><B><I>Memory Management Unit Events</I></B></TD></TR>
 * <TR>
 * <TD>112</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_MMU_TLB_HIT_DIRECT_IERAT"></a>PEVT_MMU_TLB_HIT_DIRECT_IERAT</TD>   
   
 * <TD style="text-align:left">TLB hit direct after ierat miss</TD>
      
 * <TD title="events attributable to current sw/hw thread">thread</TD>
 
 
 
 * <TD title="low latency access method supported in BGPM Island Mode;  multiplexing supported in BGPM Distributed Mode;  ">olmc</TD>
 * <TD title="Only suitable for counting numbers of events">e</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title=""></TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">TLB hit direct entry (instruct...</TD> 
 * <TD style="display:none">TLB hit direct entry (instruction, ind=0 entry hit for fetch)  </TD>
 
 * </TR>
 * <TR>
 * <TD>113</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_MMU_TLB_MISS_DIRECT_IERAT"></a>PEVT_MMU_TLB_MISS_DIRECT_IERAT</TD>   
   
 * <TD style="text-align:left">TLB miss direct after ierat miss</TD>
      
 * <TD title="events attributable to current sw/hw thread">thread</TD>
 
 
 
 * <TD title="low latency access method supported in BGPM Island Mode;  multiplexing supported in BGPM Distributed Mode;  ">olmc</TD>
 * <TD title="Only suitable for counting numbers of events">e</TD>
 
 * <TD style="text-align:left">PAPI_TLB_IM</TD>
   
 * <TD title=""></TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">TLB miss direct entry (instruc...</TD> 
 * <TD style="display:none">TLB miss direct entry (instruction, ind=0 entry missed for fetch)  </TD>
 
 * </TR>
 * <TR>
 * <TD>114</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_MMU_TLB_MISS_INDIR_IERAT"></a>PEVT_MMU_TLB_MISS_INDIR_IERAT</TD>   
   
 * <TD style="text-align:left">TLB miss indirect after ierat miss</TD>
      
 * <TD title="events attributable to current sw/hw thread">thread</TD>
 
 
 
 * <TD title="low latency access method supported in BGPM Island Mode;  multiplexing supported in BGPM Distributed Mode;  ">olmc</TD>
 * <TD title="Only suitable for counting numbers of events">e</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title=""></TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">TLB miss indirect entry (instr...</TD> 
 * <TD style="display:none">TLB miss indirect entry (instruction, ind=1 entry missed for fetch, results in i-tlb exception)  </TD>
 
 * </TR>
 * <TR>
 * <TD>115</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_MMU_HTW_HIT_IERAT"></a>PEVT_MMU_HTW_HIT_IERAT</TD>   
   
 * <TD style="text-align:left">Hardware Table walk hit after ierat miss</TD>
      
 * <TD title="events attributable to current sw/hw thread">thread</TD>
 
 
 
 * <TD title="low latency access method supported in BGPM Island Mode;  multiplexing supported in BGPM Distributed Mode;  ">olmc</TD>
 * <TD title="Only suitable for counting numbers of events">e</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title=""></TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">H/W tablewalk hit (instruction...</TD> 
 * <TD style="display:none">H/W tablewalk hit (instruction, pte reload with PTE.V=1 for fetch)  </TD>
 
 * </TR>
 * <TR>
 * <TD>116</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_MMU_HTW_MISS_IERAT"></a>PEVT_MMU_HTW_MISS_IERAT</TD>   
   
 * <TD style="text-align:left">Hardware Table walk miss after ierat miss</TD>
      
 * <TD title="events attributable to current sw/hw thread">thread</TD>
 
 
 
 * <TD title="low latency access method supported in BGPM Island Mode;  multiplexing supported in BGPM Distributed Mode;  ">olmc</TD>
 * <TD title="Only suitable for counting numbers of events">e</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title=""></TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">H/W tablewalk miss (instructio...</TD> 
 * <TD style="display:none">H/W tablewalk miss (instruction, pte reload with PTE.V=0 for fetch, results in PT fault exception -&gt; isi)  </TD>
 
 * </TR>
 * <TR>
 * <TD>117</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_MMU_TLB_HIT_DIRECT_DERAT"></a>PEVT_MMU_TLB_HIT_DIRECT_DERAT</TD>   
   
 * <TD style="text-align:left">TLB hit direct after derat miss</TD>
      
 * <TD title="events attributable to current sw/hw thread">thread</TD>
 
 
 
 * <TD title="low latency access method supported in BGPM Island Mode;  multiplexing supported in BGPM Distributed Mode;  ">olmc</TD>
 * <TD title="Only suitable for counting numbers of events">e</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title=""></TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">TLB hit direct entry (data, in...</TD> 
 * <TD style="display:none">TLB hit direct entry (data, ind=0 entry hit for load/store/cache op)  </TD>
 
 * </TR>
 * <TR>
 * <TD>118</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_MMU_TLB_MISS_DIRECT_DERAT"></a>PEVT_MMU_TLB_MISS_DIRECT_DERAT</TD>   
   
 * <TD style="text-align:left">TLB miss direct after derat miss</TD>
      
 * <TD title="events attributable to current sw/hw thread">thread</TD>
 
 
 
 * <TD title="low latency access method supported in BGPM Island Mode;  multiplexing supported in BGPM Distributed Mode;  ">olmc</TD>
 * <TD title="Only suitable for counting numbers of events">e</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title=""></TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">TLB miss direct entry (data, i...</TD> 
 * <TD style="display:none">TLB miss direct entry (data, ind=0 entry miss for load/store/cache op)  </TD>
 
 * </TR>
 * <TR>
 * <TD>119</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_MMU_TLB_MISS_INDIR_DERAT"></a>PEVT_MMU_TLB_MISS_INDIR_DERAT</TD>   
   
 * <TD style="text-align:left">TLB miss indirect after derat miss</TD>
      
 * <TD title="events attributable to current sw/hw thread">thread</TD>
 
 
 
 * <TD title="low latency access method supported in BGPM Island Mode;  multiplexing supported in BGPM Distributed Mode;  ">olmc</TD>
 * <TD title="Only suitable for counting numbers of events">e</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title=""></TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">TLB miss indirect entry (data,...</TD> 
 * <TD style="display:none">TLB miss indirect entry (data, ind=1 entry missed for load/store/cache op, results in d-tlb exception)  </TD>
 
 * </TR>
 * <TR>
 * <TD>120</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_MMU_HTW_HIT_DERAT"></a>PEVT_MMU_HTW_HIT_DERAT</TD>   
   
 * <TD style="text-align:left">Hardware Table walk hit after derat miss</TD>
      
 * <TD title="events attributable to current sw/hw thread">thread</TD>
 
 
 
 * <TD title="low latency access method supported in BGPM Island Mode;  multiplexing supported in BGPM Distributed Mode;  ">olmc</TD>
 * <TD title="Only suitable for counting numbers of events">e</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title=""></TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">H/W tablewalk hit (data, pte r...</TD> 
 * <TD style="display:none">H/W tablewalk hit (data, pte reload with PTE.V=1 for load/store/cache op)  </TD>
 
 * </TR>
 * <TR>
 * <TD>121</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_MMU_HTW_MISS_DERAT"></a>PEVT_MMU_HTW_MISS_DERAT</TD>   
   
 * <TD style="text-align:left">Hardware Table walk miss after derat miss</TD>
      
 * <TD title="events attributable to current sw/hw thread">thread</TD>
 
 
 
 * <TD title="low latency access method supported in BGPM Island Mode;  multiplexing supported in BGPM Distributed Mode;  ">olmc</TD>
 * <TD title="Only suitable for counting numbers of events">e</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title=""></TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">H/W tablewalk miss (data, pte ...</TD> 
 * <TD style="display:none">H/W tablewalk miss (data, pte reload with PTE.V=0 for load/store/cache op, results in PT fault exception -&gt; dsi)  </TD>
 
 * </TR>
 * <TR>
 * <TD>122</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_MMU_IERAT_MISS"></a>PEVT_MMU_IERAT_MISS</TD>   
   
 * <TD style="text-align:left">Ierat miss</TD>
      
 * <TD title="events attributable to current sw/hw thread">thread</TD>
 
 
 
 * <TD title="low latency access method supported in BGPM Island Mode;  multiplexing supported in BGPM Distributed Mode;  ">olmc</TD>
 * <TD title="Configured to count number of event by default">be</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title=""></TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">IERAT miss (edge) or latency (...</TD> 
 * <TD style="display:none">IERAT miss (edge) or latency (level) (total ierat misses or latency)  </TD>
 
 * </TR>
 * <TR>
 * <TD>123</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_MMU_IERAT_MISS_CYC"></a>PEVT_MMU_IERAT_MISS_CYC</TD>   
   
 * <TD style="text-align:left">Ierat miss cycles</TD>
      
 * <TD title="events attributable to current sw/hw thread">thread</TD>
 
 
 
 * <TD title="low latency access method supported in BGPM Island Mode;  multiplexing supported in BGPM Distributed Mode;  ">olmc</TD>
 * <TD title="Configured to count cycles by default">bc</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title=""></TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">Number of Cycles for IERAT mis...</TD> 
 * <TD style="display:none">Number of Cycles for IERAT miss (edge) or latency (level) (total ierat misses or latency)  </TD>
 
 * </TR>
 * <TR>
 * <TD>124</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_MMU_DERAT_MISS"></a>PEVT_MMU_DERAT_MISS</TD>   
   
 * <TD style="text-align:left">Derat miss</TD>
      
 * <TD title="events attributable to current sw/hw thread">thread</TD>
 
 
 
 * <TD title="low latency access method supported in BGPM Island Mode;  multiplexing supported in BGPM Distributed Mode;  ">olmc</TD>
 * <TD title="Configured to count number of event by default">be</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title=""></TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">DERAT miss (edge) or latency (...</TD> 
 * <TD style="display:none">DERAT miss (edge) or latency (level) (total derat misses or latency)  </TD>
 
 * </TR>
 * <TR>
 * <TD>125</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_MMU_DERAT_MISS_CYC"></a>PEVT_MMU_DERAT_MISS_CYC</TD>   
   
 * <TD style="text-align:left">Derat miss cycles</TD>
      
 * <TD title="events attributable to current sw/hw thread">thread</TD>
 
 
 
 * <TD title="low latency access method supported in BGPM Island Mode;  multiplexing supported in BGPM Distributed Mode;  ">olmc</TD>
 * <TD title="Configured to count cycles by default">bc</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title=""></TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">Number of Cycles for DERAT mis...</TD> 
 * <TD style="display:none">Number of Cycles for DERAT miss (edge) or latency (level) (total derat misses or latency)  </TD>
 
 * </TR>
 * <TR>
 * <TD>126</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_MMU_IERAT_MISS_TOT"></a>PEVT_MMU_IERAT_MISS_TOT</TD>   
   
 * <TD style="text-align:left">Ierat miss total</TD>
      
 * <TD title="events are core-wide, and only one thread may count on the core when this event is used">core-shared</TD>
 
 
 
 * <TD title="low latency access method supported in BGPM Island Mode;  multiplexing supported in BGPM Distributed Mode;  ">olmc</TD>
 * <TD title="Only suitable for counting numbers of events">e</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title=""></TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">IERAT miss total (part of dire...</TD> 
 * <TD style="display:none">IERAT miss total (part of direct entry search total)  </TD>
 
 * </TR>
 * <TR>
 * <TD>127</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_MMU_DERAT_MISS_TOT"></a>PEVT_MMU_DERAT_MISS_TOT</TD>   
   
 * <TD style="text-align:left">derat miss total</TD>
      
 * <TD title="events are core-wide, and only one thread may count on the core when this event is used">core-shared</TD>
 
 
 
 * <TD title="low latency access method supported in BGPM Island Mode;  multiplexing supported in BGPM Distributed Mode;  ">olmc</TD>
 * <TD title="Only suitable for counting numbers of events">e</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title=""></TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">DERAT miss total (part of dire...</TD> 
 * <TD style="display:none">DERAT miss total (part of direct entry search total)  </TD>
 
 * </TR>
 * <TR>
 * <TD>128</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_MMU_TLB_MISS_DIRECT_TOT"></a>PEVT_MMU_TLB_MISS_DIRECT_TOT</TD>   
   
 * <TD style="text-align:left">tlb miss direct total</TD>
      
 * <TD title="events are core-wide, and only one thread may count on the core when this event is used">core-shared</TD>
 
 
 
 * <TD title="low latency access method supported in BGPM Island Mode;  multiplexing supported in BGPM Distributed Mode;  ">olmc</TD>
 * <TD title="Only suitable for counting numbers of events">e</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title=""></TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">TLB miss direct entry total (t...</TD> 
 * <TD style="display:none">TLB miss direct entry total (total TLB ind=0 misses)  </TD>
 
 * </TR>
 * <TR>
 * <TD>129</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_MMU_TLB_HIT_FIRSTSIZE_TOT"></a>PEVT_MMU_TLB_HIT_FIRSTSIZE_TOT</TD>   
   
 * <TD style="text-align:left">tlb hit firstsize total</TD>
      
 * <TD title="events are core-wide, and only one thread may count on the core when this event is used">core-shared</TD>
 
 
 
 * <TD title="low latency access method supported in BGPM Island Mode;  multiplexing supported in BGPM Distributed Mode;  ">olmc</TD>
 * <TD title="Only suitable for counting numbers of events">e</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title=""></TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">TLB hit direct entry first pag...</TD> 
 * <TD style="display:none">TLB hit direct entry first page size (first mmucr2 size)  </TD>
 
 * </TR>
 * <TR>
 * <TD>130</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_MMU_TLB_HIT_INDIR_TOT"></a>PEVT_MMU_TLB_HIT_INDIR_TOT</TD>   
   
 * <TD style="text-align:left">tlb hit indirect total</TD>
      
 * <TD title="events are core-wide, and only one thread may count on the core when this event is used">core-shared</TD>
 
 
 
 * <TD title="low latency access method supported in BGPM Island Mode;  multiplexing supported in BGPM Distributed Mode;  ">olmc</TD>
 * <TD title="Only suitable for counting numbers of events">e</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title=""></TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">TLB indirect entry hits total ...</TD> 
 * <TD style="display:none">TLB indirect entry hits total (=page table searches)  </TD>
 
 * </TR>
 * <TR>
 * <TD>131</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_MMU_HTW_PTERELOAD_TOT"></a>PEVT_MMU_HTW_PTERELOAD_TOT</TD>   
   
 * <TD style="text-align:left">htw ptereload total</TD>
      
 * <TD title="events are core-wide, and only one thread may count on the core when this event is used">core-shared</TD>
 
 
 
 * <TD title="low latency access method supported in BGPM Island Mode;  multiplexing supported in BGPM Distributed Mode;  ">olmc</TD>
 * <TD title="Only suitable for counting numbers of events">e</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title=""></TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">H/W tablewalk successful insta...</TD> 
 * <TD style="display:none">H/W tablewalk successful installs total (with no PTfault, TLB ineligible, or LRAT miss)  </TD>
 
 * </TR>
 * <TR>
 * <TD>132</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_MMU_LRAT_TRANS_TOT"></a>PEVT_MMU_LRAT_TRANS_TOT</TD>   
   
 * <TD style="text-align:left">lrat translation total</TD>
      
 * <TD title="events are core-wide, and only one thread may count on the core when this event is used">core-shared</TD>
 
 
 
 * <TD title="low latency access method supported in BGPM Island Mode;  multiplexing supported in BGPM Distributed Mode;  ">olmc</TD>
 * <TD title="Only suitable for counting numbers of events">e</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title=""></TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">LRAT translation request total...</TD> 
 * <TD style="display:none">LRAT translation request total (for GS=1 tlbwe and ptereload)  </TD>
 
 * </TR>
 * <TR>
 * <TD>133</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_MMU_LRAT_MISS_TOT"></a>PEVT_MMU_LRAT_MISS_TOT</TD>   
   
 * <TD style="text-align:left">lrat miss total</TD>
      
 * <TD title="events are core-wide, and only one thread may count on the core when this event is used">core-shared</TD>
 
 
 
 * <TD title="low latency access method supported in BGPM Island Mode;  multiplexing supported in BGPM Distributed Mode;  ">olmc</TD>
 * <TD title="Only suitable for counting numbers of events">e</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title=""></TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">LRAT misses total (for GS=1 tl...</TD> 
 * <TD style="display:none">LRAT misses total (for GS=1 tlbwe and ptereload)  </TD>
 
 * </TR>
 * <TR>
 * <TD>134</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_MMU_PT_FAULT_TOT"></a>PEVT_MMU_PT_FAULT_TOT</TD>   
   
 * <TD style="text-align:left">pt fault total</TD>
      
 * <TD title="events are core-wide, and only one thread may count on the core when this event is used">core-shared</TD>
 
 
 
 * <TD title="low latency access method supported in BGPM Island Mode;  multiplexing supported in BGPM Distributed Mode;  ">olmc</TD>
 * <TD title="Only suitable for counting numbers of events">e</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title=""></TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">Page table faults total (PTE.V...</TD> 
 * <TD style="display:none">Page table faults total (PTE.V=0 for ptereload, resulting in isi/dsi)  </TD>
 
 * </TR>
 * <TR>
 * <TD>135</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_MMU_PT_INELIG_TOT"></a>PEVT_MMU_PT_INELIG_TOT</TD>   
   
 * <TD style="text-align:left">pt inelig total</TD>
      
 * <TD title="events are core-wide, and only one thread may count on the core when this event is used">core-shared</TD>
 
 
 
 * <TD title="low latency access method supported in BGPM Island Mode;  multiplexing supported in BGPM Distributed Mode;  ">olmc</TD>
 * <TD title="Only suitable for counting numbers of events">e</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title=""></TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">TLB ineligible total (all TLB ...</TD> 
 * <TD style="display:none">TLB ineligible total (all TLB ways are iprot=1 for ptereloads, resulting in isi/dsi)  </TD>
 
 * </TR>
 * <TR>
 * <TD>136</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_MMU_TLBWEC_FAIL_TOT"></a>PEVT_MMU_TLBWEC_FAIL_TOT</TD>   
   
 * <TD style="text-align:left">tlbwec fail total</TD>
      
 * <TD title="events are core-wide, and only one thread may count on the core when this event is used">core-shared</TD>
 
 
 
 * <TD title="low latency access method supported in BGPM Island Mode;  multiplexing supported in BGPM Distributed Mode;  ">olmc</TD>
 * <TD title="Only suitable for counting numbers of events">e</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title=""></TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">tlbwe conditional failed total...</TD> 
 * <TD style="display:none">tlbwe conditional failed total (total tlbwe WQ=01 with no reservation match)  </TD>
 
 * </TR>
 * <TR>
 * <TD>137</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_MMU_TLBWEC_SUCC_TOT"></a>PEVT_MMU_TLBWEC_SUCC_TOT</TD>   
   
 * <TD style="text-align:left">tlbwec success total</TD>
      
 * <TD title="events are core-wide, and only one thread may count on the core when this event is used">core-shared</TD>
 
 
 
 * <TD title="low latency access method supported in BGPM Island Mode;  multiplexing supported in BGPM Distributed Mode;  ">olmc</TD>
 * <TD title="Only suitable for counting numbers of events">e</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title=""></TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">tlbwe conditional success tota...</TD> 
 * <TD style="display:none">tlbwe conditional success total (total tlbwe WQ=01 with reservation match)  </TD>
 
 * </TR>
 * <TR>
 * <TD>138</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_MMU_TLBILX_SRC_TOT"></a>PEVT_MMU_TLBILX_SRC_TOT</TD>   
   
 * <TD style="text-align:left">tlbilx local source total</TD>
      
 * <TD title="events are core-wide, and only one thread may count on the core when this event is used">core-shared</TD>
 
 
 
 * <TD title="low latency access method supported in BGPM Island Mode;  multiplexing supported in BGPM Distributed Mode;  ">olmc</TD>
 * <TD title="Only suitable for counting numbers of events">e</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title=""></TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">tlbilx local invalidations sou...</TD> 
 * <TD style="display:none">tlbilx local invalidations sourced total (sourced tlbilx on this core total)  </TD>
 
 * </TR>
 * <TR>
 * <TD>139</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_MMU_TLBIVAX_SRC_TOT"></a>PEVT_MMU_TLBIVAX_SRC_TOT</TD>   
   
 * <TD style="text-align:left">tlbivax local source total</TD>
      
 * <TD title="events are core-wide, and only one thread may count on the core when this event is used">core-shared</TD>
 
 
 
 * <TD title="low latency access method supported in BGPM Island Mode;  multiplexing supported in BGPM Distributed Mode;  ">olmc</TD>
 * <TD title="Only suitable for counting numbers of events">e</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title=""></TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">tlbivax invalidations sourced ...</TD> 
 * <TD style="display:none">tlbivax invalidations sourced total (sourced tlbivax on this core total)  </TD>
 
 * </TR>
 * <TR>
 * <TD>140</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_MMU_TLBIVAX_SNOOP_TOT"></a>PEVT_MMU_TLBIVAX_SNOOP_TOT</TD>   
   
 * <TD style="text-align:left">tlbivax snoop total</TD>
      
 * <TD title="events are core-wide, and only one thread may count on the core when this event is used">core-shared</TD>
 
 
 
 * <TD title="low latency access method supported in BGPM Island Mode;  multiplexing supported in BGPM Distributed Mode;  ">olmc</TD>
 * <TD title="Only suitable for counting numbers of events">e</TD>
 
 * <TD style="text-align:left">PAPI_TLB_SD</TD>
   
 * <TD title=""></TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">tlbivax snoops total (total tl...</TD> 
 * <TD style="display:none">tlbivax snoops total (total tlbivax snoops received from bus, local bit = don't care)  </TD>
 
 * </TR>
 * <TR>
 * <TD>141</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_MMU_TLB_FLUSH_REQ_TOT"></a>PEVT_MMU_TLB_FLUSH_REQ_TOT</TD>   
   
 * <TD style="text-align:left">tlb flush req total</TD>
      
 * <TD title="events are core-wide, and only one thread may count on the core when this event is used">core-shared</TD>
 
 
 
 * <TD title="low latency access method supported in BGPM Island Mode;  multiplexing supported in BGPM Distributed Mode;  ">olmc</TD>
 * <TD title="Configured to count number of event by default">be</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title=""></TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">TLB flush requests total (TLB ...</TD> 
 * <TD style="display:none">TLB flush requests total (TLB requested flushes due to TLB busy or instruction hazards)  </TD>
 
 * </TR>
 * <TR>
 * <TD>142</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_MMU_TLB_FLUSH_REQ_TOT_CYC"></a>PEVT_MMU_TLB_FLUSH_REQ_TOT_CYC</TD>   
   
 * <TD style="text-align:left">tlb flush req total cycles</TD>
      
 * <TD title="events are core-wide, and only one thread may count on the core when this event is used">core-shared</TD>
 
 
 
 * <TD title="low latency access method supported in BGPM Island Mode;  multiplexing supported in BGPM Distributed Mode;  ">olmc</TD>
 * <TD title="Configured to count cycles by default">bc</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title=""></TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">Number of Cycles for TLB flush...</TD> 
 * <TD style="display:none">Number of Cycles for TLB flush requests total (TLB requested flushes due to TLB busy or instruction hazards)  </TD>
 
 * </TR><TR><TD colspan="11" style="text-align:left"><a class="anchor" id="a_l1p_base"></a><B><I>L1p Base Events</I></B></TD></TR>
 * <TR>
 * <TD>143</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_L1P_BAS_STRM_LINE_ESTB"></a>PEVT_L1P_BAS_STRM_LINE_ESTB</TD>   
   
 * <TD style="text-align:left">L1p stream line establish</TD>
      
 * <TD title="events attributable to current sw/hw thread">thread</TD>
 
 
 
 * <TD title="low latency access method supported in BGPM Island Mode;  multiplexing supported in BGPM Distributed Mode;  ">olm</TD>
 * <TD title="Only suitable for counting cycles">c</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title=""></TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">Lines established for stream p...</TD> 
 * <TD style="display:none">Lines established for stream prefetch </TD>
 
 * </TR>
 * <TR>
 * <TD>144</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_L1P_BAS_HIT"></a>PEVT_L1P_BAS_HIT</TD>   
   
 * <TD style="text-align:left">L1p hit </TD>
      
 * <TD title="events attributable to current sw/hw thread">thread</TD>
 
 
 
 * <TD title="low latency access method supported in BGPM Island Mode;  multiplexing supported in BGPM Distributed Mode;  ">olm</TD>
 * <TD title="Only suitable for counting cycles">c</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title=""></TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">Hits in prefetch directory ...</TD> 
 * <TD style="display:none">Hits in prefetch directory </TD>
 
 * </TR>
 * <TR>
 * <TD>145</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_L1P_BAS_PF2DFC"></a>PEVT_L1P_BAS_PF2DFC</TD>   
   
 * <TD style="text-align:left">L1p pf2dfc</TD>
      
 * <TD title="events attributable to current sw/hw thread">thread</TD>
 
 
 
 * <TD title="low latency access method supported in BGPM Island Mode;  multiplexing supported in BGPM Distributed Mode;  ">olm</TD>
 * <TD title="Only suitable for counting cycles">c</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title=""></TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">Prefetch to demand fetch conve...</TD> 
 * <TD style="display:none">Prefetch to demand fetch conversions </TD>
 
 * </TR>
 * <TR>
 * <TD>146</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_L1P_BAS_MISS"></a>PEVT_L1P_BAS_MISS</TD>   
   
 * <TD style="text-align:left">L1p miss</TD>
      
 * <TD title="events attributable to current sw/hw thread">thread</TD>
 
 
 
 * <TD title="low latency access method supported in BGPM Island Mode;  multiplexing supported in BGPM Distributed Mode;  ">olm</TD>
 * <TD title="Only suitable for counting cycles">c</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title=""></TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">Misses in L1p by prefetchable ...</TD> 
 * <TD style="display:none">Misses in L1p by prefetchable loads </TD>
 
 * </TR>
 * <TR>
 * <TD>147</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_L1P_BAS_LU_DRAIN"></a>PEVT_L1P_BAS_LU_DRAIN</TD>   
   
 * <TD style="text-align:left">L1p LU drain</TD>
      
 * <TD title="events attributable to current sw/hw thread">thread</TD>
 
 
 
 * <TD title="low latency access method supported in BGPM Island Mode;  multiplexing supported in BGPM Distributed Mode;  ">olm</TD>
 * <TD title="Configured to count number of event by default">be</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title=""></TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">Lookup was held by guarded sto...</TD> 
 * <TD style="display:none">Lookup was held by guarded store waiting for drain </TD>
 
 * </TR>
 * <TR>
 * <TD>148</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_L1P_BAS_LU_DRAIN_CYC"></a>PEVT_L1P_BAS_LU_DRAIN_CYC</TD>   
   
 * <TD style="text-align:left">L1p LU drain Cyc</TD>
      
 * <TD title="events attributable to current sw/hw thread">thread</TD>
 
 
 
 * <TD title="low latency access method supported in BGPM Island Mode;  multiplexing supported in BGPM Distributed Mode;  ">olm</TD>
 * <TD title="Configured to count cycles by default">bc</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title=""></TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">Cycles lookup was held by guar...</TD> 
 * <TD style="display:none">Cycles lookup was held by guarded store waiting for drain </TD>
 
 * </TR>
 * <TR>
 * <TD>149</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_L1P_BAS_LD"></a>PEVT_L1P_BAS_LD</TD>   
   
 * <TD style="text-align:left">L1p Load</TD>
      
 * <TD title="events attributable to current sw/hw thread">thread</TD>
 
 
 
 * <TD title="low latency access method supported in BGPM Island Mode;  multiplexing supported in BGPM Distributed Mode;  ">olm</TD>
 * <TD title="Only suitable for counting cycles">c</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title=""></TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">Loads ...</TD> 
 * <TD style="display:none">Loads </TD>
 
 * </TR>
 * <TR>
 * <TD>150</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_L1P_BAS_ST_WC"></a>PEVT_L1P_BAS_ST_WC</TD>   
   
 * <TD style="text-align:left">L1p Store-WC</TD>
      
 * <TD title="events attributable to current sw/hw thread">thread</TD>
 
 
 
 * <TD title="low latency access method supported in BGPM Island Mode;  multiplexing supported in BGPM Distributed Mode;  ">olm</TD>
 * <TD title="Only suitable for counting cycles">c</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title=""></TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">Write combined stores (even an...</TD> 
 * <TD style="display:none">Write combined stores (even and odd bits to accomidate x1 to x2 clock conversion </TD>
 
 * </TR>
 * <TR>
 * <TD>151</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_L1P_BAS_ST_32BYTE"></a>PEVT_L1P_BAS_ST_32BYTE</TD>   
   
 * <TD style="text-align:left">L1p Store 32byte</TD>
      
 * <TD title="events attributable to current sw/hw thread">thread</TD>
 
 
 
 * <TD title="low latency access method supported in BGPM Island Mode;  multiplexing supported in BGPM Distributed Mode;  ">olm</TD>
 * <TD title="Only suitable for counting cycles">c</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title=""></TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">QPX stores ...</TD> 
 * <TD style="display:none">QPX stores </TD>
 
 * </TR>
 * <TR>
 * <TD>152</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_L1P_BAS_ST_CRACKED"></a>PEVT_L1P_BAS_ST_CRACKED</TD>   
   
 * <TD style="text-align:left">L1p Store-cracked</TD>
      
 * <TD title="events attributable to current sw/hw thread">thread</TD>
 
 
 
 * <TD title="low latency access method supported in BGPM Island Mode;  multiplexing supported in BGPM Distributed Mode;  ">olm</TD>
 * <TD title="Only suitable for counting cycles">c</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title=""></TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">Stores cracked into two switch...</TD> 
 * <TD style="display:none">Stores cracked into two switch requests </TD>
 
 * </TR>
 * <TR>
 * <TD>153</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_L1P_BAS_LU_STALL_SRT"></a>PEVT_L1P_BAS_LU_STALL_SRT</TD>   
   
 * <TD style="text-align:left">L1p LU stall-SRT</TD>
      
 * <TD title="events are core-wide and events can't directly be attributed to single thread">core</TD>
 
 
 
 * <TD title="low latency access method supported in BGPM Island Mode;  multiplexing supported in BGPM Distributed Mode;  ">olm</TD>
 * <TD title="Configured to count number of event by default">be</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title=""></TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">Lookup was held by full switch...</TD> 
 * <TD style="display:none">Lookup was held by full switch request table </TD>
 
 * </TR>
 * <TR>
 * <TD>154</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_L1P_BAS_LU_STALL_SRT_CYC"></a>PEVT_L1P_BAS_LU_STALL_SRT_CYC</TD>   
   
 * <TD style="text-align:left">L1p LU stall-SRT Cyc</TD>
      
 * <TD title="events are core-wide and events can't directly be attributed to single thread">core</TD>
 
 
 
 * <TD title="low latency access method supported in BGPM Island Mode;  multiplexing supported in BGPM Distributed Mode;  ">olm</TD>
 * <TD title="Configured to count cycles by default">bc</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title=""></TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">Cycles lookup was held by full...</TD> 
 * <TD style="display:none">Cycles lookup was held by full switch request table </TD>
 
 * </TR>
 * <TR>
 * <TD>155</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_L1P_BAS_LU_STALL_MMIO_DCR"></a>PEVT_L1P_BAS_LU_STALL_MMIO_DCR</TD>   
   
 * <TD style="text-align:left">L1p LU stall-MMIO/DCR</TD>
      
 * <TD title="events are core-wide and events can't directly be attributed to single thread">core</TD>
 
 
 
 * <TD title="low latency access method supported in BGPM Island Mode;  multiplexing supported in BGPM Distributed Mode;  ">olm</TD>
 * <TD title="Configured to count number of event by default">be</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title=""></TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">Lookup was held by MMIO or DCR...</TD> 
 * <TD style="display:none">Lookup was held by MMIO or DCR access </TD>
 
 * </TR>
 * <TR>
 * <TD>156</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_L1P_BAS_LU_STALL_MMIO_DCR_CYC"></a>PEVT_L1P_BAS_LU_STALL_MMIO_DCR_CYC</TD>   
   
 * <TD style="text-align:left">L1p LU stall-MMIO/DCR Cyc</TD>
      
 * <TD title="events are core-wide and events can't directly be attributed to single thread">core</TD>
 
 
 
 * <TD title="low latency access method supported in BGPM Island Mode;  multiplexing supported in BGPM Distributed Mode;  ">olm</TD>
 * <TD title="Configured to count cycles by default">bc</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title=""></TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">Cycles lookup was held by MMIO...</TD> 
 * <TD style="display:none">Cycles lookup was held by MMIO or DCR access </TD>
 
 * </TR>
 * <TR>
 * <TD>157</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_L1P_BAS_LU_STALL_STRM_DET"></a>PEVT_L1P_BAS_LU_STALL_STRM_DET</TD>   
   
 * <TD style="text-align:left">L1p LU stall-stream detect</TD>
      
 * <TD title="events are core-wide and events can't directly be attributed to single thread">core</TD>
 
 
 
 * <TD title="low latency access method supported in BGPM Island Mode;  multiplexing supported in BGPM Distributed Mode;  ">olm</TD>
 * <TD title="Configured to count number of event by default">be</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title=""></TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">Lookup was held while a stream...</TD> 
 * <TD style="display:none">Lookup was held while a stream was established </TD>
 
 * </TR>
 * <TR>
 * <TD>158</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_L1P_BAS_LU_STALL_STRM_DET_CYC"></a>PEVT_L1P_BAS_LU_STALL_STRM_DET_CYC</TD>   
   
 * <TD style="text-align:left">L1p LU stall-stream detect Cyc</TD>
      
 * <TD title="events are core-wide and events can't directly be attributed to single thread">core</TD>
 
 
 
 * <TD title="low latency access method supported in BGPM Island Mode;  multiplexing supported in BGPM Distributed Mode;  ">olm</TD>
 * <TD title="Configured to count cycles by default">bc</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title=""></TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">Cycles lookup was held while a...</TD> 
 * <TD style="display:none">Cycles lookup was held while a stream was established </TD>
 
 * </TR>
 * <TR>
 * <TD>159</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_L1P_BAS_LU_STALL_LIST_RD"></a>PEVT_L1P_BAS_LU_STALL_LIST_RD</TD>   
   
 * <TD style="text-align:left">L1p LU stall-list read</TD>
      
 * <TD title="events are core-wide and events can't directly be attributed to single thread">core</TD>
 
 
 
 * <TD title="low latency access method supported in BGPM Island Mode;  multiplexing supported in BGPM Distributed Mode;  ">olm</TD>
 * <TD title="Configured to count number of event by default">be</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title=""></TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">Lookup was held while list fet...</TD> 
 * <TD style="display:none">Lookup was held while list fetched addresses  </TD>
 
 * </TR>
 * <TR>
 * <TD>160</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_L1P_BAS_LU_STALL_LIST_RD_CYC"></a>PEVT_L1P_BAS_LU_STALL_LIST_RD_CYC</TD>   
   
 * <TD style="text-align:left">L1p LU stall-list read Cyc</TD>
      
 * <TD title="events are core-wide and events can't directly be attributed to single thread">core</TD>
 
 
 
 * <TD title="low latency access method supported in BGPM Island Mode;  multiplexing supported in BGPM Distributed Mode;  ">olm</TD>
 * <TD title="Configured to count cycles by default">bc</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title=""></TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">Cycles lookup was held while l...</TD> 
 * <TD style="display:none">Cycles lookup was held while list fetched addresses  </TD>
 
 * </TR>
 * <TR>
 * <TD>161</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_L1P_BAS_ST"></a>PEVT_L1P_BAS_ST</TD>   
   
 * <TD style="text-align:left">L1p Store</TD>
      
 * <TD title="events attributable to current sw/hw thread">thread</TD>
 
 
 
 * <TD title="low latency access method supported in BGPM Island Mode;  multiplexing supported in BGPM Distributed Mode;  ">olm</TD>
 * <TD title="Only suitable for counting cycles">c</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title=""></TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">Stores non-combined  ...</TD> 
 * <TD style="display:none">Stores non-combined  </TD>
 
 * </TR>
 * <TR>
 * <TD>162</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_L1P_BAS_LU_STALL_LIST_WRT"></a>PEVT_L1P_BAS_LU_STALL_LIST_WRT</TD>   
   
 * <TD style="text-align:left">L1p LU stall-list write</TD>
      
 * <TD title="events are core-wide and events can't directly be attributed to single thread">core</TD>
 
 
 
 * <TD title="low latency access method supported in BGPM Island Mode;  multiplexing supported in BGPM Distributed Mode;  ">olm</TD>
 * <TD title="Configured to count number of event by default">be</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title=""></TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">Lookup was held while list wro...</TD> 
 * <TD style="display:none">Lookup was held while list wrote list data </TD>
 
 * </TR>
 * <TR>
 * <TD>163</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_L1P_BAS_LU_STALL_LIST_WRT_CYC"></a>PEVT_L1P_BAS_LU_STALL_LIST_WRT_CYC</TD>   
   
 * <TD style="text-align:left">L1p LU stall-list write Cyc</TD>
      
 * <TD title="events are core-wide and events can't directly be attributed to single thread">core</TD>
 
 
 
 * <TD title="low latency access method supported in BGPM Island Mode;  multiplexing supported in BGPM Distributed Mode;  ">olm</TD>
 * <TD title="Configured to count cycles by default">bc</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title=""></TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">Cycles lookup was held while l...</TD> 
 * <TD style="display:none">Cycles lookup was held while list wrote list data </TD>
 
 * </TR><TR><TD colspan="11" style="text-align:left"><a class="anchor" id="a_l1p_switch"></a><B><I>L1p Switch Event</I></B></TD></TR>
 * <TR>
 * <TD>164</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_L1P_SW_MAS_SW_REQ_VAL"></a>PEVT_L1P_SW_MAS_SW_REQ_VAL</TD>   
   
 * <TD style="text-align:left">L1p switch master sw req valid</TD>
      
 * <TD title="events are core-wide and events can't directly be attributed to single thread">core</TD>
 
 
 
 * <TD title="low latency access method supported in BGPM Island Mode;  multiplexing supported within BGPM Distributed Mode only within single L1p mode;  ">olM</TD>
 * <TD title="Only suitable for counting cycles">c</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title=""></TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">requests to the sw xbar ...</TD> 
 * <TD style="display:none">requests to the sw xbar </TD>
 
 * </TR>
 * <TR>
 * <TD>165</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_L1P_SW_MAS_SW_REQ_GATE"></a>PEVT_L1P_SW_MAS_SW_REQ_GATE</TD>   
   
 * <TD style="text-align:left">L1p switch master sw req gate</TD>
      
 * <TD title="events are core-wide and events can't directly be attributed to single thread">core</TD>
 
 
 
 * <TD title="low latency access method supported in BGPM Island Mode;  multiplexing supported within BGPM Distributed Mode only within single L1p mode;  ">olM</TD>
 * <TD title="Only suitable for counting cycles">c</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title=""></TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">active request info cycles to ...</TD> 
 * <TD style="display:none">active request info cycles to the sw xbar </TD>
 
 * </TR>
 * <TR>
 * <TD>166</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_L1P_SW_MAS_SW_DATA_GATE"></a>PEVT_L1P_SW_MAS_SW_DATA_GATE</TD>   
   
 * <TD style="text-align:left">L1p switch master sw data gate</TD>
      
 * <TD title="events are core-wide and events can't directly be attributed to single thread">core</TD>
 
 
 
 * <TD title="low latency access method supported in BGPM Island Mode;  multiplexing supported within BGPM Distributed Mode only within single L1p mode;  ">olM</TD>
 * <TD title="Only suitable for counting cycles">c</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title=""></TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">active write data cycles to th...</TD> 
 * <TD style="display:none">active write data cycles to the sw xbar  </TD>
 
 * </TR>
 * <TR>
 * <TD>167</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_L1P_SW_SR_MAS_RD_VAL_2"></a>PEVT_L1P_SW_SR_MAS_RD_VAL_2</TD>   
   
 * <TD style="text-align:left">L1p switch sr master rd valid 2 </TD>
      
 * <TD title="events are core-wide and events can't directly be attributed to single thread">core</TD>
 
 
 
 * <TD title="low latency access method supported in BGPM Island Mode;  multiplexing supported within BGPM Distributed Mode only within single L1p mode;  ">olM</TD>
 * <TD title="Only suitable for counting cycles">c</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title=""></TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">read response cycles from the ...</TD> 
 * <TD style="display:none">read response cycles from the sr xbar </TD>
 
 * </TR>
 * <TR>
 * <TD>168</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_L1P_SW_SI_MAS_REQ_VAL_2"></a>PEVT_L1P_SW_SI_MAS_REQ_VAL_2</TD>   
   
 * <TD style="text-align:left">L1p switch si master req valid 2</TD>
      
 * <TD title="events are core-wide and events can't directly be attributed to single thread">core</TD>
 
 
 
 * <TD title="low latency access method supported in BGPM Island Mode;  multiplexing supported within BGPM Distributed Mode only within single L1p mode;  ">olM</TD>
 * <TD title="Only suitable for counting cycles">c</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title=""></TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">requests to the invalidation(s...</TD> 
 * <TD style="display:none">requests to the invalidation(si) xbar </TD>
 
 * </TR>
 * <TR>
 * <TD>169</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_L1P_SW_SW_MAS_SKED_VAL_2"></a>PEVT_L1P_SW_SW_MAS_SKED_VAL_2</TD>   
   
 * <TD style="text-align:left">L1p switch sw master sked valid 2</TD>
      
 * <TD title="events are core-wide and events can't directly be attributed to single thread">core</TD>
 
 
 
 * <TD title="low latency access method supported in BGPM Island Mode;  multiplexing supported within BGPM Distributed Mode only within single L1p mode;  ">olM</TD>
 * <TD title="Only suitable for counting cycles">c</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title=""></TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">"normally" scheduled sw xbar r...</TD> 
 * <TD style="display:none">"normally" scheduled sw xbar requests </TD>
 
 * </TR>
 * <TR>
 * <TD>170</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_L1P_SW_SW_MAS_EAGER_VAL_2"></a>PEVT_L1P_SW_SW_MAS_EAGER_VAL_2</TD>   
   
 * <TD style="text-align:left">L1p switch sw master eager valid 2</TD>
      
 * <TD title="events are core-wide and events can't directly be attributed to single thread">core</TD>
 
 
 
 * <TD title="low latency access method supported in BGPM Island Mode;  multiplexing supported within BGPM Distributed Mode only within single L1p mode;  ">olM</TD>
 * <TD title="Only suitable for counting cycles">c</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title=""></TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">"eagerly" scheduled sw xbar re...</TD> 
 * <TD style="display:none">"eagerly" scheduled sw xbar requests </TD>
 
 * </TR>
 * <TR>
 * <TD>171</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_L1P_SW_TLB_FILL"></a>PEVT_L1P_SW_TLB_FILL</TD>   
   
 * <TD style="text-align:left">L1p switch tlb fill</TD>
      
 * <TD title="events are core-wide and events can't directly be attributed to single thread">core</TD>
 
 
 
 * <TD title="low latency access method supported in BGPM Island Mode;  multiplexing supported within BGPM Distributed Mode only within single L1p mode;  ">olM</TD>
 * <TD title="Only suitable for counting cycles">c</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title=""></TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">mmu read requests ...</TD> 
 * <TD style="display:none">mmu read requests </TD>
 
 * </TR>
 * <TR>
 * <TD>172</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_L1P_SW_STWCX_FAIL"></a>PEVT_L1P_SW_STWCX_FAIL</TD>   
   
 * <TD style="text-align:left">L1p switch stwcx fail</TD>
      
 * <TD title="events attributable to current sw/hw thread">thread</TD>
 
 
 
 * <TD title="low latency access method supported in BGPM Island Mode;  multiplexing supported within BGPM Distributed Mode only within single L1p mode;  ">olM</TD>
 * <TD title="Only suitable for counting cycles">c</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title=""></TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">failed stwcx (store with reser...</TD> 
 * <TD style="display:none">failed stwcx (store with reservation) requests </TD>
 
 * </TR>
 * <TR>
 * <TD>173</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_L1P_SW_STWCX"></a>PEVT_L1P_SW_STWCX</TD>   
   
 * <TD style="text-align:left">L1p switch stwcx</TD>
      
 * <TD title="events attributable to current sw/hw thread">thread</TD>
 
 
 
 * <TD title="low latency access method supported in BGPM Island Mode;  multiplexing supported within BGPM Distributed Mode only within single L1p mode;  ">olM</TD>
 * <TD title="Only suitable for counting cycles">c</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title=""></TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">stwcx (store with reservation)...</TD> 
 * <TD style="display:none">stwcx (store with reservation) requests </TD>
 
 * </TR>
 * <TR>
 * <TD>174</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_L1P_SW_I_FETCH"></a>PEVT_L1P_SW_I_FETCH</TD>   
   
 * <TD style="text-align:left">L1p switch I-fetch</TD>
      
 * <TD title="events attributable to current sw/hw thread">thread</TD>
 
 
 
 * <TD title="low latency access method supported in BGPM Island Mode;  multiplexing supported within BGPM Distributed Mode only within single L1p mode;  ">olM</TD>
 * <TD title="Only suitable for counting cycles">c</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title=""></TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">instruction fetch requests ...</TD> 
 * <TD style="display:none">instruction fetch requests </TD>
 
 * </TR>
 * <TR>
 * <TD>175</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_L1P_SW_MSYNC"></a>PEVT_L1P_SW_MSYNC</TD>   
   
 * <TD style="text-align:left">L1p switch Msync</TD>
      
 * <TD title="events attributable to current sw/hw thread">thread</TD>
 
 
 
 * <TD title="low latency access method supported in BGPM Island Mode;  multiplexing supported within BGPM Distributed Mode only within single L1p mode;  ">olM</TD>
 * <TD title="Only suitable for counting cycles">c</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title=""></TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">hwsync requests ...</TD> 
 * <TD style="display:none">hwsync requests </TD>
 
 * </TR>
 * <TR>
 * <TD>176</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_L1P_SW_LWARX"></a>PEVT_L1P_SW_LWARX</TD>   
   
 * <TD style="text-align:left">L1p switch Lwarx</TD>
      
 * <TD title="events attributable to current sw/hw thread">thread</TD>
 
 
 
 * <TD title="low latency access method supported in BGPM Island Mode;  multiplexing supported within BGPM Distributed Mode only within single L1p mode;  ">olM</TD>
 * <TD title="Only suitable for counting cycles">c</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title=""></TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">lwarx (load with reservation) ...</TD> 
 * <TD style="display:none">lwarx (load with reservation) requests  </TD>
 
 * </TR>
 * <TR>
 * <TD>177</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_L1P_SW_KILL_L2_RSV"></a>PEVT_L1P_SW_KILL_L2_RSV</TD>   
   
 * <TD style="text-align:left">L1p switch L1p killed L2 slice reserve</TD>
      
 * <TD title="events are core-wide and events can't directly be attributed to single thread">core</TD>
 
 
 
 * <TD title="low latency access method supported in BGPM Island Mode;  multiplexing supported within BGPM Distributed Mode only within single L1p mode;  ">olM</TD>
 * <TD title="Only suitable for counting cycles">c</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title=""></TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">Lwarx reservation kill sent fr...</TD> 
 * <TD style="display:none">Lwarx reservation kill sent from L1p to L2 slice </TD>
 
 * </TR>
 * <TR>
 * <TD>178</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_L1P_SW_L2_CANCEL_A2_RSV"></a>PEVT_L1P_SW_L2_CANCEL_A2_RSV</TD>   
   
 * <TD style="text-align:left">L1p switch L2 slice cancelled A2 reserve</TD>
      
 * <TD title="events are core-wide and events can't directly be attributed to single thread">core</TD>
 
 
 
 * <TD title="low latency access method supported in BGPM Island Mode;  multiplexing supported within BGPM Distributed Mode only within single L1p mode;  ">olM</TD>
 * <TD title="Only suitable for counting cycles">c</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title=""></TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">Lwarx reservation kill send fr...</TD> 
 * <TD style="display:none">Lwarx reservation kill send from L2 slice to L1p  </TD>
 
 * </TR>
 * <TR>
 * <TD>179</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_L1P_SW_L1_INVAL"></a>PEVT_L1P_SW_L1_INVAL</TD>   
   
 * <TD style="text-align:left">L1p switch L1 inval</TD>
      
 * <TD title="events are core-wide and events can't directly be attributed to single thread">core</TD>
 
 
 
 * <TD title="low latency access method supported in BGPM Island Mode;  multiplexing supported within BGPM Distributed Mode only within single L1p mode;  ">olM</TD>
 * <TD title="Only suitable for counting cycles">c</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title=""></TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">invalidate to L1 ...</TD> 
 * <TD style="display:none">invalidate to L1 </TD>
 
 * </TR>
 * <TR>
 * <TD>180</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_L1P_SW_WC_EVICT_ADDR"></a>PEVT_L1P_SW_WC_EVICT_ADDR</TD>   
   
 * <TD style="text-align:left">L1p switch WC evict by nearby address</TD>
      
 * <TD title="events are core-wide and events can't directly be attributed to single thread">core</TD>
 
 
 
 * <TD title="low latency access method supported in BGPM Island Mode;  multiplexing supported within BGPM Distributed Mode only within single L1p mode;  ">olM</TD>
 * <TD title="Only suitable for counting cycles">c</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title=""></TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">write combine buffer was evict...</TD> 
 * <TD style="display:none">write combine buffer was evicted by store to same 128 byte window </TD>
 
 * </TR><TR><TD colspan="11" style="text-align:left"><a class="anchor" id="a_l1p_stream"></a><B><I>L1p Stream Event</I></B></TD></TR>
 * <TR>
 * <TD>181</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_L1P_STRM_LINE_ESTB"></a>PEVT_L1P_STRM_LINE_ESTB</TD>   
   
 * <TD style="text-align:left">L1p stream L1p single line established</TD>
      
 * <TD title="events are core-wide and events can't directly be attributed to single thread">core</TD>
 
 
 
 * <TD title="low latency access method supported in BGPM Island Mode;  multiplexing supported within BGPM Distributed Mode only within single L1p mode;  ">olM</TD>
 * <TD title="Only suitable for counting cycles">c</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title=""></TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">lines established for any reas...</TD> 
 * <TD style="display:none">lines established for any reason and thread </TD>
 
 * </TR>
 * <TR>
 * <TD>182</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_L1P_STRM_HIT_FWD"></a>PEVT_L1P_STRM_HIT_FWD</TD>   
   
 * <TD style="text-align:left">L1p stream L1p hit forwarded</TD>
      
 * <TD title="events are core-wide and events can't directly be attributed to single thread">core</TD>
 
 
 
 * <TD title="low latency access method supported in BGPM Island Mode;  multiplexing supported within BGPM Distributed Mode only within single L1p mode;  ">olM</TD>
 * <TD title="Only suitable for counting cycles">c</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title=""></TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">Hit in L1p forwarded to L2 (me...</TD> 
 * <TD style="display:none">Hit in L1p forwarded to L2 (measure speculation traffic) </TD>
 
 * </TR>
 * <TR>
 * <TD>183</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_L1P_STRM_L1_HIT_FWD"></a>PEVT_L1P_STRM_L1_HIT_FWD</TD>   
   
 * <TD style="text-align:left">L1p stream L1 hit forwarded</TD>
      
 * <TD title="events are core-wide and events can't directly be attributed to single thread">core</TD>
 
 
 
 * <TD title="low latency access method supported in BGPM Island Mode;  multiplexing supported within BGPM Distributed Mode only within single L1p mode;  ">olM</TD>
 * <TD title="Only suitable for counting cycles">c</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title=""></TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">Hit in L1 forwarded to L2 ...</TD> 
 * <TD style="display:none">Hit in L1 forwarded to L2 </TD>
 
 * </TR>
 * <TR>
 * <TD>184</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_L1P_STRM_EVICT_UNUSED"></a>PEVT_L1P_STRM_EVICT_UNUSED</TD>   
   
 * <TD style="text-align:left">L1p stream L1p evict unused</TD>
      
 * <TD title="events are core-wide and events can't directly be attributed to single thread">core</TD>
 
 
 
 * <TD title="low latency access method supported in BGPM Island Mode;  multiplexing supported within BGPM Distributed Mode only within single L1p mode;  ">olM</TD>
 * <TD title="Only suitable for counting cycles">c</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title=""></TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">Lines fetched and never hit ev...</TD> 
 * <TD style="display:none">Lines fetched and never hit evicted </TD>
 
 * </TR>
 * <TR>
 * <TD>185</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_L1P_STRM_EVICT_PART_USED"></a>PEVT_L1P_STRM_EVICT_PART_USED</TD>   
   
 * <TD style="text-align:left">L1p stream L1p evict partially used</TD>
      
 * <TD title="events are core-wide and events can't directly be attributed to single thread">core</TD>
 
 
 
 * <TD title="low latency access method supported in BGPM Island Mode;  multiplexing supported within BGPM Distributed Mode only within single L1p mode;  ">olM</TD>
 * <TD title="Only suitable for counting cycles">c</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title=""></TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">Line fetched and only partiall...</TD> 
 * <TD style="display:none">Line fetched and only partially used evicted </TD>
 
 * </TR>
 * <TR>
 * <TD>186</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_L1P_STRM_REMOTE_INVAL_MATCH"></a>PEVT_L1P_STRM_REMOTE_INVAL_MATCH</TD>   
   
 * <TD style="text-align:left">L1p stream L1p Remote Invalidate match</TD>
      
 * <TD title="events are core-wide and events can't directly be attributed to single thread">core</TD>
 
 
 
 * <TD title="low latency access method supported in BGPM Island Mode;  multiplexing supported within BGPM Distributed Mode only within single L1p mode;  ">olM</TD>
 * <TD title="Only suitable for counting cycles">c</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title=""></TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">Remote invalidate collided wit...</TD> 
 * <TD style="display:none">Remote invalidate collided with a valid line </TD>
 
 * </TR>
 * <TR>
 * <TD>187</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_L1P_STRM_DONT_CACHE"></a>PEVT_L1P_STRM_DONT_CACHE</TD>   
   
 * <TD style="text-align:left">L1p stream L1p don't cache</TD>
      
 * <TD title="events are core-wide and events can't directly be attributed to single thread">core</TD>
 
 
 
 * <TD title="low latency access method supported in BGPM Island Mode;  multiplexing supported within BGPM Distributed Mode only within single L1p mode;  ">olM</TD>
 * <TD title="Only suitable for counting cycles">c</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title=""></TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">Don't cache bit was set for a ...</TD> 
 * <TD style="display:none">Don't cache bit was set for a line fill </TD>
 
 * </TR>
 * <TR>
 * <TD>188</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_L1P_STRM_STRM_DEPTH_STEAL"></a>PEVT_L1P_STRM_STRM_DEPTH_STEAL</TD>   
   
 * <TD style="text-align:left">L1p stream L1p Stream depth steal</TD>
      
 * <TD title="events attributable to current sw/hw thread">thread</TD>
 
 
 
 * <TD title="low latency access method supported in BGPM Island Mode;  multiplexing supported within BGPM Distributed Mode only within single L1p mode;  ">olM</TD>
 * <TD title="Only suitable for counting cycles">c</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title=""></TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">Adaptation events per thread ...</TD> 
 * <TD style="display:none">Adaptation events per thread </TD>
 
 * </TR>
 * <TR>
 * <TD>189</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_L1P_STRM_STRM_ESTB"></a>PEVT_L1P_STRM_STRM_ESTB</TD>   
   
 * <TD style="text-align:left">L1p stream L1p streams established</TD>
      
 * <TD title="events attributable to current sw/hw thread">thread</TD>
 
 
 
 * <TD title="low latency access method supported in BGPM Island Mode;  multiplexing supported within BGPM Distributed Mode only within single L1p mode;  ">olM</TD>
 * <TD title="Only suitable for counting cycles">c</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title=""></TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">streams detected and establish...</TD> 
 * <TD style="display:none">streams detected and established </TD>
 
 * </TR>
 * <TR>
 * <TD>190</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_L1P_STRM_WRT_INVAL"></a>PEVT_L1P_STRM_WRT_INVAL</TD>   
   
 * <TD style="text-align:left">L1p stream L1p Write Invalidate</TD>
      
 * <TD title="events attributable to current sw/hw thread">thread</TD>
 
 
 
 * <TD title="low latency access method supported in BGPM Island Mode;  multiplexing supported within BGPM Distributed Mode only within single L1p mode;  ">olM</TD>
 * <TD title="Only suitable for counting cycles">c</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title=""></TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">Local write collided with a va...</TD> 
 * <TD style="display:none">Local write collided with a valid line </TD>
 
 * </TR>
 * <TR>
 * <TD>191</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_L1P_STRM_LINE_ESTB_ALL_LIST"></a>PEVT_L1P_STRM_LINE_ESTB_ALL_LIST</TD>   
   
 * <TD style="text-align:left">L1p stream L1p line establish (all list)</TD>
      
 * <TD title="events are core-wide and events can't directly be attributed to single thread">core</TD>
 
 
 
 * <TD title="low latency access method supported in BGPM Island Mode;  multiplexing supported within BGPM Distributed Mode only within single L1p mode;  ">olM</TD>
 * <TD title="Only suitable for counting cycles">c</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title=""></TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">Lines established by all list ...</TD> 
 * <TD style="display:none">Lines established by all list engines </TD>
 
 * </TR>
 * <TR>
 * <TD>192</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_L1P_STRM_HIT_LIST"></a>PEVT_L1P_STRM_HIT_LIST</TD>   
   
 * <TD style="text-align:left">L1p stream L1p hit (list)</TD>
      
 * <TD title="events attributable to current sw/hw thread">thread</TD>
 
 
 
 * <TD title="low latency access method supported in BGPM Island Mode;  multiplexing supported within BGPM Distributed Mode only within single L1p mode;  ">olM</TD>
 * <TD title="Only suitable for counting cycles">c</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title=""></TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">Hits for lines fetched by list...</TD> 
 * <TD style="display:none">Hits for lines fetched by list engine </TD>
 
 * </TR>
 * <TR>
 * <TD>193</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_L1P_STRM_PF2DFC_LIST"></a>PEVT_L1P_STRM_PF2DFC_LIST</TD>   
   
 * <TD style="text-align:left">L1p stream L1p pf2dfc (list)</TD>
      
 * <TD title="events attributable to current sw/hw thread">thread</TD>
 
 
 
 * <TD title="low latency access method supported in BGPM Island Mode;  multiplexing supported within BGPM Distributed Mode only within single L1p mode;  ">olM</TD>
 * <TD title="Only suitable for counting cycles">c</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title=""></TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">Prefetch to demand fetch conve...</TD> 
 * <TD style="display:none">Prefetch to demand fetch conversions for line established by list </TD>
 
 * </TR>
 * <TR>
 * <TD>194</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_L1P_STRM_PART_INVAL_REFCH"></a>PEVT_L1P_STRM_PART_INVAL_REFCH</TD>   
   
 * <TD style="text-align:left">L1p stream L1p partial invalid refetched</TD>
      
 * <TD title="events attributable to current sw/hw thread">thread</TD>
 
 
 
 * <TD title="low latency access method supported in BGPM Island Mode;  multiplexing supported within BGPM Distributed Mode only within single L1p mode;  ">olM</TD>
 * <TD title="Only suitable for counting cycles">c</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title=""></TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">Partially invalid line refetch...</TD> 
 * <TD style="display:none">Partially invalid line refetched </TD>
 
 * </TR><TR><TD colspan="11" style="text-align:left"><a class="anchor" id="a_l1p_list"></a><B><I>L1p List Event</I></B></TD></TR>
 * <TR>
 * <TD>195</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_L1P_LIST_SKIP_1"></a>PEVT_L1P_LIST_SKIP_1</TD>   
   
 * <TD style="text-align:left">L1p list skip by 1</TD>
      
 * <TD title="events are core-wide and events can't directly be attributed to single thread">core</TD>
 
 
 
 * <TD title="low latency access method supported in BGPM Island Mode;  multiplexing supported within BGPM Distributed Mode only within single L1p mode;  ">olM</TD>
 * <TD title="Only suitable for counting cycles">c</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title=""></TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">core address skipped by 1 (any...</TD> 
 * <TD style="display:none">core address skipped by 1 (any thread) </TD>
 
 * </TR>
 * <TR>
 * <TD>196</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_L1P_LIST_SKIP_2"></a>PEVT_L1P_LIST_SKIP_2</TD>   
   
 * <TD style="text-align:left">L1p list skip by 2</TD>
      
 * <TD title="events are core-wide and events can't directly be attributed to single thread">core</TD>
 
 
 
 * <TD title="low latency access method supported in BGPM Island Mode;  multiplexing supported within BGPM Distributed Mode only within single L1p mode;  ">olM</TD>
 * <TD title="Only suitable for counting cycles">c</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title=""></TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">core address skipped by 2 (any...</TD> 
 * <TD style="display:none">core address skipped by 2 (any thread) </TD>
 
 * </TR>
 * <TR>
 * <TD>197</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_L1P_LIST_SKIP_3"></a>PEVT_L1P_LIST_SKIP_3</TD>   
   
 * <TD style="text-align:left">L1p list skip by 3</TD>
      
 * <TD title="events are core-wide and events can't directly be attributed to single thread">core</TD>
 
 
 
 * <TD title="low latency access method supported in BGPM Island Mode;  multiplexing supported within BGPM Distributed Mode only within single L1p mode;  ">olM</TD>
 * <TD title="Only suitable for counting cycles">c</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title=""></TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">core address skipped by 3 (any...</TD> 
 * <TD style="display:none">core address skipped by 3 (any thread) </TD>
 
 * </TR>
 * <TR>
 * <TD>198</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_L1P_LIST_SKIP_4"></a>PEVT_L1P_LIST_SKIP_4</TD>   
   
 * <TD style="text-align:left">L1p list skip by 4</TD>
      
 * <TD title="events are core-wide and events can't directly be attributed to single thread">core</TD>
 
 
 
 * <TD title="low latency access method supported in BGPM Island Mode;  multiplexing supported within BGPM Distributed Mode only within single L1p mode;  ">olM</TD>
 * <TD title="Only suitable for counting cycles">c</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title=""></TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">core address skipped by 4 (any...</TD> 
 * <TD style="display:none">core address skipped by 4 (any thread) </TD>
 
 * </TR>
 * <TR>
 * <TD>199</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_L1P_LIST_SKIP_5"></a>PEVT_L1P_LIST_SKIP_5</TD>   
   
 * <TD style="text-align:left">L1p list skip by 5</TD>
      
 * <TD title="events are core-wide and events can't directly be attributed to single thread">core</TD>
 
 
 
 * <TD title="low latency access method supported in BGPM Island Mode;  multiplexing supported within BGPM Distributed Mode only within single L1p mode;  ">olM</TD>
 * <TD title="Only suitable for counting cycles">c</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title=""></TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">core address skipped by 5 (any...</TD> 
 * <TD style="display:none">core address skipped by 5 (any thread) </TD>
 
 * </TR>
 * <TR>
 * <TD>200</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_L1P_LIST_SKIP_6"></a>PEVT_L1P_LIST_SKIP_6</TD>   
   
 * <TD style="text-align:left">L1p list skip by 6</TD>
      
 * <TD title="events are core-wide and events can't directly be attributed to single thread">core</TD>
 
 
 
 * <TD title="low latency access method supported in BGPM Island Mode;  multiplexing supported within BGPM Distributed Mode only within single L1p mode;  ">olM</TD>
 * <TD title="Only suitable for counting cycles">c</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title=""></TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">core address skipped by 6 (any...</TD> 
 * <TD style="display:none">core address skipped by 6 (any thread) </TD>
 
 * </TR>
 * <TR>
 * <TD>201</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_L1P_LIST_SKIP_7"></a>PEVT_L1P_LIST_SKIP_7</TD>   
   
 * <TD style="text-align:left">L1p list skip by 7</TD>
      
 * <TD title="events are core-wide and events can't directly be attributed to single thread">core</TD>
 
 
 
 * <TD title="low latency access method supported in BGPM Island Mode;  multiplexing supported within BGPM Distributed Mode only within single L1p mode;  ">olM</TD>
 * <TD title="Only suitable for counting cycles">c</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title=""></TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">core address skipped by 7 (any...</TD> 
 * <TD style="display:none">core address skipped by 7 (any thread) </TD>
 
 * </TR>
 * <TR>
 * <TD>202</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_L1P_LIST_ABANDON"></a>PEVT_L1P_LIST_ABANDON</TD>   
   
 * <TD style="text-align:left">L1p list abandoned</TD>
      
 * <TD title="events attributable to current sw/hw thread">thread</TD>
 
 
 
 * <TD title="low latency access method supported in BGPM Island Mode;  multiplexing supported within BGPM Distributed Mode only within single L1p mode;  ">olM</TD>
 * <TD title="Only suitable for counting cycles">c</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title=""></TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">A2 loads mismatching pattern r...</TD> 
 * <TD style="display:none">A2 loads mismatching pattern resulted in abandoned list prefetch </TD>
 
 * </TR>
 * <TR>
 * <TD>203</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_L1P_LIST_CMP"></a>PEVT_L1P_LIST_CMP</TD>   
   
 * <TD style="text-align:left">L1p list comparison</TD>
      
 * <TD title="events attributable to current sw/hw thread">thread</TD>
 
 
 
 * <TD title="low latency access method supported in BGPM Island Mode;  multiplexing supported within BGPM Distributed Mode only within single L1p mode;  ">olM</TD>
 * <TD title="Only suitable for counting cycles">c</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title=""></TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">core address was compared agai...</TD> 
 * <TD style="display:none">core address was compared against list </TD>
 
 * </TR>
 * <TR>
 * <TD>204</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_L1P_LIST_SKIP"></a>PEVT_L1P_LIST_SKIP</TD>   
   
 * <TD style="text-align:left">L1p list skip</TD>
      
 * <TD title="events attributable to current sw/hw thread">thread</TD>
 
 
 
 * <TD title="low latency access method supported in BGPM Island Mode;  multiplexing supported within BGPM Distributed Mode only within single L1p mode;  ">olM</TD>
 * <TD title="Only suitable for counting cycles">c</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title=""></TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">core address matched a non hea...</TD> 
 * <TD style="display:none">core address matched a non head of queue list address (per thread) </TD>
 
 * </TR>
 * <TR>
 * <TD>205</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_L1P_LIST_MISMATCH"></a>PEVT_L1P_LIST_MISMATCH</TD>   
   
 * <TD style="text-align:left">L1p list mismatch </TD>
      
 * <TD title="events attributable to current sw/hw thread">thread</TD>
 
 
 
 * <TD title="low latency access method supported in BGPM Island Mode;  multiplexing supported within BGPM Distributed Mode only within single L1p mode;  ">olM</TD>
 * <TD title="Only suitable for counting cycles">c</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title=""></TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">core address does not match a ...</TD> 
 * <TD style="display:none">core address does not match a list address </TD>
 
 * </TR>
 * <TR>
 * <TD>206</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_L1P_LIST_STARTED"></a>PEVT_L1P_LIST_STARTED</TD>   
   
 * <TD style="text-align:left">L1p list started.</TD>
      
 * <TD title="events attributable to current sw/hw thread">thread</TD>
 
 
 
 * <TD title="low latency access method supported in BGPM Island Mode;  multiplexing supported within BGPM Distributed Mode only within single L1p mode;  ">olM</TD>
 * <TD title="Only suitable for counting cycles">c</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title=""></TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">List prefetch process was star...</TD> 
 * <TD style="display:none">List prefetch process was started </TD>
 
 * </TR>
 * <TR>
 * <TD>207</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_L1P_LIST_OVF_MEM"></a>PEVT_L1P_LIST_OVF_MEM</TD>   
   
 * <TD style="text-align:left">L1p list overflow mem</TD>
      
 * <TD title="events attributable to current sw/hw thread">thread</TD>
 
 
 
 * <TD title="low latency access method supported in BGPM Island Mode;  multiplexing supported within BGPM Distributed Mode only within single L1p mode;  ">olM</TD>
 * <TD title="Only suitable for counting cycles">c</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title=""></TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">Written pattern exceeded alloc...</TD> 
 * <TD style="display:none">Written pattern exceeded allocated buffer </TD>
 
 * </TR>
 * <TR>
 * <TD>208</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_L1P_LIST_CMP_OVRUN_PREFCH"></a>PEVT_L1P_LIST_CMP_OVRUN_PREFCH</TD>   
   
 * <TD style="text-align:left">L1p list cmp overrun prefetch</TD>
      
 * <TD title="events attributable to current sw/hw thread">thread</TD>
 
 
 
 * <TD title="low latency access method supported in BGPM Island Mode;  multiplexing supported within BGPM Distributed Mode only within single L1p mode;  ">olM</TD>
 * <TD title="Only suitable for counting cycles">c</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title=""></TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">core address advances faster t...</TD> 
 * <TD style="display:none">core address advances faster than prefetch lines can be established dropping prefetches </TD>
 
 * </TR><TR><TD colspan="11" style="text-align:left"><a class="anchor" id="a_wakeup_unit"></a><B><I>Wakeup Unit Event</I></B></TD></TR>
 * <TR>
 * <TD>209</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_WAKE_HIT_10"></a>PEVT_WAKE_HIT_10</TD>   
   
 * <TD style="text-align:left">Wakeup hit 10</TD>
      
 * <TD title="events attributable to current sw/hw thread">thread</TD>
 
 
 
 * <TD title="low latency access method supported in BGPM Island Mode;  multiplexing supported in BGPM Distributed Mode;  ">olm</TD>
 * <TD title="Only suitable for counting cycles">c</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title=""></TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">wakeup unit address compare 10...</TD> 
 * <TD style="display:none">wakeup unit address compare 10/11 hit the target address </TD>
 
 * </TR>
 * <TR>
 * <TD>210</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_WAKE_HIT_11"></a>PEVT_WAKE_HIT_11</TD>   
   
 * <TD style="text-align:left">Wakeup hit 11</TD>
      
 * <TD title="events attributable to current sw/hw thread">thread</TD>
 
 
 
 * <TD title="low latency access method supported in BGPM Island Mode;  multiplexing supported in BGPM Distributed Mode;  ">olm</TD>
 * <TD title="Only suitable for counting cycles">c</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title=""></TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">wakeup unit address compare 10...</TD> 
 * <TD style="display:none">wakeup unit address compare 10/11 hit the target address </TD>
 
 * </TR><TR><TD colspan="11" style="text-align:left"><a class="anchor" id="a_xu_instruction"></a><B><I>XU Instruction Opcode Counting</I></B></TD></TR>
 * <TR>
 * <TD>211</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_CYCLES"></a>PEVT_CYCLES</TD>   
   
 * <TD style="text-align:left">x1 cycles</TD>
      
 * <TD title="events attributable to current sw/hw thread">thread</TD>
 
 
 
 * <TD title="low latency access method supported in BGPM Island Mode;  multiplexing supported in BGPM Distributed Mode;  ">olm</TD>
 * <TD title="Only suitable for counting cycles">c</TD>
 
 * <TD style="text-align:left">PAPI_TOT_CYC</TD>
   
 * <TD title="Bgpm validated - counts confidently verified in controlled bgpm test.">v</TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">Total CPU Cycles  ...</TD> 
 * <TD style="display:none">Total CPU Cycles  </TD>
 
 * </TR>
 * <TR>
 * <TD>212</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_INST_XU_MATCH"></a>PEVT_INST_XU_MATCH</TD>   
   
 * <TD style="text-align:left">Match XU OpCode</TD>
      
 * <TD title="events attributable to current sw/hw thread">thread</TD>
 
 
 
 * <TD title="low latency access method supported in BGPM Island Mode;  multiplexing supported in BGPM Distributed Mode;  ">olm</TD>
 * <TD title="Single cycle event, cycles and events are synonymous">s</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title="Bgpm validated - counts confidently verified in controlled bgpm test.">v</TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">Perform a major/minor opcode m...</TD> 
 * <TD style="display:none">Perform a major/minor opcode match on completed XU instructions. Use Bgpm_SetXuMatch() on event to choose pattern. Only one may be set per core. If not set, will count all completed XU Instructions. Choosing whether to match is multiplexable, but the pattern to match is not. </TD>
 
 * </TR>
 * <TR>
 * <TD>213</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_INST_XU_GRP_MASK"></a>PEVT_INST_XU_GRP_MASK</TD>   
   
 * <TD style="text-align:left">XU OpCode Mask Multiple Groups</TD>
      
 * <TD title="events attributable to current sw/hw thread">thread</TD>
 
 
 
 * <TD title="low latency access method supported in BGPM Island Mode;  multiplexing supported in BGPM Distributed Mode;  ">olm</TD>
 * <TD title="Single cycle event, cycles and events are synonymous">s</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title="Bgpm validated - counts confidently verified in controlled bgpm test.">v</TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">Build a mask from XU instructi...</TD> 
 * <TD style="display:none">Build a mask from XU instruction groups to count. See Bgpm_SetXuGrpMask(). </TD>
 
 * </TR>
 * <TR>
 * <TD>214</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_INST_XU_FLD"></a>PEVT_INST_XU_FLD</TD>   
   
 * <TD style="text-align:left">Floating Point Load Instr Compl</TD>
      
 * <TD title="events attributable to current sw/hw thread">thread</TD>
 
 
 
 * <TD title="low latency access method supported in BGPM Island Mode;  multiplexing supported in BGPM Distributed Mode;  ">olm</TD>
 * <TD title="Single cycle event, cycles and events are synonymous">s</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title="Bgpm validated - counts confidently verified in controlled bgpm test.">v</TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">Floating point load instructio...</TD> 
 * <TD style="display:none">Floating point load instructions completed (UPC_P_XU_OGRP_FLD). See Bgpm Opcode Groups page for which instructions match this group. </TD>
 
 * </TR>
 * <TR>
 * <TD>215</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_INST_XU_FST"></a>PEVT_INST_XU_FST</TD>   
   
 * <TD style="text-align:left">Floating Point Store Instr Compl</TD>
      
 * <TD title="events attributable to current sw/hw thread">thread</TD>
 
 
 
 * <TD title="low latency access method supported in BGPM Island Mode;  multiplexing supported in BGPM Distributed Mode;  ">olm</TD>
 * <TD title="Single cycle event, cycles and events are synonymous">s</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title="Bgpm validated - counts confidently verified in controlled bgpm test.">v</TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">Floating point store instructi...</TD> 
 * <TD style="display:none">Floating point store instructions completed (UPC_P_XU_OGRP_FST). See Bgpm Opcode Groups page for which instructions match this group. </TD>
 
 * </TR>
 * <TR>
 * <TD>216</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_INST_XU_QLD"></a>PEVT_INST_XU_QLD</TD>   
   
 * <TD style="text-align:left">Quad Floating Point Load Instr Compl</TD>
      
 * <TD title="events attributable to current sw/hw thread">thread</TD>
 
 
 
 * <TD title="low latency access method supported in BGPM Island Mode;  multiplexing supported in BGPM Distributed Mode;  ">olm</TD>
 * <TD title="Single cycle event, cycles and events are synonymous">s</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title="Bgpm validated - counts confidently verified in controlled bgpm test.">v</TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">Quad Floating point load instr...</TD> 
 * <TD style="display:none">Quad Floating point load instructions completed (UPC_P_XU_OGRP_QLD). See Bgpm Opcode Groups page for which instructions match this group. </TD>
 
 * </TR>
 * <TR>
 * <TD>217</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_INST_XU_QST"></a>PEVT_INST_XU_QST</TD>   
   
 * <TD style="text-align:left">Quad Floating Point Store Instr Compl</TD>
      
 * <TD title="events attributable to current sw/hw thread">thread</TD>
 
 
 
 * <TD title="low latency access method supported in BGPM Island Mode;  multiplexing supported in BGPM Distributed Mode;  ">olm</TD>
 * <TD title="Single cycle event, cycles and events are synonymous">s</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title="Bgpm validated - counts confidently verified in controlled bgpm test.">v</TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">Quad Floating point Store inst...</TD> 
 * <TD style="display:none">Quad Floating point Store instructions completed (UPC_P_XU_OGRP_QST). See Bgpm Opcode Groups page for which instructions match this group. </TD>
 
 * </TR>
 * <TR>
 * <TD>218</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_INST_XU_BITS"></a>PEVT_INST_XU_BITS</TD>   
   
 * <TD style="text-align:left">Bit manipulation Instr Compl</TD>
      
 * <TD title="events attributable to current sw/hw thread">thread</TD>
 
 
 
 * <TD title="low latency access method supported in BGPM Island Mode;  multiplexing supported in BGPM Distributed Mode;  ">olm</TD>
 * <TD title="Single cycle event, cycles and events are synonymous">s</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title="Bgpm validated - counts confidently verified in controlled bgpm test.">v</TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">Bit manipulations instructions...</TD> 
 * <TD style="display:none">Bit manipulations instructions completed (UPC_P_XU_OGRP_BITS). See Bgpm Opcode Groups page for which instructions match this group. </TD>
 
 * </TR>
 * <TR>
 * <TD>219</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_INST_XU_BRC"></a>PEVT_INST_XU_BRC</TD>   
   
 * <TD style="text-align:left">Branch Conditional Instr Compl</TD>
      
 * <TD title="events attributable to current sw/hw thread">thread</TD>
 
 
 
 * <TD title="low latency access method supported in BGPM Island Mode;  multiplexing supported in BGPM Distributed Mode;  ">olm</TD>
 * <TD title="Single cycle event, cycles and events are synonymous">s</TD>
 
 * <TD style="text-align:left">PAPI_BR_CN</TD>
   
 * <TD title="Bgpm validated - counts confidently verified in controlled bgpm test.">v</TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">Conditional Branch Instruction...</TD> 
 * <TD style="display:none">Conditional Branch Instructions Completed (UPC_P_XU_OGRP_BRC). See Bgpm Opcode Groups page for which instructions match this group. </TD>
 
 * </TR>
 * <TR>
 * <TD>220</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_INST_XU_BRU"></a>PEVT_INST_XU_BRU</TD>   
   
 * <TD style="text-align:left">Branch Unconditional Instr Compl</TD>
      
 * <TD title="events attributable to current sw/hw thread">thread</TD>
 
 
 
 * <TD title="low latency access method supported in BGPM Island Mode;  multiplexing supported in BGPM Distributed Mode;  ">olm</TD>
 * <TD title="Single cycle event, cycles and events are synonymous">s</TD>
 
 * <TD style="text-align:left">PAPI_BR_UCN</TD>
   
 * <TD title="Bgpm validated - counts confidently verified in controlled bgpm test.">v</TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">Unconditional Branch Instructi...</TD> 
 * <TD style="display:none">Unconditional Branch Instructions Completed (UPC_P_XU_OGRP_BRU). See Bgpm Opcode Groups page for which instructions match this group. </TD>
 
 * </TR>
 * <TR>
 * <TD>221</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_INST_XU_CINV"></a>PEVT_INST_XU_CINV</TD>   
   
 * <TD style="text-align:left">Cache Invalidate Instr Compl</TD>
      
 * <TD title="events attributable to current sw/hw thread">thread</TD>
 
 
 
 * <TD title="low latency access method supported in BGPM Island Mode;  multiplexing supported in BGPM Distributed Mode;  ">olm</TD>
 * <TD title="Single cycle event, cycles and events are synonymous">s</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title="Bgpm validated - counts confidently verified in controlled bgpm test.">v</TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">Cache Invalidate Instructions ...</TD> 
 * <TD style="display:none">Cache Invalidate Instructions Completed (UPC_P_XU_OGRP_CINV). See Bgpm Opcode Groups page for which instructions match this group. </TD>
 
 * </TR>
 * <TR>
 * <TD>222</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_INST_XU_CSTO"></a>PEVT_INST_XU_CSTO</TD>   
   
 * <TD style="text-align:left">Cache Store Instr Compl</TD>
      
 * <TD title="events attributable to current sw/hw thread">thread</TD>
 
 
 
 * <TD title="low latency access method supported in BGPM Island Mode;  multiplexing supported in BGPM Distributed Mode;  ">olm</TD>
 * <TD title="Single cycle event, cycles and events are synonymous">s</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title=""></TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">Cache Store Instructions Compl...</TD> 
 * <TD style="display:none">Cache Store Instructions Completed (UPC_P_XU_OGRP_CSTO). See Bgpm Opcode Groups page for which instructions match this group. </TD>
 
 * </TR>
 * <TR>
 * <TD>223</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_INST_XU_CTCH"></a>PEVT_INST_XU_CTCH</TD>   
   
 * <TD style="text-align:left">Cache Touch Instr Compl</TD>
      
 * <TD title="events attributable to current sw/hw thread">thread</TD>
 
 
 
 * <TD title="low latency access method supported in BGPM Island Mode;  multiplexing supported in BGPM Distributed Mode;  ">olm</TD>
 * <TD title="Single cycle event, cycles and events are synonymous">s</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title=""></TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">Cache Touch Instructions Compl...</TD> 
 * <TD style="display:none">Cache Touch Instructions Completed (UPC_P_XU_OGRP_CTCH). See Bgpm Opcode Groups page for which instructions match this group. </TD>
 
 * </TR>
 * <TR>
 * <TD>224</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_INST_XU_IADD"></a>PEVT_INST_XU_IADD</TD>   
   
 * <TD style="text-align:left">Integer Arithmetic Instr Compl</TD>
      
 * <TD title="events attributable to current sw/hw thread">thread</TD>
 
 
 
 * <TD title="low latency access method supported in BGPM Island Mode;  multiplexing supported in BGPM Distributed Mode;  ">olm</TD>
 * <TD title="Single cycle event, cycles and events are synonymous">s</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title="Bgpm validated - counts confidently verified in controlled bgpm test.">v</TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">Integer Arithmetic Instruction...</TD> 
 * <TD style="display:none">Integer Arithmetic Instructions Completed (UPC_P_XU_OGRP_IADD). See Bgpm Opcode Groups page for which instructions match this group. </TD>
 
 * </TR>
 * <TR>
 * <TD>225</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_INST_XU_ICMP"></a>PEVT_INST_XU_ICMP</TD>   
   
 * <TD style="text-align:left">Compare Instr Compl</TD>
      
 * <TD title="events attributable to current sw/hw thread">thread</TD>
 
 
 
 * <TD title="low latency access method supported in BGPM Island Mode;  multiplexing supported in BGPM Distributed Mode;  ">olm</TD>
 * <TD title="Single cycle event, cycles and events are synonymous">s</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title="Bgpm validated - counts confidently verified in controlled bgpm test.">v</TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">Compare Instructions Completed...</TD> 
 * <TD style="display:none">Compare Instructions Completed (UPC_P_XU_OGRP_ICMP). See Bgpm Opcode Groups page for which instructions match this group. </TD>
 
 * </TR>
 * <TR>
 * <TD>226</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_INST_XU_ICSW"></a>PEVT_INST_XU_ICSW</TD>   
   
 * <TD style="text-align:left">Coprocessor Instr Compl</TD>
      
 * <TD title="events attributable to current sw/hw thread">thread</TD>
 
 
 
 * <TD title="low latency access method supported in BGPM Island Mode;  multiplexing supported in BGPM Distributed Mode;  ">olm</TD>
 * <TD title="Single cycle event, cycles and events are synonymous">s</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title="Bgpm validated - counts confidently verified in controlled bgpm test.">v</TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">Coprocessor Instructions Compl...</TD> 
 * <TD style="display:none">Coprocessor Instructions Completed (UPC_P_XU_OGRP_ICSW). See Bgpm Opcode Groups page for which instructions match this group. </TD>
 
 * </TR>
 * <TR>
 * <TD>227</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_INST_XU_IDIV"></a>PEVT_INST_XU_IDIV</TD>   
   
 * <TD style="text-align:left">integer divide Instr Compl</TD>
      
 * <TD title="events attributable to current sw/hw thread">thread</TD>
 
 
 
 * <TD title="low latency access method supported in BGPM Island Mode;  multiplexing supported in BGPM Distributed Mode;  ">olm</TD>
 * <TD title="Single cycle event, cycles and events are synonymous">s</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title="Bgpm validated - counts confidently verified in controlled bgpm test.">v</TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">Integer Divide Instructions Co...</TD> 
 * <TD style="display:none">Integer Divide Instructions Completed (UPC_P_XU_OGRP_IDIV). See Bgpm Opcode Groups page for which instructions match this group. </TD>
 
 * </TR>
 * <TR>
 * <TD>228</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_INST_XU_ILOG"></a>PEVT_INST_XU_ILOG</TD>   
   
 * <TD style="text-align:left">Logical Instr Compl</TD>
      
 * <TD title="events attributable to current sw/hw thread">thread</TD>
 
 
 
 * <TD title="low latency access method supported in BGPM Island Mode;  multiplexing supported in BGPM Distributed Mode;  ">olm</TD>
 * <TD title="Single cycle event, cycles and events are synonymous">s</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title="Bgpm validated - counts confidently verified in controlled bgpm test.">v</TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">Logical Instructions Completed...</TD> 
 * <TD style="display:none">Logical Instructions Completed (UPC_P_XU_OGRP_ILOG). See Bgpm Opcode Groups page for which instructions match this group. </TD>
 
 * </TR>
 * <TR>
 * <TD>229</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_INST_XU_IMOV"></a>PEVT_INST_XU_IMOV</TD>   
   
 * <TD style="text-align:left">Quick Register move Instr Compl</TD>
      
 * <TD title="events attributable to current sw/hw thread">thread</TD>
 
 
 
 * <TD title="low latency access method supported in BGPM Island Mode;  multiplexing supported in BGPM Distributed Mode;  ">olm</TD>
 * <TD title="Single cycle event, cycles and events are synonymous">s</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title="Bgpm validated - counts confidently verified in controlled bgpm test.">v</TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">Move register Instructions Com...</TD> 
 * <TD style="display:none">Move register Instructions Completed (UPC_P_XU_OGRP_IMOV). See Bgpm Opcode Groups page for which instructions match this group. </TD>
 
 * </TR>
 * <TR>
 * <TD>230</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_INST_XU_IMUL"></a>PEVT_INST_XU_IMUL</TD>   
   
 * <TD style="text-align:left">integer multiply Instr Compl</TD>
      
 * <TD title="events attributable to current sw/hw thread">thread</TD>
 
 
 
 * <TD title="low latency access method supported in BGPM Island Mode;  multiplexing supported in BGPM Distributed Mode;  ">olm</TD>
 * <TD title="Single cycle event, cycles and events are synonymous">s</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title="Bgpm validated - counts confidently verified in controlled bgpm test.">v</TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">Integer Multiply Instructions ...</TD> 
 * <TD style="display:none">Integer Multiply Instructions Completed (UPC_P_XU_OGRP_IMUL). See Bgpm Opcode Groups page for which instructions match this group. </TD>
 
 * </TR>
 * <TR>
 * <TD>231</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_INST_XU_INT"></a>PEVT_INST_XU_INT</TD>   
   
 * <TD style="text-align:left">Interrupt/system call Instr Compl</TD>
      
 * <TD title="events attributable to current sw/hw thread">thread</TD>
 
 
 
 * <TD title="low latency access method supported in BGPM Island Mode;  multiplexing supported in BGPM Distributed Mode;  ">olm</TD>
 * <TD title="Single cycle event, cycles and events are synonymous">s</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title="Bgpm validated - counts confidently verified in controlled bgpm test.">v</TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">Interrupt and System call Inst...</TD> 
 * <TD style="display:none">Interrupt and System call Instructions Completed (UPC_P_XU_OGRP_INT). See Bgpm Opcode Groups page for which instructions match this group. </TD>
 
 * </TR>
 * <TR>
 * <TD>232</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_INST_XU_LD"></a>PEVT_INST_XU_LD</TD>   
   
 * <TD style="text-align:left">Load Instr Compl</TD>
      
 * <TD title="events attributable to current sw/hw thread">thread</TD>
 
 
 
 * <TD title="low latency access method supported in BGPM Island Mode;  multiplexing supported in BGPM Distributed Mode;  ">olm</TD>
 * <TD title="Single cycle event, cycles and events are synonymous">s</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title="Bgpm validated - counts confidently verified in controlled bgpm test.">v</TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">Load Instructions Completed (U...</TD> 
 * <TD style="display:none">Load Instructions Completed (UPC_P_XU_OGRP_LD). uCoded Instructions are counted once. See Bgpm Opcode Groups page for which instructions match this group </TD>
 
 * </TR>
 * <TR>
 * <TD>233</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_INST_XU_LSCX"></a>PEVT_INST_XU_LSCX</TD>   
   
 * <TD style="text-align:left">Load &amp; Store reserve conditional Instr Compl</TD>
      
 * <TD title="events attributable to current sw/hw thread">thread</TD>
 
 
 
 * <TD title="low latency access method supported in BGPM Island Mode;  multiplexing supported in BGPM Distributed Mode;  ">olm</TD>
 * <TD title="Single cycle event, cycles and events are synonymous">s</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title="Bgpm validated - counts confidently verified in controlled bgpm test.">v</TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">Load and Store Reserve Instruc...</TD> 
 * <TD style="display:none">Load and Store Reserve Instructions Completed (UPC_P_XU_OGRP_LSCX). uCoded Instructions are counted once. See Bgpm Opcode Groups page for which instructions match this group. </TD>
 
 * </TR>
 * <TR>
 * <TD>234</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_INST_XU_ST"></a>PEVT_INST_XU_ST</TD>   
   
 * <TD style="text-align:left">Store Instr Compl</TD>
      
 * <TD title="events attributable to current sw/hw thread">thread</TD>
 
 
 
 * <TD title="low latency access method supported in BGPM Island Mode;  multiplexing supported in BGPM Distributed Mode;  ">olm</TD>
 * <TD title="Single cycle event, cycles and events are synonymous">s</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title="Bgpm validated - counts confidently verified in controlled bgpm test.">v</TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">Store Instructions Completed (...</TD> 
 * <TD style="display:none">Store Instructions Completed (UPC_P_XU_OGRP_ST). uCoded Instructions are counted once. See Bgpm Opcode Groups page for which instructions match this group. </TD>
 
 * </TR>
 * <TR>
 * <TD>235</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_INST_XU_SYNC"></a>PEVT_INST_XU_SYNC</TD>   
   
 * <TD style="text-align:left">Context Sync Instr Compl</TD>
      
 * <TD title="events attributable to current sw/hw thread">thread</TD>
 
 
 
 * <TD title="low latency access method supported in BGPM Island Mode;  multiplexing supported in BGPM Distributed Mode;  ">olm</TD>
 * <TD title="Single cycle event, cycles and events are synonymous">s</TD>
 
 * <TD style="text-align:left">PAPI_SYC_INS</TD>
   
 * <TD title="Bgpm validated - counts confidently verified in controlled bgpm test.">v</TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">Completed Instructions which c...</TD> 
 * <TD style="display:none">Completed Instructions which cause a Context Sync (UPC_P_XU_OGRP_SYNC). See Bgpm Opcode Groups page for which instructions match this group. </TD>
 
 * </TR>
 * <TR>
 * <TD>236</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_INST_XU_TLB"></a>PEVT_INST_XU_TLB</TD>   
   
 * <TD style="text-align:left">TLB other Instr Compl</TD>
      
 * <TD title="events attributable to current sw/hw thread">thread</TD>
 
 
 
 * <TD title="low latency access method supported in BGPM Island Mode;  multiplexing supported in BGPM Distributed Mode;  ">olm</TD>
 * <TD title="Single cycle event, cycles and events are synonymous">s</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title=""></TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">Translation Look-aside buffer ...</TD> 
 * <TD style="display:none">Translation Look-aside buffer Instructions Completed (UPC_P_XU_OGRP_TLB). See Bgpm Opcode Groups page for which instructions match this group </TD>
 
 * </TR>
 * <TR>
 * <TD>237</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_INST_XU_TLBI"></a>PEVT_INST_XU_TLBI</TD>   
   
 * <TD style="text-align:left">TLB Invalidate Instr Compl</TD>
      
 * <TD title="events attributable to current sw/hw thread">thread</TD>
 
 
 
 * <TD title="low latency access method supported in BGPM Island Mode;  multiplexing supported in BGPM Distributed Mode;  ">olm</TD>
 * <TD title="Single cycle event, cycles and events are synonymous">s</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title=""></TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">Translation Look-aside buffer ...</TD> 
 * <TD style="display:none">Translation Look-aside buffer Invalidate Instructions Completed (UPC_P_XU_OGRP_TLBI). See Bgpm Opcode Groups page for which instructions match this group </TD>
 
 * </TR>
 * <TR>
 * <TD>238</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_INST_XU_WCH"></a>PEVT_INST_XU_WCH</TD>   
   
 * <TD style="text-align:left">Watch Instr Compl</TD>
      
 * <TD title="events attributable to current sw/hw thread">thread</TD>
 
 
 
 * <TD title="low latency access method supported in BGPM Island Mode;  multiplexing supported in BGPM Distributed Mode;  ">olm</TD>
 * <TD title="Single cycle event, cycles and events are synonymous">s</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title="Bgpm validated - counts confidently verified in controlled bgpm test.">v</TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">Watch Instructions Completed (...</TD> 
 * <TD style="display:none">Watch Instructions Completed (UPC_P_XU_OGRP_WCH). See Bgpm Opcode Groups page for which instructions match this group </TD>
 
 * </TR>
 * <TR>
 * <TD>239</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_INST_XU_ALL"></a>PEVT_INST_XU_ALL</TD>   
   
 * <TD style="text-align:left">All XU Instruction Completions</TD>
      
 * <TD title="events attributable to current sw/hw thread">thread</TD>
 
 
 
 * <TD title="low latency access method supported in BGPM Island Mode;  multiplexing supported in BGPM Distributed Mode;  ">olm</TD>
 * <TD title="Single cycle event, cycles and events are synonymous">s</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title="Bgpm validated - counts confidently verified in controlled bgpm test.">v</TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">All XU instructions completed ...</TD> 
 * <TD style="display:none">All XU instructions completed (instructions which use A2 FX unit - UPC_P_XU_OGRP_*). See Bgpm Opcode Groups page for which instructions match this group. </TD>
 
 * </TR><TR><TD colspan="11" style="text-align:left"><a class="anchor" id="a_qfpu_instruction"></a><B><I>QFPU Instruction Opcode Counting</I></B></TD></TR>
 * <TR>
 * <TD>240</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_INST_QFPU_MATCH"></a>PEVT_INST_QFPU_MATCH</TD>   
   
 * <TD style="text-align:left">AXU Opcode Match</TD>
      
 * <TD title="events attributable to current sw/hw thread">thread</TD>
 
 
 
 * <TD title="low latency access method supported in BGPM Island Mode;  multiplexing supported in BGPM Distributed Mode;  ">olm</TD>
 * <TD title="Single cycle event, cycles and events are synonymous">s</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title="Bgpm validated - counts confidently verified in controlled bgpm test.">v</TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">Perform a major/minor opcode m...</TD> 
 * <TD style="display:none">Perform a major/minor opcode match on completed AXU (QFPU) instructions. Use Bgpm_SetQfpuMatch() on event to choose pattern and possible Floating point scaling value. Only one may be set per core. If not set, will count all completed AXU Instructions. Choosing whether to match is multiplexable, but the pattern to match is not. </TD>
 
 * </TR>
 * <TR>
 * <TD>241</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_INST_QFPU_GRP_MASK"></a>PEVT_INST_QFPU_GRP_MASK</TD>   
   
 * <TD style="text-align:left">AXU Opcode Mask Multiple Groups</TD>
      
 * <TD title="events attributable to current sw/hw thread">thread</TD>
 
 
 
 * <TD title="low latency access method supported in BGPM Island Mode;  multiplexing supported in BGPM Distributed Mode;  ">olm</TD>
 * <TD title="Single cycle event, cycles and events are synonymous">s</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title="Bgpm validated - counts confidently verified in controlled bgpm test.">v</TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">Build a mask from AXU (QFPU) i...</TD> 
 * <TD style="display:none">Build a mask from AXU (QFPU) instruction groups to count. See Bgpm_SetQfpuGrpMask(). </TD>
 
 * </TR>
 * <TR>
 * <TD>242</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_INST_QFPU_FOTH"></a>PEVT_INST_QFPU_FOTH</TD>   
   
 * <TD style="text-align:left">Floating Point Other Instr Compl</TD>
      
 * <TD title="events attributable to current sw/hw thread">thread</TD>
 
 
 
 * <TD title="low latency access method supported in BGPM Island Mode;  multiplexing supported in BGPM Distributed Mode;  ">olm</TD>
 * <TD title="Single cycle event, cycles and events are synonymous">s</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title="Bgpm validated - counts confidently verified in controlled bgpm test.">v</TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">Single floating point instruct...</TD> 
 * <TD style="display:none">Single floating point instruction completes (instructions not in other groups UPC_P_AXU_OGRP_FOTH). See Bgpm Opcode Groups page for which instructions match this group </TD>
 
 * </TR>
 * <TR>
 * <TD>243</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_INST_QFPU_FRES"></a>PEVT_INST_QFPU_FRES</TD>   
   
 * <TD style="text-align:left">Floating Point Reciprocal Estimate Instr Compl</TD>
      
 * <TD title="events attributable to current sw/hw thread">thread</TD>
 
 
 
 * <TD title="low latency access method supported in BGPM Island Mode;  multiplexing supported in BGPM Distributed Mode;  ">olm</TD>
 * <TD title="Single cycle event, cycles and events are synonymous">s</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title="Bgpm validated - counts confidently verified in controlled bgpm test.">v</TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">Single floating point reciproc...</TD> 
 * <TD style="display:none">Single floating point reciprocal estimate instruction completes (UPC_P_AXU_OGRP_FRES). See Bgpm Opcode Groups page for which instructions match this group </TD>
 
 * </TR>
 * <TR>
 * <TD>244</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_INST_QFPU_FADD"></a>PEVT_INST_QFPU_FADD</TD>   
   
 * <TD style="text-align:left">Floating Point Add Instr Compl</TD>
      
 * <TD title="events attributable to current sw/hw thread">thread</TD>
 
 
 
 * <TD title="low latency access method supported in BGPM Island Mode;  multiplexing supported in BGPM Distributed Mode;  ">olm</TD>
 * <TD title="Single cycle event, cycles and events are synonymous">s</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title="Bgpm validated - counts confidently verified in controlled bgpm test.">v</TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">Single floating point addition...</TD> 
 * <TD style="display:none">Single floating point addition instructions completes (UPC_P_AXU_OGRP_FADD). See Bgpm Opcode Groups page for which instructions match this group </TD>
 
 * </TR>
 * <TR>
 * <TD>245</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_INST_QFPU_FCVT"></a>PEVT_INST_QFPU_FCVT</TD>   
   
 * <TD style="text-align:left">Floating Point Convert Instr Compl</TD>
      
 * <TD title="events attributable to current sw/hw thread">thread</TD>
 
 
 
 * <TD title="low latency access method supported in BGPM Island Mode;  multiplexing supported in BGPM Distributed Mode;  ">olm</TD>
 * <TD title="Single cycle event, cycles and events are synonymous">s</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title="Bgpm validated - counts confidently verified in controlled bgpm test.">v</TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">Single floating point convert ...</TD> 
 * <TD style="display:none">Single floating point convert instructions completes (UPC_P_AXU_OGRP_FCVT). See Bgpm Opcode Groups page for which instructions match this group </TD>
 
 * </TR>
 * <TR>
 * <TD>246</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_INST_QFPU_FCMP"></a>PEVT_INST_QFPU_FCMP</TD>   
   
 * <TD style="text-align:left">Floating Point Compare Instr Compl</TD>
      
 * <TD title="events attributable to current sw/hw thread">thread</TD>
 
 
 
 * <TD title="low latency access method supported in BGPM Island Mode;  multiplexing supported in BGPM Distributed Mode;  ">olm</TD>
 * <TD title="Single cycle event, cycles and events are synonymous">s</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title="Bgpm validated - counts confidently verified in controlled bgpm test.">v</TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">Single floating point compare ...</TD> 
 * <TD style="display:none">Single floating point compare instructions completes (UPC_P_AXU_OGRP_FCMP). See Bgpm Opcode Groups page for which instructions match this group </TD>
 
 * </TR>
 * <TR>
 * <TD>247</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_INST_QFPU_FDIV"></a>PEVT_INST_QFPU_FDIV</TD>   
   
 * <TD style="text-align:left">Floating Point Divide Instr Compl</TD>
      
 * <TD title="events attributable to current sw/hw thread">thread</TD>
 
 
 
 * <TD title="low latency access method supported in BGPM Island Mode;  multiplexing supported in BGPM Distributed Mode;  ">olm</TD>
 * <TD title="Single cycle event, cycles and events are synonymous">s</TD>
 
 * <TD style="text-align:left">PAPI_FDV_INS</TD>
   
 * <TD title="Bgpm validated - counts confidently verified in controlled bgpm test.">v</TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">Single floating point division...</TD> 
 * <TD style="display:none">Single floating point division instructions completes (UPC_P_AXU_OGRP_FDIV). See Bgpm Opcode Groups page for which instructions match this group </TD>
 
 * </TR>
 * <TR>
 * <TD>248</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_INST_QFPU_FMOV"></a>PEVT_INST_QFPU_FMOV</TD>   
   
 * <TD style="text-align:left">Floating Point Move Instr Compl</TD>
      
 * <TD title="events attributable to current sw/hw thread">thread</TD>
 
 
 
 * <TD title="low latency access method supported in BGPM Island Mode;  multiplexing supported in BGPM Distributed Mode;  ">olm</TD>
 * <TD title="Single cycle event, cycles and events are synonymous">s</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title="Bgpm validated - counts confidently verified in controlled bgpm test.">v</TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">Single floating point move ins...</TD> 
 * <TD style="display:none">Single floating point move instructions completes (UPC_P_AXU_OGRP_FMOV). See Bgpm Opcode Groups page for which instructions match this group </TD>
 
 * </TR>
 * <TR>
 * <TD>249</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_INST_QFPU_FMA"></a>PEVT_INST_QFPU_FMA</TD>   
   
 * <TD style="text-align:left">Floating Point Multiply-Add Instr Compl</TD>
      
 * <TD title="events attributable to current sw/hw thread">thread</TD>
 
 
 
 * <TD title="low latency access method supported in BGPM Island Mode;  multiplexing supported in BGPM Distributed Mode;  ">olm</TD>
 * <TD title="Single cycle event, cycles and events are synonymous">s</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title="Bgpm validated - counts confidently verified in controlled bgpm test.">v</TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">Single floating point multiply...</TD> 
 * <TD style="display:none">Single floating point multiply-add instructions completes (UPC_P_AXU_OGRP_FMA). See Bgpm Opcode Groups page for which instructions match this group </TD>
 
 * </TR>
 * <TR>
 * <TD>250</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_INST_QFPU_FMUL"></a>PEVT_INST_QFPU_FMUL</TD>   
   
 * <TD style="text-align:left">Floating Point Multiply Instr Compl</TD>
      
 * <TD title="events attributable to current sw/hw thread">thread</TD>
 
 
 
 * <TD title="low latency access method supported in BGPM Island Mode;  multiplexing supported in BGPM Distributed Mode;  ">olm</TD>
 * <TD title="Single cycle event, cycles and events are synonymous">s</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title="Bgpm validated - counts confidently verified in controlled bgpm test.">v</TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">Single floating point multiply...</TD> 
 * <TD style="display:none">Single floating point multiply instructions completes (UPC_P_AXU_OGRP_FMUL). See Bgpm Opcode Groups page for which instructions match this group </TD>
 
 * </TR>
 * <TR>
 * <TD>251</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_INST_QFPU_FRND"></a>PEVT_INST_QFPU_FRND</TD>   
   
 * <TD style="text-align:left">Floating Point Round Instr Compl</TD>
      
 * <TD title="events attributable to current sw/hw thread">thread</TD>
 
 
 
 * <TD title="low latency access method supported in BGPM Island Mode;  multiplexing supported in BGPM Distributed Mode;  ">olm</TD>
 * <TD title="Single cycle event, cycles and events are synonymous">s</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title="Bgpm validated - counts confidently verified in controlled bgpm test.">v</TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">Single floating point round in...</TD> 
 * <TD style="display:none">Single floating point round instructions completes (UPC_P_AXU_OGRP_FRND). See Bgpm Opcode Groups page for which instructions match this group </TD>
 
 * </TR>
 * <TR>
 * <TD>252</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_INST_QFPU_FSQE"></a>PEVT_INST_QFPU_FSQE</TD>   
   
 * <TD style="text-align:left">Floating Point Square Root Estimate Instr Compl</TD>
      
 * <TD title="events attributable to current sw/hw thread">thread</TD>
 
 
 
 * <TD title="low latency access method supported in BGPM Island Mode;  multiplexing supported in BGPM Distributed Mode;  ">olm</TD>
 * <TD title="Single cycle event, cycles and events are synonymous">s</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title="Bgpm validated - counts confidently verified in controlled bgpm test.">v</TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">Single floating point square r...</TD> 
 * <TD style="display:none">Single floating point square root estimate instructions completes (UPC_P_AXU_OGRP_FSQE). See Bgpm Opcode Groups page for which instructions match this group </TD>
 
 * </TR>
 * <TR>
 * <TD>253</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_INST_QFPU_FSQ"></a>PEVT_INST_QFPU_FSQ</TD>   
   
 * <TD style="text-align:left">Floating Point Square Root Instr Compl</TD>
      
 * <TD title="events attributable to current sw/hw thread">thread</TD>
 
 
 
 * <TD title="low latency access method supported in BGPM Island Mode;  multiplexing supported in BGPM Distributed Mode;  ">olm</TD>
 * <TD title="Single cycle event, cycles and events are synonymous">s</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title="Bgpm validated - counts confidently verified in controlled bgpm test.">v</TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">Single floating point square r...</TD> 
 * <TD style="display:none">Single floating point square root instructions completes (UPC_P_AXU_OGRP_FSQ). See Bgpm Opcode Groups page for which instructions match this group </TD>
 
 * </TR>
 * <TR>
 * <TD>254</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_INST_QFPU_QMOV"></a>PEVT_INST_QFPU_QMOV</TD>   
   
 * <TD style="text-align:left">Quad Floating Point Move Instr Compl</TD>
      
 * <TD title="events attributable to current sw/hw thread">thread</TD>
 
 
 
 * <TD title="low latency access method supported in BGPM Island Mode;  multiplexing supported in BGPM Distributed Mode;  ">olm</TD>
 * <TD title="Single cycle event, cycles and events are synonymous">s</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title="Bgpm validated - counts confidently verified in controlled bgpm test.">v</TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">Quad floating point move instr...</TD> 
 * <TD style="display:none">Quad floating point move instructions complete (UPC_P_AXU_OGRP_QMOV). See Bgpm Opcode Groups page for which instructions match this group </TD>
 
 * </TR>
 * <TR>
 * <TD>255</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_INST_QFPU_QOTH"></a>PEVT_INST_QFPU_QOTH</TD>   
   
 * <TD style="text-align:left">Quad Floating Point Other Instr Compl</TD>
      
 * <TD title="events attributable to current sw/hw thread">thread</TD>
 
 
 
 * <TD title="low latency access method supported in BGPM Island Mode;  multiplexing supported in BGPM Distributed Mode;  ">olm</TD>
 * <TD title="Single cycle event, cycles and events are synonymous">s</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title="Bgpm validated - counts confidently verified in controlled bgpm test.">v</TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">Quad floating point instructio...</TD> 
 * <TD style="display:none">Quad floating point instructions not counted in other groups (UPC_P_AXU_OGRP_QOTH). See Bgpm Opcode Groups page for which instructions match this group </TD>
 
 * </TR>
 * <TR>
 * <TD>256</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_INST_QFPU_QADD"></a>PEVT_INST_QFPU_QADD</TD>   
   
 * <TD style="text-align:left">Quad Floating Point Add Instr Compl</TD>
      
 * <TD title="events attributable to current sw/hw thread">thread</TD>
 
 
 
 * <TD title="low latency access method supported in BGPM Island Mode;  multiplexing supported in BGPM Distributed Mode;  ">olm</TD>
 * <TD title="Single cycle event, cycles and events are synonymous">s</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title="Bgpm validated - counts confidently verified in controlled bgpm test.">v</TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">Quad floating point add instru...</TD> 
 * <TD style="display:none">Quad floating point add instructions complete (UPC_P_AXU_OGRP_QADD). See Bgpm Opcode Groups page for which instructions match this group </TD>
 
 * </TR>
 * <TR>
 * <TD>257</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_INST_QFPU_QMUL"></a>PEVT_INST_QFPU_QMUL</TD>   
   
 * <TD style="text-align:left">Quad Floating Point Multiply Instr Compl</TD>
      
 * <TD title="events attributable to current sw/hw thread">thread</TD>
 
 
 
 * <TD title="low latency access method supported in BGPM Island Mode;  multiplexing supported in BGPM Distributed Mode;  ">olm</TD>
 * <TD title="Single cycle event, cycles and events are synonymous">s</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title="Bgpm validated - counts confidently verified in controlled bgpm test.">v</TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">Quad floating point multiply i...</TD> 
 * <TD style="display:none">Quad floating point multiply instructions complete (UPC_P_AXU_OGRP_QMUL). See Bgpm Opcode Groups page for which instructions match this group </TD>
 
 * </TR>
 * <TR>
 * <TD>258</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_INST_QFPU_QRES"></a>PEVT_INST_QFPU_QRES</TD>   
   
 * <TD style="text-align:left">Quad Floating Point Reciprocal Estimate Instr Compl</TD>
      
 * <TD title="events attributable to current sw/hw thread">thread</TD>
 
 
 
 * <TD title="low latency access method supported in BGPM Island Mode;  multiplexing supported in BGPM Distributed Mode;  ">olm</TD>
 * <TD title="Single cycle event, cycles and events are synonymous">s</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title="Bgpm validated - counts confidently verified in controlled bgpm test.">v</TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">Quad floating point reciprocal...</TD> 
 * <TD style="display:none">Quad floating point reciprocal estimate instructions complete (UPC_P_AXU_OGRP_QRES). See Bgpm Opcode Groups page for which instructions match this group </TD>
 
 * </TR>
 * <TR>
 * <TD>259</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_INST_QFPU_QMA"></a>PEVT_INST_QFPU_QMA</TD>   
   
 * <TD style="text-align:left">Quad Floating Point Multiply-Add Instr Compl</TD>
      
 * <TD title="events attributable to current sw/hw thread">thread</TD>
 
 
 
 * <TD title="low latency access method supported in BGPM Island Mode;  multiplexing supported in BGPM Distributed Mode;  ">olm</TD>
 * <TD title="Single cycle event, cycles and events are synonymous">s</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title="Bgpm validated - counts confidently verified in controlled bgpm test.">v</TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">Quad floating point multiply-a...</TD> 
 * <TD style="display:none">Quad floating point multiply-add instructions complete (UPC_P_AXU_OGRP_QMA). See Bgpm Opcode Groups page for which instructions match this group </TD>
 
 * </TR>
 * <TR>
 * <TD>260</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_INST_QFPU_QRND"></a>PEVT_INST_QFPU_QRND</TD>   
   
 * <TD style="text-align:left">Quad Floating Point Round Instr Compl</TD>
      
 * <TD title="events attributable to current sw/hw thread">thread</TD>
 
 
 
 * <TD title="low latency access method supported in BGPM Island Mode;  multiplexing supported in BGPM Distributed Mode;  ">olm</TD>
 * <TD title="Single cycle event, cycles and events are synonymous">s</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title="Bgpm validated - counts confidently verified in controlled bgpm test.">v</TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">Quad floating point round inst...</TD> 
 * <TD style="display:none">Quad floating point round instructions complete (UPC_P_AXU_OGRP_QRND). See Bgpm Opcode Groups page for which instructions match this group </TD>
 
 * </TR>
 * <TR>
 * <TD>261</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_INST_QFPU_QCVT"></a>PEVT_INST_QFPU_QCVT</TD>   
   
 * <TD style="text-align:left">Quad Floating Point Convert Instr Compl</TD>
      
 * <TD title="events attributable to current sw/hw thread">thread</TD>
 
 
 
 * <TD title="low latency access method supported in BGPM Island Mode;  multiplexing supported in BGPM Distributed Mode;  ">olm</TD>
 * <TD title="Single cycle event, cycles and events are synonymous">s</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title="Bgpm validated - counts confidently verified in controlled bgpm test.">v</TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">Quad floating point convert in...</TD> 
 * <TD style="display:none">Quad floating point convert instructions complete (UPC_P_AXU_OGRP_QCVT). See Bgpm Opcode Groups page for which instructions match this group </TD>
 
 * </TR>
 * <TR>
 * <TD>262</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_INST_QFPU_QCMP"></a>PEVT_INST_QFPU_QCMP</TD>   
   
 * <TD style="text-align:left">Quad Floating Point Compare Instr Compl</TD>
      
 * <TD title="events attributable to current sw/hw thread">thread</TD>
 
 
 
 * <TD title="low latency access method supported in BGPM Island Mode;  multiplexing supported in BGPM Distributed Mode;  ">olm</TD>
 * <TD title="Single cycle event, cycles and events are synonymous">s</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title="Bgpm validated - counts confidently verified in controlled bgpm test.">v</TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">Quad floating point compare in...</TD> 
 * <TD style="display:none">Quad floating point compare instructions complete (UPC_P_AXU_OGRP_QCMP). See Bgpm Opcode Groups page for which instructions match this group </TD>
 
 * </TR>
 * <TR>
 * <TD>263</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_INST_QFPU_UCODE"></a>PEVT_INST_QFPU_UCODE</TD>   
   
 * <TD style="text-align:left">Floating Point microcoded instructions</TD>
      
 * <TD title="events attributable to current sw/hw thread">thread</TD>
 
 
 
 * <TD title="low latency access method supported in BGPM Island Mode;  multiplexing supported in BGPM Distributed Mode;  ">olm</TD>
 * <TD title="Single cycle event, cycles and events are synonymous">s</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title="Bgpm validated - counts confidently verified in controlled bgpm test.">v</TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">Count all completed floating p...</TD> 
 * <TD style="display:none">Count all completed floating point instructions which are microcoded. Useful in CPI calculations. Ucode sub-operations are excluded. </TD>
 
 * </TR>
 * <TR>
 * <TD>264</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_INST_QFPU_ALL"></a>PEVT_INST_QFPU_ALL</TD>   
   
 * <TD style="text-align:left">All AXU Instruction Completions</TD>
      
 * <TD title="events attributable to current sw/hw thread">thread</TD>
 
 
 
 * <TD title="low latency access method supported in BGPM Island Mode;  multiplexing supported in BGPM Distributed Mode;  ">olm</TD>
 * <TD title="Single cycle event, cycles and events are synonymous">s</TD>
 
 * <TD style="text-align:left">PAPI_FP_INS</TD>
   
 * <TD title="Bgpm validated - counts confidently verified in controlled bgpm test.">v</TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">Count all completed instructio...</TD> 
 * <TD style="display:none">Count all completed instructions which processed by the QFPU unit (UPC_P_AXU_OGRP_*) </TD>
 
 * </TR><TR><TD colspan="11" style="text-align:left"><a class="anchor" id="a_qfpu_floating"></a><B><I>QFPU Floating Point Operations Counting Groups</I></B></TD></TR>
 * <TR>
 * <TD>265</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_INST_QFPU_FPGRP1"></a>PEVT_INST_QFPU_FPGRP1</TD>   
   
 * <TD style="text-align:left">FP Operations Group 1 </TD>
      
 * <TD title="events attributable to current sw/hw thread">thread</TD>
 
 
 
 * <TD title="low latency access method supported in BGPM Island Mode;  multiplexing supported in BGPM Distributed Mode;  ">olm</TD>
 * <TD title="Single cycle event, cycles and events are synonymous">s</TD>
 
 * <TD style="text-align:left">PAPI_FP_OPS</TD>
   
 * <TD title="Bgpm validated - counts confidently verified in controlled bgpm test.">v</TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">Count completed floating point...</TD> 
 * <TD style="display:none">Count completed floating point operations scaled according QFPU capacity. See the Bgpm Opcode Groups page for more information. </TD>
 
 * </TR>
 * <TR>
 * <TD>266</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_INST_QFPU_FPGRP1_SINGLE"></a>PEVT_INST_QFPU_FPGRP1_SINGLE</TD>   
   
 * <TD style="text-align:left">FP Operations Group 1 single FP subset</TD>
      
 * <TD title="events attributable to current sw/hw thread">thread</TD>
 
 
 
 * <TD title="low latency access method supported in BGPM Island Mode;  multiplexing supported in BGPM Distributed Mode;  ">olm</TD>
 * <TD title="Single cycle event, cycles and events are synonymous">s</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title=""></TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">Count completed Single (just F...</TD> 
 * <TD style="display:none">Count completed Single (just Fxx operations) subset of FPGRP1. See the Bgpm Opcode Groups page for more information  </TD>
 
 * </TR>
 * <TR>
 * <TD>267</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_INST_QFPU_FPGRP1_QUAD"></a>PEVT_INST_QFPU_FPGRP1_QUAD</TD>   
   
 * <TD style="text-align:left">FP Operations Group 1 quad FP subset</TD>
      
 * <TD title="events attributable to current sw/hw thread">thread</TD>
 
 
 
 * <TD title="low latency access method supported in BGPM Island Mode;  multiplexing supported in BGPM Distributed Mode;  ">olm</TD>
 * <TD title="Single cycle event, cycles and events are synonymous">s</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title=""></TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">Count completed Quad (just Qfx...</TD> 
 * <TD style="display:none">Count completed Quad (just Qfxx operations) subset of FPGRP1. See the Bgpm Opcode Groups page for more information. </TD>
 
 * </TR>
 * <TR>
 * <TD>268</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_INST_QFPU_FPGRP2"></a>PEVT_INST_QFPU_FPGRP2</TD>   
   
 * <TD style="text-align:left">FP Operations Group 2 </TD>
      
 * <TD title="events attributable to current sw/hw thread">thread</TD>
 
 
 
 * <TD title="low latency access method supported in BGPM Island Mode;  multiplexing supported in BGPM Distributed Mode;  ">olm</TD>
 * <TD title="Single cycle event, cycles and events are synonymous">s</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title="Bgpm validated - counts confidently verified in controlled bgpm test.">v</TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">Count completed floating point...</TD> 
 * <TD style="display:none">Count completed floating point operations, but excluding rounding, copy and conversion instructions. See the Bgpm Opcode Groups page for more information. </TD>
 
 * </TR>
 * <TR>
 * <TD>269</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_INST_QFPU_FPGRP2_SINGLE"></a>PEVT_INST_QFPU_FPGRP2_SINGLE</TD>   
   
 * <TD style="text-align:left">FP Operations Group 2 single FP subset</TD>
      
 * <TD title="events attributable to current sw/hw thread">thread</TD>
 
 
 
 * <TD title="low latency access method supported in BGPM Island Mode;  multiplexing supported in BGPM Distributed Mode;  ">olm</TD>
 * <TD title="Single cycle event, cycles and events are synonymous">s</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title=""></TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">Count completed Single (Fxx op...</TD> 
 * <TD style="display:none">Count completed Single (Fxx operations) subset of FPGRP2. See the Bgpm Opcode Groups page for more information  </TD>
 
 * </TR>
 * <TR>
 * <TD>270</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_INST_QFPU_FPGRP2_QUAD"></a>PEVT_INST_QFPU_FPGRP2_QUAD</TD>   
   
 * <TD style="text-align:left">FP Operations Group 2 quad FP subset</TD>
      
 * <TD title="events attributable to current sw/hw thread">thread</TD>
 
 
 
 * <TD title="low latency access method supported in BGPM Island Mode;  multiplexing supported in BGPM Distributed Mode;  ">olm</TD>
 * <TD title="Single cycle event, cycles and events are synonymous">s</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title=""></TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">Count completed Quad (Qfxx ope...</TD> 
 * <TD style="display:none">Count completed Quad (Qfxx operations) subset of FPGRP2. See the Bgpm Opcode Groups page for more information. </TD>
 
 * </TR><TR><TD colspan="11" style="text-align:left"><a class="anchor" id="a_qfpu_floating"></a><B><I>QFPU Floating Point Instructions Counting Groups</I></B></TD></TR>
 * <TR>
 * <TD>271</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_INST_QFPU_FPGRP1_INSTR"></a>PEVT_INST_QFPU_FPGRP1_INSTR</TD>   
   
 * <TD style="text-align:left">FP Operations Group 1 instr count</TD>
      
 * <TD title="events attributable to current sw/hw thread">thread</TD>
 
 
 
 * <TD title="low latency access method supported in BGPM Island Mode;  multiplexing supported in BGPM Distributed Mode;  ">olm</TD>
 * <TD title="Single cycle event, cycles and events are synonymous">s</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title=""></TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">Count completed floating point...</TD> 
 * <TD style="display:none">Count completed floating point instructions matching FPGRP1. See the Bgpm Opcode Groups page for more information. </TD>
 
 * </TR>
 * <TR>
 * <TD>272</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_INST_QFPU_FPGRP1_SINGLE_INSTR"></a>PEVT_INST_QFPU_FPGRP1_SINGLE_INSTR</TD>   
   
 * <TD style="text-align:left">FP Operations Group 1 single FP subset instr count</TD>
      
 * <TD title="events attributable to current sw/hw thread">thread</TD>
 
 
 
 * <TD title="low latency access method supported in BGPM Island Mode;  multiplexing supported in BGPM Distributed Mode;  ">olm</TD>
 * <TD title="Single cycle event, cycles and events are synonymous">s</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title=""></TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">Count completed Single Fxx ins...</TD> 
 * <TD style="display:none">Count completed Single Fxx instructions subset of FPGRP1. See the Bgpm Opcode Groups page for more information  </TD>
 
 * </TR>
 * <TR>
 * <TD>273</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_INST_QFPU_FPGRP1_QUAD_INSTR"></a>PEVT_INST_QFPU_FPGRP1_QUAD_INSTR</TD>   
   
 * <TD style="text-align:left">FP Operations Group 1 quad FP subset instr count</TD>
      
 * <TD title="events attributable to current sw/hw thread">thread</TD>
 
 
 
 * <TD title="low latency access method supported in BGPM Island Mode;  multiplexing supported in BGPM Distributed Mode;  ">olm</TD>
 * <TD title="Single cycle event, cycles and events are synonymous">s</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title=""></TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">Count completed Quad Qfxx inst...</TD> 
 * <TD style="display:none">Count completed Quad Qfxx instructions subset of FPGRP1. See the Bgpm Opcode Groups page for more information. </TD>
 
 * </TR>
 * <TR>
 * <TD>274</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_INST_QFPU_FPGRP2_INSTR"></a>PEVT_INST_QFPU_FPGRP2_INSTR</TD>   
   
 * <TD style="text-align:left">FP Operations Group 2 instr count</TD>
      
 * <TD title="events attributable to current sw/hw thread">thread</TD>
 
 
 
 * <TD title="low latency access method supported in BGPM Island Mode;  multiplexing supported in BGPM Distributed Mode;  ">olm</TD>
 * <TD title="Single cycle event, cycles and events are synonymous">s</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title=""></TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">Count completed floating point...</TD> 
 * <TD style="display:none">Count completed floating point instructions matching FPGRP2. See the Bgpm Opcode Groups page for more information. </TD>
 
 * </TR>
 * <TR>
 * <TD>275</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_INST_QFPU_FPGRP2_SINGLE_INSTR"></a>PEVT_INST_QFPU_FPGRP2_SINGLE_INSTR</TD>   
   
 * <TD style="text-align:left">FP Operations Group 2 single FP subset instr count</TD>
      
 * <TD title="events attributable to current sw/hw thread">thread</TD>
 
 
 
 * <TD title="low latency access method supported in BGPM Island Mode;  multiplexing supported in BGPM Distributed Mode;  ">olm</TD>
 * <TD title="Single cycle event, cycles and events are synonymous">s</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title=""></TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">Count completed Single Fxx ins...</TD> 
 * <TD style="display:none">Count completed Single Fxx instructions subset of FPGRP2. See the Bgpm Opcode Groups page for more information  </TD>
 
 * </TR>
 * <TR>
 * <TD>276</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_INST_QFPU_FPGRP2_QUAD_INSTR"></a>PEVT_INST_QFPU_FPGRP2_QUAD_INSTR</TD>   
   
 * <TD style="text-align:left">FP Operations Group 2 quad FP subset instr count</TD>
      
 * <TD title="events attributable to current sw/hw thread">thread</TD>
 
 
 
 * <TD title="low latency access method supported in BGPM Island Mode;  multiplexing supported in BGPM Distributed Mode;  ">olm</TD>
 * <TD title="Single cycle event, cycles and events are synonymous">s</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title=""></TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">Count completed Quad Qfxx inst...</TD> 
 * <TD style="display:none">Count completed Quad Qfxx instructions subset of FPGRP2. See the Bgpm Opcode Groups page for more information. </TD>
 
 * </TR><TR><TD colspan="11" style="text-align:left"><a class="anchor" id="a_all_instruction"></a><B><I>All Instruction Opcode Counting</I></B></TD></TR>
 * <TR>
 * <TD>277</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_INST_ALL"></a>PEVT_INST_ALL</TD>   
   
 * <TD style="text-align:left">All Instruction Completions</TD>
      
 * <TD title="events attributable to current sw/hw thread">thread</TD>
 
 
 
 * <TD title="low latency access method supported in BGPM Island Mode;  multiplexing supported in BGPM Distributed Mode;  ">olm</TD>
 * <TD title="Single cycle event, cycles and events are synonymous">s</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title="Bgpm validated - counts confidently verified in controlled bgpm test.">v</TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">Count all completed instructio...</TD> 
 * <TD style="display:none">Count all completed instructions.<br></br>-Equivalent to PEVT_XU_PPC_COMMIT, but this event is preferred as may use any counter, versus the XU event which is restricted to 4 available counters.  </TD>
 
 * </TR>
      
 * </TABLE> 
 * \endhtmlonly

 *
 *
 * \section l2unit_events L2unit Events
 * \htmlonly
 * <TABLE xmlns="http://www.w3.org/1999/xhtml" class="evt">
 * <TR bgcolor="AliceBlue">
 * <TH title="Unique integer value of BGPM event">EventId</TH>
 * <TH title="bgpm enum name for event (see bgpm_events.h)">Label</TH>
 * <TH title="Simple Description of event">Description</TH>
 * <TH title="Scope of the event (thread, core, core-shared, node-shared, node, exclusive, or sampled)">Scope</TH>
 


 * <TH title="Types of features supported by event (e.g. Overflow,LLAccess, multiplexing, etc)">Features</TH>
 * <TH title="Indicates the suitability of the fundamental hardware event">Tag</TH>
 * <TH title="Anticipated PAPI Preset event mapping to this event (see PAPI Presets for whole list)">PAPI</TH>
 * <TH title="Validation Testing level of this event">Status</TH>
 * <TH style="width:20em;" title="Detailed description of event">Detail</TH>
 * <TH style="display:none;">Hidden Detail</TH>
 * </TR><TR><TD colspan="11" style="text-align:left"><a class="anchor" id="a_l2_combined"></a><B><I>L2 Combined Event (all slice counts combined)</I></B></TD></TR>
 * <TR>
 * <TD>278</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_L2_HITS"></a>PEVT_L2_HITS</TD>   
   
 * <TD style="text-align:left">L2 Hits</TD>
      
 * <TD title="events are node-wide with shared counters - each thread can start/stop events of this type for all threads.">node-shared</TD>
 
 
 
 * <TD title="slow overflow supported in BGPM Distributed Mode;  ">s</TD>
 * <TD title="Only suitable for counting numbers of events">e</TD>
 
 * <TD style="text-align:left">PAPI_L2_TCH</TD>
   
 * <TD title=""></TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">hits in L2, both load and stor...</TD> 
 * <TD style="display:none">hits in L2, both load and store. Network Polling store operations from core 17 on BG/Q pollute in this count during normal use. </TD>
 
 * </TR>
 * <TR>
 * <TD>279</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_L2_MISSES"></a>PEVT_L2_MISSES</TD>   
   
 * <TD style="text-align:left">L2 Misses</TD>
      
 * <TD title="events are node-wide with shared counters - each thread can start/stop events of this type for all threads.">node-shared</TD>
 
 
 
 * <TD title="slow overflow supported in BGPM Distributed Mode;  ">s</TD>
 * <TD title="Only suitable for counting numbers of events">e</TD>
 
 * <TD style="text-align:left">PAPI_L2_TCM</TD>
   
 * <TD title=""></TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">cacheline miss in L2 (both loa...</TD> 
 * <TD style="display:none">cacheline miss in L2 (both loads and stores) </TD>
 
 * </TR>
 * <TR>
 * <TD>280</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_L2_PREFETCH"></a>PEVT_L2_PREFETCH</TD>   
   
 * <TD style="text-align:left">L2 prefetch ahead of L1P prefetch</TD>
      
 * <TD title="events are node-wide with shared counters - each thread can start/stop events of this type for all threads.">node-shared</TD>
 
 
 
 * <TD title="slow overflow supported in BGPM Distributed Mode;  ">s</TD>
 * <TD title="Only suitable for counting numbers of events">e</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title=""></TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">fetching cacheline ahead of L1...</TD> 
 * <TD style="display:none">fetching cacheline ahead of L1P prefetch </TD>
 
 * </TR>
 * <TR>
 * <TD>281</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_L2_FETCH_LINE"></a>PEVT_L2_FETCH_LINE</TD>   
   
 * <TD style="text-align:left">L2 fetch 128 byte line from main memory</TD>
      
 * <TD title="events are node-wide with shared counters - each thread can start/stop events of this type for all threads.">node-shared</TD>
 
 
 
 * <TD title="slow overflow supported in BGPM Distributed Mode;  ">s</TD>
 * <TD title="Only suitable for counting numbers of events">e</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title=""></TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">Load 128 byte line from main m...</TD> 
 * <TD style="display:none">Load 128 byte line from main memory </TD>
 
 * </TR>
 * <TR>
 * <TD>282</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_L2_STORE_LINE"></a>PEVT_L2_STORE_LINE</TD>   
   
 * <TD style="text-align:left">L2 store 128 byte line to main memory</TD>
      
 * <TD title="events are node-wide with shared counters - each thread can start/stop events of this type for all threads.">node-shared</TD>
 
 
 
 * <TD title="slow overflow supported in BGPM Distributed Mode;  ">s</TD>
 * <TD title="Only suitable for counting numbers of events">e</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title=""></TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">Store 128 byte line to main me...</TD> 
 * <TD style="display:none">Store 128 byte line to main memory </TD>
 
 * </TR>
 * <TR>
 * <TD>283</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_L2_STORE_PARTIAL_LINE"></a>PEVT_L2_STORE_PARTIAL_LINE</TD>   
   
 * <TD style="text-align:left">L2 store partial line to main memory</TD>
      
 * <TD title="events are node-wide with shared counters - each thread can start/stop events of this type for all threads.">node-shared</TD>
 
 
 
 * <TD title="slow overflow supported in BGPM Distributed Mode;  ">s</TD>
 * <TD title="Only suitable for counting numbers of events">e</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title=""></TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">Store Partial line to main mem...</TD> 
 * <TD style="display:none">Store Partial line to main memory </TD>
 
 * </TR>
 * <TR>
 * <TD>284</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_L2_REQ_REJECT"></a>PEVT_L2_REQ_REJECT</TD>   
   
 * <TD style="text-align:left">L2 Req Reject</TD>
      
 * <TD title="events are node-wide with shared counters - each thread can start/stop events of this type for all threads.">node-shared</TD>
 
 
 
 * <TD title="slow overflow supported in BGPM Distributed Mode;  ">s</TD>
 * <TD title="Only suitable for counting numbers of events">e</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title=""></TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">Request sent by the core was p...</TD> 
 * <TD style="display:none">Request sent by the core was processed, but rejected for later processing. Examples reasons are: address currently reloaded from or evicted to main memory, speculative data for address already cleaned. The request remains and is retried later </TD>
 
 * </TR>
 * <TR>
 * <TD>285</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_L2_REQ_RETIRE"></a>PEVT_L2_REQ_RETIRE</TD>   
   
 * <TD style="text-align:left">L2 Req Retire</TD>
      
 * <TD title="events are node-wide with shared counters - each thread can start/stop events of this type for all threads.">node-shared</TD>
 
 
 
 * <TD title="slow overflow supported in BGPM Distributed Mode;  ">s</TD>
 * <TD title="Only suitable for counting numbers of events">e</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title=""></TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">Request completes right away a...</TD> 
 * <TD style="display:none">Request completes right away after look-up and neither enters the hit nor missqueue. Examples are some dcbtls hitting in L2, some l1-hit notifications </TD>
 
 * </TR>
 * <TR>
 * <TD>286</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_L2_REQ_MISS_AND_EVICT"></a>PEVT_L2_REQ_MISS_AND_EVICT</TD>   
   
 * <TD style="text-align:left">L2 Req Miss and Evict</TD>
      
 * <TD title="events are node-wide with shared counters - each thread can start/stop events of this type for all threads.">node-shared</TD>
 
 
 
 * <TD title="slow overflow supported in BGPM Distributed Mode;  ">s</TD>
 * <TD title="Only suitable for counting numbers of events">e</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title=""></TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">Request misses in L2 and evict...</TD> 
 * <TD style="display:none">Request misses in L2 and evicts a line (subset of L2 misses and subset of L2 Miss and Evict) </TD>
 
 * </TR>
 * <TR>
 * <TD>287</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_L2_REQ_MISS_AND_EVICT_SPEC"></a>PEVT_L2_REQ_MISS_AND_EVICT_SPEC</TD>   
   
 * <TD style="text-align:left">L2 Req Miss and Evict Spec</TD>
      
 * <TD title="events are node-wide with shared counters - each thread can start/stop events of this type for all threads.">node-shared</TD>
 
 
 
 * <TD title="slow overflow supported in BGPM Distributed Mode;  ">s</TD>
 * <TD title="Only suitable for counting numbers of events">e</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title=""></TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">Request misses in L2 and evict...</TD> 
 * <TD style="display:none">Request misses in L2 and evicts a line, while speculative state is pending and the victim may be a speculative line (subset of L2 Req Miss and Evict and subset of L2 Miss and Evict Spec) </TD>
 
 * </TR>
 * <TR>
 * <TD>288</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_L2_MISS_AND_EVICT"></a>PEVT_L2_MISS_AND_EVICT</TD>   
   
 * <TD style="text-align:left">L2 Miss and Evict</TD>
      
 * <TD title="events are node-wide with shared counters - each thread can start/stop events of this type for all threads.">node-shared</TD>
 
 
 
 * <TD title="slow overflow supported in BGPM Distributed Mode;  ">s</TD>
 * <TD title="Only suitable for counting numbers of events">e</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title=""></TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">Request misses in L2 and evict...</TD> 
 * <TD style="display:none">Request misses in L2 and evicts line, but includes evictions due to L2-initiated prefetches </TD>
 
 * </TR>
 * <TR>
 * <TD>289</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_L2_MISS_AND_EVICT_SPEC"></a>PEVT_L2_MISS_AND_EVICT_SPEC</TD>   
   
 * <TD style="text-align:left">L2 Miss and Evict Spec</TD>
      
 * <TD title="events are node-wide with shared counters - each thread can start/stop events of this type for all threads.">node-shared</TD>
 
 
 
 * <TD title="slow overflow supported in BGPM Distributed Mode;  ">s</TD>
 * <TD title="Only suitable for counting numbers of events">e</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title=""></TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">Request misses in L2 and evict...</TD> 
 * <TD style="display:none">Request misses in L2 and evicts a line, while speculative state is pending and the victim may be a speculative line, but includes evictions due to L2-initiated prefetches (subset of L2 Miss and Evict) </TD>
 
 * </TR>
 * <TR>
 * <TD>290</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_L2_REQ_SPEC_ADDR_CLEANUP"></a>PEVT_L2_REQ_SPEC_ADDR_CLEANUP</TD>   
   
 * <TD style="text-align:left">L2 Req Spec Addr Cleanup</TD>
      
 * <TD title="events are node-wide with shared counters - each thread can start/stop events of this type for all threads.">node-shared</TD>
 
 
 
 * <TD title="slow overflow supported in BGPM Distributed Mode;  ">s</TD>
 * <TD title="Only suitable for counting numbers of events">e</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title=""></TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">Request address has speculativ...</TD> 
 * <TD style="display:none">Request address has speculative state needing cleanup (has been invalidated or committed) </TD>
 
 * </TR>
 * <TR>
 * <TD>291</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_L2_SPEC_ADDR_CLEANUP"></a>PEVT_L2_SPEC_ADDR_CLEANUP</TD>   
   
 * <TD style="text-align:left">L2 Spec Addr Cleanup</TD>
      
 * <TD title="events are node-wide with shared counters - each thread can start/stop events of this type for all threads.">node-shared</TD>
 
 
 
 * <TD title="slow overflow supported in BGPM Distributed Mode;  ">s</TD>
 * <TD title="Only suitable for counting numbers of events">e</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title=""></TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">Request address has speculativ...</TD> 
 * <TD style="display:none">Request address has speculative state needing cleanup (has been invalidated or committed), but including L2 initiated prefetches. </TD>
 
 * </TR>
 * <TR>
 * <TD>292</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_L2_SPEC_SET_CLEANUP"></a>PEVT_L2_SPEC_SET_CLEANUP</TD>   
   
 * <TD style="text-align:left">L2 Spec Set Cleanup</TD>
      
 * <TD title="events are node-wide with shared counters - each thread can start/stop events of this type for all threads.">node-shared</TD>
 
 
 
 * <TD title="slow overflow supported in BGPM Distributed Mode;  ">s</TD>
 * <TD title="Only suitable for counting numbers of events">e</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title=""></TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">Request address is mapped to a...</TD> 
 * <TD style="display:none">Request address is mapped to a set that contains other addresses needing cleanup (has been invalidated or committed) </TD>
 
 * </TR>
 * <TR>
 * <TD>293</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_L2_SPEC_RETRY"></a>PEVT_L2_SPEC_RETRY</TD>   
   
 * <TD style="text-align:left">L2 Spec Retry</TD>
      
 * <TD title="events are node-wide with shared counters - each thread can start/stop events of this type for all threads.">node-shared</TD>
 
 
 
 * <TD title="slow overflow supported in BGPM Distributed Mode;  ">s</TD>
 * <TD title="Only suitable for counting numbers of events">e</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title=""></TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">Request needs to be retried la...</TD> 
 * <TD style="display:none">Request needs to be retried later because it would interfere with a speculation commit currently in flight </TD>
 
 * </TR><TR><TD colspan="11" style="text-align:left"><a class="anchor" id="a_l2_slice"></a><B><I>L2 Slice Event (counters for each slice)</I></B></TD></TR>
 * <TR>
 * <TD>294</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_L2_HITS_SLICE"></a>PEVT_L2_HITS_SLICE</TD>   
   
 * <TD style="text-align:left">L2 Cache Slice Hits </TD>
      
 * <TD title="events are node-wide with shared counters - each thread can start/stop events of this type for all threads.">node-shared</TD>
 
 
 
 * <TD title=""></TD>
 * <TD title="Only suitable for counting numbers of events">e</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title=""></TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">hits in L2, both load and stor...</TD> 
 * <TD style="display:none">hits in L2, both load and store. Network Polling store operations from core 17 on BG/Q pollute in this count during normal use. </TD>
 
 * </TR>
 * <TR>
 * <TD>295</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_L2_MISSES_SLICE"></a>PEVT_L2_MISSES_SLICE</TD>   
   
 * <TD style="text-align:left">L2 Cache Slice Misses</TD>
      
 * <TD title="events are node-wide with shared counters - each thread can start/stop events of this type for all threads.">node-shared</TD>
 
 
 
 * <TD title=""></TD>
 * <TD title="Only suitable for counting numbers of events">e</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title=""></TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">Cacheline miss in L2 (both loa...</TD> 
 * <TD style="display:none">Cacheline miss in L2 (both loads and stores) </TD>
 
 * </TR>
 * <TR>
 * <TD>296</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_L2_PREFETCH_SLICE"></a>PEVT_L2_PREFETCH_SLICE</TD>   
   
 * <TD style="text-align:left">L2 Cache Slice prefetch ahead of L1P prefetch</TD>
      
 * <TD title="events are node-wide with shared counters - each thread can start/stop events of this type for all threads.">node-shared</TD>
 
 
 
 * <TD title=""></TD>
 * <TD title="Only suitable for counting numbers of events">e</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title=""></TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">Fetching cacheline ahead of L1...</TD> 
 * <TD style="display:none">Fetching cacheline ahead of L1P prefetch </TD>
 
 * </TR>
 * <TR>
 * <TD>297</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_L2_FETCH_LINE_SLICE"></a>PEVT_L2_FETCH_LINE_SLICE</TD>   
   
 * <TD style="text-align:left">L2 Cache Slice fetch 128 byte line from main memory</TD>
      
 * <TD title="events are node-wide with shared counters - each thread can start/stop events of this type for all threads.">node-shared</TD>
 
 
 
 * <TD title=""></TD>
 * <TD title="Only suitable for counting numbers of events">e</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title=""></TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">Load 128 byte line from main m...</TD> 
 * <TD style="display:none">Load 128 byte line from main memory </TD>
 
 * </TR>
 * <TR>
 * <TD>298</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_L2_STORE_LINE_SLICE"></a>PEVT_L2_STORE_LINE_SLICE</TD>   
   
 * <TD style="text-align:left">L2 Cache Slice store 128 byte line to main memory</TD>
      
 * <TD title="events are node-wide with shared counters - each thread can start/stop events of this type for all threads.">node-shared</TD>
 
 
 
 * <TD title=""></TD>
 * <TD title="Only suitable for counting numbers of events">e</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title=""></TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">Store 128 byte line to main me...</TD> 
 * <TD style="display:none">Store 128 byte line to main memory </TD>
 
 * </TR>
 * <TR>
 * <TD>299</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_L2_STORE_PARTIAL_LINE_SLICE"></a>PEVT_L2_STORE_PARTIAL_LINE_SLICE</TD>   
   
 * <TD style="text-align:left">L2 Cache Slice store partial line to main memory</TD>
      
 * <TD title="events are node-wide with shared counters - each thread can start/stop events of this type for all threads.">node-shared</TD>
 
 
 
 * <TD title=""></TD>
 * <TD title="Only suitable for counting numbers of events">e</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title=""></TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">Store Partial line to main mem...</TD> 
 * <TD style="display:none">Store Partial line to main memory </TD>
 
 * </TR>
 * <TR>
 * <TD>300</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_L2_REQ_REJECT_SLICE"></a>PEVT_L2_REQ_REJECT_SLICE</TD>   
   
 * <TD style="text-align:left">L2 Cache Slice Req Reject</TD>
      
 * <TD title="events are node-wide with shared counters - each thread can start/stop events of this type for all threads.">node-shared</TD>
 
 
 
 * <TD title=""></TD>
 * <TD title="Only suitable for counting numbers of events">e</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title=""></TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">Request sent by the core was p...</TD> 
 * <TD style="display:none">Request sent by the core was processed, but rejected for later processing. Examples reasons are: address currently reloaded from or evicted to main memory, speculative data for address already cleaned. The request remains and is retried later </TD>
 
 * </TR>
 * <TR>
 * <TD>301</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_L2_REQ_RETIRE_SLICE"></a>PEVT_L2_REQ_RETIRE_SLICE</TD>   
   
 * <TD style="text-align:left">L2 Cache Slice Req Retire</TD>
      
 * <TD title="events are node-wide with shared counters - each thread can start/stop events of this type for all threads.">node-shared</TD>
 
 
 
 * <TD title=""></TD>
 * <TD title="Only suitable for counting numbers of events">e</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title=""></TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">Request completes right away a...</TD> 
 * <TD style="display:none">Request completes right away after look-up and neither enters the hit nor missqueue. Examples are some dcbtls hitting in L2, some l1-hit notifications </TD>
 
 * </TR>
 * <TR>
 * <TD>302</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_L2_REQ_MISS_AND_EVICT_SLICE"></a>PEVT_L2_REQ_MISS_AND_EVICT_SLICE</TD>   
   
 * <TD style="text-align:left">L2 Cache Slice Req Miss and Evict</TD>
      
 * <TD title="events are node-wide with shared counters - each thread can start/stop events of this type for all threads.">node-shared</TD>
 
 
 
 * <TD title=""></TD>
 * <TD title="Only suitable for counting numbers of events">e</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title=""></TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">Request misses in L2 and evict...</TD> 
 * <TD style="display:none">Request misses in L2 and evicts a line (subset of L2 misses and subset of L2 Miss and Evict) </TD>
 
 * </TR>
 * <TR>
 * <TD>303</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_L2_REQ_MISS_AND_EVICT_SPEC_SLICE"></a>PEVT_L2_REQ_MISS_AND_EVICT_SPEC_SLICE</TD>   
   
 * <TD style="text-align:left">L2 Cache Slice Req Miss and Evict Spec</TD>
      
 * <TD title="events are node-wide with shared counters - each thread can start/stop events of this type for all threads.">node-shared</TD>
 
 
 
 * <TD title=""></TD>
 * <TD title="Only suitable for counting numbers of events">e</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title=""></TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">Request misses in L2 and evict...</TD> 
 * <TD style="display:none">Request misses in L2 and evicts a line, while speculative state is pending and the victim may be a speculative line (subset of L2 Req Miss and Evict and subset of L2 Miss and Evict Spec) </TD>
 
 * </TR>
 * <TR>
 * <TD>304</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_L2_MISS_AND_EVICT_SLICE"></a>PEVT_L2_MISS_AND_EVICT_SLICE</TD>   
   
 * <TD style="text-align:left">L2 Cache Slice Miss and Evict</TD>
      
 * <TD title="events are node-wide with shared counters - each thread can start/stop events of this type for all threads.">node-shared</TD>
 
 
 
 * <TD title=""></TD>
 * <TD title="Only suitable for counting numbers of events">e</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title=""></TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">Request misses in L2 and evict...</TD> 
 * <TD style="display:none">Request misses in L2 and evicts line, but includes evictions due to L2-initiated prefetches </TD>
 
 * </TR>
 * <TR>
 * <TD>305</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_L2_MISS_AND_EVICT_SPEC_SLICE"></a>PEVT_L2_MISS_AND_EVICT_SPEC_SLICE</TD>   
   
 * <TD style="text-align:left">L2 Cache Slice Miss and Evict Spec</TD>
      
 * <TD title="events are node-wide with shared counters - each thread can start/stop events of this type for all threads.">node-shared</TD>
 
 
 
 * <TD title=""></TD>
 * <TD title="Only suitable for counting numbers of events">e</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title=""></TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">Request misses in L2 and evict...</TD> 
 * <TD style="display:none">Request misses in L2 and evicts a line, while speculative state is pending and the victim may be a speculative line, but includes evictions due to L2-initiated prefetches (subset of L2 Miss and Evict) </TD>
 
 * </TR>
 * <TR>
 * <TD>306</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_L2_REQ_SPEC_ADDR_CLEANUP_SLICE"></a>PEVT_L2_REQ_SPEC_ADDR_CLEANUP_SLICE</TD>   
   
 * <TD style="text-align:left">L2 Cache Slice Req Spec Addr Cleanup</TD>
      
 * <TD title="events are node-wide with shared counters - each thread can start/stop events of this type for all threads.">node-shared</TD>
 
 
 
 * <TD title=""></TD>
 * <TD title="Only suitable for counting numbers of events">e</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title=""></TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">Request address has speculativ...</TD> 
 * <TD style="display:none">Request address has speculative state needing cleanup (has been invalidated or committed) </TD>
 
 * </TR>
 * <TR>
 * <TD>307</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_L2_SPEC_ADDR_CLEANUP_SLICE"></a>PEVT_L2_SPEC_ADDR_CLEANUP_SLICE</TD>   
   
 * <TD style="text-align:left">L2 Cache Slice Spec Addr Cleanup</TD>
      
 * <TD title="events are node-wide with shared counters - each thread can start/stop events of this type for all threads.">node-shared</TD>
 
 
 
 * <TD title=""></TD>
 * <TD title="Only suitable for counting numbers of events">e</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title=""></TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">Request address has speculativ...</TD> 
 * <TD style="display:none">Request address has speculative state needing cleanup (has been invalidated or committed), but including L2 initiated prefetches. </TD>
 
 * </TR>
 * <TR>
 * <TD>308</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_L2_SPEC_SET_CLEANUP_SLICE"></a>PEVT_L2_SPEC_SET_CLEANUP_SLICE</TD>   
   
 * <TD style="text-align:left">L2 Cache Slice Spec Set Cleanup</TD>
      
 * <TD title="events are node-wide with shared counters - each thread can start/stop events of this type for all threads.">node-shared</TD>
 
 
 
 * <TD title=""></TD>
 * <TD title="Only suitable for counting numbers of events">e</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title=""></TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">Request address is mapped to a...</TD> 
 * <TD style="display:none">Request address is mapped to a set that contains other addresses needing cleanup (has been invalidated or committed) </TD>
 
 * </TR>
 * <TR>
 * <TD>309</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_L2_SPEC_RETRY_SLICE"></a>PEVT_L2_SPEC_RETRY_SLICE</TD>   
   
 * <TD style="text-align:left">L2 Cache Slice Spec Retry</TD>
      
 * <TD title="events are node-wide with shared counters - each thread can start/stop events of this type for all threads.">node-shared</TD>
 
 
 
 * <TD title=""></TD>
 * <TD title="Only suitable for counting numbers of events">e</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title=""></TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">Request needs to be retried la...</TD> 
 * <TD style="display:none">Request needs to be retried later because it would interfere with a speculation commit currently in flight </TD>
 
 * </TR>
      
 * </TABLE> 
 * \endhtmlonly

 *
 *
 * \section iounit_events IOunit Events
 * \htmlonly
 * <TABLE xmlns="http://www.w3.org/1999/xhtml" class="evt">
 * <TR bgcolor="AliceBlue">
 * <TH title="Unique integer value of BGPM event">EventId</TH>
 * <TH title="bgpm enum name for event (see bgpm_events.h)">Label</TH>
 * <TH title="Simple Description of event">Description</TH>
 * <TH title="Scope of the event (thread, core, core-shared, node-shared, node, exclusive, or sampled)">Scope</TH>
 


 * <TH title="Types of features supported by event (e.g. Overflow,LLAccess, multiplexing, etc)">Features</TH>
 * <TH title="Indicates the suitability of the fundamental hardware event">Tag</TH>
 * <TH title="Anticipated PAPI Preset event mapping to this event (see PAPI Presets for whole list)">PAPI</TH>
 * <TH title="Validation Testing level of this event">Status</TH>
 * <TH style="width:20em;" title="Detailed description of event">Detail</TH>
 * <TH style="display:none;">Hidden Detail</TH>
 * </TR><TR><TD colspan="11" style="text-align:left"><a class="anchor" id="a_message_unit"></a><B><I>Message Unit Events</I></B></TD></TR>
 * <TR>
 * <TD>310</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_MU_PKT_INJ"></a>PEVT_MU_PKT_INJ</TD>   
   
 * <TD style="text-align:left">Mu packet injected</TD>
      
 * <TD title="events are node-wide with shared counters - each thread can start/stop events of this type for all threads.">node-shared</TD>
 
 
 
 * <TD title="slow overflow supported in BGPM Distributed Mode;  ">s</TD>
 * <TD title="Only suitable for counting numbers of events">e</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title=""></TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">A new packet has been injected...</TD> 
 * <TD style="display:none">A new packet has been injected (Packet has been stored to ND FIFO).Does not count first packet in message, so to get total injected packet count, must sum PEVT_MU_PKT_INJ + PEVT_MU_MSG_INJ events. </TD>
 
 * </TR>
 * <TR>
 * <TD>311</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_MU_MSG_INJ"></a>PEVT_MU_MSG_INJ</TD>   
   
 * <TD style="text-align:left">Mu message injected</TD>
      
 * <TD title="events are node-wide with shared counters - each thread can start/stop events of this type for all threads.">node-shared</TD>
 
 
 
 * <TD title="slow overflow supported in BGPM Distributed Mode;  ">s</TD>
 * <TD title="Only suitable for counting numbers of events">e</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title=""></TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">A new message has been injecte...</TD> 
 * <TD style="display:none">A new message has been injected (All packets of the message have been stored to ND FIFO) </TD>
 
 * </TR>
 * <TR>
 * <TD>312</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_MU_FIFO_PKT_RCV"></a>PEVT_MU_FIFO_PKT_RCV</TD>   
   
 * <TD style="text-align:left">Mu FIFO packet received</TD>
      
 * <TD title="events are node-wide with shared counters - each thread can start/stop events of this type for all threads.">node-shared</TD>
 
 
 
 * <TD title="slow overflow supported in BGPM Distributed Mode;  ">s</TD>
 * <TD title="Only suitable for counting numbers of events">e</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title=""></TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">A new FIFO packet has been rec...</TD> 
 * <TD style="display:none">A new FIFO packet has been received (The packet has been stored to L2. There is no pending switch request) </TD>
 
 * </TR>
 * <TR>
 * <TD>313</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_MU_RGET_PKT_RCV"></a>PEVT_MU_RGET_PKT_RCV</TD>   
   
 * <TD style="text-align:left">Mu RGET packet received</TD>
      
 * <TD title="events are node-wide with shared counters - each thread can start/stop events of this type for all threads.">node-shared</TD>
 
 
 
 * <TD title="slow overflow supported in BGPM Distributed Mode;  ">s</TD>
 * <TD title="Only suitable for counting numbers of events">e</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title=""></TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">A new RGET packet has been rec...</TD> 
 * <TD style="display:none">A new RGET packet has been received (The packet has been stored to L2. There is no pending switch request. ICSRAM has accepted update tail request) </TD>
 
 * </TR>
 * <TR>
 * <TD>314</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_MU_PUT_PKT_RCV"></a>PEVT_MU_PUT_PKT_RCV</TD>   
   
 * <TD style="text-align:left">Mu PUT packet received</TD>
      
 * <TD title="events are node-wide with shared counters - each thread can start/stop events of this type for all threads.">node-shared</TD>
 
 
 
 * <TD title="slow overflow supported in BGPM Distributed Mode;  ">s</TD>
 * <TD title="Only suitable for counting numbers of events">e</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title=""></TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">A new PUT packet has been rece...</TD> 
 * <TD style="display:none">A new PUT packet has been received (rME has started to process the packet). Note that this counter is incremented before packet has been completely processed (stored to L2). This is because event is checked by RPUT SRAM access but it is accessed only when rME starts to process packet. </TD>
 
 * </TR>
 * <TR>
 * <TD>315</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_MU_PORT0_16B_WRT"></a>PEVT_MU_PORT0_16B_WRT</TD>   
   
 * <TD style="text-align:left">Mu port 0 16B write</TD>
      
 * <TD title="events are node-wide with shared counters - each thread can start/stop events of this type for all threads.">node-shared</TD>
 
 
 
 * <TD title="slow overflow supported in BGPM Distributed Mode;  ">s</TD>
 * <TD title="Only suitable for counting numbers of events">e</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title=""></TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">Master port 0 has issued a 16B...</TD> 
 * <TD style="display:none">Master port 0 has issued a 16B write request to the switch. </TD>
 
 * </TR>
 * <TR>
 * <TD>316</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_MU_PORT0_32B_RD"></a>PEVT_MU_PORT0_32B_RD</TD>   
   
 * <TD style="text-align:left">Mu port 0 32B read</TD>
      
 * <TD title="events are node-wide with shared counters - each thread can start/stop events of this type for all threads.">node-shared</TD>
 
 
 
 * <TD title="slow overflow supported in BGPM Distributed Mode;  ">s</TD>
 * <TD title="Only suitable for counting numbers of events">e</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title=""></TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">Master port 0 has issued a 32B...</TD> 
 * <TD style="display:none">Master port 0 has issued a 32B read request to the switch. </TD>
 
 * </TR>
 * <TR>
 * <TD>317</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_MU_PORT0_64B_RD"></a>PEVT_MU_PORT0_64B_RD</TD>   
   
 * <TD style="text-align:left">Mu port 0 64B read</TD>
      
 * <TD title="events are node-wide with shared counters - each thread can start/stop events of this type for all threads.">node-shared</TD>
 
 
 
 * <TD title="slow overflow supported in BGPM Distributed Mode;  ">s</TD>
 * <TD title="Only suitable for counting numbers of events">e</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title=""></TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">Master port 0 has issued a 64B...</TD> 
 * <TD style="display:none">Master port 0 has issued a 64B read request to the switch. </TD>
 
 * </TR>
 * <TR>
 * <TD>318</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_MU_PORT0_128B_RD"></a>PEVT_MU_PORT0_128B_RD</TD>   
   
 * <TD style="text-align:left">Mu port 0 128B read</TD>
      
 * <TD title="events are node-wide with shared counters - each thread can start/stop events of this type for all threads.">node-shared</TD>
 
 
 
 * <TD title="slow overflow supported in BGPM Distributed Mode;  ">s</TD>
 * <TD title="Only suitable for counting numbers of events">e</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title=""></TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">Master port 0 has issued a 128...</TD> 
 * <TD style="display:none">Master port 0 has issued a 128B read request to the switch. </TD>
 
 * </TR>
 * <TR>
 * <TD>319</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_MU_PORT1_16B_WRT"></a>PEVT_MU_PORT1_16B_WRT</TD>   
   
 * <TD style="text-align:left">Mu port 1 16B write</TD>
      
 * <TD title="events are node-wide with shared counters - each thread can start/stop events of this type for all threads.">node-shared</TD>
 
 
 
 * <TD title="slow overflow supported in BGPM Distributed Mode;  ">s</TD>
 * <TD title="Only suitable for counting numbers of events">e</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title=""></TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">Master port 1 has issued a 16B...</TD> 
 * <TD style="display:none">Master port 1 has issued a 16B write request to the switch. </TD>
 
 * </TR>
 * <TR>
 * <TD>320</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_MU_PORT1_32B_RD"></a>PEVT_MU_PORT1_32B_RD</TD>   
   
 * <TD style="text-align:left">Mu port 1 32B read</TD>
      
 * <TD title="events are node-wide with shared counters - each thread can start/stop events of this type for all threads.">node-shared</TD>
 
 
 
 * <TD title="slow overflow supported in BGPM Distributed Mode;  ">s</TD>
 * <TD title="Only suitable for counting numbers of events">e</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title=""></TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">Master port 1 has issued a 32B...</TD> 
 * <TD style="display:none">Master port 1 has issued a 32B read request to the switch. </TD>
 
 * </TR>
 * <TR>
 * <TD>321</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_MU_PORT1_64B_RD"></a>PEVT_MU_PORT1_64B_RD</TD>   
   
 * <TD style="text-align:left">Mu port 1 64B read</TD>
      
 * <TD title="events are node-wide with shared counters - each thread can start/stop events of this type for all threads.">node-shared</TD>
 
 
 
 * <TD title="slow overflow supported in BGPM Distributed Mode;  ">s</TD>
 * <TD title="Only suitable for counting numbers of events">e</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title=""></TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">Master port 1 has issued a 64B...</TD> 
 * <TD style="display:none">Master port 1 has issued a 64B read request to the switch. </TD>
 
 * </TR>
 * <TR>
 * <TD>322</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_MU_PORT1_128B_RD"></a>PEVT_MU_PORT1_128B_RD</TD>   
   
 * <TD style="text-align:left">Mu port 1 128B read</TD>
      
 * <TD title="events are node-wide with shared counters - each thread can start/stop events of this type for all threads.">node-shared</TD>
 
 
 
 * <TD title="slow overflow supported in BGPM Distributed Mode;  ">s</TD>
 * <TD title="Only suitable for counting numbers of events">e</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title=""></TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">Master port 1 has issued a 128...</TD> 
 * <TD style="display:none">Master port 1 has issued a 128B read request to the switch. </TD>
 
 * </TR>
 * <TR>
 * <TD>323</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_MU_PORT2_16B_WRT"></a>PEVT_MU_PORT2_16B_WRT</TD>   
   
 * <TD style="text-align:left">Mu port 2 16B write</TD>
      
 * <TD title="events are node-wide with shared counters - each thread can start/stop events of this type for all threads.">node-shared</TD>
 
 
 
 * <TD title="slow overflow supported in BGPM Distributed Mode;  ">s</TD>
 * <TD title="Only suitable for counting numbers of events">e</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title=""></TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">Master port 2 has issued a 16B...</TD> 
 * <TD style="display:none">Master port 2 has issued a 16B write request to the switch. </TD>
 
 * </TR>
 * <TR>
 * <TD>324</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_MU_PORT2_32B_RD"></a>PEVT_MU_PORT2_32B_RD</TD>   
   
 * <TD style="text-align:left">Mu port 2 32B read</TD>
      
 * <TD title="events are node-wide with shared counters - each thread can start/stop events of this type for all threads.">node-shared</TD>
 
 
 
 * <TD title="slow overflow supported in BGPM Distributed Mode;  ">s</TD>
 * <TD title="Only suitable for counting numbers of events">e</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title=""></TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">Master port 2 has issued a 32B...</TD> 
 * <TD style="display:none">Master port 2 has issued a 32B read request to the switch. </TD>
 
 * </TR>
 * <TR>
 * <TD>325</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_MU_PORT2_64B_RD"></a>PEVT_MU_PORT2_64B_RD</TD>   
   
 * <TD style="text-align:left">Mu port 2 64B read</TD>
      
 * <TD title="events are node-wide with shared counters - each thread can start/stop events of this type for all threads.">node-shared</TD>
 
 
 
 * <TD title="slow overflow supported in BGPM Distributed Mode;  ">s</TD>
 * <TD title="Only suitable for counting numbers of events">e</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title=""></TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">Master port 2 has issued a 64B...</TD> 
 * <TD style="display:none">Master port 2 has issued a 64B read request to the switch. </TD>
 
 * </TR>
 * <TR>
 * <TD>326</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_MU_PORT2_128B_RD"></a>PEVT_MU_PORT2_128B_RD</TD>   
   
 * <TD style="text-align:left">Mu port 2 128B read</TD>
      
 * <TD title="events are node-wide with shared counters - each thread can start/stop events of this type for all threads.">node-shared</TD>
 
 
 
 * <TD title="slow overflow supported in BGPM Distributed Mode;  ">s</TD>
 * <TD title="Only suitable for counting numbers of events">e</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title=""></TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">Master port 2 has issued a 128...</TD> 
 * <TD style="display:none">Master port 2 has issued a 128B read request to the switch. </TD>
 
 * </TR>
 * <TR>
 * <TD>327</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_MU_SLV_PORT_RD"></a>PEVT_MU_SLV_PORT_RD</TD>   
   
 * <TD style="text-align:left">Mu Slave port read</TD>
      
 * <TD title="events are node-wide with shared counters - each thread can start/stop events of this type for all threads.">node-shared</TD>
 
 
 
 * <TD title="slow overflow supported in BGPM Distributed Mode;  ">s</TD>
 * <TD title="Only suitable for counting numbers of events">e</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title=""></TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">Slave port has received a read...</TD> 
 * <TD style="display:none">Slave port has received a read request. </TD>
 
 * </TR>
 * <TR>
 * <TD>328</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_MU_SLV_PORT_WRT"></a>PEVT_MU_SLV_PORT_WRT</TD>   
   
 * <TD style="text-align:left">Mu Slave port write</TD>
      
 * <TD title="events are node-wide with shared counters - each thread can start/stop events of this type for all threads.">node-shared</TD>
 
 
 
 * <TD title="slow overflow supported in BGPM Distributed Mode;  ">s</TD>
 * <TD title="Only suitable for counting numbers of events">e</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title=""></TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">Slave port has received a writ...</TD> 
 * <TD style="display:none">Slave port has received a write request. </TD>
 
 * </TR>
 * <TR>
 * <TD>329</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_MU_PORT0_PEND_WRT"></a>PEVT_MU_PORT0_PEND_WRT</TD>   
   
 * <TD style="text-align:left">Mu port 0 pending write requests</TD>
      
 * <TD title="events are node-wide with shared counters - each thread can start/stop events of this type for all threads.">node-shared</TD>
 
 
 
 * <TD title="slow overflow supported in BGPM Distributed Mode;  ">s</TD>
 * <TD title="Only suitable for counting numbers of events">e</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title=""></TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">Number of write requests pendi...</TD> 
 * <TD style="display:none">Number of write requests pending in master port 0. </TD>
 
 * </TR>
 * <TR>
 * <TD>330</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_MU_PORT0_PEND_RD"></a>PEVT_MU_PORT0_PEND_RD</TD>   
   
 * <TD style="text-align:left">Mu port 0 pending read requests</TD>
      
 * <TD title="events are node-wide with shared counters - each thread can start/stop events of this type for all threads.">node-shared</TD>
 
 
 
 * <TD title="slow overflow supported in BGPM Distributed Mode;  ">s</TD>
 * <TD title="Only suitable for counting numbers of events">e</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title=""></TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">Number of read requests pendin...</TD> 
 * <TD style="display:none">Number of read requests pending in master port 0. </TD>
 
 * </TR>
 * <TR>
 * <TD>331</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_MU_PORT1_PEND_WRT"></a>PEVT_MU_PORT1_PEND_WRT</TD>   
   
 * <TD style="text-align:left">Mu port 1 pending write requests</TD>
      
 * <TD title="events are node-wide with shared counters - each thread can start/stop events of this type for all threads.">node-shared</TD>
 
 
 
 * <TD title="slow overflow supported in BGPM Distributed Mode;  ">s</TD>
 * <TD title="Only suitable for counting numbers of events">e</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title=""></TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">Number of write requests pendi...</TD> 
 * <TD style="display:none">Number of write requests pending in master port 1. </TD>
 
 * </TR>
 * <TR>
 * <TD>332</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_MU_PORT1_PEND_RD"></a>PEVT_MU_PORT1_PEND_RD</TD>   
   
 * <TD style="text-align:left">Mu port 1 pending read requests</TD>
      
 * <TD title="events are node-wide with shared counters - each thread can start/stop events of this type for all threads.">node-shared</TD>
 
 
 
 * <TD title="slow overflow supported in BGPM Distributed Mode;  ">s</TD>
 * <TD title="Only suitable for counting numbers of events">e</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title=""></TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">Number of read requests pendin...</TD> 
 * <TD style="display:none">Number of read requests pending in master port 1. </TD>
 
 * </TR>
 * <TR>
 * <TD>333</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_MU_PORT2_PEND_WRT"></a>PEVT_MU_PORT2_PEND_WRT</TD>   
   
 * <TD style="text-align:left">Mu port 2 pending write requests</TD>
      
 * <TD title="events are node-wide with shared counters - each thread can start/stop events of this type for all threads.">node-shared</TD>
 
 
 
 * <TD title="slow overflow supported in BGPM Distributed Mode;  ">s</TD>
 * <TD title="Only suitable for counting numbers of events">e</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title=""></TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">Number of write requests pendi...</TD> 
 * <TD style="display:none">Number of write requests pending in master port 2. </TD>
 
 * </TR>
 * <TR>
 * <TD>334</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_MU_PORT2_PEND_RD"></a>PEVT_MU_PORT2_PEND_RD</TD>   
   
 * <TD style="text-align:left">Mu port 2 pending read requests</TD>
      
 * <TD title="events are node-wide with shared counters - each thread can start/stop events of this type for all threads.">node-shared</TD>
 
 
 
 * <TD title="slow overflow supported in BGPM Distributed Mode;  ">s</TD>
 * <TD title="Only suitable for counting numbers of events">e</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title=""></TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">Number of read requests pendin...</TD> 
 * <TD style="display:none">Number of read requests pending in master port 2. </TD>
 
 * </TR><TR><TD colspan="11" style="text-align:left"><a class="anchor" id="a_pcie_io"></a><B><I>PCIe I/O Unit Events</I></B></TD></TR>
 * <TR>
 * <TD>335</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_PCIE_INB_RD_BYTES"></a>PEVT_PCIE_INB_RD_BYTES</TD>   
   
 * <TD style="text-align:left">Pcie Inbound Read Bytes</TD>
      
 * <TD title="events are node-wide with shared counters - each thread can start/stop events of this type for all threads.">node-shared</TD>
 
 
 
 * <TD title="slow overflow supported in BGPM Distributed Mode;  ">s</TD>
 * <TD title="Only suitable for counting numbers of events">e</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title=""></TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">Inbound Read Bytes Requested ...</TD> 
 * <TD style="display:none">Inbound Read Bytes Requested </TD>
 
 * </TR>
 * <TR>
 * <TD>336</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_PCIE_INB_RDS"></a>PEVT_PCIE_INB_RDS</TD>   
   
 * <TD style="text-align:left">Pcie Inbound Read Requests</TD>
      
 * <TD title="events are node-wide with shared counters - each thread can start/stop events of this type for all threads.">node-shared</TD>
 
 
 
 * <TD title="slow overflow supported in BGPM Distributed Mode;  ">s</TD>
 * <TD title="Only suitable for counting numbers of events">e</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title=""></TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">Inbound Read Requests ...</TD> 
 * <TD style="display:none">Inbound Read Requests </TD>
 
 * </TR>
 * <TR>
 * <TD>337</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_PCIE_INB_RD_CMPLT"></a>PEVT_PCIE_INB_RD_CMPLT</TD>   
   
 * <TD style="text-align:left">Pcie Inbound Read Completions</TD>
      
 * <TD title="events are node-wide with shared counters - each thread can start/stop events of this type for all threads.">node-shared</TD>
 
 
 
 * <TD title="slow overflow supported in BGPM Distributed Mode;  ">s</TD>
 * <TD title="Only suitable for counting numbers of events">e</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title=""></TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">Inbound Read Completions ...</TD> 
 * <TD style="display:none">Inbound Read Completions </TD>
 
 * </TR>
 * <TR>
 * <TD>338</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_PCIE_OUTB_WRT_BYTES"></a>PEVT_PCIE_OUTB_WRT_BYTES</TD>   
   
 * <TD style="text-align:left">Pcie outbound memory write bytes</TD>
      
 * <TD title="events are node-wide with shared counters - each thread can start/stop events of this type for all threads.">node-shared</TD>
 
 
 
 * <TD title="slow overflow supported in BGPM Distributed Mode;  ">s</TD>
 * <TD title="Only suitable for counting numbers of events">e</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title=""></TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">outbound memory write bytes ...</TD> 
 * <TD style="display:none">outbound memory write bytes </TD>
 
 * </TR>
 * <TR>
 * <TD>339</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_PCIE_OUTB_CFG_X"></a>PEVT_PCIE_OUTB_CFG_X</TD>   
   
 * <TD style="text-align:left">Pcie Outbound CFG transactions</TD>
      
 * <TD title="events are node-wide with shared counters - each thread can start/stop events of this type for all threads.">node-shared</TD>
 
 
 
 * <TD title="slow overflow supported in BGPM Distributed Mode;  ">s</TD>
 * <TD title="Only suitable for counting numbers of events">e</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title=""></TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">Outbound CFG transactions ...</TD> 
 * <TD style="display:none">Outbound CFG transactions </TD>
 
 * </TR>
 * <TR>
 * <TD>340</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_PCIE_OUTB_IO_X"></a>PEVT_PCIE_OUTB_IO_X</TD>   
   
 * <TD style="text-align:left">Pcie Outbound IO transactions</TD>
      
 * <TD title="events are node-wide with shared counters - each thread can start/stop events of this type for all threads.">node-shared</TD>
 
 
 
 * <TD title="slow overflow supported in BGPM Distributed Mode;  ">s</TD>
 * <TD title="Only suitable for counting numbers of events">e</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title=""></TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">Outbound IO transactions ...</TD> 
 * <TD style="display:none">Outbound IO transactions </TD>
 
 * </TR>
 * <TR>
 * <TD>341</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_PCIE_RX_DLLP"></a>PEVT_PCIE_RX_DLLP</TD>   
   
 * <TD style="text-align:left">Pcie RX DLLP Count</TD>
      
 * <TD title="events are node-wide with shared counters - each thread can start/stop events of this type for all threads.">node-shared</TD>
 
 
 
 * <TD title="slow overflow supported in BGPM Distributed Mode;  ">s</TD>
 * <TD title="Only suitable for counting numbers of events">e</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title=""></TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">RX DLLP Count ...</TD> 
 * <TD style="display:none">RX DLLP Count </TD>
 
 * </TR>
 * <TR>
 * <TD>342</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_PCIE_RX_TLP"></a>PEVT_PCIE_RX_TLP</TD>   
   
 * <TD style="text-align:left">Pcie RX TLP Count</TD>
      
 * <TD title="events are node-wide with shared counters - each thread can start/stop events of this type for all threads.">node-shared</TD>
 
 
 
 * <TD title="slow overflow supported in BGPM Distributed Mode;  ">s</TD>
 * <TD title="Only suitable for counting numbers of events">e</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title=""></TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">RX TLP Count ...</TD> 
 * <TD style="display:none">RX TLP Count </TD>
 
 * </TR>
 * <TR>
 * <TD>343</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_PCIE_RX_TLP_SIZE"></a>PEVT_PCIE_RX_TLP_SIZE</TD>   
   
 * <TD style="text-align:left">Pcie RX TLP Size in DWORDS</TD>
      
 * <TD title="events are node-wide with shared counters - each thread can start/stop events of this type for all threads.">node-shared</TD>
 
 
 
 * <TD title="slow overflow supported in BGPM Distributed Mode;  ">s</TD>
 * <TD title="Only suitable for counting numbers of events">e</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title=""></TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">RX TLP Size in DWORDS ...</TD> 
 * <TD style="display:none">RX TLP Size in DWORDS </TD>
 
 * </TR>
 * <TR>
 * <TD>344</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_PCIE_TX_DLLP"></a>PEVT_PCIE_TX_DLLP</TD>   
   
 * <TD style="text-align:left">Pcie TX DLLP Count</TD>
      
 * <TD title="events are node-wide with shared counters - each thread can start/stop events of this type for all threads.">node-shared</TD>
 
 
 
 * <TD title="slow overflow supported in BGPM Distributed Mode;  ">s</TD>
 * <TD title="Only suitable for counting numbers of events">e</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title=""></TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">TX DLLP Count ...</TD> 
 * <TD style="display:none">TX DLLP Count </TD>
 
 * </TR>
 * <TR>
 * <TD>345</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_PCIE_TX_TLP"></a>PEVT_PCIE_TX_TLP</TD>   
   
 * <TD style="text-align:left">Pcie TX TLP Count</TD>
      
 * <TD title="events are node-wide with shared counters - each thread can start/stop events of this type for all threads.">node-shared</TD>
 
 
 
 * <TD title="slow overflow supported in BGPM Distributed Mode;  ">s</TD>
 * <TD title="Only suitable for counting numbers of events">e</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title=""></TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">TX TLP Count ...</TD> 
 * <TD style="display:none">TX TLP Count </TD>
 
 * </TR>
 * <TR>
 * <TD>346</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_PCIE_TX_TLP_SIZE"></a>PEVT_PCIE_TX_TLP_SIZE</TD>   
   
 * <TD style="text-align:left">Pcie TX TLP Size in DWORDS</TD>
      
 * <TD title="events are node-wide with shared counters - each thread can start/stop events of this type for all threads.">node-shared</TD>
 
 
 
 * <TD title="slow overflow supported in BGPM Distributed Mode;  ">s</TD>
 * <TD title="Only suitable for counting numbers of events">e</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title=""></TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">TX TLP Size in DWORDS ...</TD> 
 * <TD style="display:none">TX TLP Size in DWORDS </TD>
 
 * </TR><TR><TD colspan="11" style="text-align:left"><a class="anchor" id="a_devbus_io"></a><B><I>DevBus I/O Unit Events</I></B></TD></TR>
 * <TR>
 * <TD>347</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_DB_PCIE_INB_WRT_BYTES"></a>PEVT_DB_PCIE_INB_WRT_BYTES</TD>   
   
 * <TD style="text-align:left">Devbus PCIe inbound write bytes written</TD>
      
 * <TD title="events are node-wide with shared counters - each thread can start/stop events of this type for all threads.">node-shared</TD>
 
 
 
 * <TD title="slow overflow supported in BGPM Distributed Mode;  ">s</TD>
 * <TD title="Only suitable for counting numbers of events">e</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title=""></TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">PCIe inbound write bytes writt...</TD> 
 * <TD style="display:none">PCIe inbound write bytes written </TD>
 
 * </TR>
 * <TR>
 * <TD>348</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_DB_PCIE_OUTB_RD_BYTES"></a>PEVT_DB_PCIE_OUTB_RD_BYTES</TD>   
   
 * <TD style="text-align:left">Devbus PCIe outbound read bytes requested</TD>
      
 * <TD title="events are node-wide with shared counters - each thread can start/stop events of this type for all threads.">node-shared</TD>
 
 
 
 * <TD title="slow overflow supported in BGPM Distributed Mode;  ">s</TD>
 * <TD title="Only suitable for counting numbers of events">e</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title=""></TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">PCIe outbound read bytes reque...</TD> 
 * <TD style="display:none">PCIe outbound read bytes requested </TD>
 
 * </TR>
 * <TR>
 * <TD>349</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_DB_PCIE_OUTB_RDS"></a>PEVT_DB_PCIE_OUTB_RDS</TD>   
   
 * <TD style="text-align:left">Devbus PCIe outbound read requests</TD>
      
 * <TD title="events are node-wide with shared counters - each thread can start/stop events of this type for all threads.">node-shared</TD>
 
 
 
 * <TD title="slow overflow supported in BGPM Distributed Mode;  ">s</TD>
 * <TD title="Only suitable for counting numbers of events">e</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title=""></TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">PCIe outbound read requests ...</TD> 
 * <TD style="display:none">PCIe outbound read requests </TD>
 
 * </TR>
 * <TR>
 * <TD>350</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_DB_PCIE_OUTB_RD_CMPLT"></a>PEVT_DB_PCIE_OUTB_RD_CMPLT</TD>   
   
 * <TD style="text-align:left">Devbus PCIe outbound read completions</TD>
      
 * <TD title="events are node-wide with shared counters - each thread can start/stop events of this type for all threads.">node-shared</TD>
 
 
 
 * <TD title="slow overflow supported in BGPM Distributed Mode;  ">s</TD>
 * <TD title="Only suitable for counting numbers of events">e</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title=""></TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">PCIe outbound read completions...</TD> 
 * <TD style="display:none">PCIe outbound read completions </TD>
 
 * </TR>
 * <TR>
 * <TD>351</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_DB_BOOT_EDRAM_WRT_BYTES"></a>PEVT_DB_BOOT_EDRAM_WRT_BYTES</TD>   
   
 * <TD style="text-align:left">Devbus Boot eDRAM bytes written</TD>
      
 * <TD title="events are node-wide with shared counters - each thread can start/stop events of this type for all threads.">node-shared</TD>
 
 
 
 * <TD title="slow overflow supported in BGPM Distributed Mode;  ">s</TD>
 * <TD title="Only suitable for counting numbers of events">e</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title=""></TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">Boot eDRAM bytes written ...</TD> 
 * <TD style="display:none">Boot eDRAM bytes written </TD>
 
 * </TR>
 * <TR>
 * <TD>352</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_DB_BOOT_EDRAM_RD_BYTES"></a>PEVT_DB_BOOT_EDRAM_RD_BYTES</TD>   
   
 * <TD style="text-align:left">Devbus Boot eDRAM bytes read</TD>
      
 * <TD title="events are node-wide with shared counters - each thread can start/stop events of this type for all threads.">node-shared</TD>
 
 
 
 * <TD title="slow overflow supported in BGPM Distributed Mode;  ">s</TD>
 * <TD title="Only suitable for counting numbers of events">e</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title=""></TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">Boot eDRAM bytes read ...</TD> 
 * <TD style="display:none">Boot eDRAM bytes read </TD>
 
 * </TR>
 * <TR>
 * <TD>353</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_DB_MSG_SIG_INTS"></a>PEVT_DB_MSG_SIG_INTS</TD>   
   
 * <TD style="text-align:left">Devbus Message-Signaled Interrupts (MSIs)</TD>
      
 * <TD title="events are node-wide with shared counters - each thread can start/stop events of this type for all threads.">node-shared</TD>
 
 
 
 * <TD title="slow overflow supported in BGPM Distributed Mode;  ">s</TD>
 * <TD title="Only suitable for counting numbers of events">e</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title=""></TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">Message-Signaled Interrupts (M...</TD> 
 * <TD style="display:none">Message-Signaled Interrupts (MSIs) </TD>
 
 * </TR>
      
 * </TABLE> 
 * \endhtmlonly

 *
 *
 * \section nwunit_events NWUnit Events
 * \htmlonly
 * <TABLE xmlns="http://www.w3.org/1999/xhtml" class="evt">
 * <TR bgcolor="AliceBlue">
 * <TH title="Unique integer value of BGPM event">EventId</TH>
 * <TH title="bgpm enum name for event (see bgpm_events.h)">Label</TH>
 * <TH title="Simple Description of event">Description</TH>
 * <TH title="Scope of the event (thread, core, core-shared, node-shared, node, exclusive, or sampled)">Scope</TH>
 


 * <TH title="Types of features supported by event (e.g. Overflow,LLAccess, multiplexing, etc)">Features</TH>
 * <TH title="Indicates the suitability of the fundamental hardware event">Tag</TH>
 * <TH title="Anticipated PAPI Preset event mapping to this event (see PAPI Presets for whole list)">PAPI</TH>
 * <TH title="Validation Testing level of this event">Status</TH>
 * <TH style="width:20em;" title="Detailed description of event">Detail</TH>
 * <TH style="display:none;">Hidden Detail</TH>
 * </TR><TR><TD colspan="11" style="text-align:left"><a class="anchor" id="a_network_unit"></a><B><I>Network Unit Events</I></B></TD></TR>
 * <TR>
 * <TD>354</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_NW_USER_PP_SENT"></a>PEVT_NW_USER_PP_SENT</TD>   
   
 * <TD style="text-align:left">User pt-pt 32B packet chunks sent </TD>
      
 * <TD title="events are node-wide but counter resource is controllable by a single sw thread at a time">exclusive</TD>
 
 
 
 * <TD title=""></TD>
 * <TD title="Only suitable for counting numbers of events">e</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title=""></TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">Number of 32 byte user point t...</TD> 
 * <TD style="display:none">Number of 32 byte user point to point packet chunks sent. Includes packets originating or passing through the current node. </TD>
 
 * </TR>
 * <TR>
 * <TD>355</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_NW_USER_DYN_PP_SENT"></a>PEVT_NW_USER_DYN_PP_SENT</TD>   
   
 * <TD style="text-align:left">User dynamic pt-pt 32B packet chunks sent </TD>
      
 * <TD title="events are node-wide but counter resource is controllable by a single sw thread at a time">exclusive</TD>
 
 
 
 * <TD title=""></TD>
 * <TD title="Only suitable for counting numbers of events">e</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title=""></TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">Number of 32 byte user dynamic...</TD> 
 * <TD style="display:none">Number of 32 byte user dynamic point to point packet chunks sent. Includes packets originating or passing through the current node. </TD>
 
 * </TR>
 * <TR>
 * <TD>356</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_NW_USER_ESC_PP_SENT"></a>PEVT_NW_USER_ESC_PP_SENT</TD>   
   
 * <TD style="text-align:left">User escape pt-pt 32B packet chunks sent </TD>
      
 * <TD title="events are node-wide but counter resource is controllable by a single sw thread at a time">exclusive</TD>
 
 
 
 * <TD title=""></TD>
 * <TD title="Only suitable for counting numbers of events">e</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title=""></TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">Number of 32 byte user escape ...</TD> 
 * <TD style="display:none">Number of 32 byte user escape point to point packet chunks sent. Includes packets originating or passing through the current node. </TD>
 
 * </TR>
 * <TR>
 * <TD>357</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_NW_USER_PRI_PP_SENT"></a>PEVT_NW_USER_PRI_PP_SENT</TD>   
   
 * <TD style="text-align:left">User priority pt-pt 32B packet chunks sent </TD>
      
 * <TD title="events are node-wide but counter resource is controllable by a single sw thread at a time">exclusive</TD>
 
 
 
 * <TD title=""></TD>
 * <TD title="Only suitable for counting numbers of events">e</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title=""></TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">Number of 32 byte user priorit...</TD> 
 * <TD style="display:none">Number of 32 byte user priority point to point packet chunks sent. Includes packets originating or passing through the current node. </TD>
 
 * </TR>
 * <TR>
 * <TD>358</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_NW_SYS_PP_SENT"></a>PEVT_NW_SYS_PP_SENT</TD>   
   
 * <TD style="text-align:left">System pt-pt 32B packet chunks sent </TD>
      
 * <TD title="events are node-wide but counter resource is controllable by a single sw thread at a time">exclusive</TD>
 
 
 
 * <TD title=""></TD>
 * <TD title="Only suitable for counting numbers of events">e</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title=""></TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">Number of 32 byte system point...</TD> 
 * <TD style="display:none">Number of 32 byte system point to point packet chunks sent. Includes packets originating or passing through the current node. </TD>
 
 * </TR>
 * <TR>
 * <TD>359</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_NW_USER_WORLD_COL_SENT"></a>PEVT_NW_USER_WORLD_COL_SENT</TD>   
   
 * <TD style="text-align:left">User world collective 32B packet chunks sent </TD>
      
 * <TD title="events are node-wide but counter resource is controllable by a single sw thread at a time">exclusive</TD>
 
 
 
 * <TD title=""></TD>
 * <TD title="Only suitable for counting numbers of events">e</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title=""></TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">Number of 32 byte user world c...</TD> 
 * <TD style="display:none">Number of 32 byte user world collective packet chunks sent. Includes packets originating or passing through the current node. </TD>
 
 * </TR>
 * <TR>
 * <TD>360</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_NW_USER_SUBC_COL_SENT"></a>PEVT_NW_USER_SUBC_COL_SENT</TD>   
   
 * <TD style="text-align:left">User sub-comm collective 32B packet chunks sent </TD>
      
 * <TD title="events are node-wide but counter resource is controllable by a single sw thread at a time">exclusive</TD>
 
 
 
 * <TD title=""></TD>
 * <TD title="Only suitable for counting numbers of events">e</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title=""></TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">Number of 32 byte user sub-com...</TD> 
 * <TD style="display:none">Number of 32 byte user sub-communicator collective packet chunks sent. Includes packets originating or passing through the current node. </TD>
 
 * </TR>
 * <TR>
 * <TD>361</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_NW_SYS_COL_SENT"></a>PEVT_NW_SYS_COL_SENT</TD>   
   
 * <TD style="text-align:left">System collective 32B packet chunks sent </TD>
      
 * <TD title="events are node-wide but counter resource is controllable by a single sw thread at a time">exclusive</TD>
 
 
 
 * <TD title=""></TD>
 * <TD title="Only suitable for counting numbers of events">e</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title=""></TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">Number of 32 byte system colle...</TD> 
 * <TD style="display:none">Number of 32 byte system collective packet chunks sent. Includes packets originating or passing through the current node. </TD>
 
 * </TR>
 * <TR>
 * <TD>362</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_NW_COMMWORLD_COL_SENT"></a>PEVT_NW_COMMWORLD_COL_SENT</TD>   
   
 * <TD style="text-align:left">Comm-world collective 32B packet chunks sent </TD>
      
 * <TD title="events are node-wide but counter resource is controllable by a single sw thread at a time">exclusive</TD>
 
 
 
 * <TD title=""></TD>
 * <TD title="Only suitable for counting numbers of events">e</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title=""></TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">Number of 32 byte comm-world c...</TD> 
 * <TD style="display:none">Number of 32 byte comm-world collective packet chunks sent. Includes packets originating or passing through the current node. </TD>
 
 * </TR>
 * <TR>
 * <TD>363</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_NW_USER_PP_RECV"></a>PEVT_NW_USER_PP_RECV</TD>   
   
 * <TD style="text-align:left">User pt-pt packets received </TD>
      
 * <TD title="events are node-wide but counter resource is controllable by a single sw thread at a time">exclusive</TD>
 
 
 
 * <TD title=""></TD>
 * <TD title="Only suitable for counting numbers of events">e</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title=""></TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">Number of user point to point ...</TD> 
 * <TD style="display:none">Number of user point to point packets received. Includes packets originating or passing through the current node. </TD>
 
 * </TR>
 * <TR>
 * <TD>364</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_NW_USER_DYN_PP_RECV"></a>PEVT_NW_USER_DYN_PP_RECV</TD>   
   
 * <TD style="text-align:left">User dynamic pt-pt packets received </TD>
      
 * <TD title="events are node-wide but counter resource is controllable by a single sw thread at a time">exclusive</TD>
 
 
 
 * <TD title=""></TD>
 * <TD title="Only suitable for counting numbers of events">e</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title=""></TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">Number of user dynamic point t...</TD> 
 * <TD style="display:none">Number of user dynamic point to point packets received. Includes packets originating or passing through the current node. </TD>
 
 * </TR>
 * <TR>
 * <TD>365</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_NW_USER_ESC_PP_RECV"></a>PEVT_NW_USER_ESC_PP_RECV</TD>   
   
 * <TD style="text-align:left">User escape pt-pt packets received </TD>
      
 * <TD title="events are node-wide but counter resource is controllable by a single sw thread at a time">exclusive</TD>
 
 
 
 * <TD title=""></TD>
 * <TD title="Only suitable for counting numbers of events">e</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title=""></TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">Number of user escape point to...</TD> 
 * <TD style="display:none">Number of user escape point to point packets received. Includes packets originating or passing through the current node. </TD>
 
 * </TR>
 * <TR>
 * <TD>366</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_NW_USER_PRI_PP_RECV"></a>PEVT_NW_USER_PRI_PP_RECV</TD>   
   
 * <TD style="text-align:left">User priority pt-pt packets received </TD>
      
 * <TD title="events are node-wide but counter resource is controllable by a single sw thread at a time">exclusive</TD>
 
 
 
 * <TD title=""></TD>
 * <TD title="Only suitable for counting numbers of events">e</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title=""></TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">Number of user priority point ...</TD> 
 * <TD style="display:none">Number of user priority point to point packets received. Includes packets originating or passing through the current node. </TD>
 
 * </TR>
 * <TR>
 * <TD>367</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_NW_SYS_PP_RECV"></a>PEVT_NW_SYS_PP_RECV</TD>   
   
 * <TD style="text-align:left">System pt-pt packets received </TD>
      
 * <TD title="events are node-wide but counter resource is controllable by a single sw thread at a time">exclusive</TD>
 
 
 
 * <TD title=""></TD>
 * <TD title="Only suitable for counting numbers of events">e</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title=""></TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">Number of system point to poin...</TD> 
 * <TD style="display:none">Number of system point to point packets received. Includes packets originating or passing through the current node. </TD>
 
 * </TR>
 * <TR>
 * <TD>368</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_NW_USER_WORLD_COL_RECV"></a>PEVT_NW_USER_WORLD_COL_RECV</TD>   
   
 * <TD style="text-align:left">User world collective packets received </TD>
      
 * <TD title="events are node-wide but counter resource is controllable by a single sw thread at a time">exclusive</TD>
 
 
 
 * <TD title=""></TD>
 * <TD title="Only suitable for counting numbers of events">e</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title=""></TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">Number of user world collectiv...</TD> 
 * <TD style="display:none">Number of user world collective packets received. Includes packets originating or passing through the current node. </TD>
 
 * </TR>
 * <TR>
 * <TD>369</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_NW_USER_SUBC_COL_RECV"></a>PEVT_NW_USER_SUBC_COL_RECV</TD>   
   
 * <TD style="text-align:left">User sub-comm collective packets received </TD>
      
 * <TD title="events are node-wide but counter resource is controllable by a single sw thread at a time">exclusive</TD>
 
 
 
 * <TD title=""></TD>
 * <TD title="Only suitable for counting numbers of events">e</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title=""></TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">Number of user sub-communicato...</TD> 
 * <TD style="display:none">Number of user sub-communicator collective packets received. Includes packets originating or passing through the current node. </TD>
 
 * </TR>
 * <TR>
 * <TD>370</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_NW_SYS_COL_RECV"></a>PEVT_NW_SYS_COL_RECV</TD>   
   
 * <TD style="text-align:left">System collective packets received </TD>
      
 * <TD title="events are node-wide but counter resource is controllable by a single sw thread at a time">exclusive</TD>
 
 
 
 * <TD title=""></TD>
 * <TD title="Only suitable for counting numbers of events">e</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title=""></TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">Number of system collective pa...</TD> 
 * <TD style="display:none">Number of system collective packets received. Includes packets originating or passing through the current node. </TD>
 
 * </TR>
 * <TR>
 * <TD>371</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_NW_COMMWORLD_COL_RECV"></a>PEVT_NW_COMMWORLD_COL_RECV</TD>   
   
 * <TD style="text-align:left">Comm-world collective packets received </TD>
      
 * <TD title="events are node-wide but counter resource is controllable by a single sw thread at a time">exclusive</TD>
 
 
 
 * <TD title=""></TD>
 * <TD title="Only suitable for counting numbers of events">e</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title=""></TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">Number of comm-world collectiv...</TD> 
 * <TD style="display:none">Number of comm-world collective packets received. Includes packets originating or passing through the current node. </TD>
 
 * </TR>
 * <TR>
 * <TD>372</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_NW_USER_PP_RECV_FIFO"></a>PEVT_NW_USER_PP_RECV_FIFO</TD>   
   
 * <TD style="text-align:left">User pt-pt packets in receive fifo </TD>
      
 * <TD title="events are node-wide but counter resource is controllable by a single sw thread at a time">exclusive</TD>
 
 
 
 * <TD title=""></TD>
 * <TD title="Only suitable for counting numbers of events">e</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title=""></TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">Number of user point to point ...</TD> 
 * <TD style="display:none">Number of user point to point packets in receive fifo queue. Includes packets received or passing through the current node. Count is accumulated each cycle. </TD>
 
 * </TR>
 * <TR>
 * <TD>373</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_NW_USER_DYN_PP_RECV_FIFO"></a>PEVT_NW_USER_DYN_PP_RECV_FIFO</TD>   
   
 * <TD style="text-align:left">User dynamic pt-pt packets in receive fifo </TD>
      
 * <TD title="events are node-wide but counter resource is controllable by a single sw thread at a time">exclusive</TD>
 
 
 
 * <TD title=""></TD>
 * <TD title="Only suitable for counting numbers of events">e</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title=""></TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">Number of user dynamic point t...</TD> 
 * <TD style="display:none">Number of user dynamic point to point packets in receive fifo queue. Includes packets received or passing through the current node. Count is accumulated each cycle. </TD>
 
 * </TR>
 * <TR>
 * <TD>374</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_NW_USER_ESC_PP_RECV_FIFO"></a>PEVT_NW_USER_ESC_PP_RECV_FIFO</TD>   
   
 * <TD style="text-align:left">User escape pt-pt packets in receive fifo </TD>
      
 * <TD title="events are node-wide but counter resource is controllable by a single sw thread at a time">exclusive</TD>
 
 
 
 * <TD title=""></TD>
 * <TD title="Only suitable for counting numbers of events">e</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title=""></TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">Number of user escape point to...</TD> 
 * <TD style="display:none">Number of user escape point to point packets in receive fifo queue. Includes packets received or passing through the current node. Count is accumulated each cycle. </TD>
 
 * </TR>
 * <TR>
 * <TD>375</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_NW_USER_PRI_PP_RECV_FIFO"></a>PEVT_NW_USER_PRI_PP_RECV_FIFO</TD>   
   
 * <TD style="text-align:left">User priority pt-pt packets in receive fifo </TD>
      
 * <TD title="events are node-wide but counter resource is controllable by a single sw thread at a time">exclusive</TD>
 
 
 
 * <TD title=""></TD>
 * <TD title="Only suitable for counting numbers of events">e</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title=""></TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">Number of user priority point ...</TD> 
 * <TD style="display:none">Number of user priority point to point packets in receive fifo queue. Includes packets received or passing through the current node. Count is accumulated each cycle. </TD>
 
 * </TR>
 * <TR>
 * <TD>376</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_NW_SYS_PP_RECV_FIFO"></a>PEVT_NW_SYS_PP_RECV_FIFO</TD>   
   
 * <TD style="text-align:left">System pt-pt packets in receive fifo </TD>
      
 * <TD title="events are node-wide but counter resource is controllable by a single sw thread at a time">exclusive</TD>
 
 
 
 * <TD title=""></TD>
 * <TD title="Only suitable for counting numbers of events">e</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title=""></TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">Number of system point to poin...</TD> 
 * <TD style="display:none">Number of system point to point packets in receive fifo queue. Includes packets received or passing through the current node. Count is accumulated each cycle. </TD>
 
 * </TR>
 * <TR>
 * <TD>377</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_NW_USER_WORLD_COL_RECV_FIFO"></a>PEVT_NW_USER_WORLD_COL_RECV_FIFO</TD>   
   
 * <TD style="text-align:left">User world collective packets in receive fifo </TD>
      
 * <TD title="events are node-wide but counter resource is controllable by a single sw thread at a time">exclusive</TD>
 
 
 
 * <TD title=""></TD>
 * <TD title="Only suitable for counting numbers of events">e</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title=""></TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">Number of user world collectiv...</TD> 
 * <TD style="display:none">Number of user world collective packets in receive fifo queue. Includes packets received or passing through the current node. Count is accumulated each cycle. </TD>
 
 * </TR>
 * <TR>
 * <TD>378</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_NW_USER_SUBC_COL_RECV_FIFO"></a>PEVT_NW_USER_SUBC_COL_RECV_FIFO</TD>   
   
 * <TD style="text-align:left">User subcomm collective packets in receive fifo </TD>
      
 * <TD title="events are node-wide but counter resource is controllable by a single sw thread at a time">exclusive</TD>
 
 
 
 * <TD title=""></TD>
 * <TD title="Only suitable for counting numbers of events">e</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title=""></TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">Number of user sub-communicato...</TD> 
 * <TD style="display:none">Number of user sub-communicator collective packets in receive fifo queue. Includes packets received or passing through the current node. Count is accumulated each cycle. </TD>
 
 * </TR>
 * <TR>
 * <TD>379</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_NW_SYS_COL_RECV_FIFO"></a>PEVT_NW_SYS_COL_RECV_FIFO</TD>   
   
 * <TD style="text-align:left">System collective packets in receive fifo </TD>
      
 * <TD title="events are node-wide but counter resource is controllable by a single sw thread at a time">exclusive</TD>
 
 
 
 * <TD title=""></TD>
 * <TD title="Only suitable for counting numbers of events">e</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title=""></TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">Number of system collective pa...</TD> 
 * <TD style="display:none">Number of system collective packets in receive fifo queue. Includes packets received or passing through the current node. Count is accumulated each cycle. </TD>
 
 * </TR>
 * <TR>
 * <TD>380</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_NW_COMMWORLD_COL_RECV_FIFO"></a>PEVT_NW_COMMWORLD_COL_RECV_FIFO</TD>   
   
 * <TD style="text-align:left">Comm-world collective packets in receive fifo </TD>
      
 * <TD title="events are node-wide but counter resource is controllable by a single sw thread at a time">exclusive</TD>
 
 
 
 * <TD title=""></TD>
 * <TD title="Only suitable for counting numbers of events">e</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title=""></TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">Number of comm-world collectiv...</TD> 
 * <TD style="display:none">Number of comm-world collective packets in receive fifo queue. Includes packets received or passing through the current node. Count is accumulated each cycle. </TD>
 
 * </TR>
 * <TR>
 * <TD>381</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_NW_SENT"></a>PEVT_NW_SENT</TD>   
   
 * <TD style="text-align:left">Set attribute to filter 32B packet chunks sent </TD>
      
 * <TD title="events are node-wide but counter resource is controllable by a single sw thread at a time">exclusive</TD>
 
 
 
 * <TD title=""></TD>
 * <TD title="Only suitable for counting numbers of events">e</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title=""></TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">Use NW channel filter attribut...</TD> 
 * <TD style="display:none">Use NW channel filter attribute to select types of 32 byte packet chunks sent </TD>
 
 * </TR>
 * <TR>
 * <TD>382</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_NW_RECV"></a>PEVT_NW_RECV</TD>   
   
 * <TD style="text-align:left">Set attribute to filter packets received</TD>
      
 * <TD title="events are node-wide but counter resource is controllable by a single sw thread at a time">exclusive</TD>
 
 
 
 * <TD title=""></TD>
 * <TD title="Only suitable for counting numbers of events">e</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title=""></TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">Use NW channel filter attribut...</TD> 
 * <TD style="display:none">Use NW channel filter attribute to select types of packets received. </TD>
 
 * </TR>
 * <TR>
 * <TD>383</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_NW_RECV_FIFO"></a>PEVT_NW_RECV_FIFO</TD>   
   
 * <TD style="text-align:left">Set attribute to filter packets in receive fifo</TD>
      
 * <TD title="events are node-wide but counter resource is controllable by a single sw thread at a time">exclusive</TD>
 
 
 
 * <TD title=""></TD>
 * <TD title="Only suitable for counting numbers of events">e</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title=""></TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">Use NW channel filter attribut...</TD> 
 * <TD style="display:none">Use NW channel filter attribute to select types of packets counted which remain in receive fifo queue. </TD>
 
 * </TR>
 * <TR>
 * <TD>384</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_NW_CYCLES"></a>PEVT_NW_CYCLES</TD>   
   
 * <TD style="text-align:left">Network cycles since reset</TD>
      
 * <TD title="events are node-wide but counter resource is controllable by a single sw thread at a time">exclusive</TD>
 
 
 
 * <TD title=""></TD>
 * <TD title="Only suitable for counting numbers of events">e</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title=""></TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">The delta network cycles which...</TD> 
 * <TD style="display:none">The delta network cycles which have occurred since the last network counter reset. </TD>
 
 * </TR>
      
 * </TABLE> 
 * \endhtmlonly

 *
 *
 * \section cnkunit_events CNKunit Events
 * \htmlonly
 * <TABLE xmlns="http://www.w3.org/1999/xhtml" class="evt">
 * <TR bgcolor="AliceBlue">
 * <TH title="Unique integer value of BGPM event">EventId</TH>
 * <TH title="bgpm enum name for event (see bgpm_events.h)">Label</TH>
 * <TH title="Simple Description of event">Description</TH>
 * <TH title="Scope of the event (thread, core, core-shared, node-shared, node, exclusive, or sampled)">Scope</TH>
 


 * <TH title="Types of features supported by event (e.g. Overflow,LLAccess, multiplexing, etc)">Features</TH>
 * <TH title="Indicates the suitability of the fundamental hardware event">Tag</TH>
 * <TH title="Anticipated PAPI Preset event mapping to this event (see PAPI Presets for whole list)">PAPI</TH>
 * <TH title="Validation Testing level of this event">Status</TH>
 * <TH style="width:20em;" title="Detailed description of event">Detail</TH>
 * <TH style="display:none;">Hidden Detail</TH>
 * </TR><TR><TD colspan="11" style="text-align:left"><a class="anchor" id="a_compute_node"></a><B><I>Compute Node Kernel Node-Wide Events</I></B></TD></TR>
 * <TR>
 * <TD>385</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_CNKNODE_MUINT"></a>PEVT_CNKNODE_MUINT</TD>   
   
 * <TD style="text-align:left">MU Non-fatal interrupt</TD>
      
 * <TD title="events are node-wide but counting is independently controllable by each thread (that is, each thread sees a personal delta value between the start and stop of the event)">node</TD>
 
 
 
 * <TD title=""></TD>
 * <TD title="Only suitable for counting numbers of events">e</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title=""></TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">Number of Message Unit non-fat...</TD> 
 * <TD style="display:none">Number of Message Unit non-fatal interrupts </TD>
 
 * </TR>
 * <TR>
 * <TD>386</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_CNKNODE_NDINT"></a>PEVT_CNKNODE_NDINT</TD>   
   
 * <TD style="text-align:left">ND Non-fatal interrupt</TD>
      
 * <TD title="events are node-wide but counting is independently controllable by each thread (that is, each thread sees a personal delta value between the start and stop of the event)">node</TD>
 
 
 
 * <TD title=""></TD>
 * <TD title="Only suitable for counting numbers of events">e</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title=""></TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">Number of Network Device non-f...</TD> 
 * <TD style="display:none">Number of Network Device non-fatal interrupts </TD>
 
 * </TR><TR><TD colspan="11" style="text-align:left"><a class="anchor" id="a_compute_node"></a><B><I>Compute Node Kernel Process Events</I></B></TD></TR>
 * <TR>
 * <TD>387</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_CNKPROC_RSV"></a>PEVT_CNKPROC_RSV</TD>   
   
 * <TD style="text-align:left">Reserved - NO CNK Process events are defined</TD>
      
 * <TD title="events are node-wide but counting is independently controllable by each thread (that is, each thread sees a personal delta value between the start and stop of the event)">node</TD>
 
 
 
 * <TD title=""></TD>
 * <TD title="Only suitable for counting numbers of events">e</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title=""></TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)"> ...</TD> 
 * <TD style="display:none"> </TD>
 
 * </TR><TR><TD colspan="11" style="text-align:left"><a class="anchor" id="a_compute_node"></a><B><I>Compute Node Kernel Hardware Thread Events</I></B></TD></TR>
 * <TR>
 * <TD>388</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_CNKHWT_SYSCALL "></a>PEVT_CNKHWT_SYSCALL </TD>   
   
 * <TD style="text-align:left">System Calls</TD>
      
 * <TD title="events are node-wide but counting is independently controllable by each thread (that is, each thread sees a personal delta value between the start and stop of the event)">node</TD>
 
 
 
 * <TD title=""></TD>
 * <TD title="Only suitable for counting numbers of events">e</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title=""></TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">System Calls ...</TD> 
 * <TD style="display:none">System Calls </TD>
 
 * </TR>
 * <TR>
 * <TD>389</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_CNKHWT_STANDARD "></a>PEVT_CNKHWT_STANDARD </TD>   
   
 * <TD style="text-align:left">External Input Interrupts</TD>
      
 * <TD title="events are node-wide but counting is independently controllable by each thread (that is, each thread sees a personal delta value between the start and stop of the event)">node</TD>
 
 
 
 * <TD title=""></TD>
 * <TD title="Only suitable for counting numbers of events">e</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title=""></TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">External Input Interrupts ...</TD> 
 * <TD style="display:none">External Input Interrupts </TD>
 
 * </TR>
 * <TR>
 * <TD>390</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_CNKHWT_CRITICAL "></a>PEVT_CNKHWT_CRITICAL </TD>   
   
 * <TD style="text-align:left">Critical Input Interrupts</TD>
      
 * <TD title="events are node-wide but counting is independently controllable by each thread (that is, each thread sees a personal delta value between the start and stop of the event)">node</TD>
 
 
 
 * <TD title=""></TD>
 * <TD title="Only suitable for counting numbers of events">e</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title=""></TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">Critical Input Interrupts ...</TD> 
 * <TD style="display:none">Critical Input Interrupts </TD>
 
 * </TR>
 * <TR>
 * <TD>391</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_CNKHWT_DECREMENTER"></a>PEVT_CNKHWT_DECREMENTER</TD>   
   
 * <TD style="text-align:left">Decrementer Interrupts</TD>
      
 * <TD title="events are node-wide but counting is independently controllable by each thread (that is, each thread sees a personal delta value between the start and stop of the event)">node</TD>
 
 
 
 * <TD title=""></TD>
 * <TD title="Only suitable for counting numbers of events">e</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title=""></TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">Decrementer Interrupts ...</TD> 
 * <TD style="display:none">Decrementer Interrupts </TD>
 
 * </TR>
 * <TR>
 * <TD>392</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_CNKHWT_FIT "></a>PEVT_CNKHWT_FIT </TD>   
   
 * <TD style="text-align:left">Fixed Interval Timer Interrupts</TD>
      
 * <TD title="events are node-wide but counting is independently controllable by each thread (that is, each thread sees a personal delta value between the start and stop of the event)">node</TD>
 
 
 
 * <TD title=""></TD>
 * <TD title="Only suitable for counting numbers of events">e</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title=""></TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">Fixed Interval Timer Interrupt...</TD> 
 * <TD style="display:none">Fixed Interval Timer Interrupts </TD>
 
 * </TR>
 * <TR>
 * <TD>393</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_CNKHWT_WATCHDOG "></a>PEVT_CNKHWT_WATCHDOG </TD>   
   
 * <TD style="text-align:left">Watchdog Timer Interrupts</TD>
      
 * <TD title="events are node-wide but counting is independently controllable by each thread (that is, each thread sees a personal delta value between the start and stop of the event)">node</TD>
 
 
 
 * <TD title=""></TD>
 * <TD title="Only suitable for counting numbers of events">e</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title=""></TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">Watchdog Timer Interrupts ...</TD> 
 * <TD style="display:none">Watchdog Timer Interrupts </TD>
 
 * </TR>
 * <TR>
 * <TD>394</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_CNKHWT_UDEC "></a>PEVT_CNKHWT_UDEC </TD>   
   
 * <TD style="text-align:left">User Decrementer Interrupts</TD>
      
 * <TD title="events are node-wide but counting is independently controllable by each thread (that is, each thread sees a personal delta value between the start and stop of the event)">node</TD>
 
 
 
 * <TD title=""></TD>
 * <TD title="Only suitable for counting numbers of events">e</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title=""></TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">User Decrementer Interrupts ...</TD> 
 * <TD style="display:none">User Decrementer Interrupts </TD>
 
 * </TR>
 * <TR>
 * <TD>395</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_CNKHWT_PERFMON "></a>PEVT_CNKHWT_PERFMON </TD>   
   
 * <TD style="text-align:left">Performance Monitor interrupts</TD>
      
 * <TD title="events are node-wide but counting is independently controllable by each thread (that is, each thread sees a personal delta value between the start and stop of the event)">node</TD>
 
 
 
 * <TD title=""></TD>
 * <TD title="Only suitable for counting numbers of events">e</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title=""></TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">Performance Monitor interrupts...</TD> 
 * <TD style="display:none">Performance Monitor interrupts </TD>
 
 * </TR>
 * <TR>
 * <TD>396</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_CNKHWT_UNKDEBUG "></a>PEVT_CNKHWT_UNKDEBUG </TD>   
   
 * <TD style="text-align:left">Unknown/Invalid Interrupts</TD>
      
 * <TD title="events are node-wide but counting is independently controllable by each thread (that is, each thread sees a personal delta value between the start and stop of the event)">node</TD>
 
 
 
 * <TD title=""></TD>
 * <TD title="Only suitable for counting numbers of events">e</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title=""></TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">Unknown/Invalid Interrupts ...</TD> 
 * <TD style="display:none">Unknown/Invalid Interrupts </TD>
 
 * </TR>
 * <TR>
 * <TD>397</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_CNKHWT_DEBUG "></a>PEVT_CNKHWT_DEBUG </TD>   
   
 * <TD style="text-align:left">Debug Interrupts</TD>
      
 * <TD title="events are node-wide but counting is independently controllable by each thread (that is, each thread sees a personal delta value between the start and stop of the event)">node</TD>
 
 
 
 * <TD title=""></TD>
 * <TD title="Only suitable for counting numbers of events">e</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title=""></TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">Debug Interrupts ...</TD> 
 * <TD style="display:none">Debug Interrupts </TD>
 
 * </TR>
 * <TR>
 * <TD>398</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_CNKHWT_DSI "></a>PEVT_CNKHWT_DSI </TD>   
   
 * <TD style="text-align:left">Data Storage Interrupts</TD>
      
 * <TD title="events are node-wide but counting is independently controllable by each thread (that is, each thread sees a personal delta value between the start and stop of the event)">node</TD>
 
 
 
 * <TD title=""></TD>
 * <TD title="Only suitable for counting numbers of events">e</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title=""></TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">Data Storage Interrupts ...</TD> 
 * <TD style="display:none">Data Storage Interrupts </TD>
 
 * </TR>
 * <TR>
 * <TD>399</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_CNKHWT_ISI "></a>PEVT_CNKHWT_ISI </TD>   
   
 * <TD style="text-align:left">Instruction Storage Interrupts</TD>
      
 * <TD title="events are node-wide but counting is independently controllable by each thread (that is, each thread sees a personal delta value between the start and stop of the event)">node</TD>
 
 
 
 * <TD title=""></TD>
 * <TD title="Only suitable for counting numbers of events">e</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title=""></TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">Instruction Storage Interrupts...</TD> 
 * <TD style="display:none">Instruction Storage Interrupts </TD>
 
 * </TR>
 * <TR>
 * <TD>400</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_CNKHWT_ALIGNMENT "></a>PEVT_CNKHWT_ALIGNMENT </TD>   
   
 * <TD style="text-align:left">Alignment Interrupts</TD>
      
 * <TD title="events are node-wide but counting is independently controllable by each thread (that is, each thread sees a personal delta value between the start and stop of the event)">node</TD>
 
 
 
 * <TD title=""></TD>
 * <TD title="Only suitable for counting numbers of events">e</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title=""></TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">Alignment Interrupts ...</TD> 
 * <TD style="display:none">Alignment Interrupts </TD>
 
 * </TR>
 * <TR>
 * <TD>401</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_CNKHWT_PROGRAM "></a>PEVT_CNKHWT_PROGRAM </TD>   
   
 * <TD style="text-align:left">Program Interrupts</TD>
      
 * <TD title="events are node-wide but counting is independently controllable by each thread (that is, each thread sees a personal delta value between the start and stop of the event)">node</TD>
 
 
 
 * <TD title=""></TD>
 * <TD title="Only suitable for counting numbers of events">e</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title=""></TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">Program Interrupts ...</TD> 
 * <TD style="display:none">Program Interrupts </TD>
 
 * </TR>
 * <TR>
 * <TD>402</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_CNKHWT_FPU "></a>PEVT_CNKHWT_FPU </TD>   
   
 * <TD style="text-align:left">FPU Unavailable Interrupts</TD>
      
 * <TD title="events are node-wide but counting is independently controllable by each thread (that is, each thread sees a personal delta value between the start and stop of the event)">node</TD>
 
 
 
 * <TD title=""></TD>
 * <TD title="Only suitable for counting numbers of events">e</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title=""></TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">FPU Unavailable Interrupts ...</TD> 
 * <TD style="display:none">FPU Unavailable Interrupts </TD>
 
 * </TR>
 * <TR>
 * <TD>403</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_CNKHWT_APU "></a>PEVT_CNKHWT_APU </TD>   
   
 * <TD style="text-align:left">APU Unavailable Interrupts</TD>
      
 * <TD title="events are node-wide but counting is independently controllable by each thread (that is, each thread sees a personal delta value between the start and stop of the event)">node</TD>
 
 
 
 * <TD title=""></TD>
 * <TD title="Only suitable for counting numbers of events">e</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title=""></TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">APU Unavailable Interrupts ...</TD> 
 * <TD style="display:none">APU Unavailable Interrupts </TD>
 
 * </TR>
 * <TR>
 * <TD>404</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_CNKHWT_DTLB "></a>PEVT_CNKHWT_DTLB </TD>   
   
 * <TD style="text-align:left">Data TLB Interrupts</TD>
      
 * <TD title="events are node-wide but counting is independently controllable by each thread (that is, each thread sees a personal delta value between the start and stop of the event)">node</TD>
 
 
 
 * <TD title=""></TD>
 * <TD title="Only suitable for counting numbers of events">e</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title=""></TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">Data TLB Interrupts ...</TD> 
 * <TD style="display:none">Data TLB Interrupts </TD>
 
 * </TR>
 * <TR>
 * <TD>405</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_CNKHWT_ITLB "></a>PEVT_CNKHWT_ITLB </TD>   
   
 * <TD style="text-align:left">Instruction TLB Interrupts</TD>
      
 * <TD title="events are node-wide but counting is independently controllable by each thread (that is, each thread sees a personal delta value between the start and stop of the event)">node</TD>
 
 
 
 * <TD title=""></TD>
 * <TD title="Only suitable for counting numbers of events">e</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title=""></TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">Instruction TLB Interrupts ...</TD> 
 * <TD style="display:none">Instruction TLB Interrupts </TD>
 
 * </TR>
 * <TR>
 * <TD>406</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_CNKHWT_VECTOR "></a>PEVT_CNKHWT_VECTOR </TD>   
   
 * <TD style="text-align:left">Vector Unavailable Interrupts</TD>
      
 * <TD title="events are node-wide but counting is independently controllable by each thread (that is, each thread sees a personal delta value between the start and stop of the event)">node</TD>
 
 
 
 * <TD title=""></TD>
 * <TD title="Only suitable for counting numbers of events">e</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title=""></TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">Vector Unavailable Interrupts ...</TD> 
 * <TD style="display:none">Vector Unavailable Interrupts </TD>
 
 * </TR>
 * <TR>
 * <TD>407</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_CNKHWT_UNDEF "></a>PEVT_CNKHWT_UNDEF </TD>   
   
 * <TD style="text-align:left">Undefined Interrupts</TD>
      
 * <TD title="events are node-wide but counting is independently controllable by each thread (that is, each thread sees a personal delta value between the start and stop of the event)">node</TD>
 
 
 
 * <TD title=""></TD>
 * <TD title="Only suitable for counting numbers of events">e</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title=""></TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">Undefined Interrupts ...</TD> 
 * <TD style="display:none">Undefined Interrupts </TD>
 
 * </TR>
 * <TR>
 * <TD>408</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_CNKHWT_PDBI "></a>PEVT_CNKHWT_PDBI </TD>   
   
 * <TD style="text-align:left">Processor Doorbell Interrupts</TD>
      
 * <TD title="events are node-wide but counting is independently controllable by each thread (that is, each thread sees a personal delta value between the start and stop of the event)">node</TD>
 
 
 
 * <TD title=""></TD>
 * <TD title="Only suitable for counting numbers of events">e</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title=""></TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">Processor Doorbell Interrupts ...</TD> 
 * <TD style="display:none">Processor Doorbell Interrupts </TD>
 
 * </TR>
 * <TR>
 * <TD>409</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_CNKHWT_PDBCI "></a>PEVT_CNKHWT_PDBCI </TD>   
   
 * <TD style="text-align:left">Processor Doorbell Critical Ints</TD>
      
 * <TD title="events are node-wide but counting is independently controllable by each thread (that is, each thread sees a personal delta value between the start and stop of the event)">node</TD>
 
 
 
 * <TD title=""></TD>
 * <TD title="Only suitable for counting numbers of events">e</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title=""></TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">Processor Doorbell Critical In...</TD> 
 * <TD style="display:none">Processor Doorbell Critical Ints </TD>
 
 * </TR>
 * <TR>
 * <TD>410</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_CNKHWT_GDBI "></a>PEVT_CNKHWT_GDBI </TD>   
   
 * <TD style="text-align:left">Guest Doorbell Interrupts</TD>
      
 * <TD title="events are node-wide but counting is independently controllable by each thread (that is, each thread sees a personal delta value between the start and stop of the event)">node</TD>
 
 
 
 * <TD title=""></TD>
 * <TD title="Only suitable for counting numbers of events">e</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title=""></TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">Guest Doorbell Interrupts ...</TD> 
 * <TD style="display:none">Guest Doorbell Interrupts </TD>
 
 * </TR>
 * <TR>
 * <TD>411</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_CNKHWT_GDBCI "></a>PEVT_CNKHWT_GDBCI </TD>   
   
 * <TD style="text-align:left">Guest Doorbell Crit or MChk Ints</TD>
      
 * <TD title="events are node-wide but counting is independently controllable by each thread (that is, each thread sees a personal delta value between the start and stop of the event)">node</TD>
 
 
 
 * <TD title=""></TD>
 * <TD title="Only suitable for counting numbers of events">e</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title=""></TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">Guest Doorbell Crit or MChk In...</TD> 
 * <TD style="display:none">Guest Doorbell Crit or MChk Ints </TD>
 
 * </TR>
 * <TR>
 * <TD>412</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_CNKHWT_EHVSC "></a>PEVT_CNKHWT_EHVSC </TD>   
   
 * <TD style="text-align:left">Embedded Hypervisor System Calls</TD>
      
 * <TD title="events are node-wide but counting is independently controllable by each thread (that is, each thread sees a personal delta value between the start and stop of the event)">node</TD>
 
 
 
 * <TD title=""></TD>
 * <TD title="Only suitable for counting numbers of events">e</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title=""></TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">Embedded Hypervisor System Cal...</TD> 
 * <TD style="display:none">Embedded Hypervisor System Calls </TD>
 
 * </TR>
 * <TR>
 * <TD>413</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_CNKHWT_EHVPRIV "></a>PEVT_CNKHWT_EHVPRIV </TD>   
   
 * <TD style="text-align:left">Embedded Hypervisor Privileged</TD>
      
 * <TD title="events are node-wide but counting is independently controllable by each thread (that is, each thread sees a personal delta value between the start and stop of the event)">node</TD>
 
 
 
 * <TD title=""></TD>
 * <TD title="Only suitable for counting numbers of events">e</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title=""></TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">Embedded Hypervisor Privileged...</TD> 
 * <TD style="display:none">Embedded Hypervisor Privileged </TD>
 
 * </TR>
 * <TR>
 * <TD>414</TD>
 
   
 * <TD style="text-align:left"><a class="anchor" id="A_PEVT_CNKHWT_LRATE "></a>PEVT_CNKHWT_LRATE </TD>   
   
 * <TD style="text-align:left">LRAT exception</TD>
      
 * <TD title="events are node-wide but counting is independently controllable by each thread (that is, each thread sees a personal delta value between the start and stop of the event)">node</TD>
 
 
 
 * <TD title=""></TD>
 * <TD title="Only suitable for counting numbers of events">e</TD>
 
 * <TD style="text-align:left"></TD>
   
 * <TD title=""></TD>
 
    
 
 * <TD class="detail" style="text-align:left" title="click to toggle" onclick="toggleDetail(event)">LRAT exception ...</TD> 
 * <TD style="display:none">LRAT exception </TD>
 
 * </TR>
      
 * </TABLE> 
 * \endhtmlonly

 *    
 */      
