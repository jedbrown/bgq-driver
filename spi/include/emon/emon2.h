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
/* (C) Copyright IBM Corp.  2013, 2013                              */
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
#ifndef _EMON_EMON2_H_ // Prevent multiple inclusion.
#define _EMON_EMON2_H_
#include <hwi/include/bqc/en_dcr.h>
#include <spi/include/kernel/envmon.h>


//********************************************************************************
// Error definitions
//********************************************************************************

//----------------------------------------
// List of errors

#define EMON2_ERROR_LIST					                      \
    EMON2_DEFERR(EMON2_ERR_SYNC, "FPGA did not respond withn a timeout"),		\
	EMON2_DEFERR(EMON2_ERR_ENVIF, "Failed to execute FPGA envmon command"),         \
	EMON2_DEFERR(EMON2_ERR_PERS, "Failed to get Personality"),			\
	EMON2_DEFERR(EMON2_ERR_MODE, "Undefined mode parameter"),			\
	EMON2_DEFERR(EMON2_ERR_LEADER, "EMON2 function called by a non-leader node"), \
	EMON2_DEFERR(EMON2_ERR_I2C,    "FPGA failed to start i2c transaction within a timeout"), \
	EMON2_DEFERR(EMON2_ERR_I2CREAD,"FPGA failed to return i2c access result within a timeout"), \
	EMON2_DEFERR(EMON2_ERR_ADJUSTMENT,"Current adjustment value too big"), \
	EMON2_DEFERR(EMON2_ERR_LIB, "EMON2 library internal error")

//----------------------------------------
// Instantiate the list of negative error code

#define EMON2_DEFERR(SYM, MSG) SYM ## _NEG

enum {
    EMON2_ERROR_LIST,
    EMON2_ERR_LIMIT_NEG
};

#undef EMON2_DEFERR

//----------------------------------------
// Instantiate the list of error code

#define EMON2_DEFERR(SYM, MSG) SYM = ~ SYM ## _NEG

enum {
    EMON2_ERROR_LIST,
    EMON2_ERR_LIMIT = EMON2_ERR_LIMIT_NEG
};

#undef EMON2_DEFERR

//----------------------------------------
// Instantiate the list of error messages

extern const char* EMON2_error_message_table[];

#ifdef EMON2_DEFINE_GLOBALS

#define EMON2_DEFERR(SYM, MSG) MSG

const char* EMON2_error_message_table[] = {
    EMON2_ERROR_LIST
};

#undef EMON2_DEFERR

#endif // EMON2_DEFINE_GLOBALS

//----------------------------------------
// utility function to return error message

extern char* EMON2_GetErrorMessage(int);

#ifdef EMON2_DEFINE_GLOBALS

char* EMON2_GetErrorMessage(int error_code) 
{
    uint32_t idx = ~error_code;
    if (error_code >= EMON2_ERR_LIMIT_NEG) {
        return "Undefined error code";
    } else {
        return (char*)EMON2_error_message_table[idx];
    }
}

#endif // EMON2_DEFINE_GLOBALS

//********************************************************************************
// Constant tables
//********************************************************************************

extern const double EMON2_voltage_unit[];
extern const double EMON2_current_unit[];
extern const double EMON2_energy_unit[];
extern const uint32_t EMON2_domain_num[];
extern const uint32_t EMON2_default_adjustment[];
extern const uint32_t EMON2_no_adjustment[];

static const double EMON2_DCA_TICK_PERIOD = 0.01 / 9216.0;
static const double EMON2_TOTAL_ENERGY_UNIT = 8.0 * 4096.0 * 0.001 * .457763671875;

#ifdef EMON2_DEFINE_GLOBALS

const double EMON2_voltage_unit[] = {
    3.0/1024,  // dom 1
    3.0/1024,  // 2
    3.0/1024 * 2, // 3
    3.0/1024 * 2, // 4
    3.0/1024,    // 6
    3.0/1024,  // 8
    3.0/1024,  // 7
};

const double EMON2_current_unit[] = {
    10.0/1024 * 7,  // dom 1
    10.0/1024 * 3,  // 2
    10.0/1024, // 3
    10.0/1024, // 4
    10.0/1024,    // 6
    10.0/1024,  // 8
    10.0/1024,  // 7
};

const double EMON2_energy_unit[] = {
    0.001 *.457763671875 * 7*40,     
    0.001 *.457763671875 * 3*32,
    0.001 *.457763671875 * 2*16,
    0.001 *.457763671875 * 2*12,
    0.001 *.457763671875 * 1*32,
    0.001 *.457763671875 * 1*40,
    0.001 *.457763671875 * 1*32,
};

const uint32_t EMON2_domain_num[] = {
    1, 2, 3, 4, 6, 8, 7
};

const uint32_t EMON2_default_adjustment[] = {
    3700/2, // dom 1
    1400/2, // dom 2
    0, 0, 0, 0, 0 // other domains are all 0s
};
const uint32_t EMON2_no_adjustment[] = {
    0, 0, 0, 0, 0, 0, 0 
};

#endif // EMON2_DEFINE_GLOBALS

