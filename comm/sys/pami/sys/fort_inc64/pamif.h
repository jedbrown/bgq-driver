! begin_generated_IBM_copyright_prolog                             !
!                                                                  !
! This is an automatically generated copyright prolog.             !
! After initializing,  DO NOT MODIFY OR MOVE                       !
!  --------------------------------------------------------------- !
! Licensed Materials - Property of IBM                             !
! Blue Gene/Q 5765-PER 5765-PRP                                    !
!                                                                  !
! (C) Copyright IBM Corp. 2011, 2012 All Rights Reserved           !
! US Government Users Restricted Rights -                          !
! Use, duplication, or disclosure restricted                       !
! by GSA ADP Schedule Contract with IBM Corp.                      !
!                                                                  !
!  --------------------------------------------------------------- !
!                                                                  !
! end_generated_IBM_copyright_prolog                               !
!
!   \file sys/fort_inc64/pamif.h
!   \brief Common external 64-bit FORTRAN interface for IBM's PAMI message layer.
!

external PAMI_ADDR_NULL

!
!   \brief PAMI result status codes
!
    integer PAMI_SUCCESS
    integer PAMI_NERROR
    integer PAMI_ERROR
    integer PAMI_INVAL
    integer PAMI_UNIMPL
    integer PAMI_EAGAIN
    integer PAMI_ENOMEM
    integer PAMI_SHUTDOWN
    integer PAMI_CHECK_ERRNO
    integer PAMI_OTHER
    integer PAMI_RESULT_EXT
    parameter (PAMI_SUCCESS=0)
    parameter (PAMI_NERROR=-1)
    parameter (PAMI_ERROR=1)
    parameter (PAMI_INVAL=2)
    parameter (PAMI_UNIMPL=3)
    parameter (PAMI_EAGAIN=4)
    parameter (PAMI_ENOMEM=5)
    parameter (PAMI_SHUTDOWN=6)
    parameter (PAMI_CHECK_ERRNO=7)
    parameter (PAMI_OTHER=8)
    parameter (PAMI_RESULT_EXT=1000)


!
!   \brief size difference between 32-bit and 64-bit
!
    integer WORDSIZE
    parameter (WORDSIZE=8)

!    
!   \brief The sizes (in byte) of PAMI basic types
!
    integer PAMI_CLIENT_T
    integer PAMI_CONTEXT_T
    integer PAMI_TYPE_T
    integer PAMI_TASK_T
    integer PAMI_ENDPOINT_T
    integer PAMI_USER_KEY_T
    integer PAMI_USER_VALUE_T
    integer PAMI_CLIENT_MEMREGION_SIZE_STATIC ! //TODO: this value differs from BG and power
    integer PAMI_GEOMETRY_T
    integer PAMI_ALGORITHM_T
    integer PAMI_DATA_FUNCTION_T
    integer PAMI_SIZE_T
    integer PAMI_VOID_T
    integer PAMI_INT_T
    integer PAMI_DBL_T
    integer PAMI_EXTENSION_T
    ! different for 32/64 bit
    parameter (PAMI_CLIENT_T=WORDSIZE)
    parameter (PAMI_CONTEXT_T=WORDSIZE)
    parameter (PAMI_TYPE_T=WORDSIZE)
    parameter (PAMI_USER_KEY_T=WORDSIZE)
    parameter (PAMI_USER_VALUE_T=WORDSIZE)
    parameter (PAMI_GEOMETRY_T=WORDSIZE)
    parameter (PAMI_ALGORITHM_T=WORDSIZE)
    parameter (PAMI_SIZE_T=WORDSIZE)
    parameter (PAMI_VOID_T=WORDSIZE)
    parameter (PAMI_EXTENSION_T=WORDSIZE)
    parameter (PAMI_DATA_FUNCTION_T=WORDSIZE)
    ! same for 32/64 bit
    parameter (PAMI_CLIENT_MEMREGION_SIZE_STATIC=64)
    parameter (PAMI_TASK_T=4)
    parameter (PAMI_ENDPOINT_T=4)
    parameter (PAMI_INT_T=4)
    parameter (PAMI_DBL_T=8)

    type pami_user_config_t
        sequence
        integer(PAMI_USER_KEY_T)    :: key
        integer(PAMI_USER_VALUE_T)  :: value
    end type pami_user_config_t

!
!   \brief All possible attributes for all hardware  
!

    integer PAMI_CLIENT_CLOCK_MHZ
    integer PAMI_CLIENT_CONST_CONTEXTS
    integer PAMI_CLIENT_HWTHREADS_AVAILABLE
    integer PAMI_CLIENT_MEMREGION_SIZE
    integer PAMI_CLIENT_MEM_SIZE
    integer PAMI_CLIENT_NUM_TASKS
    integer PAMI_CLIENT_NUM_LOCAL_TASKS
    integer PAMI_CLIENT_LOCAL_TASKS
    integer PAMI_CLIENT_NUM_CONTEXTS
    integer PAMI_CLIENT_PROCESSOR_NAME
    integer PAMI_CLIENT_TASK_ID
    integer PAMI_CLIENT_WTIMEBASE_MHZ
    integer PAMI_CLIENT_WTICK
    integer PAMI_CONTEXT_DISPATCH_ID_MAX
    integer PAMI_DISPATCH_RECV_IMMEDIATE_MAX
    integer PAMI_DISPATCH_SEND_IMMEDIATE_MAX
    integer PAMI_DISPATCH_ATOM_SIZE_MAX
    integer PAMI_GEOMETRY_OPTIMIZE
    integer PAMI_TYPE_DATA_SIZE
    integer PAMI_TYPE_DATA_EXTENT
    integer PAMI_TYPE_ATOM_SIZE
    integer PAMI_ATTRIBUTE_NAME_EXT
    parameter (PAMI_CLIENT_CLOCK_MHZ=100)
    parameter (PAMI_CLIENT_CONST_CONTEXTS=101)
    parameter (PAMI_CLIENT_HWTHREADS_AVAILABLE=102)
    parameter (PAMI_CLIENT_MEMREGION_SIZE=103)
    parameter (PAMI_CLIENT_MEM_SIZE=104)
    parameter (PAMI_CLIENT_NUM_TASKS=105)
    parameter (PAMI_CLIENT_NUM_LOCAL_TASKS=106)
    parameter (PAMI_CLIENT_LOCAL_TASKS=107)
    parameter (PAMI_CLIENT_NUM_CONTEXTS=108)
    parameter (PAMI_CLIENT_PROCESSOR_NAME=109)
    parameter (PAMI_CLIENT_TASK_ID=110)
    parameter (PAMI_CLIENT_WTIMEBASE_MHZ=111)
    parameter (PAMI_CLIENT_WTICK=112)
    parameter (PAMI_CONTEXT_DISPATCH_ID_MAX = 200)
    parameter (PAMI_DISPATCH_RECV_IMMEDIATE_MAX = 300)
    parameter (PAMI_DISPATCH_SEND_IMMEDIATE_MAX=301)
    parameter (PAMI_DISPATCH_ATOM_SIZE_MAX=302)
    parameter (PAMI_GEOMETRY_OPTIMIZE=400)
    parameter (PAMI_TYPE_DATA_SIZE=500)
    parameter (PAMI_TYPE_DATA_EXTENT=501)
    parameter (PAMI_TYPE_ATOM_SIZE=502)
    parameter (PAMI_ATTRIBUTE_NAME_EXT=1000)

