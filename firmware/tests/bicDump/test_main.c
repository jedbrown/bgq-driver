/* begin_generated_IBM_copyright_prolog                             */
/*                                                                  */
/* This is an automatically generated copyright prolog.             */
/* After initializing,  DO NOT MODIFY OR MOVE                       */
/*  --------------------------------------------------------------- */
/*                                                                  */
/* Licensed Materials - Property of IBM                             */
/* Blue Gene/Q                                                      */
/* (C) Copyright IBM Corp.  2010, 2012                              */
/* US Government Users Restricted Rights - Use, duplication or      */
/*   disclosure restricted by GSA ADP Schedule Contract with IBM    */
/*   Corp.                                                          */
/*                                                                  */
/* This software is available to you under the Eclipse Public       */
/* License (EPL).                                                   */
/*                                                                  */
/*  --------------------------------------------------------------- */
/*                                                                  */
/* end_generated_IBM_copyright_prolog                               */


#include <firmware/include/Firmware_Interface.h>
#include <firmware/include/fwext/fwext.h>
#include <hwi/include/bqc/A2_inlines.h>
#include <hwi/include/bqc/testint_inlines.h>
#include <hwi/include/bqc/gea_dcr.h>
#include <hwi/include/bqc/devbus_dcr.h>
#include <stdlib.h>
#include <string.h>


char _on_bits_buffer[256];

unsigned _strlen(char* str) {
    unsigned n = 0;
    while ( *(str++) != 0 )
	n++;

    return n;
}

char* _strcat( char* dest, char* src ) {
    char* p1 = dest + _strlen(dest);
    char* p2 = src;

    while (*p2 != 0) 
	*(p1++) = *(p2++);

    return dest;
}

char* onbits( uint64_t value ) {
    
    char littleBuff[16];

    _on_bits_buffer[0] = 0;

    unsigned i;

    for ( i = 0; i < 64; i++ ) {
	uint64_t mask = 1ull << (63-i);
	if ( ( value & mask ) != 0 ) {
	    sprintf( littleBuff, "%d,", i);
	    _strcat(_on_bits_buffer,littleBuff);
	}
    }

    
    return _on_bits_buffer;
}

void dumpGEAmap( void ) {

    uint64_t* map = (uint64_t*)&(GEA_DCR_PRIV_PTR->gea_interrupt_map0);
    int i;

    printf("  GEA Map:\n");
    for ( i = 0; i <= 9; i++ ) {
	printf("    %c %d) %016lX\n", (map[i] != 0) ? '*' : ' ', i, map[i]);
    }
}

void dumpGEAStatus(void) {
    int i;
    uint64_t* status;
    printf("  GEA Status:\n");
    printf("    raw   %016lX-%016lX-%016lX\n", GEA_DCR_PRIV_PTR->gea_interrupt_status0, GEA_DCR_PRIV_PTR->gea_interrupt_status1, GEA_DCR_PRIV_PTR->gea_interrupt_status2);

    status = (uint64_t*)&(GEA_DCR_PRIV_PTR->gea_mapped_interrupt_status0_0);

    for ( i = 0; i < 16; i++, status += 3) {
	if ( (status[0] !=0) || (status[1] != 0) || (status[2] != 0) ) {
	    printf("    map %d : %016lX-%016lX-%016lX\n", i, status[0], status[1], status[2]);
	}
    }

    printf("    state         : %016lX\n", GEA_DCR_PRIV_PTR->gea_interrupt_state__state );
    printf("    machine check : %016lX\n", GEA_DCR_PRIV_PTR->gea_interrupt_state__machine_check );
    printf("    critical      : %016lX\n", GEA_DCR_PRIV_PTR->gea_interrupt_state__critical );
    printf("    non-critical  : %016lX\n", GEA_DCR_PRIV_PTR->gea_interrupt_state__noncritical );
    printf("    control-low   : %016lX\n", GEA_DCR_PRIV_PTR->gea_interrupt_state_control_low );
    printf("    control-high  : %016lX /* %s */\n", GEA_DCR_PRIV_PTR->gea_interrupt_state_control_high, onbits(GEA_DCR_PRIV_PTR->gea_interrupt_state_control_high));


}

void dumpGEA( void ) {
    printf("GEA settings\n");
    dumpGEAmap();
    dumpGEAStatus();
}

void dumpDevbus(void) {
    printf("DevBus\n");
    printf("    state         : %016lX /* %s */\n", DEVBUS_DCR_PRIV_PTR->db_interrupt_state__state, onbits(DEVBUS_DCR_PRIV_PTR->db_interrupt_state__state ));
    printf("    machine check : %016lX /* %s */\n", DEVBUS_DCR_PRIV_PTR->db_interrupt_state__machine_check, onbits(DEVBUS_DCR_PRIV_PTR->db_interrupt_state__machine_check));
    printf("    critical      : %016lX /* %s */\n", DEVBUS_DCR_PRIV_PTR->db_interrupt_state__critical, onbits(DEVBUS_DCR_PRIV_PTR->db_interrupt_state__critical));
    printf("    non-critical  : %016lX /* %s */\n", DEVBUS_DCR_PRIV_PTR->db_interrupt_state__noncritical, onbits(DEVBUS_DCR_PRIV_PTR->db_interrupt_state__noncritical));
    printf("    control-low   : %016lX /* %s */\n", DEVBUS_DCR_PRIV_PTR->db_interrupt_state_control_low, onbits(DEVBUS_DCR_PRIV_PTR->db_interrupt_state_control_low));
    printf("    control-high  : %016lX /* %s */\n", DEVBUS_DCR_PRIV_PTR->db_interrupt_state_control_high, onbits(DEVBUS_DCR_PRIV_PTR->db_interrupt_state_control_high));


}

int test_main( void ) {

    if (ProcessorID() == 0) {
	dumpGEA();
	dumpDevbus();
    }
  exit(0);
}



