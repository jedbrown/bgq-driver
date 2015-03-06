/* begin_generated_IBM_copyright_prolog                             */
/*                                                                  */
/* This is an automatically generated copyright prolog.             */
/* After initializing,  DO NOT MODIFY OR MOVE                       */
/*  --------------------------------------------------------------- */
/* Licensed Materials - Property of IBM                             */
/* Blue Gene/Q 5765-PER 5765-PRP                                    */
/*                                                                  */
/* (C) Copyright IBM Corp. 2011, 2012 All Rights Reserved           */
/* US Government Users Restricted Rights -                          */
/* Use, duplication, or disclosure restricted                       */
/* by GSA ADP Schedule Contract with IBM Corp.                      */
/*                                                                  */
/*  --------------------------------------------------------------- */
/*                                                                  */
/* end_generated_IBM_copyright_prolog                               */
/**
 * \file api/extension/c/collsel/AdvisorTable.h
 */
#ifndef __api_extension_c_collsel_AdvisorTable_h__
#define __api_extension_c_collsel_AdvisorTable_h__

#include "CollselExtension.h"
#include "Benchmark.h"
#include "CollselData.h"
#include "CollselXMLWriter.h"
#include "CollselXMLParser.h"
#include "algorithms/geometry/Geometry.h"
#include <sys/stat.h>
#include <string>
#include <dirent.h>
#include <stdio.h>

using namespace std;

#define DOUBLE_DIG (9.999999999999999e99)
#define COLLSEL_MAX_ALGO 40 // Maximum number of algorithms per collective
#define COLLSEL_MAX_NUM_MSG_SZ 64 // Maximum number of message sizes
#define ROOT 0
typedef enum {
  FILE_CHECK = 0,
  FILE_NOT_EXIST,
  FILE_BAD,
  FILE_OK
} file_state_t;

// check statement
#define CRC(STATE, MSG) { \
  if (STATE) { \
    printf("ERR: failed to %s\n", (MSG)); \
    exit(-1); \
  } \
} 

typedef struct {
  size_t    algo;
  string    algo_name;
  double    results[3];
} algo_record_t;

const char *TMP_OUTPUT_DIR=".collsel";
const char *TMP_OUTPUT_PREFIX="/collsel";
const size_t SSIZE_T=sizeof(size_t);
const size_t SDOUBLE=sizeof(double);
extern int _g_verify;
extern int _g_verbose;
int _g_checkpoint;

namespace PAMI{

  class AdvisorTable
  {
    public:
      AdvisorTable(Advisor &advisor);
      ~AdvisorTable();
      pami_result_t generate(char             *filename,
                             advisor_params_t *params,
                             int               mode);
      pami_result_t load(char* filename);

      pami_result_t unload();

      pami_result_t query(pami_geometry_t geometry,
                          fast_query_t   *query_handle);

      pami_result_t advise(fast_query_t     *query_handle,
                           pami_xfer_type_t  xfer_type,
                           pami_xfer_t        *xfer,
                           advisor_algorithm_t optimal_algorithms[],
                           size_t              max_algorithms);
 
    private:
      pami_result_t  check_params(advisor_params_t *params, char *filename, int mode);
      void init(advisor_params_t *params);
      void sort_algo_list(sorted_list* algo_list, AlgoList currAlgoList, int length);

      Advisor &_advisor;
      advisor_params_t _params;
      pami_task_t      _task_id;
      size_t           _ntasks;
      bool             _free_geometry_sz;
      bool             _free_message_sz;
      CollselData      _collsel_data;
  };


  inline AdvisorTable::AdvisorTable(Advisor &advisor):
    _advisor(advisor),
    _task_id(0),
    _ntasks(0),
    _free_geometry_sz(false),
    _free_message_sz(false)
  {
  }

  inline AdvisorTable::~AdvisorTable()
  {
    if(_free_geometry_sz)
      free(_params.geometry_sizes);
    if(_free_message_sz)
      free(_params.message_sizes);
  }

  int cmp_by_time(const void *a, const void *b)
  {
    sorted_list *ia = (sorted_list *)a;
    sorted_list *ib = (sorted_list *)b;
    return (int)(100.f*ia->times[2] - 100.f*ib->times[2]);
	/* float comparison: returns negative if b > a 
	and positive if a > b. We multiplied result by 100.0
	to preserve decimal fraction */
  }

