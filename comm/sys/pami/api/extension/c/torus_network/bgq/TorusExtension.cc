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
 * \file api/extension/c/torus_network/bgq/TorusExtension.cc
 * \brief ???
 */
#include "api/extension/c/torus_network/TorusExtension.h"

#include "Global.h"

PAMI::TorusExtension::torus_info_t PAMI::TorusExtension::info;
size_t PAMI::TorusExtension::coords[1024];


PAMI::TorusExtension::TorusExtension ()
{
  size_t * ptr = coords;

  info.dims = 6;
  info.coord = ptr;
  ptr += info.dims;
  info.size  = ptr;
  ptr += info.dims;
  info.torus = ptr;
  ptr += info.dims;

  info.size[0] = __global.personality.aSize();
  info.size[1] = __global.personality.bSize();
  info.size[2] = __global.personality.cSize();
  info.size[3] = __global.personality.dSize();
  info.size[4] = __global.personality.eSize();
  info.size[5] = __global.personality.tSize();

  info.coord[0] = __global.personality.aCoord();
  info.coord[1] = __global.personality.bCoord();
  info.coord[2] = __global.personality.cCoord();
  info.coord[3] = __global.personality.dCoord();
  info.coord[4] = __global.personality.eCoord();
  info.coord[5] = __global.personality.tCoord();

  info.torus[0] = __global.personality.isTorusA();
  info.torus[1] = __global.personality.isTorusB();
  info.torus[2] = __global.personality.isTorusC();
  info.torus[3] = __global.personality.isTorusD();
  info.torus[4] = __global.personality.isTorusE();
  info.torus[5] = 1; // t coordinate is always a torus ?
}

const PAMI::TorusExtension::torus_info_t * PAMI::TorusExtension::information ()
{
  return & PAMI::TorusExtension::info;
};

pami_result_t PAMI::TorusExtension::task2torus (pami_task_t task, size_t addr[])
{
  array_t<size_t, 6> * resized = (array_t<size_t, 6> *) addr;

  // Using non-standard interface!
  return __global.mapping.task2global ((size_t) task, resized->array);
};

pami_result_t PAMI::TorusExtension::torus2task (size_t addr[], pami_task_t * task)
{
  size_t t = (size_t) - 1;
  array_t<size_t, 6> * resized = (array_t<size_t, 6> *) addr;
  pami_result_t result =
    __global.mapping.torus2task_impl (resized->array, t);
  *task = t;

  return result;
};
