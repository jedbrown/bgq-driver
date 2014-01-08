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
/* (C) Copyright IBM Corp.  2010, 2011                              */
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
 * \file bgsched/SchedUtil.h
 * \brief SchedUtil class definition.
 */

#ifndef BGSCHED_SCHEDUTIL_H_
#define BGSCHED_SCHEDUTIL_H_

#include <bgsched/Exception.h>
#include <bgsched/Midplane.h>

#include <string>
#include <vector>

namespace bgsched {

/*!
 * \brief Generic scheduler utility functions.
 */
class SchedUtil
{
public:
    /*!
     * \brief Enumerated error codes.
     */
    struct Errors
    {
        /*!
         * \brief Error codes.
         */
        enum Value
        {
            InputVectorSizeMismatch = 1,
            VectorIndexOutOfBounds,
        };


        /*!
         * \brief Error message string.
         *
         * \return Error message string.
         */
        static std::string toString(Value v, const std::string& what);
    };

    /*!
     * \brief Exception class for scheduler utility failures.
     */
    typedef RuntimeError<Errors> Exception;

    /*!
     * \brief Create block name based on date/time algorithm and prefix.
     *
     * Rules:
     *    If prefix exceeds 16 characters only the first 16 characters will be used.
     *    If an empty string is passed then the default prefix used will be "BLOCK_".
     *    If a prefix character outside of abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789-_
     *    is specified the default prefix of "BLOCK_" will be used instead.
     *
     * \return Generated block name string.
     */
    static std::string createBlockName(
            const std::string& prefix   //!< [in] Prefix for generated name
            );

    /*!
     * \brief Iterate over all dimensions in the order given.
     *
     * This executes the function given in a series of nested loops.  The bounds
     * for the loops are given by the mins/maxes.  The values passed to the
     * function when it is called are set in the coordinates vector.
     *
     * \param[in] dimOrder Vector giving the order to iterate the dimensions.
     * The first dimension is the outermost loop, the last dimension is the
     * innermost loop. To execute the loops in left to right order of the
     * coordinates, give a vector of indices in numerical order {0,1,2,3,...}.
     * If an empty vector is given, the loops execute in left to right order.
     *
     * \param[in] dimReverse Vector of booleans indicating whether to iterate
     * the dimension in reverse. For dimensions where this value is true,
     * iteration will be from the maximum value to the minimum value. If an
     * empty vector is given, iteration will be from minimum value to maximum value.
     *
     * \param[in] mins Vector of starting values for the loops. To begin at
     * zero always, specify a vector of all zeros.
     *
     * \param[in] maxes Vector of maximum values for the loops.
     *
     * \param[in] function Pointer to the function to call in the innermost loop.
     * Function takes a vector of coordinate values and a void pointer as its
     * parameters.  The void pointer is for additional function arguments. The
     * function signature should be:
     * void function( const std::vector<uint32_t>& coordinates, void* otherArgs );
     * Early exit from the loops may be implemented by throwing an exception in the
     * function.
     *
     * \param[in] otherArgs Void pointer to any additional arguments required
     * by the function to call.
     */
    static void iterateAll(
        const std::vector<uint32_t>& dimOrder,
        const std::vector<bool>& dimReverse,
        const std::vector<uint32_t>& mins,
        const std::vector<uint32_t>& maxes,
        void(*function)(std::vector<uint32_t>,void*),
        void* otherArgs
        );

    /*!
     * \brief Iterate over all midplanes in the given extent.
     *
     * This executes the function given in a series of nested loops. The bounds
     * for the loops are given by the mins/maxes. The values passed to the
     * function when it is called are set in the coordinates vector.
     *
     * \param[in] dimOrder Vector giving the order to iterate the dimensions.
     * The first dimension is the outermost loop, the last dimension is the
     * innermost loop. To execute the loops in left to right order of the
     * coordinates, give a vector of indices in numerical order {0,1,2,3,...}.
     * If an empty vector is given, the loops execute in left to right order.
     *
     * \param[in] dimReverse Vector of booleans indicating whether to iterate
     * the dimension in reverse. For dimensions where this value is true,
     * iteration will be from the maximum value to the minimum value. If an
     * empty vector is given, iteration will be from minimum value to maximum value.
     *
     * \param[in] origin Coordinates of the starting midplane.
     *
     * \param[in] extent Coordinates of the "far corner" midplane.
     *
     * \param[in] function Pointer to the function to call in the innermost loop.
     * It takes a vector of coordinate values and a void pointer as its
     * parameters.  The void pointer is for additional function arguments. The
     * function signature should be:
     * void function( const Coordinates& coordinates, void* otherArgs );
     * Early exit from the loops may be implemented by throwing an exception in the
     * function.
     *
     * \param[in] otherArgs Void pointer to any additional arguments required
     * by the function to call.
     */
    static void iterateAllMidplanes(
            const std::vector<uint32_t>& dimOrder,
            const std::vector<bool>& dimReverse,
            const Coordinates& origin,
            const Coordinates& extent,
            void(*function)(const Coordinates&,void*),
            void* otherArgs
            );

