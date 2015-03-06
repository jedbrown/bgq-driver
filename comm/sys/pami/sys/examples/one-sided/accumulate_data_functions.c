
/* ------------------------------------------------------------------------- */
/* Licensed Materials - Property of IBM                                      */
/* Blue Gene/Q 5765-PER 5765-PRP                                             */
/* © Copyright IBM Corp. 2012 All Rights Reserved                            */
/* US Government Users Restricted Rights - Use, duplication or disclosure    */
/*   restricted by GSA ADP Schedule Contract with IBM Corp.                  */
/*                                                                           */
/* This software is available to you under the Eclipse Public License (EPL). */
/* ------------------------------------------------------------------------- */


/*
 * The operations supported by Acc shall include both scalar and vector
 * addition/subtraction (e.g. y[i]+=c*x[i] and y[i]+=c[i]*x[i], where c is local
 * input), max/min (e.g. y[i]+=max(x[i],y[i])) as well as user-defined functions
 * of the form y[i]=f(x[i],y[i],..) which are atomic for each math operation in
 * buffers x and y, where y is existing buffer to be accumulated upon with
 * incoming buffer x.
 */

#include <stddef.h>
#include <math.h>

/**
 * \brief scalar double sum data function
 *
 * Implements 'y[i]+=c*x[i]'
 *
 * \see pami_data_function
 *
 * \param [in,out] target  The address of a contiguous target buffer
 * \param [in]     source  The address of a contiguous source buffer
 * \param [in]     bytes   The number of bytes to handle
 * \param [in]     cookie  A user-specified value; in this case, a pointer to
 *                         the scalar
 */
void accumulate_scalar_sum_data_function (void * target,
                                          void * source,
                                          size_t bytes,
                                          void * cookie)
{
  size_t count = bytes >> (2 + (sizeof(double)==8));

  double       * y = (double *) target;
  const double * x = (const double *) source;
  const double   c = *((const double *) cookie);

  size_t i;
  for (i=0; i<count; i++)
    y[i]+=c*x[i];

  return;
}

/**
 * \brief vector double sum data function
 *
 * Implements 'y[i]+=c[i]*x[i]'
 *
 * \see pami_data_function
 *
 * \param [in,out] target  The address of a contiguous target buffer
 * \param [in]     source  The address of a contiguous source buffer
 * \param [in]     bytes   The number of bytes to handle
 * \param [in]     cookie  A user-specified value; in this case, a pointer to
 *                         an array of multipliers
 */
void accumulate_vector_sum_data_function (void * target,
                                          void * source,
                                          size_t bytes,
                                          void * cookie)
{
  size_t count = bytes >> (2 + (sizeof(double)==8));

  double       * y = (double *) target;
  const double * x = (const double *) source;
  const double * c = (const double *) cookie;

  size_t i;
  for (i=0; i<count; i++)
    y[i]+=c[i]*x[i];

  return;
}

/**
 * \brief scalar subtract data function
 *
 * Implements 'y[i]-=c*x[i]'
 *
 * \see pami_data_function
 *
 * \param [in,out] target  The address of a contiguous target buffer
 * \param [in]     source  The address of a contiguous source buffer
 * \param [in]     bytes   The number of bytes to handle
 * \param [in]     cookie  A user-specified value; in this case, a pointer to
 *                         the scalar
 */
void accumulate_scalar_subtract_data_function (void * target,
                                               void * source,
                                               size_t bytes,
                                               void * cookie)
{
  size_t count = bytes >> (2 + (sizeof(double)==8));

  double       * y = (double *) target;
  const double * x = (const double *) source;
  const double   c = *((const double *) cookie);

  size_t i;
  for (i=0; i<count; i++)
    y[i]-=c*x[i];

  return;
}

/**
 * \brief vector double subtract data function
 *
 * Implements 'y[i]-=c[i]*x[i]'
 *
 * \see pami_data_function
 *
 * \param [in,out] target  The address of a contiguous target buffer
 * \param [in]     source  The address of a contiguous source buffer
 * \param [in]     bytes   The number of bytes to handle
 * \param [in]     cookie  A user-specified value; in this case, a pointer to
 *                         an array of multipliers
 */
void accumulate_vector_subtract_data_function (void * target,
                                               void * source,
                                               size_t bytes,
                                               void * cookie)
{
  size_t count = bytes >> (2 + (sizeof(double)==8));

  double       * y = (double *) target;
  const double * x = (const double *) source;
  const double * c = (const double *) cookie;

  size_t i;
  for (i=0; i<count; i++)
    y[i]-=c[i]*x[i];

  return;
}

/**
 * \brief vector double max sum data function
 *
 * Implements 'y[i]+=max(x[i],y[i])'
 *
 * \see pami_data_function
 *
 * \param [in,out] target  The address of a contiguous target buffer
 * \param [in]     source  The address of a contiguous source buffer
 * \param [in]     bytes   The number of bytes to handle
 * \param [in]     cookie  A user-specified value; unused
 *
 */
void accumulate_vector_max_sum_data_function (void * target,
                                              void * source,
                                              size_t bytes,
                                              void * cookie)
{
  size_t count = bytes >> (2 + (sizeof(double)==8));

  double       * y = (double *) target;
  const double * x = (const double *) source;

  size_t i;
  for (i=0; i<count; i++)
    y[i]+=fmaxf(x[i],y[i]);

  return;
}

/**
 * \brief vector double min sum data function
 *
 * Implements 'y[i]+=min(x[i],y[i])'
 *
 * \see pami_data_function
 *
 * \param [in,out] target  The address of a contiguous target buffer
 * \param [in]     source  The address of a contiguous source buffer
 * \param [in]     bytes   The number of bytes to handle
 * \param [in]     cookie  A user-specified value
 */
void accumulate_vector_min_sum_data_function (void * target,
                                              void * source,
                                              size_t bytes,
                                              void * cookie)
{
  size_t count = bytes >> (2 + (sizeof(double)==8));

  double       * y = (double *) target;
  const double * x = (const double *) source;

  size_t i;
  for (i=0; i<count; i++)
    y[i]+=fminf(x[i],y[i]);

  return;
}
