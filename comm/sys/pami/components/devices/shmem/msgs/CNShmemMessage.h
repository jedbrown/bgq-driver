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
 * \file components/devices/shmem/msgs/CNShmemMessage.h
 * \brief ???
 */

#ifndef __components_devices_shmem_msgs_CNShmemMessage_h__
#define __components_devices_shmem_msgs_CNShmemMessage_h__

//#include "ShaddrMcombMessagePipe.h"

#undef TRACE_ERR

#ifndef TRACE_ERR
#define TRACE_ERR(x) // fprintf x
#endif
//#include "../a2qpx_nway_sum.h"
//#include "../a2qpx_nway_max.h"
#include "../a2qpx_nway_math.h"
#include "../bgq_math.h"
#include "components/devices/shmem/CNShmemDesc.h"

#include "Memory.h"

namespace PAMI
{
  namespace Device
  {
    namespace Shmem
    {
        //This class has routines for advancing short/medium/long multicombines and multicasts
        class CNShmemMessage 
        {
          protected:

//#define	ChunkSize 	4096
//#define	ChunkSize 	512
#define	ChunkSize   1024
#define NumDblsPerChunk (ChunkSize/sizeof(double))
#define NumChunks(bytes)	((bytes%ChunkSize == 0) ? (bytes/ChunkSize):(bytes/ChunkSize + 1))
#define G_Srcs(x) 	((double*)(_controlB->GAT.srcbufs[x]))
#define G_Dsts(x) 	((double*)(_controlB->GAT.dstbufs[x]))
#define P_Dsts(x) 	((double*)(_controlB->phybufs[x]))
#define S_Bufs(x)       ((double*)(my_desc->get_buffer(x)))
#define C_Bufs(x)       ((char*)(my_desc->get_buffer(x)))
#define G_Counter 	((uint64_t*)(_controlB->GAT.counter_addr))
#define MinChunkSize  64
#define ShmBufSize  SHORT_MSG_CUTOFF
#define Num32kChunks  37
#define Num16kChunks  21
#define Num8kChunks  13

#define MATH_16WAY(dst, src0, src1, src2, src3, src4, src5, src6, src7, src8, src9, src10, src11, src12, src13, src14, src15, dataSize)   \
            quad_double_math_16way(dst + chunk* NumDblsPerChunk+offset_dbl, src0 +chunk*NumDblsPerChunk+offset_dbl,\
                src1+chunk*NumDblsPerChunk+offset_dbl, src2+chunk*NumDblsPerChunk+offset_dbl,\
                src3+chunk*NumDblsPerChunk+offset_dbl, src4+chunk*NumDblsPerChunk+offset_dbl,\
                src5+chunk*NumDblsPerChunk+offset_dbl, src6+chunk*NumDblsPerChunk+offset_dbl,\
                src7+chunk*NumDblsPerChunk+offset_dbl, src8+chunk*NumDblsPerChunk+offset_dbl,\
                src9+chunk*NumDblsPerChunk+offset_dbl, src10+chunk*NumDblsPerChunk+offset_dbl,\
                src11+chunk*NumDblsPerChunk+offset_dbl, src12+chunk*NumDblsPerChunk+offset_dbl, \
                src13+chunk*NumDblsPerChunk+offset_dbl, src14+chunk*NumDblsPerChunk+offset_dbl,\
                src15+chunk*NumDblsPerChunk+offset_dbl, dataSize, _opcode);

            inline void advance_2way_math(unsigned _local_rank, unsigned _npeers, size_t bytes, unsigned offset_dbl)
            {
              /* local ranks other than 0 do the following quad math */
              unsigned chunk;

              for (chunk=0; chunk < NumChunks(bytes)-1; chunk++){
                if ((chunk%(_npeers-1) +1) == _local_rank){

                  quad_double_math_2way(_rcvbuf+ chunk* NumDblsPerChunk+offset_dbl, G_Srcs(0)+chunk*NumDblsPerChunk+offset_dbl,
                      G_Srcs(1)+chunk*NumDblsPerChunk+offset_dbl, NumDblsPerChunk, _opcode);
                  Memory::sync();
                  _controlB->chunk_done[_local_rank] = chunk;
                }
                TRACE_ERR((stderr,"_rcvbuf[%u]:%f\n", chunk*NumDblsPerChunk, _rcvbuf[chunk*NumDblsPerChunk]));
              }

              /* summing last chunk which can be of any size */
              if ((chunk%(_npeers-1) +1) == _local_rank){
                if (bytes%ChunkSize == 0)
                {
                  quad_double_math_2way(_rcvbuf+ chunk* NumDblsPerChunk+offset_dbl, G_Srcs(0)+chunk*NumDblsPerChunk+offset_dbl,
                      G_Srcs(1)+chunk*NumDblsPerChunk+offset_dbl, NumDblsPerChunk, _opcode);

                }
                else
                {
                  quad_double_math_2way(_rcvbuf+ chunk* NumDblsPerChunk+offset_dbl, G_Srcs(0)+chunk*NumDblsPerChunk+offset_dbl,
                      G_Srcs(1)+chunk*NumDblsPerChunk+offset_dbl, (bytes%ChunkSize)/sizeof(double), _opcode);
                }
                  Memory::sync();
                _controlB->chunk_done[_local_rank] = chunk;
              }

            }

            inline void advance_4way_math(unsigned _local_rank, unsigned _npeers, size_t bytes, unsigned offset_dbl)
            {
              /* local ranks other than 0 do the following quad math */
              unsigned chunk;

              for (chunk=0; chunk < NumChunks(bytes)-1; chunk++){
                if ((chunk%(_npeers-1) +1) == _local_rank){

                  quad_double_math_4way(_rcvbuf+ chunk* NumDblsPerChunk+offset_dbl, G_Srcs(0)+chunk*NumDblsPerChunk+offset_dbl,
                      G_Srcs(1)+chunk*NumDblsPerChunk+offset_dbl, G_Srcs(2)+chunk*NumDblsPerChunk+offset_dbl,
                      G_Srcs(3)+chunk*NumDblsPerChunk+offset_dbl, NumDblsPerChunk, _opcode);
                  Memory::sync();
                  _controlB->chunk_done[_local_rank] = chunk;
                }
                TRACE_ERR((stderr,"_rcvbuf[%u]:%f\n", chunk*NumDblsPerChunk, _rcvbuf[chunk*NumDblsPerChunk]));
              }

              /* summing last chunk which can be of any size */
              if ((chunk%(_npeers-1) +1) == _local_rank){
                if (bytes%ChunkSize == 0)
                {
                  quad_double_math_4way(_rcvbuf+ chunk* NumDblsPerChunk+offset_dbl, G_Srcs(0)+chunk*NumDblsPerChunk+offset_dbl,
                      G_Srcs(1)+chunk*NumDblsPerChunk+offset_dbl, G_Srcs(2)+chunk*NumDblsPerChunk+offset_dbl,
                      G_Srcs(3)+chunk*NumDblsPerChunk+offset_dbl, NumDblsPerChunk, _opcode);

                }
                else
                {
                  quad_double_math_4way(_rcvbuf+ chunk* NumDblsPerChunk+offset_dbl, G_Srcs(0)+chunk*NumDblsPerChunk+offset_dbl,
                      G_Srcs(1)+chunk*NumDblsPerChunk+offset_dbl, G_Srcs(2)+chunk*NumDblsPerChunk+offset_dbl,
                      G_Srcs(3)+chunk*NumDblsPerChunk+offset_dbl, (bytes%ChunkSize)/sizeof(double), _opcode);
                }
                  Memory::sync();
                _controlB->chunk_done[_local_rank] = chunk;
              }

            }