  inline void AdvisorTable::sort_algo_list(sorted_list* algo_list, AlgoList currAlgoList, int length)
  {
     int i;
     qsort(algo_list, length, sizeof(sorted_list), cmp_by_time);

     char * tmpCurrAlgoList = (char *)currAlgoList;
     int    lenChars        = 0;
     for(i = 0; i < length-1; i++)
     {
       lenChars = sprintf(tmpCurrAlgoList, "%d,",algo_list[i].algo_id);
       tmpCurrAlgoList += lenChars;
     }
     lenChars = sprintf(tmpCurrAlgoList, "%d",algo_list[i].algo_id);
  }

  inline pami_result_t  AdvisorTable::check_params(advisor_params_t *params, char *filename, int mode)
  {
    // Check only on task 0
    if(!_task_id)
    {
      if(!filename)
      {
        fprintf(stderr, "Invalid input file name: %s\n", filename);
        return PAMI_INVAL;
      }
      else
      {
        /* Verify if the output file can be created */
        FILE *of = fopen(filename, "w");
        if (of == NULL)
        {
          fprintf(stderr, "Error creating output file: %s\n", filename);
          return PAMI_ERROR;
        }
        else
        {
          fclose(of);
          remove(filename);
        }
      }
    }

    if(!params || mode != 1) // For now we support only mode = 1
    {
      if(!_task_id)
        fprintf(stderr, "Invalid input parameters\n");
      return PAMI_INVAL;
    }

    for (size_t i = 0; i < params->num_geometry_sizes; ++i)
    {
      if(params->geometry_sizes[i] == 0 || params->geometry_sizes[i] > _ntasks)
      {
        if(!_task_id)
          fprintf(stderr, "Invalid input geometry size: %zu\n", params->geometry_sizes[i]);
        return PAMI_INVAL;
      }
    }
    return PAMI_SUCCESS;
  }

  inline void AdvisorTable::init(advisor_params_t *params)
  {
    _params = *params;

    // User did not provide geometry sizes, use power of two geometries and adjacent values ( 2^k + 1, 2^k, 2^k - 1) 
    if(!_params.num_geometry_sizes)
    {
      // Geometry list = 2,3,4,5,7,8,9,15,16,17,31,32,33..... 
      if(_ntasks < 7)
      {
        _params.num_geometry_sizes = _ntasks - 1;
        _params.geometry_sizes = (size_t *) malloc(_params.num_geometry_sizes * SSIZE_T);
        for(unsigned i = 0; i < _params.num_geometry_sizes; ++i)
          _params.geometry_sizes[i] = _ntasks - i;
      }
      else
      {
        bool is_pow2 = !(_ntasks & (_ntasks - 1));
        // Find largest power of 2 smaller than or equal to num tasks
        unsigned i = ipow2(ilog2(_ntasks));
        _params.num_geometry_sizes = ((ilog2(_ntasks) - 1) * 3);
        if(!is_pow2)
        {
          _params.num_geometry_sizes++;
          if(_ntasks != i+1)
            _params.num_geometry_sizes++;
        }
        _params.geometry_sizes = (size_t *) malloc(_params.num_geometry_sizes * SSIZE_T);

        unsigned j = 0;
        if(!is_pow2)
        {
          _params.geometry_sizes[j++] = _ntasks;
          // If num tasks is 1 greater than power of 2, avoid duplicate 
          if(_ntasks != i+1)
            _params.geometry_sizes[j++] = i + 1;
        }
        _params.geometry_sizes[j++] = i;
        _params.geometry_sizes[j++] = i - 1;

        i >>=1;
        unsigned k = i + 1;
        for(; k > 2 && j < (_params.num_geometry_sizes - 1); ++j)
        {
          _params.geometry_sizes[j] = k--;
          if(k-i == (unsigned) -2)
          {
            i >>=1;
            k = i + 1;
          }
        }
        _params.geometry_sizes[_params.num_geometry_sizes - 1] = 2;
      }
      _free_geometry_sz = true;
    }

    if(!_params.num_message_sizes)
    {
      // 1 byte -> 4194304 bytes
      _params.num_message_sizes = 23;
      _params.message_sizes = (size_t *) malloc(_params.num_message_sizes*SSIZE_T);

      for (unsigned i=1, j=0; i < (1U<<23); i <<=1)
        _params.message_sizes[j++] = i;

      _free_message_sz = true;
    }
    _g_verify         = _params.verify;
    _g_verbose        = _params.verbose;
    _g_checkpoint     = _params.checkpoint;
  }