//********************************************************************************
// Data type and keywords
//********************************************************************************
#define EMON2_DOM_ALL 0x1F
#define EMON2_DOM_3   0x01
#define EMON2_DOM_4   0x02
#define EMON2_DOM_6   0x04
#define EMON2_DOM_8   0x08
#define EMON2_DOM_7   0x10

enum {
    EMON2_MODE_TOTAL,
    EMON2_MODE_EACH_DOM ,
    EMON2_MODE_EACH_VA,
    EMON2_MODE_MAX,
};

// (Debug) show sample rate
#define EMON2_OPT_SRATE 0x0001

// (Debug) show maximum interval between timer
#define EMON2_OPT_TDIFF 0x0002



typedef struct {
    uint8_t   is_leader;
    uint8_t   tx_magic;
    uint8_t   dom_set;
    uint8_t   mode;
    uint32_t  i2c_rdata_size;
    uint32_t  i2c_rdend;
    uint8_t   adjustment[7];
    int       options;
    uint32_t  rank_id;
} EMON2_context_t;

typedef struct EMON2_measurement_data_t {
    // internal use
    uint64_t api_ctime; // time consumed by GetMeasurement, in BGQ clk (i.e. 0.625ns)
    struct {
	uint64_t _time_stamp;
	uint32_t _total_ener; // total energy
	uint32_t _debug_word;
	uint16_t _magic;
	uint16_t _imon1;
	uint16_t _imon2;
	uint16_t _tdiff;
	uint8_t  _sample_count;
	uint8_t  _parity_syn;
	uint8_t  _sample_count_x256;
	struct {
	    uint64_t curr_int; // integrated current
	    uint64_t volt_int; // integrated voltage
	    uint64_t ener_int; // integrated energy
	    uint16_t curr_ts;  // timestamp of current
	    uint16_t volt_ts;  // timestamp of voltage
	    uint16_t curr_adc; // adc raw value of current
	    uint16_t volt_adc; // adc raw value of voltage
	} _dom_info[7];
    } _dca[2];
} EMON2_measurement_data_t;

typedef struct EMON2_parsed_data_t {
    double period[2];
    double total_energy;
    double domain_energy[7];
    uint16_t sample_count[2];
} EMON2_parsed_data_t;


extern EMON2_context_t EMON2_context;

#ifdef EMON2_DEFINE_GLOBALS

EMON2_context_t EMON2_context;


#endif // EMON2_DEFINE_GLOBALS

#define EMON2_DOMAIN_ISSET(D,DSET) (((D)<2) || (((DSET)>>((D)-2)) & 1))

//********************************************************************************
// Internal parser macros
//********************************************************************************

#define EMON2_PARSE_U16(X) ((rdat[X] << 8) | rdat[(X)+1])
#define EMON2_PARSE_U32(X) ((EMON2_PARSE_U16(X) << 16) | EMON2_PARSE_U16((X)+2))
#define EMON2_PARSE_U64(X) ((((uint64_t)EMON2_PARSE_U32(X)) << 32) | EMON2_PARSE_U32((X)+4))

#define EMON2_PARSE_U24(X) ((EMON2_PARSE_U16(X) << 8) | rdat[(X)+2])
#define EMON2_PARSE_U40(X) ((((uint64_t)EMON2_PARSE_U32(X)) << 8) | rdat[(X)+4])
#define EMON2_PARSE_U48(X) ((((uint64_t)EMON2_PARSE_U32(X)) << 16) | EMON2_PARSE_U16((X)+4))


//********************************************************************************
// Internal utility functions to manipulate envmon I/F command
//********************************************************************************

__INLINE__ uint64_t EMON2_set_cmd(uint64_t cmd)
{
    EMON2_context.tx_magic ++;
    return _BGQ_SET(2,14,EMON2_context.tx_magic) | _BGQ_SET(3,17, cmd);
}

#define EMON2_MAGIC_MATCHED(TXM, RXM) ((((RXM)-(TXM)) &0x3) == 1)
__INLINE__ int EMON2_wait_magic()
{
    uint64_t rx_magic;
    uint64_t start_time = GetTimeBase();

    // printf("waiting for magic = %lx\n", EMON2_context.tx_magic);

    do {
        uint64_t en5i = DCRReadUser(EN_DCR(EN5_INPUT));

        if (GetTimeBase() > start_time + (1600UL * 1000)) { // 1ms timeout

	    // printf("current en5i = %lx, en_user = %lx\n", en5i, DCRReadUser(EN_DCR(USER)));

            return EMON2_ERR_SYNC;
        }

        rx_magic = _BGQ_GET(2, 55, en5i);
    } while (!EMON2_MAGIC_MATCHED(EMON2_context.tx_magic, rx_magic));
    return 0;
}

__INLINE__ int EMON2_exec_cmd(uint64_t cmd, uint64_t parms)
{
    uint64_t en_user = EMON2_set_cmd(cmd) | parms;
    // DCRWritePriv(EN_DCR(USER), en_user);
    uint64_t rcl;
    int rc;

    // printf("sending cmd = %lx\n", en_user);

    rcl = Kernel_ExecEnvmon2Command(en_user);
    // printf("syscall returned %016lx\n", rcl);
    // if (rc < 0) return EMON2_ERR_ENVIF;

    rc = EMON2_wait_magic();
    if (rc < 0) return rc;
    return 0;
}