            inline void advance_8way_math(unsigned _local_rank, unsigned _npeers, size_t bytes, unsigned offset_dbl)
            {
              /* local ranks other than 0 do the following quad sum */
              unsigned chunk;

              for (chunk=0; chunk < NumChunks(bytes)-1; chunk++){
                if ((chunk%(_npeers-1) +1) == _local_rank){

                  quad_double_math_8way(_rcvbuf+ chunk* NumDblsPerChunk+offset_dbl, G_Srcs(0)+chunk*NumDblsPerChunk+offset_dbl,
                      G_Srcs(1)+chunk*NumDblsPerChunk+offset_dbl, G_Srcs(2)+chunk*NumDblsPerChunk+offset_dbl,
                      G_Srcs(3)+chunk*NumDblsPerChunk+offset_dbl, G_Srcs(4)+chunk*NumDblsPerChunk+offset_dbl,
                      G_Srcs(5)+chunk*NumDblsPerChunk+offset_dbl, G_Srcs(6)+chunk*NumDblsPerChunk+offset_dbl,
                      G_Srcs(7)+chunk*NumDblsPerChunk+offset_dbl, NumDblsPerChunk, _opcode);
                  Memory::sync();
                  _controlB->chunk_done[_local_rank] = chunk;
                }
              }

              /* summing last chunk which can be of any size */
              if ((chunk%(_npeers-1) +1) == _local_rank){
                if (bytes%ChunkSize == 0)
                {
                  quad_double_math_8way(_rcvbuf+ chunk* NumDblsPerChunk+offset_dbl, G_Srcs(0)+chunk*NumDblsPerChunk+offset_dbl,
                      G_Srcs(1)+chunk*NumDblsPerChunk+offset_dbl, G_Srcs(2)+chunk*NumDblsPerChunk+offset_dbl,
                      G_Srcs(3)+chunk*NumDblsPerChunk+offset_dbl, G_Srcs(4)+chunk*NumDblsPerChunk+offset_dbl,
                      G_Srcs(5)+chunk*NumDblsPerChunk+offset_dbl, G_Srcs(6)+chunk*NumDblsPerChunk+offset_dbl,
                      G_Srcs(7)+chunk*NumDblsPerChunk+offset_dbl, NumDblsPerChunk, _opcode);
                }
                else
                {
                  quad_double_math_8way(_rcvbuf+ chunk* NumDblsPerChunk+offset_dbl, G_Srcs(0)+chunk*NumDblsPerChunk+offset_dbl,
                      G_Srcs(1)+chunk*NumDblsPerChunk+offset_dbl, G_Srcs(2)+chunk*NumDblsPerChunk+offset_dbl,
                      G_Srcs(3)+chunk*NumDblsPerChunk+offset_dbl, G_Srcs(4)+chunk*NumDblsPerChunk+offset_dbl,
                      G_Srcs(5)+chunk*NumDblsPerChunk+offset_dbl, G_Srcs(6)+chunk*NumDblsPerChunk+offset_dbl,
                      G_Srcs(7)+chunk*NumDblsPerChunk+offset_dbl, (bytes%ChunkSize)/sizeof(double), _opcode);
                }
                //mbar();
                  Memory::sync();
                _controlB->chunk_done[_local_rank] = chunk;
              }
            }

            inline void advance_16way_math(unsigned _local_rank, unsigned _npeers, size_t bytes, unsigned offset_dbl)
            {
              /* local ranks other than 0 do the following quad sum */
              unsigned chunk;
                

              for (chunk=0; chunk < NumChunks(bytes)-1; chunk++){
                if ((chunk%(_npeers-1) +1) == _local_rank){
                  if (likely(_in_place  ==  0))
                  {
                    quad_double_math_16way(_rcvbuf+ chunk* NumDblsPerChunk+offset_dbl, G_Srcs(0)+chunk*NumDblsPerChunk+offset_dbl,
                        G_Srcs(1)+chunk*NumDblsPerChunk+offset_dbl, G_Srcs(2)+chunk*NumDblsPerChunk+offset_dbl,
                        G_Srcs(3)+chunk*NumDblsPerChunk+offset_dbl, G_Srcs(4)+chunk*NumDblsPerChunk+offset_dbl,
                        G_Srcs(5)+chunk*NumDblsPerChunk+offset_dbl, G_Srcs(6)+chunk*NumDblsPerChunk+offset_dbl,
                        G_Srcs(7)+chunk*NumDblsPerChunk+offset_dbl, G_Srcs(8)+chunk*NumDblsPerChunk+offset_dbl,
                        G_Srcs(9)+chunk*NumDblsPerChunk+offset_dbl, G_Srcs(10)+chunk*NumDblsPerChunk+offset_dbl,
                        G_Srcs(11)+chunk*NumDblsPerChunk+offset_dbl, G_Srcs(12)+chunk*NumDblsPerChunk+offset_dbl,
                        G_Srcs(13)+chunk*NumDblsPerChunk+offset_dbl, G_Srcs(14)+chunk*NumDblsPerChunk+offset_dbl,
                        G_Srcs(15)+chunk*NumDblsPerChunk+offset_dbl, NumDblsPerChunk, _opcode);
                  }
                  else
                  {
                    double* srcs[16] = { G_Srcs(0)+chunk*NumDblsPerChunk+offset_dbl,
                        G_Srcs(1)+chunk*NumDblsPerChunk+offset_dbl, G_Srcs(2)+chunk*NumDblsPerChunk+offset_dbl,
                        G_Srcs(3)+chunk*NumDblsPerChunk+offset_dbl, G_Srcs(4)+chunk*NumDblsPerChunk+offset_dbl,
                        G_Srcs(5)+chunk*NumDblsPerChunk+offset_dbl, G_Srcs(6)+chunk*NumDblsPerChunk+offset_dbl,
                        G_Srcs(7)+chunk*NumDblsPerChunk+offset_dbl, G_Srcs(8)+chunk*NumDblsPerChunk+offset_dbl,
                        G_Srcs(9)+chunk*NumDblsPerChunk+offset_dbl, G_Srcs(10)+chunk*NumDblsPerChunk+offset_dbl,
                        G_Srcs(11)+chunk*NumDblsPerChunk+offset_dbl, G_Srcs(12)+chunk*NumDblsPerChunk+offset_dbl,
                        G_Srcs(13)+chunk*NumDblsPerChunk+offset_dbl, G_Srcs(14)+chunk*NumDblsPerChunk+offset_dbl,
                        G_Srcs(15)+chunk*NumDblsPerChunk+offset_dbl};
                    coremath func = MATH_OP_FUNCS(PAMI_DOUBLE, _opcode, 16);
                    func(_rcvbuf+ chunk* NumDblsPerChunk+offset_dbl, (void**)srcs,  16, NumDblsPerChunk);
                  }
                  Memory::sync();
                  _controlB->chunk_done[_local_rank] = chunk;
                }
              }

              /* summing last chunk which can be of any size */
              if ((chunk%(_npeers-1) +1) == _local_rank){
                if (bytes%ChunkSize == 0)
                {
                  if (likely(_in_place == 0))
                  {
                    quad_double_math_16way(_rcvbuf+ chunk* NumDblsPerChunk+offset_dbl, G_Srcs(0)+chunk*NumDblsPerChunk+offset_dbl,
                        G_Srcs(1)+chunk*NumDblsPerChunk+offset_dbl, G_Srcs(2)+chunk*NumDblsPerChunk+offset_dbl,
                        G_Srcs(3)+chunk*NumDblsPerChunk+offset_dbl, G_Srcs(4)+chunk*NumDblsPerChunk+offset_dbl,
                        G_Srcs(5)+chunk*NumDblsPerChunk+offset_dbl, G_Srcs(6)+chunk*NumDblsPerChunk+offset_dbl,
                        G_Srcs(7)+chunk*NumDblsPerChunk+offset_dbl, G_Srcs(8)+chunk*NumDblsPerChunk+offset_dbl,
                        G_Srcs(9)+chunk*NumDblsPerChunk+offset_dbl, G_Srcs(10)+chunk*NumDblsPerChunk+offset_dbl,
                        G_Srcs(11)+chunk*NumDblsPerChunk+offset_dbl, G_Srcs(12)+chunk*NumDblsPerChunk+offset_dbl,
                        G_Srcs(13)+chunk*NumDblsPerChunk+offset_dbl, G_Srcs(14)+chunk*NumDblsPerChunk+offset_dbl,
                        G_Srcs(15)+chunk*NumDblsPerChunk+offset_dbl, NumDblsPerChunk, _opcode);
                  }
                  else
                  {
                    double* srcs[16] = { G_Srcs(0)+chunk*NumDblsPerChunk+offset_dbl,
                        G_Srcs(1)+chunk*NumDblsPerChunk+offset_dbl, G_Srcs(2)+chunk*NumDblsPerChunk+offset_dbl,
                        G_Srcs(3)+chunk*NumDblsPerChunk+offset_dbl, G_Srcs(4)+chunk*NumDblsPerChunk+offset_dbl,
                        G_Srcs(5)+chunk*NumDblsPerChunk+offset_dbl, G_Srcs(6)+chunk*NumDblsPerChunk+offset_dbl,
                        G_Srcs(7)+chunk*NumDblsPerChunk+offset_dbl, G_Srcs(8)+chunk*NumDblsPerChunk+offset_dbl,
                        G_Srcs(9)+chunk*NumDblsPerChunk+offset_dbl, G_Srcs(10)+chunk*NumDblsPerChunk+offset_dbl,
                        G_Srcs(11)+chunk*NumDblsPerChunk+offset_dbl, G_Srcs(12)+chunk*NumDblsPerChunk+offset_dbl,
                        G_Srcs(13)+chunk*NumDblsPerChunk+offset_dbl, G_Srcs(14)+chunk*NumDblsPerChunk+offset_dbl,
                        G_Srcs(15)+chunk*NumDblsPerChunk+offset_dbl};
                    coremath func = MATH_OP_FUNCS(PAMI_DOUBLE, _opcode, 16);
                    func(_rcvbuf+ chunk* NumDblsPerChunk+offset_dbl, (void**)srcs,  16, NumDblsPerChunk);
                  }

                }
                else
                {
                  if (likely(_in_place == 0))
                  {
                    quad_double_math_16way(_rcvbuf+ chunk* NumDblsPerChunk+offset_dbl, G_Srcs(0)+chunk*NumDblsPerChunk+offset_dbl,
                        G_Srcs(1)+chunk*NumDblsPerChunk+offset_dbl, G_Srcs(2)+chunk*NumDblsPerChunk+offset_dbl,
                        G_Srcs(3)+chunk*NumDblsPerChunk+offset_dbl, G_Srcs(4)+chunk*NumDblsPerChunk+offset_dbl,
                        G_Srcs(5)+chunk*NumDblsPerChunk+offset_dbl, G_Srcs(6)+chunk*NumDblsPerChunk+offset_dbl,
                        G_Srcs(7)+chunk*NumDblsPerChunk+offset_dbl, G_Srcs(8)+chunk*NumDblsPerChunk+offset_dbl,
                        G_Srcs(9)+chunk*NumDblsPerChunk+offset_dbl, G_Srcs(10)+chunk*NumDblsPerChunk+offset_dbl,
                        G_Srcs(11)+chunk*NumDblsPerChunk+offset_dbl, G_Srcs(12)+chunk*NumDblsPerChunk+offset_dbl,
                        G_Srcs(13)+chunk*NumDblsPerChunk+offset_dbl, G_Srcs(14)+chunk*NumDblsPerChunk+offset_dbl,
                        G_Srcs(15)+chunk*NumDblsPerChunk+offset_dbl, (bytes%ChunkSize)/sizeof(double), _opcode);
                  }
                  else
                  {
                    double* srcs[16] = { G_Srcs(0)+chunk*NumDblsPerChunk+offset_dbl,
                        G_Srcs(1)+chunk*NumDblsPerChunk+offset_dbl, G_Srcs(2)+chunk*NumDblsPerChunk+offset_dbl,
                        G_Srcs(3)+chunk*NumDblsPerChunk+offset_dbl, G_Srcs(4)+chunk*NumDblsPerChunk+offset_dbl,
                        G_Srcs(5)+chunk*NumDblsPerChunk+offset_dbl, G_Srcs(6)+chunk*NumDblsPerChunk+offset_dbl,
                        G_Srcs(7)+chunk*NumDblsPerChunk+offset_dbl, G_Srcs(8)+chunk*NumDblsPerChunk+offset_dbl,
                        G_Srcs(9)+chunk*NumDblsPerChunk+offset_dbl, G_Srcs(10)+chunk*NumDblsPerChunk+offset_dbl,
                        G_Srcs(11)+chunk*NumDblsPerChunk+offset_dbl, G_Srcs(12)+chunk*NumDblsPerChunk+offset_dbl,
                        G_Srcs(13)+chunk*NumDblsPerChunk+offset_dbl, G_Srcs(14)+chunk*NumDblsPerChunk+offset_dbl,
                        G_Srcs(15)+chunk*NumDblsPerChunk+offset_dbl};
                    coremath func = MATH_OP_FUNCS(PAMI_DOUBLE, _opcode, 16);
                    func(_rcvbuf+ chunk* NumDblsPerChunk+offset_dbl, (void**)srcs,  16, (bytes%ChunkSize)/sizeof(double));
                  }
                }
                Memory::sync();
                _controlB->chunk_done[_local_rank] = chunk;
              }
            }

