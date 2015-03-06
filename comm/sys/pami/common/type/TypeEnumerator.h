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
 * \file common/type/TypeEnumerator.h
 * \brief Enumerates the contiguous blocks common to two types.
 */
#ifndef __common_type_TypeEnumerator_h__
#define __common_type_TypeEnumerator_h__

#include "TypeCode.h"
#include "TypeMachine.h"

namespace PAMI
{
  namespace Type
  {
    ///
    /// \brief Enumerate the contiguous data blocks common to two types
    ///
    /// Each time the \c next() method is invoked the offset from the start of
    /// the input type, the offset from the start of the output type, and the
    /// number of contiguous bytes in the next block is returned
    ///
    class TypeEnumerator
    {
      protected:

        static const size_t array_size = 128;

        typedef struct
        {
          size_t disp[TypeEnumerator::array_size];
          size_t size[TypeEnumerator::array_size];
          size_t index;
        } state_t;

        Type::TypeMachine _input_machine;
        Type::TypeMachine _output_machine;

        state_t _input_state;
        state_t _output_state;

      public:

        ///
        /// \brief Construct a new type enumerator for two types
        ///
        /// Internally, this will create two type machines, one for each type.
        ///
        /// param [in] input_type  Pointer to the input (source) type
        /// param [in] output_type Pointer to the output (destination) type
        ///
        TypeEnumerator (Type::TypeCode * input_type, Type::TypeCode * output_type) :
            _input_machine (input_type),
            _output_machine (output_type)
        {
          _input_state.index = 0;
          _output_state.index = 0;

          // Enumerate the first set of input blocks from the input machine
          _input_machine.EnumerateBlocks (_input_state.disp,
                                          _input_state.size,
                                          TypeEnumerator::array_size);

          // Enumerate the first set of output blocks from the output machine
          _output_machine.EnumerateBlocks (_output_state.disp,
                                           _output_state.size,
                                           TypeEnumerator::array_size);
        };

        ///
        /// \brief Destructor
        ///
        ~TypeEnumerator () {};

        ///
        /// \brief Retrieve the next contiguous block common to both types
        ///
        /// \param [out] input_offset  Offset of the block for the input type
        /// \param [out] output_offset Offset of the block for the output type
        /// \param [out] bytes         Number of contiguous bytes in this block
        ///
        inline void next (size_t & input_offset, size_t & output_offset, size_t & bytes)
        {
          size_t & input_size  = _input_state.size[_input_state.index];
          size_t & input_disp  = _input_state.disp[_input_state.index];

          size_t & output_size = _output_state.size[_output_state.index];
          size_t & output_disp = _output_state.disp[_output_state.index];

          const size_t size = MIN(input_size, output_size);

          // Set the output parameter values.
          bytes = size;
          input_offset = input_disp;
          output_offset = output_disp;

          // Update the current input block
          input_size -= size;
          input_disp += size;

          // Move to the next input block if this one is empty
          _input_state.index += (input_size == 0);

          // Enumerate the next set of input blocks if the current set is empty
          if (unlikely(_input_state.index == TypeEnumerator::array_size))
            {
              _input_state.index = 0;
              _input_machine.EnumerateBlocks (_input_state.disp,
                                              _input_state.size,
                                              TypeEnumerator::array_size);
            }

          // Update the current output block
          output_size -= size;
          output_disp += size;

          // Move to the next output block if this one is empty
          _output_state.index += (output_size == 0);

          // Enumerate the next set of output blocks if the current set is empty
          if (unlikely(_output_state.index == TypeEnumerator::array_size))
            {
              _output_state.index = 0;
              _output_machine.EnumerateBlocks (_output_state.disp,
                                               _output_state.size,
                                               TypeEnumerator::array_size);
            }

          return;
        }
    };
  }
}

#endif // __common_type_TypeEnumerator_h__
//
// astyle info    http://astyle.sourceforge.net
//
// astyle options --style=gnu --indent=spaces=2 --indent-classes
// astyle options --indent-switches --indent-namespaces --break-blocks
// astyle options --pad-oper --keep-one-line-blocks --max-instatement-indent=79
//
