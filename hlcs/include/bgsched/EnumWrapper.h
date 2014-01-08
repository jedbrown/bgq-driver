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
 * \file bgsched/EnumWrapper.h
 * \brief EnumWrapper class definition.
 */

#ifndef BGSCHED_ENUM_WRAPPER_H_
#define BGSCHED_ENUM_WRAPPER_H_

namespace bgsched {

/*!
 * \brief Wrappers an enum in a type-safe class.
 * \param T enum type.
 *
 * The motivation for this class stems from C++ converting enums
 * to integer types for comparisons.  This can cause the following
 * scenario to compile:
 *
 * \code
 *
 * enum Foo { Red = 0, Yellow = 2, Blue = 3 };
 * enum Bar { Circle = 0, Square = 2, Triangle = 3 };
 *
 * void
 * doStuff(Foo foo)
 * {
 *     assert(foo == Circle);
 *     assert(foo == Red);
 *     // what is foo??
 *
 *     ...
 * }
 *
 * \endcode
 *
 * If your enum is State, hold a member in your class by EnumWrapper<State>
 * rather than a plain State type.  Then when exposing the State value, return
 * an EnumWrapper<State> object.
 *
 * \code
 *
 * class Widget {
 *     public:
 *       enum State { Good = 0, Bad, Ugly };
 *       EnumWrapper<State> getState() const { return _state; }
 *     private:
 *       EnumWrapper<State> _state;
 * };
 *
 * \endcode
 *
 * The implicit constructor in this class will allow users of your class to
 * write the following:
 *
 * \code
 *
 * Widget widget;
 * if (widget.getState() == Widget::Bad) {
 *    ...
 * }
 *
 * \endcode
 */
template <typename T>
class EnumWrapper
{
public:
      /*!
       * \brief
       */
      EnumWrapper(
              T value //!< Value
              ) : _value(value) { }

      /*!
       * \brief Equality operator.
       */
      bool operator==(const EnumWrapper<T>& other) const { return _value == other._value; }

      /*!
       * \brief Not equal operator.
       */
      bool operator!=(const EnumWrapper<T>& other) const { return _value != other._value; }

      /*!
       * \brief Explicit conversion.
       */
      T toValue() const { return _value; }

private:
      const T _value;
};

} // namespace bgsched

#endif