            inline void advance_16way_math_16k(unsigned _local_rank, unsigned _npeers, unsigned num_chunks)
            {
              /* local ranks other than 0 do the following quad sum */
              unsigned chunk, numDbls, dblsSoFar =0, bytesSoFar=0;
            

              //for (chunk=0; chunk < Num16kChunks; chunk++){
              //for (chunk=0; chunk < Num8kChunks; chunk++){
              //for (chunk=0; chunk < Num32kChunks; chunk++){
              for (chunk=0; chunk < num_chunks; chunk++){
                if ((chunk%(_npeers-1) +1) == _local_rank){
                  numDbls = _chunk_array[chunk] >>3;            
                  dblsSoFar = bytesSoFar >> 3;
                  //printf("chunk:%d dblsSoFar:%d\n", chunk, dblsSoFar); 
                  quad_double_math_16way(_rcvbuf+ dblsSoFar, G_Srcs(0)+dblsSoFar,
                      G_Srcs(1)+dblsSoFar, G_Srcs(2)+dblsSoFar,
                      G_Srcs(3)+dblsSoFar, G_Srcs(4)+dblsSoFar,
                      G_Srcs(5)+dblsSoFar, G_Srcs(6)+dblsSoFar,
                      G_Srcs(7)+dblsSoFar, G_Srcs(8)+dblsSoFar,
                      G_Srcs(9)+dblsSoFar, G_Srcs(10)+dblsSoFar,
                      G_Srcs(11)+dblsSoFar, G_Srcs(12)+dblsSoFar,
                      G_Srcs(13)+dblsSoFar, G_Srcs(14)+dblsSoFar,
                      G_Srcs(15)+dblsSoFar, numDbls, _opcode);
                  _controlB->chunk_done[_local_rank] = chunk;
                }
                bytesSoFar+= _chunk_array[chunk];
              }
            }