  // check if the directory for tmp output files exists
  // if not, create the directory
  inline void dir_check() 
  {
    struct stat st;
    if (stat(TMP_OUTPUT_DIR, &st) != 0) {
      if (mkdir(TMP_OUTPUT_DIR, S_IRWXU|S_IRGRP|S_IXGRP) != 0) {
        printf("Failed to create directory %s\n", TMP_OUTPUT_DIR);
        exit(-1);
      } else {
        if (_g_verbose)
          printf("Created directory %s\n", TMP_OUTPUT_DIR);
      }
    } else {
      if (_g_verbose) 
        printf("Directory %s already exists\n", TMP_OUTPUT_DIR);
    }
    return;
  }

  inline void dir_clean()
  {
    DIR *dir;
    struct dirent *entry;
    char path[PATH_MAX];

    dir = opendir(TMP_OUTPUT_DIR);
    if (dir == NULL) {
      printf("Error opening directory %s\n", TMP_OUTPUT_DIR);
      return;
    }

    while ((entry = readdir(dir)) != NULL) {
      if (strcmp(entry->d_name, ".") && strcmp(entry->d_name, "..")) {
        snprintf(path, PATH_MAX, "%s/%s", TMP_OUTPUT_DIR, entry->d_name);
        if (remove(path) != 0) {
          printf("Error deleting file %s\n", path);
        } else {
          if (_g_verbose)
            printf("File %s has been deleted\n", path);
        }
      }
    }
    closedir(dir);

    if (_g_verbose)
      printf("Directory %s has been cleaned\n", TMP_OUTPUT_DIR);

    if (rmdir(TMP_OUTPUT_DIR) == 0) {
      if (_g_verbose)
        printf("Directory %s has been removed\n", TMP_OUTPUT_DIR);
    } else {
      printf("Failed to remove directory %s\n", TMP_OUTPUT_DIR);
    }


  }

  // make sure the file has the right size
  inline file_state_t file_check(char* fname) 
  {
    FILE* file;
    file_state_t res = FILE_BAD;
    size_t readfsize = 0;
    size_t fsize = 0;

    file = fopen(fname, "r");
    if (file != NULL) {
      if (fseek(file, 0, SEEK_END) == 0) {
        fsize = ftell(file);
        if (fseek(file, fsize - SSIZE_T, SEEK_SET) == 0) {
          if (fread(&readfsize, 1, SSIZE_T, file) == SSIZE_T) {
            if (fsize == readfsize) {
              res = FILE_OK;
            }
          }
        }
      }
      fclose(file);
    } else {
      res = FILE_NOT_EXIST;
    }

    return res;
  }

  inline void file_write(char* fname, algo_record_t* records, size_t record_num) 
  {
    FILE*   wFile;
    size_t  wlen, i, j, algo_name_len, fsize=0;

    // when the tmp output file doesn't exist
    wFile = fopen(fname, "w");
    CRC(wFile == NULL, "create tmp output file");

    wlen = fwrite(&record_num, 1, SSIZE_T, wFile);
    CRC(wlen != SSIZE_T, "write algo_name_len");

    for (i = 0; i < record_num; i ++) {
      // write the output of the current algorithm into the tmp output file
      wlen = fwrite(&(records[i].algo), 1, SSIZE_T, wFile);
      CRC(wlen != SSIZE_T, "write algo");

      algo_name_len = records[i].algo_name.size();
      wlen = fwrite(&algo_name_len, 1, SSIZE_T, wFile);
      CRC(wlen != SSIZE_T, "write algo_name_len");

      wlen = fwrite(records[i].algo_name.c_str(), 1, algo_name_len, wFile);
      CRC(wlen != algo_name_len, "write algo_name");

      for (j = 0; j < 3; j ++) {
        wlen = fwrite(&(records[i].results[j]), 1, SDOUBLE, wFile);
        CRC(wlen != SDOUBLE, "write results");
      }

      fsize += SSIZE_T*2 + SDOUBLE*3 + algo_name_len;
    }

    fsize += SSIZE_T*2;

    // add file size at the end of the file 
    wlen = fwrite(&fsize, 1, SSIZE_T, wFile);
    CRC(wlen != SSIZE_T, "write file size");

    fclose(wFile);
  }