__INLINE__ int EMON2_exec_cmd_auth(uint64_t cmd, uint64_t parms)
{
    return EMON2_exec_cmd(cmd, parms | _BGQ_SET(14, 45, 0x3abeUL));
}

__INLINE__ int EMON2_xi2c_init(void)
{
    uint64_t en_user = DCRReadUser(EN_DCR(USER));
    EMON2_context.tx_magic = _BGQ_GET(2, 14, en_user);

    int rc = EMON2_exec_cmd(0, 0); // Do nop
    // printf("tx magic = %lx\n", tx_magic);
    return rc;
}

__INLINE__ int EMON2_write_line(uint64_t index, uint64_t data)
{
    int rc;

    //exec_cmd(3, 0); // dummy start to allow execution
    rc = EMON2_exec_cmd_auth(2,  _BGQ_SET(9, 31, 0)); // dummy read to allow exec
    if (rc < 0) return rc;
    rc = EMON2_exec_cmd_auth(1,  _BGQ_SET(9, 31, index) | _BGQ_SET(18, 63, data));
    return rc;
}

//********************************************************************************
// Envmon/xi2c command execution functions
//********************************************************************************

// static uint32_t mux_addr = 0xe6; // DELETE this !

#define EMON2_XI2C_WAITX(X) ((X)<<11)
#define EMON2_XI2C_START  0x400UL
#define EMON2_XI2C_STOP   0x200UL
#define EMON2_XI2C_ACK    0x100UL

extern int EMON2_xi2c_setup_write_buffer(uint8_t write_data[], uint32_t n_data, uint32_t dca);
extern int EMON2_xi2c_setup_read(uint32_t len);
extern int EMON2_xi2c_exec(uint32_t i2c_end, uint32_t data[], uint32_t read_end);
extern int EMON2_xi2c_write_both_dcas(uint8_t write_data[], uint32_t i2c_resdata0[], uint32_t i2c_resdata1[], uint32_t len);

#ifdef EMON2_DEFINE_GLOBALS

#define EMON2_DCA_MUX_ADDR(X) ((X)*2 + 0xe4)

int EMON2_xi2c_setup_write_buffer(uint8_t write_data[], uint32_t n_data, uint32_t dca)
{
    uint32_t idx = 0;

    uint32_t mux_addr = EMON2_DCA_MUX_ADDR(dca);
    int rc = EMON2_write_line(idx++, 0x400UL | mux_addr); // start, write to addr 0x72    
    if (rc < 0) return rc;
    rc = EMON2_write_line(idx++, 0x200UL | 0x80); // stop, open port 7  
    if (rc < 0) return rc;    
    EMON2_write_line(idx++, 0x400UL | 0x30); // start, write to addr 0x18
    if (rc < 0) return rc;
    
    rc = EMON2_write_line(idx++, 0x000UL | 0x7); // write key = 0x07
    if (rc < 0) return rc;
    uint32_t u;
    
    for(u = 0; u < n_data-1; u++) {
	rc = EMON2_write_line(idx++, 0x000UL | write_data[u]); // middle data
	if (rc < 0) return rc;
    }
    
    rc = EMON2_write_line(idx++, 0x200UL | write_data[u]); // stop
    if (rc < 0) return rc;
    rc = EMON2_write_line(idx++, 0x8000UL | 0x400UL | mux_addr); // delay, start, write to addr 0x73
    if (rc < 0) return rc;
    rc = EMON2_write_line(idx++, 0x200UL | 0x00); // stop, close port
    if (rc < 0) return rc;

    // printf("write trans len = %u\n", idx);
    return idx-1;

    // do transaction
}

int EMON2_xi2c_setup_read(uint32_t len)
{
    // EMON2_write_line(0, 0x400UL | 0xe6); // start, write to addr 0x73
    uint32_t idx = 0;  
    uint32_t dca;
    int rc;
    for(dca = 0; dca < 2; dca ++) {
	uint32_t mux_addr = EMON2_DCA_MUX_ADDR(dca);       
	rc = EMON2_write_line(idx++, 0x400UL | mux_addr); // start, write to addr 0x72
	if (rc < 0) return rc;
	rc = EMON2_write_line(idx++, 0x200UL | 0x80); // stop, open port 7
	if (rc < 0) return rc;
	/*EMON2_write_line(idx++, 0x400UL | 0x30); // start, write to addr 0x18
	  EMON2_write_line(idx++, 0x000UL | 0x60); //        specify CB index
	  EMON2_write_line(idx++, 0x200UL | 0x31); // stop,  specify CB data
	*/

	rc = EMON2_write_line(idx++, 0x400UL | 0x31); // delay, start, read from addr 0x18
	if (rc < 0) return rc;
	uint64_t u;
	// printf("header write done\n");
	for(u = 0; u < len-1; u++) {
	    rc = EMON2_write_line(idx++, 0x100UL | 0xff); // data0, ack
	    if (rc < 0) return rc;
	}
	rc = EMON2_write_line(idx++, 0x200UL | 0xff); // stop data1, noack
	if (rc < 0) return rc;
	rc = EMON2_write_line(idx++, 0x8000UL | 0x400UL | mux_addr); // delay, start, write to addr 0x73
	if (rc < 0) return rc;
	rc = EMON2_write_line(idx++, 0x200UL | 0x00); // stop, close port
	if (rc < 0) return rc;
    }
    if (idx > 254) return EMON2_ERR_LIB;
//    printf("read trans len = %u\n", idx);
    return idx-1;
}

