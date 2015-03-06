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
///
/// \file common/TorusMappingInterface.h
/// \brief ???
///
#ifndef __common_TorusMappingInterface_h__
#define __common_TorusMappingInterface_h__

#include <pami.h>

namespace PAMI
{
  namespace Interface
  {
    namespace Mapping
    {
      static const unsigned char Mesh          = 0x00; /// no torus                           
      static const unsigned char TorusPositive = 0x01; /// positive torus direction flag
      static const unsigned char TorusNegative = 0x02; /// negative torus direction flag
      ///
      /// \param T_Mapping Torus mapping template class
      ///
      template <class T_Mapping, unsigned T_Dimensions>
      class Torus
      {


        public:
#if 0
          ///
          /// \brief Get the torus coordinate of a particular dimension for the local task
          ///
          /// \note The C++ template parameter \c T_Dimension is used to specify
          ///       torus coordinate to retrieve. The use of C++ templates for this
          ///       method allows the compiler to generate code without an if-branch
          ///       to determine which coordinate to retrieve.
          ///
          /// \code
          ///   size_t x = mapping.torusCoord<0>();
          ///   size_t y = mapping.torusCoord<1>();
          ///   size_t z = mapping.torusCoord<2>();
          ///   size_t t = mapping.torusCoord<3>();
          /// \endcode
          ///
          /// \return Torus coordinate for this task
          ///
          template <int T_Dimension>
          inline size_t torusCoord () const
          {
            abort();
            return;
          };
#endif

          ///
          /// \brief Get the size of a torus dimension
          /// \param i - the dimension
          /// \return Torus dimension size
          ///
          inline size_t torusSize (size_t i);

          ///
          /// \brief Get the number of torus (physical network) dimensions
          ///
          /// Does not include local, on-node, pseudo-dimensions
          ///
          /// \return Torus dimensions
          ///
          inline size_t torusDims() const
          {
            return T_Dimensions;

          };

          ///
          /// \brief Get the torus link information
          ///
          inline void torusInformation(pami_coord_t &ll, pami_coord_t &ur, unsigned char info[]);

          ///
          /// \brief Get the torus address for the local task
          ///
          /// \note The C++ template parameter \c T_Dimension is used to specify
          ///       the number torus dimensions. The use of C++ templates for this
          ///       method allows the compiler to generate code specific to the
          ///       number of torus dimensions for a specific platform.
          ///       Additionally, the template parameter will enforce a range check
          ///       on the output array.
          ///
          /// \todo Determine if the range check on the output array is a
          ///       hinderance or is helpful.
          ///
          /// \code
          ///   // Example torus mapping implentation:
          ///   template <>
          ///   inline void torusAddr<4> (size_t (&addr)[4]) const
          ///   {
          ///     ...
          ///   };
          ///
          ///   size_t addr[4];
          ///   mapping.torusAddr<4>(addr);
          ///   size_t x = addr[0];
          ///   size_t y = addr[1];
          ///   size_t z = addr[2];
          ///   size_t t = addr[3];
          /// \endcode
          ///
          /// \param[out] addr Array of torus coordinates
          ///
//          inline void torusAddr (size_t (&addr)[T_Dimensions]) const;
          inline void torusAddr (size_t (&addr)[T_Dimensions]);

          ///
          /// \brief Get the torus address for a specific task
          ///
          /// \note The C++ template parameter \c T_Dimension is used to specify
          ///       the number torus dimensions. The use of C++ templates for this
          ///       method allows the compiler to generate code specific to the
          ///       number of torus dimensions for a specific platform.
          ///       Additionally, the template parameter will enforce a range check
          ///       on the output array.
          ///
          /// \todo Determine if the range check on the output array is a
          ///       hinderance or is helpful.
          ///
          /// \code
          ///   size_t addr[4];
          ///   mapping.task2torus<4>(0, addr);
          ///   size_t x = addr[0];
          ///   size_t y = addr[1];
          ///   size_t z = addr[2];
          ///   size_t t = addr[3];
          /// \endcode
          ///
          /// \param[in]  task Global task identifier
          /// \param[out] addr Array of torus coordinates
          ///
          /// \retval PAMI_SUCCESS
          /// \retval PAMI_INVAL   Invalid task used as an input
          ///
          inline pami_result_t task2torus (size_t task, size_t (&addr)[T_Dimensions]);

          ///
          /// \brief Get the global task for a specific torus address
          ///
          /// \note The C++ template parameter \c T_Dimension is used to specify
          ///       the number torus dimensions. The use of C++ templates for this
          ///       method allows the compiler to generate code specific to the
          ///       number of torus dimensions for a specific platform.
          ///       Additionally, the template parameter will enforce a range check
          ///       on the output array.
          ///
          /// \todo Determine if the range check on the output array is a
          ///       hinderance or is helpful.
          ///
          /// \code
          ///   size_t task;
          ///   size_t addr[4];
          ///   addr[0] = 0;
          ///   addr[1] = 1;
          ///   addr[2] = 2;
          ///   addr[3] = 3;
          ///   if (mapping.torus2task<4>(task, addr) == PAMI_SUCCESS)
          ///   { ... }
          ///
          ///   if (mapping.torus2task<4>(task, {3,2,1,0]) == PAMI_SUCCESS)
          ///   { ... }
          /// \endcode
          ///
          /// \param[in]  addr Array of torus coordinates
          /// \param[out] task Global task identifier
          ///
          /// \retval PAMI_SUCCESS
          /// \retval PAMI_INVAL   Invalid torus address used as an input
          ///
//          template <int T_Dimension>
          inline pami_result_t torus2task (size_t (&addr)[T_Dimensions], size_t & task);
          inline size_t       torusgetcoord (size_t dimension);
#if 0
        //protected:
          ///
          /// \brief Default torus coordintate template implementation.
          ///
          /// If a torus mapping implementation does not define a method for a
          /// specific torus coordinate dimension then the base torus mapping
          /// template implementation will be invoked and will abort.
          ///
          /// \see PAMI::Mapping::Torus::torusCoord()
          ///
          template <int T_Dimension>
          inline size_t torusCoord_impl<T_Dimension> () const;