            inline void advance_64way_math(unsigned _local_rank, unsigned _npeers, size_t bytes, unsigned offset_dbl)
            {
              /* local ranks other than 0 do the following quad sum */
              unsigned chunk;

              for (chunk=0; chunk < NumChunks(bytes)-1; chunk++){
                if ((chunk%(_npeers-1) +1) == _local_rank){
      
                MATH_16WAY(_rcvbuf, G_Srcs(0),G_Srcs(4),G_Srcs(8),G_Srcs(12),G_Srcs(16),G_Srcs(20),G_Srcs(24),G_Srcs(28),G_Srcs(32),
                            G_Srcs(36),G_Srcs(40),G_Srcs(44),G_Srcs(48),G_Srcs(52),G_Srcs(56),G_Srcs(60), NumDblsPerChunk);
                MATH_16WAY(_rcvbuf, _rcvbuf, G_Srcs(5),G_Srcs(9),G_Srcs(13),G_Srcs(17),G_Srcs(21),G_Srcs(25),G_Srcs(29),G_Srcs(33),
                            G_Srcs(37), G_Srcs(41),G_Srcs(45),G_Srcs(49),G_Srcs(53),G_Srcs(57),G_Srcs(61), NumDblsPerChunk);
                MATH_16WAY(_rcvbuf, _rcvbuf, G_Srcs(6),G_Srcs(10),G_Srcs(14),G_Srcs(18),G_Srcs(22),G_Srcs(26),G_Srcs(30),G_Srcs(34),
                            G_Srcs(38), G_Srcs(42),G_Srcs(46),G_Srcs(50),G_Srcs(54),G_Srcs(58),G_Srcs(62), NumDblsPerChunk);
                MATH_16WAY(_rcvbuf, _rcvbuf, G_Srcs(7),G_Srcs(11),G_Srcs(15),G_Srcs(19),G_Srcs(23),G_Srcs(27),G_Srcs(31),G_Srcs(35),
                            G_Srcs(39), G_Srcs(43),G_Srcs(47),G_Srcs(51),G_Srcs(55),G_Srcs(59),G_Srcs(63), NumDblsPerChunk);
                quad_double_math_4way(_rcvbuf+ chunk* NumDblsPerChunk+offset_dbl, _rcvbuf+ chunk* NumDblsPerChunk+offset_dbl, 
                G_Srcs(1)+chunk*NumDblsPerChunk+offset_dbl, G_Srcs(2)+chunk*NumDblsPerChunk+offset_dbl, 
                G_Srcs(3)+chunk*NumDblsPerChunk+offset_dbl, NumDblsPerChunk, _opcode);
                Memory::sync();

                  _controlB->chunk_done[_local_rank] = chunk;
                }
              }

              /* summing last chunk which can be of any size */
              if ((chunk%(_npeers-1) +1) == _local_rank){
                if (bytes%ChunkSize == 0)
                {
                MATH_16WAY(_rcvbuf, G_Srcs(0),G_Srcs(4),G_Srcs(8),G_Srcs(12),G_Srcs(16),G_Srcs(20),G_Srcs(24),G_Srcs(28),G_Srcs(32),
                            G_Srcs(36),G_Srcs(40),G_Srcs(44),G_Srcs(48),G_Srcs(52),G_Srcs(56),G_Srcs(60), NumDblsPerChunk);
                MATH_16WAY(_rcvbuf, _rcvbuf, G_Srcs(5),G_Srcs(9),G_Srcs(13),G_Srcs(17),G_Srcs(21),G_Srcs(25),G_Srcs(29),G_Srcs(33),
                            G_Srcs(37), G_Srcs(41),G_Srcs(45),G_Srcs(49),G_Srcs(53),G_Srcs(57),G_Srcs(61), NumDblsPerChunk);
                MATH_16WAY(_rcvbuf, _rcvbuf, G_Srcs(6),G_Srcs(10),G_Srcs(14),G_Srcs(18),G_Srcs(22),G_Srcs(26),G_Srcs(30),G_Srcs(34),
                            G_Srcs(38), G_Srcs(42),G_Srcs(46),G_Srcs(50),G_Srcs(54),G_Srcs(58),G_Srcs(62), NumDblsPerChunk);
                MATH_16WAY(_rcvbuf, _rcvbuf, G_Srcs(7),G_Srcs(11),G_Srcs(15),G_Srcs(19),G_Srcs(23),G_Srcs(27),G_Srcs(31),G_Srcs(35),
                            G_Srcs(39), G_Srcs(43),G_Srcs(47),G_Srcs(51),G_Srcs(55),G_Srcs(59),G_Srcs(63), NumDblsPerChunk);
                quad_double_math_4way(_rcvbuf+ chunk* NumDblsPerChunk+offset_dbl, _rcvbuf+ chunk* NumDblsPerChunk+offset_dbl, 
                G_Srcs(1)+chunk*NumDblsPerChunk+offset_dbl, G_Srcs(2)+chunk*NumDblsPerChunk+offset_dbl, 
                G_Srcs(3)+chunk*NumDblsPerChunk+offset_dbl, NumDblsPerChunk, _opcode);

                }
                else
                {
                MATH_16WAY(_rcvbuf, G_Srcs(0),G_Srcs(4),G_Srcs(8),G_Srcs(12),G_Srcs(16),G_Srcs(20),G_Srcs(24),G_Srcs(28),G_Srcs(32),
                            G_Srcs(36),G_Srcs(40),G_Srcs(44),G_Srcs(48),G_Srcs(52),G_Srcs(56),G_Srcs(60), ((bytes%ChunkSize)/sizeof(double)));
                MATH_16WAY(_rcvbuf, _rcvbuf, G_Srcs(5),G_Srcs(9),G_Srcs(13),G_Srcs(17),G_Srcs(21),G_Srcs(25),G_Srcs(29),G_Srcs(33),
                            G_Srcs(37), G_Srcs(41),G_Srcs(45),G_Srcs(49),G_Srcs(53),G_Srcs(57),G_Srcs(61), ((bytes%ChunkSize)/sizeof(double)));
                MATH_16WAY(_rcvbuf, _rcvbuf, G_Srcs(6),G_Srcs(10),G_Srcs(14),G_Srcs(18),G_Srcs(22),G_Srcs(26),G_Srcs(30),G_Srcs(34),
                            G_Srcs(38), G_Srcs(42),G_Srcs(46),G_Srcs(50),G_Srcs(54),G_Srcs(58),G_Srcs(62), ((bytes%ChunkSize)/sizeof(double)));
                MATH_16WAY(_rcvbuf, _rcvbuf, G_Srcs(7),G_Srcs(11),G_Srcs(15),G_Srcs(19),G_Srcs(23),G_Srcs(27),G_Srcs(31),G_Srcs(35),
                            G_Srcs(39), G_Srcs(43),G_Srcs(47),G_Srcs(51),G_Srcs(55),G_Srcs(59),G_Srcs(63), ((bytes%ChunkSize)/sizeof(double)));
                quad_double_math_4way(_rcvbuf+ chunk* NumDblsPerChunk+offset_dbl, _rcvbuf+ chunk* NumDblsPerChunk+offset_dbl, 
                G_Srcs(1)+chunk*NumDblsPerChunk+offset_dbl, G_Srcs(2)+chunk*NumDblsPerChunk+offset_dbl, 
                G_Srcs(3)+chunk*NumDblsPerChunk+offset_dbl, ((bytes%ChunkSize)/sizeof(double)), _opcode);
                }
                Memory::sync();
                _controlB->chunk_done[_local_rank] = chunk;
              }
            }

            inline void advance_32way_math(unsigned _local_rank, unsigned _npeers, size_t bytes, unsigned offset_dbl)
            {
              /* local ranks other than 0 do the following quad sum */
              unsigned chunk;
              for (chunk=0; chunk < NumChunks(bytes)-1; chunk++){
                if ((chunk%(_npeers-1) +1) == _local_rank){
      
                MATH_16WAY(_rcvbuf, G_Srcs(0),G_Srcs(2),G_Srcs(4),G_Srcs(6),G_Srcs(8),G_Srcs(10),G_Srcs(12),G_Srcs(14),G_Srcs(16),
                            G_Srcs(18),G_Srcs(20),G_Srcs(22),G_Srcs(24),G_Srcs(26),G_Srcs(28),G_Srcs(30), NumDblsPerChunk);
                MATH_16WAY(_rcvbuf, _rcvbuf, G_Srcs(1),G_Srcs(3),G_Srcs(5),G_Srcs(7),G_Srcs(9),G_Srcs(11),G_Srcs(13),G_Srcs(15),
                            G_Srcs(17), G_Srcs(19),G_Srcs(21),G_Srcs(23),G_Srcs(25),G_Srcs(27),G_Srcs(29), NumDblsPerChunk);
                quad_double_math_2way(_rcvbuf+ chunk* NumDblsPerChunk+offset_dbl, _rcvbuf+ chunk* NumDblsPerChunk+offset_dbl, 
                G_Srcs(31)+chunk*NumDblsPerChunk+offset_dbl, NumDblsPerChunk, _opcode);
                ppc_msync();
                Memory::sync();

                  _controlB->chunk_done[_local_rank] = chunk;
                }
              }

              /* summing last chunk which can be of any size */
              if ((chunk%(_npeers-1) +1) == _local_rank){
                if (bytes%ChunkSize == 0)
                {
                MATH_16WAY(_rcvbuf, G_Srcs(0),G_Srcs(2),G_Srcs(4),G_Srcs(6),G_Srcs(8),G_Srcs(10),G_Srcs(12),G_Srcs(14),G_Srcs(16),
                            G_Srcs(18),G_Srcs(20),G_Srcs(22),G_Srcs(24),G_Srcs(26),G_Srcs(28),G_Srcs(30), NumDblsPerChunk);
                MATH_16WAY(_rcvbuf, _rcvbuf, G_Srcs(1),G_Srcs(3),G_Srcs(5),G_Srcs(7),G_Srcs(9),G_Srcs(11),G_Srcs(13),G_Srcs(15),
                            G_Srcs(17), G_Srcs(19),G_Srcs(21),G_Srcs(23),G_Srcs(25),G_Srcs(27),G_Srcs(29), NumDblsPerChunk);
                quad_double_math_2way(_rcvbuf+ chunk* NumDblsPerChunk+offset_dbl, _rcvbuf+ chunk* NumDblsPerChunk+offset_dbl, 
                G_Srcs(31)+chunk*NumDblsPerChunk+offset_dbl, NumDblsPerChunk, _opcode);

                }
                else
                {
                MATH_16WAY(_rcvbuf, G_Srcs(0),G_Srcs(2),G_Srcs(4),G_Srcs(6),G_Srcs(8),G_Srcs(10),G_Srcs(12),G_Srcs(14),G_Srcs(16),
                            G_Srcs(18),G_Srcs(20),G_Srcs(22),G_Srcs(24),G_Srcs(26),G_Srcs(28),G_Srcs(30), ((bytes%ChunkSize)/sizeof(double)));
                MATH_16WAY(_rcvbuf, _rcvbuf, G_Srcs(1),G_Srcs(3),G_Srcs(5),G_Srcs(7),G_Srcs(9),G_Srcs(11),G_Srcs(13),G_Srcs(15),
                            G_Srcs(17), G_Srcs(19),G_Srcs(21),G_Srcs(23),G_Srcs(25),G_Srcs(27),G_Srcs(29), ((bytes%ChunkSize)/sizeof(double)));
                quad_double_math_2way(_rcvbuf+ chunk* NumDblsPerChunk+offset_dbl, _rcvbuf+ chunk* NumDblsPerChunk+offset_dbl, 
                G_Srcs(31)+chunk*NumDblsPerChunk+offset_dbl, ((bytes%ChunkSize)/sizeof(double)), _opcode);
                }
                ppc_msync();
                Memory::sync();
                _controlB->chunk_done[_local_rank] = chunk;
              }
            }

            //The buffer is partitioned among "npeers" in a balanced manner depending on the "MinChunkSize", total_bytes
            // Used for short to medium messages