!
!   \brief General purpose configuration structure: 3 types
!
    type pami_configuration_int_t
        sequence
        integer(PAMI_INT_T)  :: name
        integer(PAMI_INT_T)  :: paddings
        integer(PAMI_SIZE_T) :: value
    end type pami_configuration_int_t

    type pami_configuration_double_t
        sequence
        integer(PAMI_INT_T)  :: name
        integer(PAMI_INT_T)  :: paddings
        integer(PAMI_DBL_T)  :: value
    end type pami_configuration_double_t

    type pami_configuration_addr_t
        sequence
        integer(PAMI_INT_T)  :: name
        integer(PAMI_INT_T)  :: paddings
        integer(PAMI_VOID_T) :: value
    end type pami_configuration_addr_t

!
!   \brief PAMI Collectives Metadata
!
    integer(PAMI_INT_T) PAMI_UNSPECIFIED_FAILURE
    integer(PAMI_INT_T) PAMI_OUT_OF_RANGE
    integer(PAMI_INT_T) PAMI_ALIGN_SEND_BUFFER
    integer(PAMI_INT_T) PAMI_ALIGN_RECV_BUFFER
    integer(PAMI_INT_T) PAMI_DATATYPE_OP_NOT_VALID
    integer(PAMI_INT_T) PAMI_CONTIGUOUS_SEND
    integer(PAMI_INT_T) PAMI_CONTIGUOUS_RECV
    integer(PAMI_INT_T) PAMI_CONTINUOUS_SEND
    integer(PAMI_INT_T) PAMI_CONTINUOUS_RECV    
    integer(PAMI_INT_T) PAMI_REQUIRED_NON_LOCAL
    parameter (PAMI_UNSPECIFIED_FAILURE=-2147483648)    ! 0x80000000
    parameter (PAMI_OUT_OF_RANGE=1073741824)            ! 0x40000000
    parameter (PAMI_ALIGN_SEND_BUFFER=536870912)        ! 0x20000000
    parameter (PAMI_ALIGN_RECV_BUFFER=268435456)        ! 0x10000000
    parameter (PAMI_DATATYPE_OP_NOT_VALID=134217728)    ! 0x08000000
    parameter (PAMI_CONTIGUOUS_SEND=67108864)           ! 0x04000000
    parameter (PAMI_CONTIGUOUS_RECV=33554432)           ! 0x02000000
    parameter (PAMI_CONTINUOUS_SEND=16777216)           ! 0x01000000
    parameter (PAMI_CONTINUOUS_RECV=8388608)            ! 0x00800000
    parameter (PAMI_REQUIRED_NON_LOCAL=4194304)         ! 0x00400000

    type metadata_result_t
        sequence
        integer(PAMI_INT_T)  :: bitmask
    end type metadata_result_t
  
!
!   \brief A metadata structure to describe a collective protocol
!
    integer(PAMI_INT_T) PAMI_CHECK_REQUIRED
    integer(PAMI_INT_T) PAMI_NON_LOCAL
    integer(PAMI_INT_T) PAMI_RANGE_MIN_MAX
    integer(PAMI_INT_T) PAMI_SEND_MIN_ALIGN
    integer(PAMI_INT_T) PAMI_RECV_MIN_ALIGN
    integer(PAMI_INT_T) PAMI_ALL_DT_OP
    integer(PAMI_INT_T) PAMI_CONTIG_S_FLAGS
    integer(PAMI_INT_T) PAMI_CONTIG_R_FLAGS
    integer(PAMI_INT_T) PAMI_CONTIN_S_FLAGS
    integer(PAMI_INT_T) PAMI_CONTIN_R_FLAGS 
    parameter (PAMI_CHECK_REQUIRED=-2147483648) ! 0x80000000
    parameter (PAMI_NON_LOCAL=1073741824)       ! 0x40000000
    parameter (PAMI_RANGE_MIN_MAX=536870912)    ! 0x20000000
    parameter (PAMI_SEND_MIN_ALIGN=268435456)   ! 0x10000000
    parameter (PAMI_RECV_MIN_ALIGN=134217728)   ! 0x08000000
    parameter (PAMI_ALL_DT_OP=67108864)         ! 0x04000000
    parameter (PAMI_CONTIG_S_FLAGS=33554432)    ! 0x02000000
    parameter (PAMI_CONTIG_R_FLAGS=16777216)    ! 0x01000000
    parameter (PAMI_CONTIN_S_FLAGS=8388608)     ! 0x00800000
    parameter (PAMI_CONTIN_R_FLAGS=4194304)     ! 0x00400000

    integer(PAMI_INT_T) PAMI_HW_ACCEL
    parameter (PAMI_HW_ACCEL=-2147483648)       ! 0x80000000

    type pami_metadata_t
        sequence
        integer(PAMI_VOID_T)        :: name
        integer(PAMI_INT_T)         :: version
        integer(PAMI_INT_T)         :: paddings
        integer(PAMI_VOID_T)        :: check_fn
        integer(PAMI_SIZE_T)        :: range_lo
        integer(PAMI_SIZE_T)        :: range_hi
        integer(PAMI_INT_T)         :: send_min_align
        integer(PAMI_INT_T)         :: recv_min_align
        integer(PAMI_INT_T)         :: bitmask_correct
        integer(PAMI_INT_T)         :: bitmask_perf
        integer(PAMI_SIZE_T)        :: range_lo_perf
        integer(PAMI_SIZE_T)        :: range_hi_perf
        integer(PAMI_INT_T)         :: min_align_perf
    end type pami_metadata_t