int EMON2_xi2c_exec(uint32_t i2c_end, uint32_t data[], uint32_t read_end)
{
    uint32_t u;
    int rc = EMON2_exec_cmd_auth(3, _BGQ_SET(8, 31, i2c_end));
    if (rc < 0) return rc;
    uint32_t en0, en1, en2; 
    // uint64_t tidx = 0;

    // trec[tidx++] = GetTimeBase();
    // printf("waiting for xi2c to be accepted\n");
    uint64_t st = GetTimeBase();
    do {
        en1 = DCRReadUser(EN_DCR(EN1_INPUT));
        if (GetTimeBase() > st + (1600UL*1000*1000)) { // timeout = 1s, could be busy due to host activity
            return EMON2_ERR_I2C;
        }
    } while((en1 & 0x200UL) != 0); // wait for xi2c to start
    // trec[tidx++] = GetTimeBase();

    for(u = read_end & 1; u <= read_end; u+=2) {

        rc = EMON2_exec_cmd_auth(2,  _BGQ_SET(9, 31, 0x100 | u)); // read cmd
        if (rc < 0) return rc;
        // trec[tidx++] = GetTimeBase();
        // printf("waiting for idx %lu to be ready\n", u);
        // uint64_t prev = GetTimeBase();
        st = GetTimeBase();
        do {
            // en0 = DCRReadUser(EN_DCR(EN0_INPUT));
            en2 = DCRReadUser(EN_DCR(EN2_INPUT));
            
            if (GetTimeBase() > st + (1600UL*1000*5)) { // timeout = 1ms, this should be fast
                return EMON2_ERR_I2CREAD;
            }
            /*if (GetTimeBase() > prev + 1600UL*1000*1000) {
                printf("%lx\n", en2);
                prev = GetTimeBase();
                }*/
        } while (((en2 & 0x100UL) != 0) && ((en2 & 0xff) <= u));
        
        // trec[tidx++] = GetTimeBase();
        //      exec_cmd_auth(2, _BGQ_SET(9, 31, 0x100UL | u));

        en0 = DCRReadUser(EN_DCR(EN0_INPUT));
        en1 = DCRReadUser(EN_DCR(EN1_INPUT));

        if (u > 0) data[u-1] = ((en1 << 1) | (en0 >> 9)) & 0x1ff;
        data[u]   = en0 & 0x1ff;
	// printf("data[%u,%u] = %03x %03x\n", u-1, u, (u > 0) ? data[u-1]:0, data[u]);
        // trec[tidx++] = GetTimeBase();
    }
    return 0;
    // trec_index = tidx;
}

int EMON2_xi2c_write_both_dcas(uint8_t write_data[], uint32_t i2c_resdata0[], uint32_t i2c_resdata1[], uint32_t len)
{
    int rc;
    uint32_t dca;
    for(dca = 0; dca < 2; dca ++) {
	rc = EMON2_xi2c_setup_write_buffer(write_data, len, dca);
	if (rc < 0) return rc;
	rc = EMON2_xi2c_exec(rc, (dca == 0) ? i2c_resdata0 : i2c_resdata1, rc);
	if (rc < 0) return rc;
    }
    return 0;
}

#endif // EMON2_DEFINE_GLOBALS

//********************************************************************************
// public functions called by user code
//********************************************************************************

extern int EMON2_SetupPower(void);
extern int EMON2_SetTargets(uint8_t, int, int);
extern int EMON2_SetCurrentAdjustment(const uint32_t adjustment[]);
extern int EMON2_GetMeasurementData(EMON2_measurement_data_t* mdata);
extern int EMON2_ParseData(EMON2_measurement_data_t* mdata1, EMON2_measurement_data_t* mdata2, EMON2_parsed_data_t *pdata);
extern int EMON2_Report(EMON2_measurement_data_t* mdata1, EMON2_measurement_data_t* mdata2);

__INLINE__ int EMON2_CleanUp(void)
{

    uint8_t write_data[64];
    uint32_t i2c_resdata[64];
    uint32_t u = 0;
    int rc;

    if (!EMON2_context.is_leader) return EMON2_ERR_LEADER;
    EMON2_context.is_leader = 0; // clear flag to avoid further EMON2 calls by mistake

    // setup domain set                                    
    write_data[u++] = 0x21; // interrupt priority          
    write_data[u++] = 0x00; // lower interrupt priority
    write_data[u++] = 0x07; // timer interval;   
    write_data[u++] = 50; // slow down timer

    rc = EMON2_xi2c_write_both_dcas(write_data, i2c_resdata, i2c_resdata, u);
    return rc;
}

#ifdef EMON2_DEFINE_GLOBALS

//********************************************************************************
// set up power measurement
//********************************************************************************

