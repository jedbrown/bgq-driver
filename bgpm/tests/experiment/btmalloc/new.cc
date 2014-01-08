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
/* (C) Copyright IBM Corp.  2012, 2012                              */
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

#include <new>
#include <malloc.h>

// Replace Global New and Delete for debug???
void* operator new (std::size_t size) throw(std::bad_alloc) { return malloc(size); }

void operator delete (void* ptr) throw()  { free(ptr); }


void* operator new (std::size_t size,const std::nothrow_t& t) throw() { return malloc(size); }

void operator delete (void* ptr, const std::nothrow_t&t ) throw() { free(ptr); }


void* operator new  [](std::size_t size) throw(std::bad_alloc) { return malloc(size); }

void operator delete[](void* ptr) throw() { free(ptr); }


//array, nothrow new and matching delete[]
void* operator new [](std::size_t size, const std::nothrow_t&t) throw() { return malloc(size); }

void operator delete[](void* ptr, const std::nothrow_t&t) throw() { free(ptr); }