!
!   \brief Hints (2-bit-size) that can be used for send/dispatch/collective hint
!
    integer(PAMI_INT_T) PAMI_MULTICONTEXT_DEFAULT
    integer(PAMI_INT_T) PAMI_MULTICONTEXT_ENABLE
    integer(PAMI_INT_T) PAMI_MULTICONTEXT_DISABLE
    integer(PAMI_INT_T) PAMI_LONG_HEADER_DEFAULT
    integer(PAMI_INT_T) PAMI_LONG_HEADER_ENABLE
    integer(PAMI_INT_T) PAMI_LONG_HEADER_DISABLE
    integer(PAMI_INT_T) PAMI_RECV_CONTIGUOUS_DEFAULT
    integer(PAMI_INT_T) PAMI_RECV_CONTIGUOUS_ENABLE
    integer(PAMI_INT_T) PAMI_RECV_CONTIGUOUS_DISABLE
    integer(PAMI_INT_T) PAMI_RECV_COPY_DEFAULT
    integer(PAMI_INT_T) PAMI_RECV_COPY_ENABLE
    integer(PAMI_INT_T) PAMI_RECV_COPY_DISABLE
    integer(PAMI_INT_T) PAMI_RECV_IMMEDIATE_DEFAULT
    integer(PAMI_INT_T) PAMI_RECV_IMMEDIATE_ENABLE
    integer(PAMI_INT_T) PAMI_RECV_IMMEDIATE_DISABLE
    integer(PAMI_INT_T) PAMI_CONSISTENCY_DEFAULT
    integer(PAMI_INT_T) PAMI_CONSISTENCY_ENABLE
    integer(PAMI_INT_T) PAMI_CONSISTENCY_DISABLE
    integer(PAMI_INT_T) PAMI_BUFFER_REGISTERED_DEFAULT 
    integer(PAMI_INT_T) PAMI_BUFFER_REGISTERED_ENABLE 
    integer(PAMI_INT_T) PAMI_BUFFER_REGISTERED_DISABLE 
    integer(PAMI_INT_T) PAMI_REMOTE_ASYNC_PROGRESS_DEFAULT
    integer(PAMI_INT_T) PAMI_REMOTE_ASYNC_PROGRESS_ENABLE
    integer(PAMI_INT_T) PAMI_REMOTE_ASYNC_PROGRESS_DISABLE
    integer(PAMI_INT_T) PAMI_USE_RDMA_DEFAULT
    integer(PAMI_INT_T) PAMI_USE_RDMA_ENABLE
    integer(PAMI_INT_T) PAMI_USE_RDMA_DISABLE
    integer(PAMI_INT_T) PAMI_USE_SHMEM_DEFAULT
    integer(PAMI_INT_T) PAMI_USE_SHMEM_ENABLE
    integer(PAMI_INT_T) PAMI_USE_SHMEM_DISABLE
    integer(PAMI_INT_T) PAMI_QUEUE_IMMEDIATE_DEFAULT
    integer(PAMI_INT_T) PAMI_QUEUE_IMMEDIATE_ENABLE
    integer(PAMI_INT_T) PAMI_QUEUE_IMMEDIATE_DISABLE
    parameter (PAMI_MULTICONTEXT_DEFAULT=0)
    parameter (PAMI_MULTICONTEXT_ENABLE=1073741824)       !0x40000000
    parameter (PAMI_MULTICONTEXT_DISABLE=-2147483648)     !0x80000000
    parameter (PAMI_LONG_HEADER_DEFAULT=0)
    parameter (PAMI_LONG_HEADER_ENABLE=268435456)         !0x10000000
    parameter (PAMI_LONG_HEADER_DISABLE=536870912)        !0x20000000
    parameter (PAMI_RECV_CONTIGUOUS_DEFAULT=0)
    parameter (PAMI_RECV_CONTIGUOUS_ENABLE=67108864)      !0x04000000
    parameter (PAMI_RECV_CONTIGUOUS_DISABLE=134217728)    !0x08000000
    parameter (PAMI_RECV_COPY_DEFAULT=0)
    parameter (PAMI_RECV_COPY_ENABLE=16777216)            !0x01000000
    parameter (PAMI_RECV_COPY_DISABLE=33554432)           !0x02000000
    parameter (PAMI_RECV_IMMEDIATE_DEFAULT=0)
    parameter (PAMI_RECV_IMMEDIATE_ENABLE=4194304)        !0x00400000
    parameter (PAMI_RECV_IMMEDIATE_DISABLE=8388608)       !0x00800000
    parameter (PAMI_CONSISTENCY_DEFAULT=0)
    parameter (PAMI_CONSISTENCY_ENABLE=1048576)           !0x00100000
    parameter (PAMI_CONSISTENCY_DISABLE=2097152)          !0x00200000
    parameter (PAMI_BUFFER_REGISTERED_DEFAULT=0) 
    parameter (PAMI_BUFFER_REGISTERED_ENABLE=262144)      !0x00040000
    parameter (PAMI_BUFFER_REGISTERED_DISABLE=524288)     !0x00080000
    parameter (PAMI_REMOTE_ASYNC_PROGRESS_DEFAULT=0)
    parameter (PAMI_REMOTE_ASYNC_PROGRESS_ENABLE=65536)   !0x00010000
    parameter (PAMI_REMOTE_ASYNC_PROGRESS_DISABLE=131072) !0x00020000
    parameter (PAMI_USE_RDMA_DEFAULT=0)
    parameter (PAMI_USE_RDMA_ENABLE=16384)                !0x00004000
    parameter (PAMI_USE_RDMA_DISABLE=32768)               !0x00008000
    parameter (PAMI_USE_SHMEM_DEFAULT=0)
    parameter (PAMI_USE_SHMEM_ENABLE=4096)                !0x00001000
    parameter (PAMI_USE_SHMEM_DISABLE=8192)               !0x00002000
    parameter (PAMI_QUEUE_IMMEDIATE_DEFAULT=0)
    parameter (PAMI_QUEUE_IMMEDIATE_ENABLE=1024)          !0x00000400
    parameter (PAMI_QUEUE_IMMEDIATE_DISABLE=2048)         !0x00000800

!
!   \brief "hard" hints for registering a send dispatch
!  
!   These hints are considered 'hard' hints that must be honored by the
!   implementation or the dispatch set must fail and return an error.
!  
!   Alternatively, hints may be specified for each send operation. Hints
!   specified in this way are considered 'soft' hints and may be silently
!   ignored by the implementation during a send operation.
!  
!   Hints are used to improve performance by allowing the send implementation
!   to safely assume that certain use cases will not ever, or will always,
!   be valid.
!  
    type pami_dispatch_hint_t
        sequence
        integer(PAMI_INT_T)  :: hints
    end type pami_dispatch_hint_t
    
!
!   \brief "Soft" hints for sending a message
!  
!   These hints are considered 'soft' hints that may be silently ignored
!   by the implementation during a send operation.
!  
!   Alternatively, hints may be specified when a send dispatch identifier
!   is registered using PAMI_Dispatch_set().  Hints set in this way are
!   considered 'hard' hints and must be honored by the implementation,
!   or the dispatch set must fail and return an error.
!
    type pami_send_hint_t
        sequence
        integer(PAMI_INT_T)  :: hints
    end type pami_send_hint_t

    type pami_collective_hint_t
        sequence
        integer(PAMI_INT_T)  :: hints
    end type pami_collective_hint_t