  inline pami_result_t AdvisorTable::generate(char             *filename,
                                              advisor_params_t *params,
                                              int               mode)
  {
    pami_result_t result;
    pami_configuration_t config;
    pami_client_t client = _advisor._client;
    pami_context_t *contexts = _advisor._contexts;

    config.name = PAMI_CLIENT_TASK_ID;
    result = PAMI_Client_query(client, &config, 1);
    PAMI_assertf(result == PAMI_SUCCESS, "Failed to query client task id");
    _task_id = config.value.intval;

    config.name = PAMI_CLIENT_NUM_TASKS;
    result = PAMI_Client_query(client, &config, 1);
    PAMI_assertf(result == PAMI_SUCCESS, "Failed to query number of tasks");
    _ntasks = config.value.intval;

    pami_geometry_t world_geometry;
    result = PAMI_Geometry_world(client, &world_geometry);
    PAMI_assertf(result == PAMI_SUCCESS, "Failed to get geometry world");

    // Setup bcast on world geometry
    pami_xfer_t bcast;
    char buf;
    pami_endpoint_t root_ep;
    pami_task_t root_task = (pami_task_t)ROOT;
    PAMI_Endpoint_create(client, root_task, 0, &root_ep);
    volatile unsigned bcast_poll_flag = 0;
    query_geometry_algorithm_aw(client, contexts[0], world_geometry, PAMI_XFER_BROADCAST, &bcast);
    bcast.cb_done                      = cb_done;
    bcast.cookie                       = (void*)&bcast_poll_flag;
    bcast.cmd.xfer_broadcast.buf       = &buf;
    bcast.cmd.xfer_broadcast.type      = PAMI_TYPE_BYTE;
    bcast.cmd.xfer_broadcast.typecount = 1;
    bcast.cmd.xfer_broadcast.root      = root_ep;

    result = check_params(params, filename, mode);
    // Send result of param check to all other tasks (only task 0 checks o/p file)
    if(!_task_id)
      buf = (char) result;

    blocking_coll(contexts[0], &bcast, &bcast_poll_flag);

    if(buf != PAMI_SUCCESS)
      return (pami_result_t) buf;

    init(params);
    init_tables();
    init_cutoff_tables();

    /* Setup operation and datatype tables*/
    //gValidTable = alloc2DContig(op_count, dt_count);
    //setup_op_dt(gValidTable,sDt,sOp);

    /* XML and CollselData related */
    GeometryShapeMap     &ppn_map       = _collsel_data.get_datastore();
    AlgoMap              *algo_map      = _collsel_data.get_algorithm_map();
    /* Used to get correct ids for algorithm based on their names */
    AlgoNameToIdMap      *algo_name_map = _collsel_data.get_algorithm_name_map();
    AlgoMap              *tmp_algo_map  = NULL;
    GeometrySizeMap      *geo_map       = NULL;
    CollectivesMap       *coll_map      = NULL;
    MessageSizeMap       *msg_map       = NULL;
    MessageSizeMap        garbCollList;
    size_t                garbCollIndx  = 0;
    unsigned              algo_ids[PAMI_XFER_COUNT] = {0};
    AlgoList              currAlgoList  = NULL;
    AlgoList              tempAlgoList  = (AlgoList)malloc(COLLSEL_MAX_ALGO*2);
    size_t byte_thresh = (1*1024*1024);

    // tmp output file name
    char   tmp_output_fname[128];
    FILE  *tFile = NULL;

    if(mode == 0){
      //read in the file
    }
    else if(mode == 1)
    {
      size_t psize, gsize, csize, msize, algo, i, j, flen;
      size_t act_msg_size[COLLSEL_MAX_NUM_MSG_SZ];
      int geometry_id = 1;
      // Setup barrier on world geometry
      pami_xfer_t barrier;
      volatile unsigned barrier_poll_flag = 0;
      query_geometry_algorithm_aw(client, contexts[0], world_geometry, PAMI_XFER_BARRIER, &barrier);
      barrier.cb_done   = cb_done;
      barrier.cookie    = (void*) & barrier_poll_flag;

      buf = FILE_CHECK;

      // Allocate buffers to store algorithms and metadata
      pami_algorithm_t  col_algo[COLLSEL_MAX_ALGO];
      pami_metadata_t   col_md[COLLSEL_MAX_ALGO];
      pami_algorithm_t  q_col_algo[COLLSEL_MAX_ALGO];
      pami_metadata_t   q_col_md[COLLSEL_MAX_ALGO];
      sorted_list       algo_list[COLLSEL_MAX_ALGO];

      if(_g_checkpoint && !_task_id) {
        dir_check();
      }

      algo_record_t *algo_record = NULL;

      // loop on each setting of procs_per_node
      for (psize = 0; psize < _params.num_procs_per_node; psize ++) {

        /* Inserting geo_map in ppn_map with key _params.procs_per_node[psize] */
        geo_map         = &ppn_map[_params.procs_per_node[psize]];
        // Loop on geometry size
        for(gsize = 0; gsize < _params.num_geometry_sizes; gsize ++) {
          PAMI_assertf(_params.geometry_sizes[gsize] <= _ntasks, 
              "Geometry size (%zu) exceeds the number of tasks (%zu)", 
              _params.geometry_sizes[gsize], _ntasks);

          size_t geo_size =  _params.geometry_sizes[gsize];
          /* Inserting coll_map in geo_map with key geo_size */
          coll_map        = &(*geo_map)[geo_size];

          // Create subgeometry
          size_t geo_ntasks = _ntasks;
          pami_task_t local_task_id = _task_id;
          pami_task_t root = 0;
          pami_geometry_t new_geometry = world_geometry;

          //printf("%d:Create geometry with size %zu and id %d\n", _task_id, geo_size, geometry_id);
          result = create_geometry(&geo_ntasks,
                                   &local_task_id,
                                   _task_id,
                                   &root,
                                   geo_size,
                                   geometry_id,
                                   &new_geometry,
                                   client,
                                   PAMI_GEOMETRY_NULL,
                                   contexts[0]);

          ++geometry_id;
          if(new_geometry != PAMI_GEOMETRY_NULL) {

            //printf("%d:Created geometry<%p> with size %zu and id %d\n", _task_id, new_geometry, geo_size, geometry_id);

            // Loop on each collective
            for(csize = 0; csize < _params.num_collectives; csize ++) {

              //query algo for barrier, collective and reduce
              pami_xfer_t coll[3];

              pami_xfer_type_t coll_xfer_type = _params.collectives[csize];
              /* Inserting msg_map in coll_map with key coll_xfer_type */
              msg_map = &(*coll_map)[coll_xfer_type];
              /* The algolists will be the sorted list */
              currAlgoList = NULL;
              /* we will set the names, pami_algorithm_t and ids (key) in the algomap */
              tmp_algo_map = &algo_map[coll_xfer_type];
              size_t msg_thresh = get_msg_thresh(byte_thresh, coll_xfer_type, geo_size);
              coll_mem_alloc(&coll[0], coll_xfer_type, msg_thresh, geo_size);
              size_t col_num_algo[2];

              // Query the 
              int rc = query_geometry_algorithms_num(new_geometry,
                                                     coll_xfer_type,
                                                     col_num_algo);
              rc |= query_geometry_algorithms(client,
                                              contexts[0],
                                              new_geometry,
                                              coll_xfer_type,
                                              col_num_algo,
                                              col_algo,
                                              col_md,
                                              q_col_algo,
                                              q_col_md);

              // Query an always works barrier algorithm
              rc |= query_geometry_algorithm_aw(client, contexts[0], new_geometry, PAMI_XFER_BARRIER,  &(coll[1]));
              // Query an always works reduce algorithm
              rc |= query_geometry_algorithm_aw(client, contexts[0], new_geometry, PAMI_XFER_REDUCE, &(coll[2]));

              if(!_task_id) {
                printf("# Collective: %-20s   Geometry Size: %-8zu \n",
                    xfer_type_str[_params.collectives[csize]], geo_size);
                printf("# -------------------------------------------------------\n");
              }

              size_t num_algo = col_num_algo[0] + col_num_algo[1];

              // Loop on message size
              size_t msg_sz_num;
              if (coll_xfer_type == PAMI_XFER_BARRIER) {
                act_msg_size[0] = 0;
                msg_sz_num      = 1;
              } else {
                for (i = 0; i < _params.num_message_sizes &&
                    _params.message_sizes[i] <= msg_thresh; i ++) {
                  act_msg_size[i] = MIN(_params.message_sizes[i], msg_thresh);
                }
                msg_sz_num = i;
              }
              if(_g_verbose && !_task_id) 
                printf("Number of message sizes for this collective is %zu\n", 
                    msg_sz_num);

              for(msize = 0; msize < msg_sz_num; msize ++) {
                if(_g_verbose && !_task_id) 
                {
                    printf("# Algorithm                           Message Size    Min (usec)        Max (usec)        Avg (usec)\n");
                    printf("# ---------                           ------------    ----------        ----------        ----------\n");
                }

                buf = FILE_CHECK;

                if(_g_checkpoint && !_task_id) {
                  sprintf(tmp_output_fname, "%s%s_%zu_%zu_%d_%zu",
                      TMP_OUTPUT_DIR,
                      TMP_OUTPUT_PREFIX,
                      _params.procs_per_node[psize],
                      _params.geometry_sizes[gsize],
                      _params.collectives[csize],
                      act_msg_size[msize]);

                  buf = file_check(tmp_output_fname);
                }

                if (_g_checkpoint) {
                  blocking_coll(contexts[0], &bcast, &bcast_poll_flag);

                  PAMI_assertf(buf != FILE_CHECK, "broadcast for FILE CHECK failed"); 
                  //printf("After bcast, buf is %d\n", buf);
                }

                if (_g_checkpoint && _g_verbose && !_task_id) {
                  switch (buf) {
                    case FILE_NOT_EXIST:
                      printf("File %s doesn't exist, about to run fresh iteration\n", 
                          tmp_output_fname);
                      break;
                    case FILE_OK:
                      printf("File %s passed sanity check, about to load data from it\n", 
                          tmp_output_fname);
                      break;
                    case FILE_BAD:
                      printf("File %s failed sanity check, about to run this iteration\n", 
                          tmp_output_fname);
                  }
                }

                if (buf == FILE_OK) {
                  if (!_task_id) {
                    tFile = fopen(tmp_output_fname, "r");

                    // when the tmp output file exists
                    size_t tmp_num_algo, tmp_algo, tmp_algo_name_len;
                    char *tmp_algo_name;
                    double tmp_results[3];

                    flen = fread(&tmp_num_algo, 1, SSIZE_T, tFile);
                    CRC(flen != SSIZE_T, "read tmp_num_algo");

                    for (i = 0; i < tmp_num_algo; i ++) {
                      flen = fread(&tmp_algo, 1, SSIZE_T, tFile); 
                      CRC(flen != SSIZE_T, "read tmp_algo");

                      flen = fread(&tmp_algo_name_len, 1, SSIZE_T, tFile); 
                      CRC(flen != SSIZE_T, "read tmp_algo_name_len");

                      tmp_algo_name = (char *)malloc(tmp_algo_name_len + 1);
                      memset(tmp_algo_name, 0, tmp_algo_name_len + 1);
                      garbCollList[garbCollIndx++] = (AlgoList)tmp_algo_name;
                      flen = fread(tmp_algo_name, 1, tmp_algo_name_len, tFile);
                      CRC(flen != tmp_algo_name_len, "read tmp_algo_name");
                       /* SSS: TODO.. Allocate tmp_algo_name for each iter */
                      AlgoNameToIdMap::iterator iter1;
                      if (_g_verbose) {
                        printf("%zu data in garbColl %s at %p or [%s at %p]\n",
                            garbCollIndx, tmp_algo_name, tmp_algo_name, 
                            garbCollList[garbCollIndx-1],
                            garbCollList[garbCollIndx-1]);
                      }
                      int found   = 0;
                      int foundId = 0;
                      for (iter1 = algo_name_map[coll_xfer_type].begin(); iter1 != algo_name_map[coll_xfer_type].end(); iter1++)
                      {
                        if(strcmp(iter1->first, tmp_algo_name)==0) {found = 1; foundId = iter1->second; break;}
                      }
                      if(!found)
                      {
                        algo_name_map[coll_xfer_type][tmp_algo_name] = algo_ids[coll_xfer_type];
                        foundId = algo_ids[coll_xfer_type]++;
                      }
                      (*tmp_algo_map)[foundId] = tmp_algo_name;
                      algo_list[i].algo_name = tmp_algo_name;
                      algo_list[i].algo_id   = foundId;

                      for (j = 0; j < 3; j ++) {
                        flen = fread(&tmp_results[j], 1, SDOUBLE, tFile); 
                        CRC(flen != SDOUBLE, "read tmp_results");

                      }
                      memcpy(algo_list[i].times, tmp_results, 3*SDOUBLE);
                      /*
                         printf("READ of (%s): %zu of %zu record: algo(%zu) algo.name.len(%zu) algo.name(%s) results(%6f:%6f:%6f)\n",
                         tmp_output_fname,
                         i, tmp_num_algo, tmp_algo, tmp_algo_name_len,
                         tmp_algo_name,
                         tmp_results[0], 
                         tmp_results[1], 
                         tmp_results[2]);
                       */

                      // these tmp_ data can be put into the xml
                      // file now
                    }
                    fclose(tFile);
                  }
                } else {

                  if(_g_checkpoint && !_task_id) {
                    // allocate enough space for algo_record, the actual number
                    // of records could be smaller
                    algo_record = new algo_record_t[num_algo];
                  }

                  // to keep track of the actual number of algorithms being used
                  size_t act_algo_num = 0;

                  //loop on algorithms
                  for(algo = 0; algo < num_algo; algo ++)
                  {
                    size_t low, high;
                    char *algo_name =
                      algo<col_num_algo[0]?col_md[algo].name:q_col_md[algo-col_num_algo[0]].name;


                    metadata_result_t result = {0};
                    fill_coll(client, contexts[0], coll, coll_xfer_type, &low, &high,
                        algo, act_msg_size[msize], _task_id, geo_size, root,
                        col_num_algo, &result, col_algo, col_md, q_col_algo, q_col_md);

                    AlgoNameToIdMap::iterator iter1;
                    int found   = 0;
                    int foundId = 0;
                    for (iter1 = algo_name_map[coll_xfer_type].begin(); iter1 != algo_name_map[coll_xfer_type].end(); iter1++)
                    {
                      if(strcmp(iter1->first, algo_name)==0) {found = 1; foundId = iter1->second; break;}
                    }
                    if(!found)
                    {
                      algo_name_map[coll_xfer_type][algo_name] = algo_ids[coll_xfer_type];
                      foundId = algo_ids[coll_xfer_type]++;
                    }
                    (*tmp_algo_map)[foundId]  = algo_name;
                    //(*tmp_algo_map)[foundId].algorithm      = coll[0].algorithm;
                    algo_list[algo].algo      = coll[0].algorithm;
                    algo_list[algo].algo_name = algo_name;
                    algo_list[algo].algo_id   = foundId;


                    if(act_msg_size[msize] <low || act_msg_size[msize] > high)
                    {
                      if(_g_verbose && !_task_id)
                        printf("  %s skipped as message size %zu is not in range (%zu-%zu)\n", 
                            algo_name, act_msg_size[msize], low, high);
                      algo_list[algo].times[0] = DOUBLE_DIG;
                      algo_list[algo].times[1] = DOUBLE_DIG;
                      algo_list[algo].times[2] = DOUBLE_DIG;
                      continue;
                    }
                    if(result.bitmask)
                    {
                      algo_list[algo].times[0] = DOUBLE_DIG;
                      algo_list[algo].times[1] = DOUBLE_DIG;
                      algo_list[algo].times[2] = DOUBLE_DIG;
                      continue;
                    }

                    //run benchmark 
                    bench_setup bench[1];
                    bench[0].xfer    = coll_xfer_type;
                    bench[0].bytes   = act_msg_size[msize];
                    bench[0].np      = geo_size;
                    bench[0].task_id = local_task_id;
                    bench[0].data_check = _params.verify;
                    bench[0].iters   = _params.iter;
                    bench[0].verbose = _params.verbose;
                    bench[0].isRoot  = (local_task_id == root)?1:0;
                    measure_collective(contexts[0], coll, bench);
                    memcpy(algo_list[algo].times, bench[0].times, 3*SDOUBLE);

                    if(!_task_id) {
                      if(_g_checkpoint && !_task_id) {
                        algo_record[algo].algo = algo;
                        algo_record[algo].algo_name = string(algo_name);
                        for (i = 0; i < 3; i ++) {
                          algo_record[algo].results[i] = algo_list[algo].times[i];
                        }
                      }

                      if (_g_verbose) 
                        printf("  %-35s %-15zu %-17f %-17f %-17f\n", algo_name,
                            act_msg_size[msize], algo_list[algo].times[0],
                            algo_list[algo].times[1], algo_list[algo].times[2]);
                    }

                    act_algo_num ++;

                  } // end of algorithm loop

                  if(_g_checkpoint && !_task_id) {
                      file_write(tmp_output_fname, algo_record, act_algo_num);
                      delete[] algo_record;
                  }

                }
                if(!_task_id)
                {
                  /* We have currAlgoList and prevAlgoList to compare and set message min and max ranges in xml */
                  sort_algo_list(algo_list, tempAlgoList, num_algo);
                  if(currAlgoList == NULL)
                  {
                    currAlgoList = (AlgoList)malloc(strlen((char*)tempAlgoList) + 1);
                    memset(currAlgoList, 0,  strlen((char*)tempAlgoList) + 1);
                    garbCollList[garbCollIndx++]      = currAlgoList;
                    strcpy((char *)currAlgoList, (const char *)tempAlgoList);
                    (*msg_map)[act_msg_size[msize]]   = currAlgoList;
                  }

                  if(strcmp((const char *)currAlgoList, (const char *)tempAlgoList) != 0)
                  {
                    (*msg_map)[act_msg_size[msize-1]] = currAlgoList;
                    currAlgoList = (AlgoList)malloc(strlen((char*)tempAlgoList) + 1);
                    memset(currAlgoList, 0,  strlen((char*)tempAlgoList) + 1);
                    garbCollList[garbCollIndx++]      = currAlgoList;
                    strcpy((char *)currAlgoList, (const char *)tempAlgoList);
                    (*msg_map)[act_msg_size[msize]]   = currAlgoList;
                  }
                }
              } // end of message size loop
              if(!_task_id)
              {
                 if(currAlgoList != NULL)
                  (*msg_map)[act_msg_size[msg_sz_num-1]] = currAlgoList;
              }
              //a bunch of free here
              release_coll(coll, coll_xfer_type);
              // Destroy the sub geometry
            } // end collectives loop

            if(new_geometry != world_geometry)
              destroy_geometry(client, contexts[0], &new_geometry);
          }
          // every task needs to do the barrier on world geometry
          blocking_coll(contexts[0], &barrier, &barrier_poll_flag);
        } // end geometry size loop
      } // end procs_per_node loop

      if(!_task_id)
      {
        PAMI::XMLWriter<>  xml_creator;
        if(xml_creator.write_xml(filename, ppn_map, algo_map))
          return PAMI_ERROR;

        // since all the interations passed with no issue
        // remove all the tmp output files
        if(_g_checkpoint) {
          dir_clean();
        }

        MessageSizeMap::iterator iter;
        for (iter = garbCollList.begin(); iter != garbCollList.end(); iter++) {
          free(iter->second);
        }
        garbCollList.clear();
      }
    }

    return PAMI_SUCCESS;
  }