            inline  bool  get_partition_info(unsigned npeers, unsigned local_rank, unsigned total_bytes, unsigned* offset_b,
                unsigned* chunk_size_b)
            {

              if (npeers == 64)
              {
                if (local_rank%4 != 0)
                  return false;
                else  
                {
                  npeers  = 16;
                  local_rank  /= 4;
                } 
              }
              else if (npeers == 32)
              {
                if (local_rank%2 != 0)
                  return false;
                else  
                {
                  npeers  = 16;
                  local_rank  /= 2;
                } 
              }
              
              unsigned num_min_chunks = total_bytes/MinChunkSize;
              unsigned end_bytes  = total_bytes%MinChunkSize;
              unsigned  rounds =  num_min_chunks/npeers; 
              unsigned remaining_chunks = num_min_chunks%npeers;

                  
              if (local_rank < remaining_chunks)
              {
                *chunk_size_b= (rounds+1)*MinChunkSize;
                *offset_b = (rounds+1)*MinChunkSize*local_rank;
                if ((local_rank == remaining_chunks-1) && (rounds == 0)) 
                  *chunk_size_b += end_bytes;
                return true;
              }
              else
              {
                if (rounds == 0) return false;
                *chunk_size_b= rounds*MinChunkSize;
                *offset_b = rounds*MinChunkSize*local_rank + MinChunkSize*remaining_chunks;
                if (local_rank == npeers-1)  
                  *chunk_size_b += end_bytes;
                return true;
              }
              
            }

          public:

            //Very short multicast..synchronization is done via a flag
            inline static pami_result_t very_short_msg_multicast(CNShmemDesc* my_desc, PipeWorkQueue *dpwq, unsigned total_bytes, unsigned npeers, unsigned local_rank,
                                                                   uint64_t* counter_addr, uint64_t &counter_curr)
            {
              void* buf = (void*)my_desc->get_buffer();
              void* rcvbuf = dpwq->bufferToProduce();

              if (local_rank == 0)
              {
                //if (*counter_addr == counter_curr)
                if (*counter_addr != 0)
                  return PAMI_EAGAIN;

                Memory::sync();
                my_desc->signal_flag();
              }
              else
              {
                if (my_desc->get_flag() == 0) 
                  return PAMI_EAGAIN;
              }

              memcpy(rcvbuf, buf, total_bytes);
              return PAMI_SUCCESS;
            }

            //Short multicast..data is taken from a shared buffer after CN deposits into it, supports pipelining
            inline pami_result_t short_msg_multicast(unsigned total_bytes, unsigned npeers, unsigned local_rank, uint64_t* counter_addr, uint64_t &counter_curr)
            {
              uint64_t  bytes_arrived =0;
              void* buf = (void*)_my_desc->get_buffer();
              unsigned bytes_so_far = total_bytes - counter_curr;

              if (local_rank == 0)
              {
                if (*counter_addr == counter_curr)
                  return PAMI_EAGAIN;

                bytes_arrived = counter_curr - *counter_addr;
                Memory::sync();
                _controlB->bytes_incoming+= (unsigned)bytes_arrived;

                //memcpy((char*)_rcvbuf + bytes_so_far, (char*)buf + bytes_so_far, bytes_arrived);
                Core_memcpy((char*)_rcvbuf + bytes_so_far, (char*)buf + bytes_so_far, bytes_arrived);
              }
              else
              {
                if (_controlB->bytes_incoming > bytes_so_far)
                {
                  bytes_arrived = _controlB->bytes_incoming - bytes_so_far;
                  //memcpy((char*)_rcvbuf + bytes_so_far, (char*)buf + bytes_so_far, bytes_arrived);
                  Core_memcpy((char*)_rcvbuf + bytes_so_far, (char*)buf + bytes_so_far, bytes_arrived);
                }
              }

              counter_curr -= bytes_arrived;
              if (counter_curr)
                return PAMI_EAGAIN;

              return PAMI_SUCCESS;
            }

            // Large message multicast..data is read directly from the master's receive buffer. supports pipelining
            inline pami_result_t large_msg_multicast(unsigned total_bytes, unsigned npeers, unsigned local_rank, volatile uint64_t* counter_addr, uint64_t &counter_curr)
            {
              //uint64_t  bytes_arrived =0;
              //void* buf = (void*)G_Dsts(0);
              //unsigned bytes_so_far = total_bytes - counter_curr;

              /* Non blocking until all the peers arrive at the collective */
              if (_my_desc->arrived_peers() != (unsigned) npeers)
              {
                TRACE_ERR((stderr,"arrived_peers:%u waiting for:%u\n", _my_desc->arrived_peers(), (unsigned) npeers));
                return PAMI_EAGAIN;
              }

              uint64_t  bytes_arrived =0;
              void* buf = (void*)G_Dsts(0);
              unsigned bytes_so_far = total_bytes - counter_curr;

              if (local_rank == 0)
              {
                if (*counter_addr == counter_curr)
                  if (counter_curr != 0) 
                    return PAMI_EAGAIN;

                bytes_arrived = counter_curr - *counter_addr;
                Memory::sync();
                _controlB->bytes_incoming+= (unsigned)bytes_arrived;
                counter_curr -= bytes_arrived;
              }
              else
              {
                bytes_arrived = _controlB->bytes_incoming - bytes_so_far;
                
                if (bytes_arrived > 0)
                {
                  Core_memcpy((char*)_rcvbuf + bytes_so_far, (char*)buf + bytes_so_far, bytes_arrived);
                  //quad_double_copy((double*)((char*)_rcvbuf + bytes_so_far), (double*)((char*)buf + bytes_so_far), bytes_arrived/sizeof(double));
                  counter_curr -= bytes_arrived;
                }
              }

              if (counter_curr)
                return PAMI_EAGAIN;
                
              _my_desc->signal_done();
              if (local_rank == 0)
                while (_my_desc->in_use()){};

              return PAMI_SUCCESS;
            }