!
!   \brief Active message send common parameters structure
!
    type pami_iovec_t
        sequence
        integer(PAMI_VOID_T)  :: iov_base
        integer(PAMI_SIZE_T)  :: iov_len
    end type pami_iovec_t

    type pami_send_immediate_t
        sequence
        type(pami_iovec_t)          :: header
        type(pami_iovec_t)          :: data
        integer(PAMI_SIZE_T)        :: dispatch
        type(pami_send_hint_t)      :: hints
        integer(PAMI_ENDPOINT_T)    :: dest
    end type pami_send_immediate_t

!
!   \brief Structure for event notification
!
    type pami_send_event_t
        sequence
        integer(PAMI_VOID_T)  :: cookie
        integer(PAMI_VOID_T)  :: local_fn
        integer(PAMI_VOID_T)  :: remote_fn
    end type pami_send_event_t

!
!   \brief Structure for send parameters unique to a simple active message send
!
    type pami_send_t
        sequence
        type(pami_send_immediate_t) :: send
        type(pami_send_event_t)     :: events
    end type pami_send_t

!
!   \brief Structure for send parameters of a typed active message send
!
    type pami_send_typed_t
        sequence
        type(pami_send_immediate_t) :: send
        type(pami_send_event_t)     :: events
        integer(PAMI_TYPE_T)        :: type
        integer(PAMI_SIZE_T)        :: offset
        integer(PAMI_VOID_T)        :: data_fn
        integer(PAMI_VOID_T)        :: data_cookie
    end type pami_send_typed_t

!
!   \brief Receive message structure
!
    type pami_recv_t
        sequence
        integer(PAMI_VOID_T)        :: cookie
        integer(PAMI_VOID_T)        :: local_fn
        integer(PAMI_VOID_T)        :: addr
        integer(PAMI_TYPE_T)        :: type
        integer(PAMI_SIZE_T)        :: offset
        integer(PAMI_VOID_T)        :: data_fn
        integer(PAMI_VOID_T)        :: data_cookie
    end type pami_recv_t

!
!   \brief Input parameters common to all rma transfers
!
    type pami_rma_t
        sequence
        integer(PAMI_ENDPOINT_T)    :: dest
        type(pami_send_hint_t)      :: hints
        integer(PAMI_SIZE_T)        :: bytes
        integer(PAMI_VOID_T)        :: cookie
        integer(PAMI_VOID_T)        :: done_fn
    end type pami_rma_t

!
!   \brief Input parameters for rma simple transfers
!
    type pami_rma_addr_t
        sequence
        integer(PAMI_VOID_T)        :: local
        integer(PAMI_VOID_T)        :: remote
    end type pami_rma_addr_t

!
!   \brief Input parameters for rma typed transfers
!
    type pami_rma_typed_t
        sequence
        integer(PAMI_VOID_T)        :: local
        integer(PAMI_VOID_T)        :: remote
    end type pami_rma_typed_t

!
!   \brief Input parameters for rma put transfers
!
    type pami_rma_put_t
        sequence
        integer(PAMI_VOID_T)        :: rdone_fn
    end type pami_rma_put_t

! 
!   \brief Input parameters for simple put transfers
!
    type pami_put_simple_t
        sequence
        type(pami_rma_t)        :: rma
        type(pami_rma_addr_t)   :: addr
        type(pami_rma_put_t)    :: put
    end type pami_put_simple_t

!
!   \brief Input parameters for simple typed put transfers
!
    type pami_put_typed_t
        sequence
        type(pami_rma_t)        :: rma
        type(pami_rma_addr_t)   :: addr
        type(pami_rma_typed_t)  :: type
        type(pami_rma_put_t)    :: put
    end type pami_put_typed_t

!
!   \brief Input parameter structure for simple get transfers
!
    type pami_get_simple_t
        sequence
        type(pami_rma_t)        :: rma
        type(pami_rma_addr_t)   :: addr
    end type pami_get_simple_t

!
!   \brief Input parameter structure for typed get transfers
!
    type pami_get_typed_t
        sequence
        type(pami_rma_t)        :: rma
        type(pami_rma_addr_t)   :: addr
        type(pami_rma_typed_t)  :: type
    end type pami_get_typed_t

!
!   \brief Atomic rmw operations
!
    integer PAMI_ATOMIC_FETCH
    integer PAMI_ATOMIC_COMPARE
    integer PAMI_ATOMIC_SET
    integer PAMI_ATOMIC_ADD
    integer PAMI_ATOMIC_OR
    integer PAMI_ATOMIC_AND
    integer PAMI_ATOMIC_XOR
    integer PAMI_ATOMIC_FETCH_SET
    integer PAMI_ATOMIC_FETCH_ADD
    integer PAMI_ATOMIC_FETCH_OR
    integer PAMI_ATOMIC_FETCH_AND
    integer PAMI_ATOMIC_FETCH_XOR
    integer PAMI_ATOMIC_COMPARE_SET
    integer PAMI_ATOMIC_COMPARE_ADD
    integer PAMI_ATOMIC_COMPARE_OR
    integer PAMI_ATOMIC_COMPARE_AND
    integer PAMI_ATOMIC_COMPARE_XOR
    integer PAMI_ATOMIC_FETCH_COMPARE_SET
    integer PAMI_ATOMIC_FETCH_COMPARE_ADD
    integer PAMI_ATOMIC_FETCH_COMPARE_OR
    integer PAMI_ATOMIC_FETCH_COMPARE_AND
    integer PAMI_ATOMIC_FETCH_COMPARE_XOR
    integer PAMI_RMW_ASSIGNMENT_SET
    integer PAMI_RMW_ASSIGNMENT_ADD
    integer PAMI_RMW_ASSIGNMENT_OR
    integer PAMI_RMW_ASSIGNMENT_AND
    parameter (PAMI_ATOMIC_FETCH=1)
    parameter (PAMI_ATOMIC_COMPARE=2)
    parameter (PAMI_ATOMIC_SET=4)
    parameter (PAMI_ATOMIC_ADD=8)
    parameter (PAMI_ATOMIC_OR=12)
    parameter (PAMI_ATOMIC_AND=16)
    parameter (PAMI_ATOMIC_XOR=20)
    parameter (PAMI_ATOMIC_FETCH_SET=5)
    parameter (PAMI_ATOMIC_FETCH_ADD=9)
    parameter (PAMI_ATOMIC_FETCH_OR=13)
    parameter (PAMI_ATOMIC_FETCH_AND=17)
    parameter (PAMI_ATOMIC_FETCH_XOR=21)
    parameter (PAMI_ATOMIC_COMPARE_SET=6)
    parameter (PAMI_ATOMIC_COMPARE_ADD=10)
    parameter (PAMI_ATOMIC_COMPARE_OR=14)
    parameter (PAMI_ATOMIC_COMPARE_AND=18)
    parameter (PAMI_ATOMIC_COMPARE_XOR=22)
    parameter (PAMI_ATOMIC_FETCH_COMPARE_SET=7)
    parameter (PAMI_ATOMIC_FETCH_COMPARE_ADD=11)
    parameter (PAMI_ATOMIC_FETCH_COMPARE_OR=15)
    parameter (PAMI_ATOMIC_FETCH_COMPARE_AND=19)
    parameter (PAMI_ATOMIC_FETCH_COMPARE_XOR=23)

    type pami_rmw_t
        sequence
        integer(PAMI_ENDPOINT_T)    :: dest
        type(pami_send_hint_t)      :: hints
        integer(PAMI_VOID_T)        :: cookie
        integer(PAMI_VOID_T)        :: local
        integer(PAMI_VOID_T)        :: remote
        integer(PAMI_VOID_T)        :: value
        integer(PAMI_VOID_T)        :: test
        integer(PAMI_INT_T)         :: operation
        integer(PAMI_INT_T)         :: paddings
        integer(PAMI_TYPE_T)        :: type
    end type pami_rmw_t