int EMON2_SetupPower(void)
{
    int rc = EMON2_xi2c_init();
    if (rc < 0) return rc;

    Personality_t pers;
    Kernel_GetPersonality(&pers, sizeof(pers));

    // pers = fwext_getPersonality();
    // if (pers == NULL) return EMON2_ERR_PERS;

    int x = 0;
    x |= pers.Network_Config.Acoord;
    x |= pers.Network_Config.Bcoord;
    x |= pers.Network_Config.Ccoord;
    x |= pers.Network_Config.Dcoord;
    x |= pers.Network_Config.Ecoord;
    // printf("x = %d\n", x);
    int is_leader = ((~x) & 1); // leader = A-B coords are all even

    EMON2_context.is_leader = is_leader;
    if (is_leader) {
        int rc;
        rc = EMON2_SetTargets(EMON2_DOM_ALL, EMON2_MODE_EACH_DOM, 0);
        if (rc < 0) return rc;
        rc = EMON2_SetCurrentAdjustment(EMON2_default_adjustment);
        if (rc < 0) return rc;
//	EMON2_context.rank_id = Kernel_GetRank();
    }
    return is_leader;
}

//********************************************************************************
// SetTargets and helper functions
//********************************************************************************
__INLINE__ void EMON2_add_data_selection(uint8_t write_data[], uint32_t *p_i2c_pos, uint32_t *p_idx, uint32_t i2c_pool_index, uint32_t n_bytes)
{
    uint32_t u;
    uint32_t idx = *p_idx;
    uint32_t i2c_pos = *p_i2c_pos;

    for(u = 0; u < n_bytes; u++) {
	write_data[idx++] = 0x80 + (i2c_pos++);
	write_data[idx++] = i2c_pool_index++;
    }
    *p_i2c_pos = i2c_pos;
    *p_idx = idx;
}

void EMON2_add_options(uint8_t write_data[], uint32_t *p_i2c_pos, uint32_t *p_idx, int options)
{
    if (options & EMON2_OPT_SRATE){
	EMON2_add_data_selection(write_data, p_i2c_pos, p_idx, 0x6, 1);
    }
    if (options & EMON2_OPT_TDIFF){
	EMON2_add_data_selection(write_data, p_i2c_pos, p_idx, 0x2, 2);
    }
}

int EMON2_make_xi2c_total_mode_request(uint8_t write_data[], uint32_t* p_i2c_rdat_size, int options)
{
    uint32_t idx = 0;
    uint32_t i2c_pos = 0;

    // time stamp (32 LSBs)
    EMON2_add_data_selection(write_data, &i2c_pos, &idx, 0x0c, 4);
    // total power
    EMON2_add_data_selection(write_data, &i2c_pos, &idx, 0x14, 4);
    EMON2_add_options(write_data, &i2c_pos, &idx, options);

    *p_i2c_rdat_size = i2c_pos;
    return idx;
}

int EMON2_make_xi2c_each_dom_request(uint8_t write_data[], uint8_t dom_set, uint32_t* p_i2c_rdat_size, int options)
{
    uint32_t v;
    uint32_t idx = 0;
    uint32_t i2c_pos = 0;
    // time stamp (48 LSBs)
    EMON2_add_data_selection(write_data, &i2c_pos, &idx, 0x0a, 6);    

    for(v = 0; v < 7; v++) {
        uint32_t dom_pool_pos = 24 + v * 32;
        if (EMON2_DOMAIN_ISSET(v, dom_set)) { // measure this domain
	    EMON2_add_data_selection(write_data, &i2c_pos, &idx, dom_pool_pos+13, 3); // 24 MSBs
	    EMON2_add_data_selection(write_data, &i2c_pos, &idx, dom_pool_pos+28, 3); // 24 LSBs
        }
    }
    EMON2_add_options(write_data, &i2c_pos, &idx, options);
    *p_i2c_rdat_size = i2c_pos;
    return idx;
}

int EMON2_make_xi2c_each_va_request(uint8_t write_data[], uint8_t dom_set, uint32_t* p_i2c_rdat_size, int options)
{
    uint32_t v;
    uint32_t idx = 0;
    uint32_t i2c_pos = 0;
    // the first 8B
    EMON2_add_data_selection(write_data, &i2c_pos, &idx, 0x00, 8);

    // time stamp 32 LSBs
    EMON2_add_data_selection(write_data, &i2c_pos, &idx, 0x0c, 4);

    // total power
    EMON2_add_data_selection(write_data, &i2c_pos, &idx, 0x14, 4);

    // for each domain
    for(v = 0; v < 7; v++) {
        uint32_t dom_pool_pos = 24 + v * 32;
        if (EMON2_DOMAIN_ISSET(v, dom_set)) { // measure this domain
            
            // adc Voltage
	    EMON2_add_data_selection(write_data, &i2c_pos, &idx, dom_pool_pos+10, 2);

            // adc Current
	    EMON2_add_data_selection(write_data, &i2c_pos, &idx, dom_pool_pos+26, 2);

            // ts Voltage
	    EMON2_add_data_selection(write_data, &i2c_pos, &idx, dom_pool_pos+8,  2);

            // integrated Current
	    EMON2_add_data_selection(write_data, &i2c_pos, &idx, dom_pool_pos+21, 3);

            // energy 5B
	    EMON2_add_data_selection(write_data, &i2c_pos, &idx, dom_pool_pos+15, 1); // 8 MSBs
	    EMON2_add_data_selection(write_data, &i2c_pos, &idx, dom_pool_pos+28, 4); // 32 LSBs
        }
    }
    EMON2_add_options(write_data, &i2c_pos, &idx, options);
    *p_i2c_rdat_size = i2c_pos;

    return idx;
}