  inline pami_result_t AdvisorTable::load(char* filename)
  {
    pami_result_t ret = PAMI_SUCCESS;
    PAMI::XMLParser<> parser;
    if(parser.read_xml(filename, &_collsel_data))
    {
      ret = PAMI_ERROR;
    }
    return ret;
  }

  inline pami_result_t AdvisorTable::unload()
  {
    return PAMI_SUCCESS;
  }

  inline pami_result_t AdvisorTable::query(pami_geometry_t geo,
                                           fast_query_t   *query)
  {
    pami_result_t ret = PAMI_SUCCESS;
    PAMI::Geometry::Common *geometry = (PAMI::Geometry::Common*) geo;

    size_t geo_size = geometry->size();
    // We have just a static ppn entry for now
    GeometrySizeMap *geo_size_map =
        _collsel_data.find_nearest_geometry_shape(1);

    CollectivesMap *coll_map =
        _collsel_data.find_nearest_geometry_size(geo_size, geo_size_map);

    pami_algorithm_t **algorithms;
    pami_metadata_t  **metadata;
    algorithms = (pami_algorithm_t **)
      calloc(PAMI_XFER_COUNT, sizeof(pami_algorithm_t *));
    metadata = (pami_metadata_t **)
      calloc(PAMI_XFER_COUNT, sizeof(pami_metadata_t *));

    size_t algo_num[2];
    pami_xfer_type_t coll;
    CollectivesMap::iterator c_iter;

    for(c_iter = coll_map->begin(); c_iter != coll_map->end(); ++c_iter)
    {
      coll = (pami_xfer_type_t)c_iter->first;
      geometry->algorithms_num(coll, algo_num);

      algorithms[coll] = (pami_algorithm_t *)
        malloc((algo_num[0] + algo_num[1]) * sizeof(pami_algorithm_t));
      metadata[coll] = (pami_metadata_t *)
        malloc((algo_num[0] + algo_num[1]) * sizeof(pami_metadata_t));

      geometry->algorithms_info(coll,
                                &algorithms[coll][0],
                                &metadata[coll][0],
                                algo_num[0],
                                &algorithms[coll][algo_num[0]],
                                &metadata[coll][algo_num[0]],
                                algo_num[1]);

    }

    CollselQuery *q = new CollselQuery();
    q->coll_map   = coll_map;
    q->algorithms = algorithms;
    q->metadata   = metadata;

    *query = q;

    return ret;
  }

  pami_result_t AdvisorTable::advise(fast_query_t       *query,
                                     pami_xfer_type_t    xfer_type,
                                     pami_xfer_t        *xfer,
                                     advisor_algorithm_t optimal_algorithms[],
                                     size_t              max_algorithms)
  {
    pami_result_t ret = PAMI_SUCCESS;
    return ret;
  }

}

#endif // __api_extension_c_collsel_Extension_h__