!
!   \brief Memory region type
!
    type pami_rma_mr_t
        sequence
        CHARACTER(PAMI_CLIENT_MEMREGION_SIZE_STATIC)    :: mr
        integer(PAMI_SIZE_T)                            :: offset
    end type pami_rma_mr_t

    type pami_rdma_t
        sequence
        type(pami_rma_mr_t) :: local
        type(pami_rma_mr_t) :: remote
    end type pami_rdma_t

!
!   \brief Input parameter structure for simple rdma put transfers
!
    type pami_rput_simple_t
        sequence
        type(pami_rma_t)        :: rma
        type(pami_rdma_t)       :: rdma
        type(pami_rma_put_t)    :: put
    end type pami_rput_simple_t

!
!   \brief Input parameter structure for typed rdma put transfers
!
    type pami_rput_typed_t
        sequence
        type(pami_rma_t)        :: rma
        type(pami_rdma_t)       :: rdma
        type(pami_rma_typed_t)  :: type
        type(pami_rma_put_t)    :: put
    end type pami_rput_typed_t

!
!   \brief Input parameter structure for simple rdma get transfers
!
    type pami_rget_simple_t
        sequence
        type(pami_rma_t)        :: rma
        type(pami_rdma_t)       :: rdma
    end type pami_rget_simple_t

!
!   \brief Input parameter structure for typed rdma get transfers
!
    type pami_rget_typed_t
        sequence
        type(pami_rma_t)        :: rma
        type(pami_rdma_t)       :: rdma
        type(pami_rma_typed_t)  :: type
    end type pami_rget_typed_t

!
!   \brief Transfer Types,used by geometry and xfer routines
!
    integer PAMI_XFER_BROADCAST
    integer PAMI_XFER_ALLREDUCE
    integer PAMI_XFER_REDUCE
    integer PAMI_XFER_ALLGATHER
    integer PAMI_XFER_ALLGATHERV
    integer PAMI_XFER_ALLGATHERV_INT
    integer PAMI_XFER_SCATTER
    integer PAMI_XFER_SCATTERV
    integer PAMI_XFER_SCATTERV_INT
    integer PAMI_XFER_GATHER
    integer PAMI_XFER_GATHERV
    integer PAMI_XFER_GATHERV_INT
    integer PAMI_XFER_BARRIER
    integer PAMI_XFER_FENCE
    integer PAMI_XFER_ALLTOALL
    integer PAMI_XFER_ALLTOALLV
    integer PAMI_XFER_ALLTOALLV_INT
    integer PAMI_XFER_SCAN
    integer PAMI_XFER_REDUCE_SCATTER
    integer PAMI_XFER_AMBROADCAST
    integer PAMI_XFER_AMSCATTER
    integer PAMI_XFER_AMGATHER
    integer PAMI_XFER_AMREDUCE
    integer PAMI_XFER_COUNT
    integer PAMI_XFER_TYPE_EXT
    parameter (PAMI_XFER_BROADCAST=0)
    parameter (PAMI_XFER_ALLREDUCE=1)
    parameter (PAMI_XFER_REDUCE=2)
    parameter (PAMI_XFER_ALLGATHER=3)
    parameter (PAMI_XFER_ALLGATHERV=4)
    parameter (PAMI_XFER_ALLGATHERV_INT=5)
    parameter (PAMI_XFER_SCATTER=6)
    parameter (PAMI_XFER_SCATTERV=7)
    parameter (PAMI_XFER_SCATTERV_INT=8)
    parameter (PAMI_XFER_GATHER=9)
    parameter (PAMI_XFER_GATHERV=10)
    parameter (PAMI_XFER_GATHERV_INT=11)
    parameter (PAMI_XFER_BARRIER=12)
    parameter (PAMI_XFER_ALLTOALL=13)
    parameter (PAMI_XFER_ALLTOALLV=14)
    parameter (PAMI_XFER_ALLTOALLV_INT=15)
    parameter (PAMI_XFER_SCAN=16)
    parameter (PAMI_XFER_REDUCE_SCATTER=17)
    parameter (PAMI_XFER_AMBROADCAST=18)
    parameter (PAMI_XFER_AMSCATTER=19)
    parameter (PAMI_XFER_AMGATHER=20)
    parameter (PAMI_XFER_AMREDUCE=21)
    parameter (PAMI_XFER_COUNT=22)
    parameter (PAMI_XFER_TYPE_EXT=1000)

!
!   \brief Geometry (like groups/communicators)
!
    type pami_geometry_range_t
        sequence
        integer(PAMI_TASK_T)  :: lo
        integer(PAMI_TASK_T)  :: hi
    end type pami_geometry_range_t

!
!   \brief Common fields for PAMI collective operations
!
    type pami_xfer_comm_t
        sequence
        integer(PAMI_VOID_T)        :: cb_done
        integer(PAMI_VOID_T)        :: cookie
        integer(PAMI_ALGORITHM_T)   :: algorithm
        type(pami_collective_hint_t):: options
        integer(PAMI_INT_T)         :: paddings
    end type pami_xfer_comm_t

!
!   \brief Alltoall vector type
!
    type pami_xfer_alltoallv_t
        sequence
        type(pami_xfer_comm_t):: xfer
        integer(PAMI_VOID_T)  :: sndbuf
        integer(PAMI_TYPE_T)  :: stype
        integer(PAMI_SIZE_T)  :: stypecounts
        integer(PAMI_SIZE_T)  :: sdispls
        integer(PAMI_VOID_T)  :: rcvbuf
        integer(PAMI_TYPE_T)  :: rtype
        integer(PAMI_SIZE_T)  :: rtypecounts
        integer(PAMI_SIZE_T)  :: rdispls
    end type pami_xfer_alltoallv_t