    /*!
     * \brief Iterate over all midplanes in the order given.
     *
     * This executes the function given in a series of nested loops. The bounds
     * for the loops are given by the mins/maxes. The values passed to the
     * function when it is called are set in the coordinates vector.
     *
     * \param[in] dimOrder Vector giving the order to iterate the dimensions.
     * The first dimension is the outermost loop, the last dimension is the
     * innermost loop. To execute the loops in left to right order of the
     * coordinates, give a vector of indices in numerical order {0,1,2,3,...}.
     * If an empty vector is given, the loops execute in left to right order.
     *
     * \param[in] dimReverse Vector of booleans indicating whether to iterate
     * the dimension in reverse. For dimensions where this value is true,
     * iteration will be from the maximum value to the minimum value. If an
     * empty vector is given, iteration will be from minimum value to maximum value.
     *
     * \param[in] mins Vector of starting values for the loops. To begin at
     * zero always, specify a vector of all zeros.
     *
     * \param[in] maxes Vector of maximum values for the loops.
     *
     * \param[in] function Pointer to the function to call in the innermost loop.
     * It takes a vector of coordinate values and a void pointer as its
     * parameters. The void pointer is for additional function arguments. The
     * function signature should be:
     * void function( const Coordinates& coordinates, void* otherArgs );
     * Early exit from the loops may be implemented by throwing an exception in the
     * function.
     *
     * \param[in] otherArgs Void pointer to any additional arguments required
     * by the function to call.
     */
    static void iterateMidplanes(
        const std::vector<uint32_t>& dimOrder,
        const std::vector<bool>& dimReverse,
        const std::vector<uint32_t>& mins,
        const std::vector<uint32_t>& maxes,
        void(*function)(const Coordinates&,void*),
        void* otherArgs
        );

    /*!
     * \brief Iterate over sets of values.
     *
     * This executes the function given in a series of nested loops. The number
     * of elements in the vector is the number of nested loops. The values
     * for each iteration are given by the sets in the vector.
     *
     * \param[in] values Vector of sets of values.
     * The first set is the outermost loop, the last set is the innermost loop.
     *
     * \param[in] function Pointer to the function to call in the innermost loop.
     * It takes a set of coordinate values and a void pointer as its
     * parameters. The void pointer is for additional function arguments. The
     * function signature should be:
     * void function( const std::set<uint32_t>& coordinates, void* otherArgs );
     * Early exit from the loops may be implemented by throwing an exception in the
     * function.
     *
     * \param[in] otherArgs Void pointer to any additional arguments required
     * by the function to call.
     */
    static void iterate(
        const std::vector<std::vector<uint32_t> >& values,
        void(*function)(const std::vector<uint32_t>&,void*),
        void* otherArgs
        );

private:

    /*!
      * \brief Private method used by iterate for recursion.
      *
      * First three parameters are the same as those passed to iterate.
      *
      * \param[in] coordinates Vector of coordinate values to pass to the function in
      * the innermost loop. The values of the coordinates are the values iterated in
      * each loop.
      *
      * \param[in] dimIndex Tracks which of the nested loops we are currently
      * executing. This gives position in the values vector and ending criteria.
      */
    static void recurse(
        const std::vector<std::vector<uint32_t> >& values,
        void(*function)(const std::vector<uint32_t>&,void*),
        void* otherArgs,
        std::vector<uint32_t> coordinates,
        uint32_t dimIndex
        );

   /*!
     * \brief Private method used by iterateAll for recursion.
     *
     * First six parameters are the same as those passed to iterateAll.
     *
     * \param[in] coordinates Vector of coordinate values to pass to the function in
     * the innermost loop. The values of the coordinates are the values iterated in each loop.
     *
     * \param[in] dimIndex Tracks which of the nested loops are currently executing.
     * This gives position in the dimOrder, mins, and maxes vectors and ending criteria.
     */
    static void recurseAll(
        const std::vector<uint32_t>& dimOrder,
        const std::vector<bool>& dimReverse,
        const std::vector<uint32_t>& mins,
        const std::vector<uint32_t>& maxes,
        void(*function)(std::vector<uint32_t>,void*),
        void* otherArgs,
        std::vector<uint32_t> coordinates,
        uint32_t dimIndex
        );

    /*!
     * \brief Private method used by iterateMidplanes for recursion.
     *
     * First six parameters are the same as those passed to iterateAll.
     *
     * \param[in] coordinates Coordinate of a midplane to pass to the function in
     * the innermost loop. The values of the coordinates are the values iterated in
     * each loop.
     *
     * \param[in] dimIndex Tracks which of the nested loops are currently executing.
     * This gives position in the dimOrder, mins, and maxes vectors and ending criteria.
     */
    static void recurseMidplanes(
        const std::vector<uint32_t>& dimOrder,
        const std::vector<bool>& dimReverse,
        const std::vector<uint32_t>& mins,
        const std::vector<uint32_t>& maxes,
        void(*function)(const Coordinates&,void*),
        void* otherArgs,
        Coordinates coordinates,
        uint32_t dimIndex
        );
};

} // namespace bgsched

#endif