            // Combining very short messages..the messages are fit into L2 cachelines to minimise the number of L2 loads
            inline static pami_result_t very_short_msg_combine(CNShmemDesc *my_desc, unsigned total_bytes, pami_op opcode, pami_dt dt,  unsigned npeers, unsigned local_rank,
                                                              bool& done_flag)
            {

              if (my_desc->arrived_peers() != npeers)
                return PAMI_EAGAIN;


              if (local_rank == 0)
              {
            
                if (total_bytes <= 32)
                {
                  char* shm_buf = (char*)(my_desc->get_buffer());

                  if (npeers == 2)
                  {
                    bgq_math_2way( shm_buf, shm_buf, shm_buf+total_bytes, total_bytes,opcode, dt); 
                  }
                  else if (npeers == 4)
                  {
                    bgq_math_4way( shm_buf, shm_buf, shm_buf+total_bytes, shm_buf+2*total_bytes, shm_buf+3*total_bytes, total_bytes,opcode, dt); 
                  }
                  else if (npeers == 8)
                  {
                    bgq_math_8way( shm_buf, shm_buf, shm_buf+total_bytes, shm_buf+2*total_bytes, shm_buf+3*total_bytes,
                        shm_buf+4*total_bytes,  shm_buf+5*total_bytes,shm_buf+6*total_bytes,shm_buf+7*total_bytes, total_bytes, opcode, dt); 
                  }
                  else if (npeers == 16)
                  {
                    bgq_math_16way( shm_buf, shm_buf, shm_buf+total_bytes, shm_buf+2*total_bytes, shm_buf+3*total_bytes,
                        shm_buf+4*total_bytes,  shm_buf+5*total_bytes,shm_buf+6*total_bytes,shm_buf+7*total_bytes, shm_buf+8*total_bytes,
                        shm_buf+9*total_bytes,shm_buf+10*total_bytes,  shm_buf+11*total_bytes, shm_buf+12*total_bytes,  
                        shm_buf+13*total_bytes, shm_buf+14*total_bytes, shm_buf+15*total_bytes, total_bytes, opcode, dt); 
                  }
                  else if (npeers == 32)
                  {
                    bgq_math_16way( shm_buf, shm_buf, shm_buf+total_bytes, shm_buf+2*total_bytes, shm_buf+3*total_bytes,
                        shm_buf+4*total_bytes,  shm_buf+5*total_bytes,shm_buf+6*total_bytes,shm_buf+7*total_bytes, shm_buf+8*total_bytes,
                        shm_buf+9*total_bytes,shm_buf+10*total_bytes,  shm_buf+11*total_bytes, shm_buf+12*total_bytes,  
                        shm_buf+13*total_bytes, shm_buf+14*total_bytes, shm_buf+15*total_bytes, total_bytes, opcode, dt); 
                    bgq_math_16way( shm_buf+16*total_bytes, shm_buf+16*total_bytes, shm_buf+17*total_bytes, shm_buf+18*total_bytes, shm_buf+19*total_bytes,
                        shm_buf+20*total_bytes,  shm_buf+21*total_bytes,shm_buf+22*total_bytes,shm_buf+23*total_bytes, shm_buf+24*total_bytes,
                        shm_buf+25*total_bytes,shm_buf+26*total_bytes,  shm_buf+27*total_bytes, shm_buf+28*total_bytes,  
                        shm_buf+29*total_bytes, shm_buf+30*total_bytes, shm_buf+31*total_bytes, total_bytes, opcode, dt); 
                    bgq_math_2way( shm_buf, shm_buf, shm_buf+16*total_bytes, total_bytes,opcode, dt); 
                  }
                  else if (npeers == 64)
                  {
                    bgq_math_16way( shm_buf, shm_buf, shm_buf+total_bytes, shm_buf+2*total_bytes, shm_buf+3*total_bytes,
                        shm_buf+4*total_bytes,  shm_buf+5*total_bytes,shm_buf+6*total_bytes,shm_buf+7*total_bytes, shm_buf+8*total_bytes,
                        shm_buf+9*total_bytes,shm_buf+10*total_bytes,  shm_buf+11*total_bytes, shm_buf+12*total_bytes,  
                        shm_buf+13*total_bytes, shm_buf+14*total_bytes, shm_buf+15*total_bytes, total_bytes, opcode, dt); 
                    bgq_math_16way( shm_buf+16*total_bytes, shm_buf+16*total_bytes, shm_buf+17*total_bytes, shm_buf+18*total_bytes, shm_buf+19*total_bytes,
                        shm_buf+20*total_bytes,  shm_buf+21*total_bytes,shm_buf+22*total_bytes,shm_buf+23*total_bytes, shm_buf+24*total_bytes,
                        shm_buf+25*total_bytes,shm_buf+26*total_bytes,  shm_buf+27*total_bytes, shm_buf+28*total_bytes,  
                        shm_buf+29*total_bytes, shm_buf+30*total_bytes, shm_buf+31*total_bytes, total_bytes, opcode, dt); 
                    bgq_math_16way( shm_buf+32*total_bytes, shm_buf+32*total_bytes, shm_buf+33*total_bytes, shm_buf+34*total_bytes, shm_buf+35*total_bytes,
                        shm_buf+36*total_bytes,  shm_buf+37*total_bytes,shm_buf+38*total_bytes,shm_buf+39*total_bytes, shm_buf+40*total_bytes,
                        shm_buf+41*total_bytes,shm_buf+42*total_bytes,  shm_buf+43*total_bytes, shm_buf+44*total_bytes,  
                        shm_buf+45*total_bytes, shm_buf+46*total_bytes, shm_buf+47*total_bytes, total_bytes, opcode, dt); 
                    bgq_math_16way( shm_buf+48*total_bytes, shm_buf+48*total_bytes, shm_buf+49*total_bytes, shm_buf+50*total_bytes, shm_buf+51*total_bytes,
                        shm_buf+52*total_bytes,  shm_buf+53*total_bytes,shm_buf+54*total_bytes,shm_buf+55*total_bytes, shm_buf+56*total_bytes,
                        shm_buf+57*total_bytes,shm_buf+58*total_bytes,  shm_buf+59*total_bytes, shm_buf+60*total_bytes,  
                        shm_buf+61*total_bytes, shm_buf+62*total_bytes, shm_buf+63*total_bytes, total_bytes, opcode, dt); 
                    bgq_math_4way( shm_buf, shm_buf, shm_buf+16*total_bytes, shm_buf+32*total_bytes, shm_buf+48*total_bytes, total_bytes,opcode, dt); 
                  }
                  else
                  {
                    PAMI_abortf("%s:%u npeers %u math not yet supported\n",__FILE__,__LINE__,npeers);
                  }
                }
                else
                {
                  if (npeers == 2)
                  {
                    bgq_math_2way( C_Bufs(0), C_Bufs(0), C_Bufs(1), total_bytes, opcode, dt); 
                  }
                  else if (npeers == 4)
                  {
                    bgq_math_4way( C_Bufs(0), C_Bufs(0), C_Bufs(1), C_Bufs(2), C_Bufs(3), total_bytes, opcode, dt); 
                  }
                  else if (npeers == 8)
                  {
                    bgq_math_8way( C_Bufs(0), C_Bufs(0), C_Bufs(1), C_Bufs(2), C_Bufs(3), C_Bufs(4), C_Bufs(5), C_Bufs(6), C_Bufs(7),  total_bytes, opcode, dt); 
                  }
                  else if (npeers == 16)
                  {
                    bgq_math_16way( C_Bufs(0), C_Bufs(0), C_Bufs(1), C_Bufs(2), C_Bufs(3), C_Bufs(4), C_Bufs(5), C_Bufs(6), C_Bufs(7), C_Bufs(8), C_Bufs(9), C_Bufs(10), C_Bufs(11), C_Bufs(12), C_Bufs(13), C_Bufs(14), C_Bufs(15), total_bytes, opcode, dt); 
                  }
                  else if (npeers == 32)
                  {
                    bgq_math_16way( C_Bufs(0), C_Bufs(0), C_Bufs(1), C_Bufs(2), C_Bufs(3), C_Bufs(4), C_Bufs(5), C_Bufs(6), C_Bufs(7), C_Bufs(8), C_Bufs(9), C_Bufs(10), C_Bufs(11), C_Bufs(12), C_Bufs(13), C_Bufs(14), C_Bufs(15), total_bytes, opcode, dt); 
                    bgq_math_16way( C_Bufs(16), C_Bufs(16), C_Bufs(17), C_Bufs(18), C_Bufs(19), C_Bufs(20), C_Bufs(21), C_Bufs(22), C_Bufs(23), C_Bufs(24), C_Bufs(25), C_Bufs(26), C_Bufs(27), C_Bufs(28), C_Bufs(29), C_Bufs(30), C_Bufs(31), total_bytes, opcode, dt); 
                    bgq_math_2way( C_Bufs(0), C_Bufs(0), C_Bufs(16), total_bytes, opcode, dt); 
                  }
                  else if (npeers == 64)
                  {
                    bgq_math_16way( C_Bufs(0), C_Bufs(0), C_Bufs(1), C_Bufs(2), C_Bufs(3), C_Bufs(4), C_Bufs(5), C_Bufs(6), C_Bufs(7), C_Bufs(8), C_Bufs(9), C_Bufs(10), C_Bufs(11), C_Bufs(12), C_Bufs(13), C_Bufs(14), C_Bufs(15), total_bytes, opcode, dt); 
                    bgq_math_16way( C_Bufs(16), C_Bufs(16), C_Bufs(17), C_Bufs(18), C_Bufs(19), C_Bufs(20), C_Bufs(21), C_Bufs(22), C_Bufs(23), C_Bufs(24), C_Bufs(25), C_Bufs(26), C_Bufs(27), C_Bufs(28), C_Bufs(29), C_Bufs(30), C_Bufs(31), total_bytes, opcode, dt); 
                    bgq_math_16way( C_Bufs(32), C_Bufs(32), C_Bufs(33), C_Bufs(34), C_Bufs(35), C_Bufs(36), C_Bufs(37), C_Bufs(38), C_Bufs(39), C_Bufs(40), C_Bufs(41), C_Bufs(42), C_Bufs(43), C_Bufs(44), C_Bufs(45), C_Bufs(46), C_Bufs(47), total_bytes, opcode, dt); 
                    bgq_math_16way( C_Bufs(48), C_Bufs(48), C_Bufs(49), C_Bufs(50), C_Bufs(51), C_Bufs(52), C_Bufs(53), C_Bufs(54), C_Bufs(55), C_Bufs(56), C_Bufs(57), C_Bufs(58), C_Bufs(59), C_Bufs(60), C_Bufs(61), C_Bufs(62), C_Bufs(63), total_bytes, opcode, dt); 
                    bgq_math_4way( C_Bufs(0), C_Bufs(0), C_Bufs(16), C_Bufs(32), C_Bufs(48), total_bytes, opcode, dt); 
                  }
                  else
                  {
                    PAMI_abortf("%s:%u npeers %u sum not yet supported\n",__FILE__,__LINE__,npeers);
                  }

                }

              }
                done_flag = true;
              return PAMI_SUCCESS;
            }