!
!   \brief Non-blocking alltoall vector type
!
    type pami_xfer_alltoallv_int_t
        sequence  
        type(pami_xfer_comm_t):: xfer
        integer(PAMI_VOID_T)  :: sndbuf
        integer(PAMI_TYPE_T)  :: stype
        integer(PAMI_SIZE_T)  :: stypecounts
        integer(PAMI_SIZE_T)  :: sdispls
        integer(PAMI_VOID_T)  :: rcvbuf
        integer(PAMI_TYPE_T)  :: rtype
        integer(PAMI_SIZE_T)  :: rtypecounts
        integer(PAMI_SIZE_T)  :: rdispls
    end type pami_xfer_alltoallv_int_t

!
!   \brief Non-blocking alltoall type
!
    type pami_xfer_alltoall_t
        sequence
        type(pami_xfer_comm_t):: xfer
        integer(PAMI_VOID_T)  :: sndbuf
        integer(PAMI_TYPE_T)  :: stype
        integer(PAMI_SIZE_T)  :: stypecounts
        integer(PAMI_SIZE_T)  :: sdispls
        integer(PAMI_VOID_T)  :: rcvbuf
        integer(PAMI_TYPE_T)  :: rtype
        integer(PAMI_SIZE_T)  :: rtypecounts
    end type pami_xfer_alltoall_t

!
!   \brief Non-blocking reduce type
!
    type pami_xfer_reduce_t
        sequence
        type(pami_xfer_comm_t)        :: xfer
        integer(PAMI_SIZE_T)          :: root
        integer(PAMI_VOID_T)          :: sndbuf
        integer(PAMI_TYPE_T)          :: stype
        integer(PAMI_SIZE_T)          :: stypecount
        integer(PAMI_VOID_T)          :: rcvbuf
        integer(PAMI_TYPE_T)          :: rtype
        integer(PAMI_SIZE_T)          :: rtypecount
        integer(PAMI_DATA_FUNCTION_T) :: op
        integer(PAMI_VOID_T)          :: data_cookie
        integer(PAMI_INT_T)           :: commutative
    end type pami_xfer_reduce_t

!
!   \brief Non-blocking reduce_scatter type
!
    type pami_xfer_reduce_scatter_t
        sequence
        type(pami_xfer_comm_t)        :: xfer
        integer(PAMI_VOID_T)          :: sndbuf
        integer(PAMI_TYPE_T)          :: stype
        integer(PAMI_SIZE_T)          :: stypecount
        integer(PAMI_VOID_T)          :: rcvbuf
        integer(PAMI_TYPE_T)          :: rtype
        integer(PAMI_SIZE_T)          :: rcounts
        integer(PAMI_DATA_FUNCTION_T) :: op
        integer(PAMI_VOID_T)          :: data_cookie
        integer(PAMI_INT_T)           :: commutative
    end type pami_xfer_reduce_scatter_t

!
!   \brief Non-blocking broadcast type
!
    type pami_xfer_broadcast_t
        sequence
        type(pami_xfer_comm_t):: xfer
        integer(PAMI_SIZE_T)  :: root
        integer(PAMI_VOID_T)  :: buf
        integer(PAMI_TYPE_T)  :: type
        integer(PAMI_SIZE_T)  :: typecount
    end type pami_xfer_broadcast_t

!
!   \brief Non-blocking allgather type
!
    type pami_xfer_allgather_t
        sequence
        type(pami_xfer_comm_t):: xfer
        integer(PAMI_VOID_T)  :: sndbuf
        integer(PAMI_TYPE_T)  :: stype
        integer(PAMI_SIZE_T)  :: stypecount
        integer(PAMI_VOID_T)  :: rcvbuf
        integer(PAMI_TYPE_T)  :: rtype
        integer(PAMI_SIZE_T)  :: rtypecount
    end type pami_xfer_allgather_t

!
!   \brief Non-blocking allgather type
!
    type pami_xfer_gather_t
        sequence
        type(pami_xfer_comm_t):: xfer
        integer(PAMI_SIZE_T)  :: root
        integer(PAMI_VOID_T)  :: sndbuf
        integer(PAMI_TYPE_T)  :: stype
        integer(PAMI_SIZE_T)  :: stypecount
        integer(PAMI_VOID_T)  :: rcvbuf
        integer(PAMI_TYPE_T)  :: rtype
        integer(PAMI_SIZE_T)  :: rtypecount
    end type pami_xfer_gather_t

!
!   \brief Non-blocking gatherv type
!
    type pami_xfer_gatherv_t
        sequence
        type(pami_xfer_comm_t):: xfer
        integer(PAMI_SIZE_T)  :: root
        integer(PAMI_VOID_T)  :: sndbuf
        integer(PAMI_TYPE_T)  :: stype
        integer(PAMI_SIZE_T)  :: stypecount
        integer(PAMI_VOID_T)  :: rcvbuf
        integer(PAMI_TYPE_T)  :: rtype
        integer(PAMI_SIZE_T)  :: rtypecounts
        integer(PAMI_SIZE_T)  :: rdispls
    end type pami_xfer_gatherv_t

!
!   \brief Non-blocking gatherv type
!
    type pami_xfer_gatherv_int_t
        sequence
        type(pami_xfer_comm_t):: xfer
        integer(PAMI_SIZE_T)  :: root
        integer(PAMI_VOID_T)  :: sndbuf
        integer(PAMI_TYPE_T)  :: stype
        integer(PAMI_SIZE_T)  :: stypecount
        integer(PAMI_VOID_T)  :: rcvbuf
        integer(PAMI_TYPE_T)  :: rtype
        integer(PAMI_SIZE_T)  :: rtypecounts
        integer(PAMI_SIZE_T)  :: rdispls
    end type pami_xfer_gatherv_int_t

!
!   \brief Non-blocking allgatherv type
!
    type pami_xfer_allgatherv_t
        sequence
        type(pami_xfer_comm_t):: xfer
        integer(PAMI_VOID_T)  :: sndbuf
        integer(PAMI_TYPE_T)  :: stype
        integer(PAMI_SIZE_T)  :: stypecount
        integer(PAMI_VOID_T)  :: rcvbuf
        integer(PAMI_TYPE_T)  :: rtype
        integer(PAMI_SIZE_T)  :: rtypecounts
        integer(PAMI_SIZE_T)  :: rdispls
    end type pami_xfer_allgatherv_t