          ///
          /// \brief Default torus address template implementation.
          ///
          /// If a torus mapping implementation does not define a method for a
          /// specific dimension then the base torus mapping
          /// template implementation will be invoked and will abort.
          ///
          /// \see PAMI::Mapping::Torus::torusAddr()
          ///
          template <int T_Dimension>
          inline void torusAddr_impl (size_t (&addr)[T_Dimension]) const;

          ///
          /// \brief Default task to torus address template implementation.
          ///
          /// If a torus mapping implementation does not define a method for a
          /// specific dimension then the base torus mapping
          /// template implementation will be invoked and will abort.
          ///
          /// \see PAMI::Mapping::Torus::task2torus()
          ///
          template <int T_Dimension>
          inline pami_result_t task2torus_impl (size_t task, size_t (&addr)[T_Dimension]) const;

          ///
          /// \brief Default torus to task address template implementation.
          ///
          /// If a torus mapping implementation does not define a method for a
          /// specific dimension then the base torus mapping
          /// template implementation will be invoked and will abort.
          ///
          /// \see PAMI::Mapping::Torus::torus2task()
          ///
          template <int T_Dimension>
          inline pami_result_t torus2task_impl (size_t (&addr)[T_Dimension], size_t & task) const;
#endif
          ///
          /// \brief Default torus dimension size implementation.
          ///
          /// If a torus mapping implementation does not define a method for a
          /// specific dimension then the base torus mapping
          /// template implementation will be invoked and will abort.
          ///
          /// \see PAMI::Mapping::Torus::torusSize()
          ///
          inline size_t torusSize_impl (size_t i) const;

      }; // class Torus
#if 0
      template <class T_Mapping, unsigned T_Dimensions>
      template <int T_Dimension>
      inline size_t Torus<T_Mapping,T_Dimensions>::torusCoord<T_Dimension> () const
      {
        abort();
        return 0;
        //return static_cast<T_Mapping*>(this)->torusCoord_impl[T_Dimension];
      }
#endif

      template <class T_Mapping, unsigned T_Dimensions>
      inline size_t Torus<T_Mapping, T_Dimensions>::torusSize(size_t i)
      {
        return static_cast<T_Mapping*>(this)->torusSize_impl(i);
      }

      template <class T_Mapping, unsigned T_Dimensions>
      //template <int T_Dimension>
      //inline size_t Torus<T_Mapping,T_Dimensions>::torusAddr (size_t (&addr)[T_Dimensions]) const
      inline void Torus<T_Mapping,T_Dimensions>::torusAddr (size_t (&addr)[T_Dimensions])
      {
        return static_cast<T_Mapping*>(this)->torusAddr_impl (addr);
      }

      template <class T_Mapping, unsigned T_Dimensions>
      inline void Torus<T_Mapping,T_Dimensions>::torusInformation(pami_coord_t &ll, pami_coord_t &ur, unsigned char info[])
      {
        return static_cast<T_Mapping*>(this)->torusInformation_impl(ll,ur,info);
      }


      template <class T_Mapping, unsigned T_Dimensions>
      inline pami_result_t Torus<T_Mapping,T_Dimensions>::task2torus (size_t task, size_t (&addr)[T_Dimensions])
      {
        return static_cast<T_Mapping*>(this)->task2torus_impl (task, addr);
      }

      template <class T_Mapping, unsigned T_Dimensions>
      //template <int T_Dimension>
      inline pami_result_t Torus<T_Mapping,T_Dimensions>::torus2task (size_t (&addr)[T_Dimensions], size_t & task)
      {
        return static_cast<T_Mapping*>(this)->torus2task_impl (addr, task);
      }
          inline size_t       torusgetcoord (size_t dimension);

      template <class T_Mapping, unsigned T_Dimensions>
      //template <int T_Dimension>
      inline size_t Torus<T_Mapping,T_Dimensions>::torusgetcoord (size_t dimension)
      {
        return static_cast<T_Mapping*>(this)->torusgetcoord_impl (dimension);
      }

      template <class T_Mapping, unsigned T_Dimensions>
      inline size_t Torus<T_Mapping,T_Dimensions>::torusSize_impl(size_t i) const
      {
        abort();
        return 0;
      }

#if 0
      template <class T_Mapping>
      template <int T_Dimension>
      inline size_t Torus<T_Mapping>::torusCoord_impl<T_Dimension> () const
      {
        abort();
        return 0;
      }

//      template <class T_Mapping>
      template <class T_Mapping>
      template <int T_Dimension>
      inline void Torus<T_Mapping>::torusAddr_impl (size_t (&addr)[T_Dimension]) const;
      {
        abort();
        return 0;
      }

      template <class T_Mapping>
      template <int T_Dimension>
      inline pami_result_t Torus<T_Mapping>::task2torus_impl (size_t task, size_t (&addr)[T_Dimension]) const;
      {
        abort();
        return 0;
      }

      template <class T_Mapping>
      template <int T_Dimension>
      inline pami_result_t Torus<T_Mapping>::torus2task_impl (size_t (&addr)[T_Dimension], size_t & task) const;
      {
        abort();
        return 0;
      }
#endif
  };	// namespace Mapping
};	// namespace Interface
};	// namespace PAMI
#endif // __components_mapping_torusmapping_h__