int EMON2_SetTargets(uint8_t dom_set, int mode, int options)
{
    uint8_t write_data[512];
    uint32_t i2c_resdata[512];
    uint32_t u = 0;
    int rc;

    if (!EMON2_context.is_leader) return EMON2_ERR_LEADER;

    // setup domain set
    write_data[u++] = 0x22; // control_block[0x22] = dom_set;
    write_data[u++] = dom_set;
    write_data[u++] = 0x21; // interrupt priority
    write_data[u++] = 0xff;
    write_data[u++] = 0x07; // timer interval;
    write_data[u++] = 13; 
    EMON2_context.dom_set = dom_set;
    EMON2_context.options = options;

    /*rc = EMON2_xi2c_setup_write_buffer(write_data, u);
    if (rc < 0) return rc;
    rc = EMON2_xi2c_exec(rc, i2c_resdata, rc);
    if (rc < 0) return rc;*/
    rc = EMON2_xi2c_write_both_dcas(write_data, i2c_resdata, i2c_resdata, u);
    if (rc < 0) return rc;

    uint32_t i2c_rdata_size;

    // setup mode
    switch (mode) {
        case EMON2_MODE_TOTAL: // retrieve time stamp (4) and total power (4) = 8
            u = EMON2_make_xi2c_total_mode_request(write_data,  &i2c_rdata_size, options);
            break;
        case EMON2_MODE_EACH_DOM: // retrieve time stamp (6) and total power (6) for each domain = 6*7+6 = 48B
            u = EMON2_make_xi2c_each_dom_request(write_data, dom_set, &i2c_rdata_size, options);
            break;
        case EMON2_MODE_EACH_VA:  // retrieve detailed info for each domain (mainly for debug)
            u = EMON2_make_xi2c_each_va_request(write_data, dom_set, &i2c_rdata_size, options);
            break;
        default:
            return EMON2_ERR_MODE;
    }
    /*
    rc = EMON2_xi2c_setup_write_buffer(write_data, u);
    if (rc < 0) return rc;
    rc = EMON2_xi2c_exec(rc, i2c_resdata, rc);
    if (rc < 0) return rc;*/

    rc = EMON2_xi2c_write_both_dcas(write_data, i2c_resdata, i2c_resdata, u);
    if (rc < 0) return rc;

    EMON2_context.i2c_rdata_size = i2c_rdata_size;
    EMON2_context.mode = mode;
    rc = EMON2_xi2c_setup_read(i2c_rdata_size+3);
    if (rc < 0) return rc;
    EMON2_context.i2c_rdend = rc;

    return 0;
}


//********************************************************************************
// set current adjustment
//********************************************************************************
int EMON2_SetCurrentAdjustment(const uint32_t adjustment[])
{
    uint8_t write_data[512];
    uint32_t i2c_resdata[512];
    uint32_t u = 0;
    int rc;

    if (!EMON2_context.is_leader) return EMON2_ERR_LEADER;

    // setup adjustment value
    uint32_t d;
    for(d = 0; d < 7; d++) {
        uint64_t setting = (uint64_t)((2.0 / 1000.0) * adjustment[d] / EMON2_current_unit[d]);
	// printf("domain %u  adj = %lu\n", d, setting);
        if (setting > 255) {
            return EMON2_ERR_ADJUSTMENT;
        }
        write_data[u++] = 0x71 + 2*d;
        write_data[u++] = setting;
        EMON2_context.adjustment[d] = setting;

    }
    /*
    rc = EMON2_xi2c_setup_write_buffer(write_data, u);
    if (rc < 0) return rc;
    rc = EMON2_xi2c_exec(rc, i2c_resdata, rc);
    if (rc < 0) return rc;*/
    rc = EMON2_xi2c_write_both_dcas(write_data, i2c_resdata, i2c_resdata, u);
    if (rc < 0) return rc;

    uint32_t rs = EMON2_context.i2c_rdata_size;
    if (rs != 0) {
        rc = EMON2_xi2c_setup_read(rs+3);
        if (rc < 0) return rc;
        EMON2_context.i2c_rdend = rc;
    }
    return 0;
}

//********************************************************************************
// get power measurement result
//********************************************************************************

void EMON2_parse_optional_data(uint32_t rdat[], uint32_t dca, EMON2_measurement_data_t* mdata)
{
    uint32_t idx = 0;
    if (EMON2_context.options & EMON2_OPT_SRATE) {
	mdata->_dca[dca]._sample_count_x256 = rdat[idx++];
	// printf("sc = %u\n", rdat[8]);
    }    
    if (EMON2_context.options & EMON2_OPT_TDIFF) {
	mdata->_dca[dca]._tdiff = EMON2_PARSE_U16(idx);
	idx += 2;
    }
}