!
!   \brief Non-blocking allgatherv type
!
    type pami_xfer_allgatherv_int_t
        sequence
        type(pami_xfer_comm_t):: xfer
        integer(PAMI_VOID_T)  :: sndbuf
        integer(PAMI_TYPE_T)  :: stype
        integer(PAMI_SIZE_T)  :: stypecount
        integer(PAMI_VOID_T)  :: rcvbuf
        integer(PAMI_TYPE_T)  :: rtype
        integer(PAMI_SIZE_T)  :: rtypecounts
        integer(PAMI_SIZE_T)  :: rdispls
    end type pami_xfer_allgatherv_int_t

!
!   \brief Non-blocking scatter type
!
    type pami_xfer_scatter_t
        sequence
        type(pami_xfer_comm_t):: xfer
        integer(PAMI_SIZE_T)  :: root
        integer(PAMI_VOID_T)  :: sndbuf
        integer(PAMI_TYPE_T)  :: stype
        integer(PAMI_SIZE_T)  :: stypecount
        integer(PAMI_VOID_T)  :: rcvbuf
        integer(PAMI_TYPE_T)  :: rtype
        integer(PAMI_SIZE_T)  :: rtypecount
    end type pami_xfer_scatter_t

!
!   \brief Non-blocking scatterv type
!
    type pami_xfer_scatterv_t
        sequence
        type(pami_xfer_comm_t):: xfer
        integer(PAMI_SIZE_T)  :: root
        integer(PAMI_VOID_T)  :: sndbuf
        integer(PAMI_TYPE_T)  :: stype
        integer(PAMI_SIZE_T)  :: stypecounts
        integer(PAMI_SIZE_T)  :: sdispls
        integer(PAMI_VOID_T)  :: rcvbuf
        integer(PAMI_TYPE_T)  :: rtype
        integer(PAMI_SIZE_T)  :: rtypecount
    end type pami_xfer_scatterv_t

!
!   \brief Non-blocking scatterv type
!
    type pami_xfer_scatterv_int_t
        sequence
        type(pami_xfer_comm_t):: xfer
        integer(PAMI_SIZE_T)  :: root
        integer(PAMI_VOID_T)  :: sndbuf
        integer(PAMI_TYPE_T)  :: stype
        integer(PAMI_SIZE_T)  :: stypecounts
        integer(PAMI_SIZE_T)  :: sdispls
        integer(PAMI_VOID_T)  :: rcvbuf
        integer(PAMI_TYPE_T)  :: rtype
        integer(PAMI_SIZE_T)  :: rtypecount
    end type pami_xfer_scatterv_int_t

!
!   \brief Non-blocking allreduce type
!
    type pami_xfer_allreduce_t
        sequence
        type(pami_xfer_comm_t)        :: xfer
        integer(PAMI_VOID_T)          :: sndbuf
        integer(PAMI_TYPE_T)          :: stype
        integer(PAMI_SIZE_T)          :: stypecount
        integer(PAMI_VOID_T)          :: rcvbuf
        integer(PAMI_TYPE_T)          :: rtype
        integer(PAMI_SIZE_T)          :: rtypecount
        integer(PAMI_DATA_FUNCTION_T) :: op
        integer(PAMI_VOID_T)          :: data_cookie
        integer(PAMI_INT_T)           :: commutative
    end type pami_xfer_allreduce_t

!
!   \brief Non-blocking scan type
!
    type pami_xfer_scan_t
        sequence
        type(pami_xfer_comm_t)        :: xfer
        integer(PAMI_VOID_T)          :: sndbuf
        integer(PAMI_TYPE_T)          :: stype
        integer(PAMI_SIZE_T)          :: stypecount
        integer(PAMI_VOID_T)          :: rcvbuf
        integer(PAMI_TYPE_T)          :: rtype
        integer(PAMI_SIZE_T)          :: rtypecount
        integer(PAMI_DATA_FUNCTION_T) :: op
        integer(PAMI_VOID_T)          :: data_cookie
        integer(PAMI_INT_T)           :: exclusive
    end type pami_xfer_scan_t

!
!   \brief Non-blocking barrier type
!
    type pami_xfer_barrier_t
        sequence
        type(pami_xfer_comm_t):: xfer
    end type pami_xfer_barrier_t

!
!   \brief Non-blocking fence type
!
    type pami_xfer_fence_t
        sequence
        type(pami_xfer_comm_t):: xfer
    end type pami_xfer_fence_t

!
!   \brief Non-blocking active message broadcast type
!
    type pami_xfer_ambroadcast_t
        sequence
        type(pami_xfer_comm_t):: xfer
        integer(PAMI_SIZE_T)  :: dispatch
        integer(PAMI_VOID_T)  :: user_header
        integer(PAMI_SIZE_T)  :: headerlen
        integer(PAMI_VOID_T)  :: sndbuf
        integer(PAMI_TYPE_T)  :: stype
        integer(PAMI_SIZE_T)  :: stypecount
    end type pami_xfer_ambroadcast_t

!
!   \brief Non-blocking active message scatter type
!
    type pami_xfer_amscatter_t
        sequence
        type(pami_xfer_comm_t):: xfer
        integer(PAMI_SIZE_T)  :: dispatch
        integer(PAMI_VOID_T)  :: headers
        integer(PAMI_SIZE_T)  :: headerlen
        integer(PAMI_VOID_T)  :: sndbuf
        integer(PAMI_TYPE_T)  :: stype
        integer(PAMI_SIZE_T)  :: stypecount
    end type pami_xfer_amscatter_t

!
!   \brief Non-blocking active message gather type
!
    type pami_xfer_amgather_t
        sequence
        type(pami_xfer_comm_t):: xfer
        integer(PAMI_SIZE_T)  :: dispatch
        integer(PAMI_VOID_T)  :: headers
        integer(PAMI_SIZE_T)  :: headerlen
        integer(PAMI_VOID_T)  :: rcvbuf
        integer(PAMI_TYPE_T)  :: rtype
        integer(PAMI_SIZE_T)  :: rtypecount
    end type pami_xfer_amgather_t

!
!   \brief Non-blocking active message reduce type
!
    type pami_xfer_amreduce_t
        sequence
        type(pami_xfer_comm_t)        :: xfer
        integer(PAMI_SIZE_T)          :: dispatch
        integer(PAMI_VOID_T)          :: user_header
        integer(PAMI_SIZE_T)          :: headerlen
        integer(PAMI_VOID_T)          :: rcvbuf
        integer(PAMI_TYPE_T)          :: rtype
        integer(PAMI_SIZE_T)          :: rtypecount
        integer(PAMI_DATA_FUNCTION_T) :: op
        integer(PAMI_VOID_T)          :: data_cookie
        integer(PAMI_INT_T)           :: commutative
    end type pami_xfer_amreduce_t