            // multicombine with reads from the global virtual addresses of the buffers and writes to a shmem buffer
            // which is then injected into the Collective network
            inline pami_result_t short_msg_combine(unsigned total_bytes, unsigned npeers, unsigned local_rank, bool& done_flag)
            {

              /* Non blocking until all the peers arrive at the collective */
              if (_my_desc->arrived_peers() != npeers)
                return PAMI_EAGAIN;

              unsigned offset_b=0, chunk_size_b=0, offset_dbl, chunk_size_dbl;
              bool  is_participant;
              is_participant  = get_partition_info(npeers, local_rank, total_bytes, &offset_b, &chunk_size_b);
              offset_dbl  = offset_b >> 3;
              chunk_size_dbl = chunk_size_b >> 3;

              if (npeers == 2)
              {
                if (is_participant)
                  quad_double_math_2way((double*)_controlB->buffer+ offset_dbl, G_Srcs(0)+offset_dbl, 
                      G_Srcs(1)+offset_dbl, chunk_size_dbl, _opcode);
              }
              else if (npeers == 4)
              {
                if (is_participant)
                  quad_double_math_4way((double*)_controlB->buffer+ offset_dbl, G_Srcs(0)+offset_dbl, 
                      G_Srcs(1)+offset_dbl, G_Srcs(2)+offset_dbl, G_Srcs(3)+offset_dbl, chunk_size_dbl, _opcode);
              }
              else if (npeers == 8)
              {
                if (is_participant)
                  quad_double_math_8way((double*)_controlB->buffer+ offset_dbl, G_Srcs(0)+offset_dbl, 
                      G_Srcs(1)+offset_dbl, G_Srcs(2)+offset_dbl, G_Srcs(3)+offset_dbl, G_Srcs(4)+offset_dbl,
                      G_Srcs(5)+offset_dbl, G_Srcs(6)+offset_dbl, G_Srcs(7)+offset_dbl, chunk_size_dbl, _opcode);
              }
              else if (npeers == 16)
              {
                if (is_participant)
                {
                  quad_double_math_16way((double*)_controlB->buffer+ offset_dbl, G_Srcs(0)+offset_dbl, 
                      G_Srcs(1)+offset_dbl, G_Srcs(2)+offset_dbl, G_Srcs(3)+offset_dbl, G_Srcs(4)+offset_dbl,
                      G_Srcs(5)+offset_dbl, G_Srcs(6)+offset_dbl, G_Srcs(7)+offset_dbl, G_Srcs(8)+offset_dbl,
                      G_Srcs(9)+offset_dbl, G_Srcs(10)+offset_dbl, G_Srcs(11)+offset_dbl, G_Srcs(12)+offset_dbl,
                      G_Srcs(13)+offset_dbl, G_Srcs(14)+offset_dbl, G_Srcs(15)+offset_dbl, chunk_size_dbl, _opcode);
                }
              }
              else if (npeers == 32)
              {
                if (is_participant)
                {
                  quad_double_math_16way((double*)_controlB->buffer+ offset_dbl, G_Srcs(0)+offset_dbl, 
                      G_Srcs(1)+offset_dbl, G_Srcs(2)+offset_dbl, G_Srcs(3)+offset_dbl, G_Srcs(4)+offset_dbl,
                      G_Srcs(5)+offset_dbl, G_Srcs(6)+offset_dbl, G_Srcs(7)+offset_dbl, G_Srcs(8)+offset_dbl,
                      G_Srcs(9)+offset_dbl, G_Srcs(10)+offset_dbl, G_Srcs(11)+offset_dbl, G_Srcs(12)+offset_dbl,
                      G_Srcs(13)+offset_dbl, G_Srcs(14)+offset_dbl, G_Srcs(15)+offset_dbl, chunk_size_dbl, _opcode);
                  quad_double_math_16way((double*)_controlB->buffer+ offset_dbl, (double*)_controlB->buffer+ offset_dbl, 
                      G_Srcs(16)+offset_dbl, G_Srcs(17)+offset_dbl, G_Srcs(18)+offset_dbl, G_Srcs(19)+offset_dbl, 
                      G_Srcs(20)+offset_dbl, G_Srcs(21)+offset_dbl, G_Srcs(22)+offset_dbl, G_Srcs(23)+offset_dbl, 
                      G_Srcs(24)+offset_dbl, G_Srcs(25)+offset_dbl, G_Srcs(26)+offset_dbl, G_Srcs(27)+offset_dbl, 
                      G_Srcs(28)+offset_dbl, G_Srcs(29)+offset_dbl, G_Srcs(30)+offset_dbl, chunk_size_dbl, _opcode);
                  quad_double_math_2way((double*)_controlB->buffer+ offset_dbl, (double*)_controlB->buffer+ offset_dbl,
                      G_Srcs(31)+offset_dbl, chunk_size_dbl, _opcode);
                }
              }
              else if (npeers == 64)
              {
                if (is_participant)
                {
                  quad_double_math_16way((double*)_controlB->buffer+ offset_dbl, G_Srcs(0)+offset_dbl, 
                      G_Srcs(1)+offset_dbl, G_Srcs(2)+offset_dbl, G_Srcs(3)+offset_dbl, G_Srcs(4)+offset_dbl,
                      G_Srcs(5)+offset_dbl, G_Srcs(6)+offset_dbl, G_Srcs(7)+offset_dbl, G_Srcs(8)+offset_dbl,
                      G_Srcs(9)+offset_dbl, G_Srcs(10)+offset_dbl, G_Srcs(11)+offset_dbl, G_Srcs(12)+offset_dbl,
                      G_Srcs(13)+offset_dbl, G_Srcs(14)+offset_dbl, G_Srcs(15)+offset_dbl, chunk_size_dbl, _opcode);
                  quad_double_math_16way((double*)_controlB->buffer+ offset_dbl, (double*)_controlB->buffer+ offset_dbl, 
                      G_Srcs(16)+offset_dbl, G_Srcs(17)+offset_dbl, G_Srcs(18)+offset_dbl, G_Srcs(19)+offset_dbl, 
                      G_Srcs(20)+offset_dbl, G_Srcs(21)+offset_dbl, G_Srcs(22)+offset_dbl, G_Srcs(23)+offset_dbl, 
                      G_Srcs(24)+offset_dbl, G_Srcs(25)+offset_dbl, G_Srcs(26)+offset_dbl, G_Srcs(27)+offset_dbl, 
                      G_Srcs(28)+offset_dbl, G_Srcs(29)+offset_dbl, G_Srcs(30)+offset_dbl, chunk_size_dbl, _opcode);
                  quad_double_math_16way((double*)_controlB->buffer+ offset_dbl, (double*)_controlB->buffer+ offset_dbl, 
                      G_Srcs(31)+offset_dbl, G_Srcs(32)+offset_dbl, G_Srcs(33)+offset_dbl, G_Srcs(34)+offset_dbl, 
                      G_Srcs(35)+offset_dbl, G_Srcs(36)+offset_dbl, G_Srcs(37)+offset_dbl, G_Srcs(38)+offset_dbl, 
                      G_Srcs(39)+offset_dbl, G_Srcs(40)+offset_dbl, G_Srcs(41)+offset_dbl, G_Srcs(42)+offset_dbl, 
                      G_Srcs(43)+offset_dbl, G_Srcs(44)+offset_dbl, G_Srcs(45)+offset_dbl, chunk_size_dbl, _opcode);
                  quad_double_math_16way((double*)_controlB->buffer+ offset_dbl, (double*)_controlB->buffer+ offset_dbl, 
                      G_Srcs(46)+offset_dbl, G_Srcs(47)+offset_dbl, G_Srcs(48)+offset_dbl, G_Srcs(49)+offset_dbl, 
                      G_Srcs(50)+offset_dbl, G_Srcs(51)+offset_dbl, G_Srcs(52)+offset_dbl, G_Srcs(53)+offset_dbl, 
                      G_Srcs(54)+offset_dbl, G_Srcs(55)+offset_dbl, G_Srcs(56)+offset_dbl, G_Srcs(57)+offset_dbl, 
                      G_Srcs(58)+offset_dbl, G_Srcs(59)+offset_dbl, G_Srcs(60)+offset_dbl, chunk_size_dbl, _opcode);
                  quad_double_math_4way((double*)_controlB->buffer+ offset_dbl, (double*)_controlB->buffer+ offset_dbl,
                      G_Srcs(61)+offset_dbl, G_Srcs(62)+offset_dbl, G_Srcs(63)+offset_dbl, chunk_size_dbl, _opcode);
                }
              }
              else
              {
                PAMI_abortf("%s:%u npeers %u math not yet supported\n",__FILE__,__LINE__,npeers);
              }

              _my_desc->signal_done();
              while (_my_desc->in_use()){};

              done_flag = true;
              return PAMI_SUCCESS;
            }