void EMON2_parse_mode_total(uint32_t rdat0[], uint32_t rdat1[], EMON2_measurement_data_t* mdata)
{
    uint32_t dca;
    for(dca = 0; dca < 2; dca ++) {
	uint32_t *rdat = dca ? rdat1 : rdat0;

	uint32_t ts = EMON2_PARSE_U32(0);
	uint32_t total_en = EMON2_PARSE_U32(4);
	mdata->_dca[dca]._time_stamp = ts;
	mdata->_dca[dca]._total_ener = total_en;
	EMON2_parse_optional_data(rdat+8, dca, mdata);
    }
}

void EMON2_parse_mode_each_dom(uint32_t rdat0[], uint32_t rdat1[], EMON2_measurement_data_t* mdata)
{
    uint32_t dca;

    for(dca = 0; dca < 2; dca ++) {
	uint32_t *rdat = dca ? rdat1 : rdat0;

	uint32_t idx = 0;
	uint64_t ts = EMON2_PARSE_U48(idx);
	idx += 6;
	uint32_t d;
	mdata->_dca[dca]._time_stamp = ts;

	for(d = 0; d < 7; d++) {
	    if (EMON2_DOMAIN_ISSET(d, EMON2_context.dom_set)){ // this domain is measured
		uint64_t ene = EMON2_PARSE_U48(idx);
		mdata->_dca[dca]._dom_info[d].ener_int = ene << 8; // 8 LSBs not retrieved
		idx += 6;
	    }
	}
	EMON2_parse_optional_data(rdat+idx, dca, mdata);
    }
}

void EMON2_parse_mode_each_va(uint32_t rdat0[], uint32_t rdat1[], EMON2_measurement_data_t* mdata)
{
    uint32_t dca;

    for(dca = 0; dca < 2; dca ++) {
	uint32_t *rdat = dca ? rdat1 : rdat0;

	uint16_t imon1 = (rdat[0] << 2) | (rdat[2] >> 6);
	uint16_t imon2 = (rdat[1] << 2) | ((rdat[2] >> 4)&0x3);
	uint16_t syn = rdat[0] ^ rdat[1] ^ rdat[2];
	syn ^= (syn >> 4);
	syn &= 0xf;
	mdata->_dca[dca]._imon1 = imon1;
	mdata->_dca[dca]._imon2 = imon2;
	mdata->_dca[dca]._parity_syn = syn;
	uint32_t idx = 3;
	mdata->_dca[dca]._magic = EMON2_PARSE_U16(idx);
	mdata->_dca[dca]._tdiff = EMON2_PARSE_U16(idx+2);
	mdata->_dca[dca]._debug_word = EMON2_PARSE_U32(idx+4);
	mdata->_dca[dca]._sample_count = rdat[idx+6];
	mdata->_dca[dca]._time_stamp = EMON2_PARSE_U32(idx+8);
	mdata->_dca[dca]._total_ener = EMON2_PARSE_U32(idx+12);
	idx += 16;
	
	uint32_t d;

	for(d = 0; d < 7; d++) {
	    if (EMON2_DOMAIN_ISSET(d, EMON2_context.dom_set)){ // this domain is measured
		mdata->_dca[dca]._dom_info[d].volt_adc = EMON2_PARSE_U16(idx+0);
		mdata->_dca[dca]._dom_info[d].curr_adc = EMON2_PARSE_U16(idx+2);
		mdata->_dca[dca]._dom_info[d].volt_ts  = EMON2_PARSE_U16(idx+4);
		mdata->_dca[dca]._dom_info[d].curr_int = EMON2_PARSE_U24(idx+6);
		mdata->_dca[dca]._dom_info[d].ener_int = EMON2_PARSE_U40(idx+9);
		idx += 14;
	    }
	}
	EMON2_parse_optional_data(rdat+idx, dca, mdata);
    }
}

int EMON2_GetMeasurementData(EMON2_measurement_data_t* mdata)
{
    int rc;
    uint32_t resdata[512];
    uint64_t sttime = GetTimeBase();

    if (EMON2_context.is_leader == 0) 
        return EMON2_ERR_LEADER;
    if (EMON2_context.mode >= EMON2_MODE_MAX)
        return EMON2_ERR_MODE;

    uint32_t rend = EMON2_context.i2c_rdend;
    uint32_t rsiz = EMON2_context.i2c_rdata_size;

    rc = EMON2_xi2c_exec(rend, resdata, rend-2);

    // printf("%x --\n", resdata[rend-2]);
    if (rc) return rc;

    uint32_t *dca1_start = resdata + rsiz + 8;
    resdata[rend-2] &= 0xff;
    resdata[rsiz+8-3] &= 0xff;

    switch(EMON2_context.mode){
        case EMON2_MODE_TOTAL:
            EMON2_parse_mode_total(resdata+6, dca1_start + 6, mdata);
            break;
        case EMON2_MODE_EACH_DOM:
            EMON2_parse_mode_each_dom(resdata+6, dca1_start + 6, mdata);
            break;
        case EMON2_MODE_EACH_VA:
            EMON2_parse_mode_each_va(resdata+3, dca1_start + 6, mdata);
            break;
    }
    mdata->api_ctime = GetTimeBase() - sttime;
    return 0;
}