!
!   \brief Predefined PAMI data type for various data layout
!
    integer(PAMI_TYPE_T) PAMI_TYPE_NULL
    integer(PAMI_TYPE_T) PAMI_TYPE_BYTE
    integer(PAMI_TYPE_T) PAMI_TYPE_SIGNED_CHAR
    integer(PAMI_TYPE_T) PAMI_TYPE_UNSIGNED_CHAR
    integer(PAMI_TYPE_T) PAMI_TYPE_SIGNED_SHORT
    integer(PAMI_TYPE_T) PAMI_TYPE_UNSIGNED_SHORT
    integer(PAMI_TYPE_T) PAMI_TYPE_SIGNED_INT
    integer(PAMI_TYPE_T) PAMI_TYPE_UNSIGNED_INT
    integer(PAMI_TYPE_T) PAMI_TYPE_SIGNED_LONG
    integer(PAMI_TYPE_T) PAMI_TYPE_UNSIGNED_LONG
    integer(PAMI_TYPE_T) PAMI_TYPE_SIGNED_LONG_LONG
    integer(PAMI_TYPE_T) PAMI_TYPE_UNSIGNED_LONG_LONG
    integer(PAMI_TYPE_T) PAMI_TYPE_FLOAT
    integer(PAMI_TYPE_T) PAMI_TYPE_DOUBLE
    integer(PAMI_TYPE_T) PAMI_TYPE_LONG_DOUBLE
    integer(PAMI_TYPE_T) PAMI_TYPE_LOGICAL1
    integer(PAMI_TYPE_T) PAMI_TYPE_LOGICAL2
    integer(PAMI_TYPE_T) PAMI_TYPE_LOGICAL4
    integer(PAMI_TYPE_T) PAMI_TYPE_LOGICAL8
    integer(PAMI_TYPE_T) PAMI_TYPE_SINGLE_COMPLEX
    integer(PAMI_TYPE_T) PAMI_TYPE_DOUBLE_COMPLEX
    integer(PAMI_TYPE_T) PAMI_TYPE_LOC_2INT
    integer(PAMI_TYPE_T) PAMI_TYPE_LOC_2FLOAT
    integer(PAMI_TYPE_T) PAMI_TYPE_LOC_2DOUBLE
    integer(PAMI_TYPE_T) PAMI_TYPE_LOC_SHORT_INT
    integer(PAMI_TYPE_T) PAMI_TYPE_LOC_FLOAT_INT
    integer(PAMI_TYPE_T) PAMI_TYPE_LOC_DOUBLE_INT
    integer(PAMI_TYPE_T) PAMI_TYPE_LOC_LONG_INT
    integer(PAMI_TYPE_T) PAMI_TYPE_LOC_LONGDOUBLE_INT

!
!   \brief Predefined PAMI data operation functions
!
    integer(PAMI_VOID_T) PAMI_DATA_COPY
    integer(PAMI_VOID_T) PAMI_DATA_NOOP
    integer(PAMI_VOID_T) PAMI_DATA_MAX
    integer(PAMI_VOID_T) PAMI_DATA_MIN
    integer(PAMI_VOID_T) PAMI_DATA_SUM
    integer(PAMI_VOID_T) PAMI_DATA_PROD
    integer(PAMI_VOID_T) PAMI_DATA_LAND
    integer(PAMI_VOID_T) PAMI_DATA_LOR
    integer(PAMI_VOID_T) PAMI_DATA_LXOR
    integer(PAMI_VOID_T) PAMI_DATA_BAND
    integer(PAMI_VOID_T) PAMI_DATA_BOR
    integer(PAMI_VOID_T) PAMI_DATA_BXOR
    integer(PAMI_VOID_T) PAMI_DATA_MAXLOC
    integer(PAMI_VOID_T) PAMI_DATA_MINLOC

!
!   \brief Setters that allow PAMI users to set and test values for
!   uninitialized object handles
!
    integer(PAMI_CLIENT_T)    PAMI_CLIENT_NULL
    integer(PAMI_CONTEXT_T)   PAMI_CONTEXT_NULL
    integer(PAMI_GEOMETRY_T)  PAMI_GEOMETRY_NULL
    integer(PAMI_ALGORITHM_T) PAMI_ALGORITHM_NULL
    integer(PAMI_ENDPOINT_T)  PAMI_ENDPOINT_NULL
    integer(PAMI_SIZE_T)      PAMI_ALL_CONTEXTS

    common / PAMI_FORT_GLOBALS / &
    PAMI_TYPE_NULL, &
    PAMI_TYPE_BYTE, &
    PAMI_TYPE_SIGNED_CHAR, &
    PAMI_TYPE_UNSIGNED_CHAR, &
    PAMI_TYPE_SIGNED_SHORT, &
    PAMI_TYPE_UNSIGNED_SHORT, &
    PAMI_TYPE_SIGNED_INT, &
    PAMI_TYPE_UNSIGNED_INT, &
    PAMI_TYPE_SIGNED_LONG, &
    PAMI_TYPE_UNSIGNED_LONG, &
    PAMI_TYPE_SIGNED_LONG_LONG, &
    PAMI_TYPE_UNSIGNED_LONG_LONG, &
    PAMI_TYPE_FLOAT, &
    PAMI_TYPE_DOUBLE, &
    PAMI_TYPE_LONG_DOUBLE, &
    PAMI_TYPE_LOGICAL1, &
    PAMI_TYPE_LOGICAL2, &
    PAMI_TYPE_LOGICAL4, &
    PAMI_TYPE_LOGICAL8, &
    PAMI_TYPE_SINGLE_COMPLEX, &
    PAMI_TYPE_DOUBLE_COMPLEX, &
    PAMI_TYPE_LOC_2INT, &
    PAMI_TYPE_LOC_2FLOAT, &
    PAMI_TYPE_LOC_2DOUBLE, &
    PAMI_TYPE_LOC_SHORT_INT, &
    PAMI_TYPE_LOC_FLOAT_INT, &
    PAMI_TYPE_LOC_DOUBLE_INT, &
    PAMI_TYPE_LOC_LONG_INT, &
    PAMI_TYPE_LOC_LONGDOUBLE_INT, &
    PAMI_DATA_COPY, &
    PAMI_DATA_NOOP, &
    PAMI_DATA_MAX, &
    PAMI_DATA_MIN, &
    PAMI_DATA_SUM, &
    PAMI_DATA_PROD, &
    PAMI_DATA_LAND, &
    PAMI_DATA_LOR, &
    PAMI_DATA_LXOR, &
    PAMI_DATA_BAND, &
    PAMI_DATA_BOR, &
    PAMI_DATA_BXOR, &
    PAMI_DATA_MAXLOC, &
    PAMI_DATA_MINLOC, &
    PAMI_CLIENT_NULL, &
    PAMI_CONTEXT_NULL, &
    PAMI_GEOMETRY_NULL, &
    PAMI_ALGORITHM_NULL, &
    PAMI_ENDPOINT_NULL, &
    PAMI_ALL_CONTEXTS