            //Only the non-master peers participate in this..data is combined directly in chunks, from the peers' buffers using
            //Global Virtual addresses on the node and each peer updates its chunk index once done
            inline pami_result_t large_msg_combine_peers (size_t total_bytes, unsigned npeers, unsigned local_rank, unsigned offset_dbl)
            {
              /* Non blocking until all the peers arrive at the collective */
              if (_my_desc->arrived_peers() != (unsigned) npeers)
              {
                TRACE_ERR((stderr,"arrived_peers:%u waiting for:%u\n", _my_desc->arrived_peers(), (unsigned) npeers));
                return PAMI_EAGAIN;
              }


              /* All nodes except master(local rank 0), do the math */
              if (npeers == 2)
                advance_2way_math(local_rank, npeers, total_bytes, offset_dbl);
              else if (npeers == 4)
                advance_4way_math(local_rank, npeers, total_bytes, offset_dbl);
              else if (npeers == 8)
                advance_8way_math(local_rank, npeers, total_bytes, offset_dbl);
              else if (npeers == 16)
                //(total_bytes == 16384) ?  advance_16way_math_16k(local_rank, npeers):
                //(total_bytes == 8192) ?  advance_16way_math_16k(local_rank, npeers):
                ((total_bytes == 32768)||(total_bytes == 16384)||(total_bytes == 8192))
                                         ?  advance_16way_math_16k(local_rank, npeers, ((total_bytes-4096) >> 10)+9):
                                        advance_16way_math(local_rank, npeers, total_bytes, offset_dbl);
              else if (npeers == 32)
                advance_32way_math((local_rank/2), 16, total_bytes, offset_dbl);
              else if (npeers == 64)
                advance_64way_math((local_rank/4), 16, total_bytes, offset_dbl);
              else
              {
                PAMI_abortf("%s:%u npeers %u math not yet supported\n",__FILE__,__LINE__,npeers);
              }

              return PAMI_SUCCESS;
            }

#if 0
            //The master process gets the next chunk to inject into Collective network
            inline void* next_injection_buffer (uint64_t *bytes_available, unsigned total_bytes, unsigned npeers)
            {
              unsigned  my_peer = (_chunk_for_injection)%(npeers-1)+1;
              char* rbuf = (char*) P_Dsts(my_peer);

              if (_controlB->chunk_done[my_peer] >= _chunk_for_injection)
              { 
                *bytes_available  = ((unsigned)_chunk_for_injection < (NumChunks(total_bytes)-1)) ? (ChunkSize):
                  (total_bytes - ChunkSize*(NumChunks(total_bytes)-1));
                return (void*)(rbuf + _chunk_for_injection*ChunkSize);
              }
              return  NULL;  
            }
            //The master process gets the next chunk to inject into Collective network
            inline void* next_injection_buffer (uint64_t *bytes_available, unsigned total_bytes, unsigned npeers)
            {
              unsigned  my_peer = (_chunk_for_injection)%(npeers-1)+1;
              char* rbuf = (char*) P_Dsts(my_peer) + _cur_offset;

              if (_controlB->chunk_done[my_peer] >= _chunk_for_injection)
              { 
                //printf("_cur_offset:%d\n", _cur_offset);
                *bytes_available  = _chunk_array[_chunk_for_injection];
                _cur_offset+= *bytes_available;
                return (void*)(rbuf);
              }
              return  NULL;  
            }
#endif
            //The master process gets the next chunk to inject into Collective network
            inline void* next_injection_buffer (uint64_t *bytes_available, unsigned total_bytes, unsigned npeers)
            {
            
              //unsigned  my_peer = (_chunk_for_injection)%(npeers-1)+1;
              //char* rbuf = (char*) P_Dsts(my_peer) + _cur_offset;
              unsigned  my_peer;
              //my_peer = (npeers != 64) ? (_chunk_for_injection)%(npeers-1)+1 : (_chunk_for_injection)%(16-1)+1;
              my_peer = ((npeers != 64) && (npeers != 32)) ? (_chunk_for_injection)%(npeers-1)+1 : (_chunk_for_injection)%(16-1)+1;
              char* rbuf;
              //rbuf = (npeers != 64) ? ((char*) P_Dsts(my_peer) + _cur_offset):((char*) P_Dsts(my_peer*4) + _cur_offset);
              rbuf = ((npeers != 64) && (npeers != 32)) ? ((char*) P_Dsts(my_peer) + _cur_offset):
                     ((npeers == 32) ? ((char*) P_Dsts(my_peer*2)+ _cur_offset) : ((char*) P_Dsts(my_peer*4) + _cur_offset));

              if (_controlB->chunk_done[my_peer] >= _chunk_for_injection)
              { 
                //printf("_cur_offset:%d\n", _cur_offset);
                //if (total_bytes == 16384)
                //if (total_bytes == 8192)
                if (((total_bytes == 32768) || (total_bytes == 16384) || (total_bytes == 8192)) && (npeers == 16))
                {  
                  *bytes_available  = _chunk_array[_chunk_for_injection];
                  _cur_offset+= *bytes_available;
                  return (void*)(rbuf);
                }
                else
                {
                  *bytes_available  = ((unsigned)_chunk_for_injection < (NumChunks(total_bytes)-1)) ? (ChunkSize):
                    (total_bytes - ChunkSize*(NumChunks(total_bytes)-1));
                  return (void*)(rbuf + _chunk_for_injection*ChunkSize);
                }
              }
              return  NULL;  
            }

            //Update the chunk index once injection is complete
            inline  void  injection_complete() { ++_chunk_for_injection;  }


            inline CNShmemMessage () {};
            inline CNShmemMessage (CNShmemDesc* my_desc, uint32_t length):_my_desc(my_desc),_total_bytes(length),_chunk_for_injection(0) {};

            inline ~CNShmemMessage() {};

            //Initialize all the Virtual,Global and Physical addreses required in the operation
            //Initialize the shmem descriptor used for staging data and synchronization
            inline void init (void* srcbuf, void* rcvbuf, void* srcbuf_gva, void* rcvbuf_gva, void* rcvbuf_phy, void* shmbuf_phy, unsigned local_rank, pami_op opcode, pami_dt  dt)
            {

              _srcbuf = (double*)srcbuf;
              _rcvbuf = (double*)rcvbuf;
              _opcode = opcode;
              _dt     = dt;

              _in_place = 0;
              if (_srcbuf == _rcvbuf)
                _in_place = 1;

              void* buf = _my_desc->get_buffer();
              _controlB = (ControlBlock*)buf;

              _controlB->GAT.srcbufs[local_rank] = srcbuf_gva;
              _controlB->GAT.dstbufs[local_rank] = rcvbuf_gva;
              _controlB->phybufs[local_rank] = rcvbuf_phy;
              _shm_phy_addr = shmbuf_phy;

              if (local_rank == 0)
              {
                _controlB->bytes_incoming=0;
                for (unsigned i=0; i < __global.mapping.tSize(); i++)
                {
                  _controlB->chunk_done[i] = -1;
                }
              }
#if 1
              _chunk_array[0] = 256;
              _chunk_array[1] = 256;
              _chunk_array[2] = 512;
              _chunk_array[3] = 512;
              _chunk_array[4] = 512;
              _chunk_array[5] = 512;
              _chunk_array[6] = 512;
              _chunk_array[7] = 512;
              _chunk_array[8] = 512;
              /*_chunk_array[9] = 1024;
              _chunk_array[10] = 1024;
              _chunk_array[11] = 1024;
              _chunk_array[12] = 1024;
              _chunk_array[13] = 1024;
              _chunk_array[14] = 1024;
              _chunk_array[15] = 1024;
              _chunk_array[16] = 1024;
              _chunk_array[17] = 1024;
              _chunk_array[18] = 1024;
              _chunk_array[19] = 1024;
              _chunk_array[20] = 1024;*/
            
                for (unsigned i=9; i< Num32kChunks; i++)
                  _chunk_array[i] = 1024;
#endif
              /*unsigned total_buf;
                for (unsigned i=0; i< Num16kChunks; i++)
                  total_buf+=_chunk_array[i];
              assert(total_buf == 16384);*/
              _cur_offset = 0;

            };

            void*     _shm_phy_addr;
          private:

            CNShmemDesc    *_my_desc;
            unsigned  _total_bytes;
            double*     _srcbuf;
            double*     _rcvbuf;
            int32_t _chunk_for_injection;
            ControlBlock* _controlB;
            pami_op _opcode;
            pami_dt _dt;
    
            //unsigned  _chunk_array[Num16kChunks];
            unsigned  _chunk_array[Num32kChunks];
            unsigned  _cur_offset;
            unsigned  _in_place;


        };  // PAMI::Device::CNShmemMessage class
    };
  };    // PAMI::Device namespace
};      // PAMI namespace

#undef TRACE_ERR
#endif  // __components_devices_shmem_CNShmemMessage_h__

//
// astyle info    http://astyle.sourceforge.net
//
// astyle options --style=gnu --indent=spaces=2 --indent-classes
// astyle options --indent-switches --indent-namespaces --break-blocks
// astyle options --pad-oper --keep-one-line-blocks --max-instatement-indent=79
//