int EMON2_ParseData(EMON2_measurement_data_t* mdata1, EMON2_measurement_data_t* mdata2, EMON2_parsed_data_t *pdata)
{
    if (EMON2_context.mode >= EMON2_MODE_MAX)
        return EMON2_ERR_MODE;
    
    uint32_t v, dca;
    uint64_t ts_mask = ~0UL;

    pdata->total_energy = 0;
    for(v = 0; v < 7; v++) pdata->domain_energy[v] = 0;

    switch(EMON2_context.mode) {
        case EMON2_MODE_TOTAL:

            ts_mask = (1UL << 32) -1; // 32 bit
	    for(dca = 0; dca < 2; dca++) {
		double e = mdata2->_dca[dca]._total_ener - mdata1->_dca[dca]._total_ener; 
		e *=  (EMON2_TOTAL_ENERGY_UNIT * EMON2_DCA_TICK_PERIOD);
		pdata->total_energy += e;
	    }
            break;

        case EMON2_MODE_EACH_DOM:

            ts_mask = (1UL << 48) - 1; // 48 bits
            for(v = 0; v < 7; v++) {
                if (EMON2_DOMAIN_ISSET(v, EMON2_context.dom_set)) {
		    for(dca = 0; dca < 2; dca ++) {
			// printf("ediff = %lu\n", mdata2->_dom_info[v].ener_int - mdata1->_dom_info[v].ener_int);
			double e = (mdata2->_dca[dca]._dom_info[v].ener_int - mdata1->_dca[dca]._dom_info[v].ener_int) & ((1UL << 56) - 1);
			e *= EMON2_energy_unit[v] * EMON2_DCA_TICK_PERIOD;
			pdata->total_energy += e;
			pdata->domain_energy[v] += e;
		    }
		}
	    }
            break;

        case EMON2_MODE_EACH_VA:
	    for(dca = 0; dca < 2; dca++) {
		double e = mdata2->_dca[dca]._total_ener - mdata1->_dca[dca]._total_ener; 
		e *=  (EMON2_TOTAL_ENERGY_UNIT * EMON2_DCA_TICK_PERIOD);
		pdata->total_energy += e;
	    }

            for(v = 0; v < 7; v++) {
                if (EMON2_DOMAIN_ISSET(v, EMON2_context.dom_set)) {
		    for(dca = 0; dca < 2; dca ++) {
			double e = (mdata2->_dca[dca]._dom_info[v].ener_int - mdata1->_dca[dca]._dom_info[v].ener_int) & ((1UL<<40) - 1);
			e *= EMON2_energy_unit[v] * EMON2_DCA_TICK_PERIOD;
			pdata->domain_energy[v] = e;
		    }
		}
	    }
            break;
    }
    for(v = 0; v < 2; v++) {
	double p = (mdata2->_dca[v]._time_stamp - mdata1->_dca[v]._time_stamp) & ts_mask;
	pdata->period[v] = p * EMON2_DCA_TICK_PERIOD;
    }
    if (EMON2_context.options & EMON2_OPT_SRATE){
	for(v = 0; v < 2; v++) {
	    pdata->sample_count[v] = (mdata2->_dca[v]._sample_count_x256 - mdata1->_dca[v]._sample_count_x256) * 256;
	}
    }
    return 0;
}

int EMON2_Report(EMON2_measurement_data_t* mdata1, EMON2_measurement_data_t* mdata2)
{
    if (EMON2_context.mode >= EMON2_MODE_MAX)
        return EMON2_ERR_MODE;
    // printf("parsing data\n");
    EMON2_parsed_data_t pdata;
    memset(&pdata, 0, sizeof(pdata));
    EMON2_ParseData(mdata1, mdata2, &pdata);
    uint32_t d;
    // printf("parsing done\n");
    // double dca1_ts_diff = (int64_t)(pdata.period[1] - pdata.period[0]);

    printf("EMON2_Report(mode %u) : total_energy = %lu [mJ]  in  %lu (%ld) us  (%lu mW)\n",
           EMON2_context.mode, 
           (uint64_t)(1000*pdata.total_energy),
           (uint64_t)(1000000*pdata.period[0]),
           (uint64_t)(1000000*pdata.period[1]),
	   // (int64_t)(1000000*dca1_ts_diff),
           (uint64_t)(1000*pdata.total_energy/pdata.period[0]));

    switch(EMON2_context.mode) {
        case EMON2_MODE_TOTAL:
            break;

        case EMON2_MODE_EACH_DOM:
        case EMON2_MODE_EACH_VA:
            for(d = 0; d < 7; d++) {
                if (EMON2_DOMAIN_ISSET(d, EMON2_context.dom_set)) {
                    printf("   dom%u  total_energy = %lu [mJ] (%lu mW)\n", 
			   EMON2_domain_num[d],
                           (uint64_t)(1000*pdata.domain_energy[d]),
                           (uint64_t)(1000*pdata.domain_energy[d]/pdata.period[0]));
                }
            }
    }

    if (EMON2_context.options & EMON2_OPT_SRATE){
	printf("   SAMPLE count : DCA0 = %u (%u / sec), DCA1 = %u (%u / sec)\n",
	       pdata.sample_count[0], 	(uint32_t)(pdata.sample_count[0] / pdata.period[0]),
	       pdata.sample_count[1], 	(uint32_t)(pdata.sample_count[1] / pdata.period[0])
	    );
    }
    return 0;
}
#endif // EMON2_DEFINE_GLOBALS


#endif // _EMON_EMON2_H_
