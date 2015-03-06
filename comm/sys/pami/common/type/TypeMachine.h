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
 * \file common/type/TypeMachine.h
 * \brief ???
 */
#ifndef __common_type_TypeMachine_h__
#define __common_type_TypeMachine_h__

#include "TypeCode.h"
#include "TypeFunc.h"

/*
   0         1         2         3
   0123456789012345678901234567890 <-- byte displacement in data type
   -------------------------------
   ..##..##..##......##..##..##...
   -------------------------------
     0                       1
     01  23  45      67  89  01    <-- byte offset in data type
             ^
             |------------------------ cursor, pointing to offset 4
                                       at displacement 10

 */

namespace PAMI
{
  namespace Type
  {
    class TypeMachine
    {
        public:
            TypeMachine(TypeCode *type);
            TypeMachine(TypeCode *type, void *custom_stack_addr, size_t custom_stack_size);
            TypeMachine(void *type_code_addr, size_t type_code_size,
                        void *custom_stack_addr, size_t custom_stack_size);
            ~TypeMachine();
            void SetType(TypeCode *new_type);

            void Show() const;

            void SetCopyFunc(TypeFunc::CopyFunction new_copy_func, void *new_cookie);
            TypeCode * GetType() const;
            size_t GetCursor() const;
            size_t GetCursorDisp() const;

            void MoveCursor(size_t offset);
            void EnumerateBlocks(size_t disp[], size_t bytes[], size_t count);
            void Pack(void *target, void *origin, size_t bytes);
            void Unpack(void *target, void *origin, size_t bytes);
            size_t GetContigBytes();

        private:
            TypeCode  *type;
            TypeCode  *orig_type;
            void  *cookie;
            TypeFunc::CopyFunction  copy_func;
            uint8_t _type[sizeof(TypeCode)];

            struct Cursor {
                size_t  pc;        // program counter for code
                size_t  disp;      // cursor displacement
                size_t  offset;    // cursor offset
                size_t  rep_num;   // current repetition number
                size_t  rep_bytes; // bytes consumed in current repetition

                Cursor() { }
                Cursor(size_t pc, size_t disp, size_t offset,
                        size_t rep_num, size_t rep_bytes)
                    : pc(pc), disp(disp), offset(offset),
                    rep_num(rep_num), rep_bytes(rep_bytes) { }

                void Show() const {
                    printf("pc %zu disp %zu offset %zu rep_num %zu rep_bytes %zu\n",
                            pc, disp, offset, rep_num, rep_bytes);
                }
            };

            static const int SMALL_STACK_DEPTH = 4;
            Cursor small_stack[SMALL_STACK_DEPTH];
            Cursor *stack;
            size_t top;
            bool deallocate_stack;

            void InternalMemCopy(void *to, void *from, size_t size);
            template <bool PACK, bool INTERNAL>
                void MemCopy(char *to, char *from, size_t size);
            template <bool PACK, bool INTERNAL>
                void Copy(char *target, char *origin, size_t size,
                        TypeCode::Copy &copy, Cursor &cursor);
            template <bool PACK, bool INTERNAL>
                void Run(char *target, char *origin, size_t size);

            void ExecuteCall(TypeCode::Op *op);
            void ExecuteShift(TypeCode::Op *op);
            void ExecuteEnd();


        public:
            static const size_t cursor_size = sizeof(Cursor);
    };

    inline TypeMachine::TypeMachine(TypeCode *atype)
      : type(atype), orig_type(NULL), cookie(NULL), copy_func(NULL), top(0), deallocate_stack(false)
    {
        assert(type->IsCompleted());
        type->AcquireReference();

        // Check the type and, if contiguous, change it to
        // a max contiguous type
        if (type->IsContiguous()) {
            orig_type = type;
            type = PAMI_TYPE_CONTIG_MAX;
        }

        if (type->GetDepth() <= (unsigned)SMALL_STACK_DEPTH)
            stack = small_stack;
        else
          {
            stack = new Cursor[type->GetDepth()];
            deallocate_stack = true;
          }

        stack[top] = Cursor(sizeof(TypeCode::Begin), 0, 0, 0, 0);
    }

    inline TypeMachine::TypeMachine(TypeCode *atype, void *custom_stack_addr, size_t custom_stack_size)
      : type(atype), orig_type(NULL), cookie(NULL), copy_func(NULL), top(0), deallocate_stack(false)
    {
        assert(type->IsCompleted());
        type->AcquireReference();

        // Check the type and, if contiguous, change it to
        // a max contiguous type
        if (type->IsContiguous()) {
            orig_type = type;
            type = PAMI_TYPE_CONTIG_MAX;
        }

        if (type->GetDepth() <= custom_stack_size/sizeof(Cursor))
            stack = (Cursor *) custom_stack_addr;
        else
          {
            stack = new Cursor[type->GetDepth()];
            deallocate_stack = true;
          }

        stack[top] = Cursor(sizeof(TypeCode::Begin), 0, 0, 0, 0);
    }

    inline TypeMachine::TypeMachine(void *type_code_addr, size_t type_code_size,
                                    void *custom_stack_addr, size_t custom_stack_size)
      : type(NULL), orig_type(NULL), cookie(NULL), copy_func(NULL), top(0), deallocate_stack(false)
    {
        new (_type) TypeCode(type_code_addr,type_code_size,false);
        type = (TypeCode *) & _type;

        type->AcquireReference();

        // Check the type and, if contiguous, change it to
        // a max contiguous type
        if (type->IsContiguous()) {
            orig_type = type;
            type = PAMI_TYPE_CONTIG_MAX;
        }

        if (type->GetDepth() <= custom_stack_size/sizeof(Cursor))
            stack = (Cursor *) custom_stack_addr;
        else
          {
            stack = new Cursor[type->GetDepth()];
            deallocate_stack = true;
          }

        stack[top] = Cursor(sizeof(TypeCode::Begin), 0, 0, 0, 0);
    }


    inline TypeMachine::~TypeMachine()
    {
        // recover the original type if optimized
        if (orig_type) {
            type = orig_type;
            orig_type = (TypeCode *)NULL;
        }
        type->ReleaseReference();
        if (deallocate_stack)
            delete[] stack;
    }

    inline void TypeMachine::SetType(TypeCode *new_type)
    {
        assert(new_type->IsCompleted()&&(new_type!=PAMI_TYPE_CONTIG_MAX));

        new_type->AcquireReference();

        // recover the initial object if optimized
        if (orig_type) {
            type = orig_type;
            orig_type = (TypeCode *)NULL;
        }

        type->ReleaseReference();

        // Check the new type and, if contiguous, change it to
        // the max contiguous type
        if (new_type->IsContiguous()) {
            orig_type = new_type;
            new_type = PAMI_TYPE_CONTIG_MAX;
        }
		
        // adjust stack if new_type requires more
        size_t new_depth = new_type->GetDepth();
        if (new_depth > type->GetDepth() && new_depth > (unsigned)SMALL_STACK_DEPTH) {
            if (deallocate_stack)
                delete[] stack;
            stack = new Cursor[new_depth];
            deallocate_stack = true;
        }

        type = new_type;

        top  = 0;
        stack[top] = Cursor(sizeof(TypeCode::Begin), 0, 0, 0, 0);
    }

    inline void TypeMachine::Show() const
    {
        for (unsigned i = 0; i <= top; i++) {
            printf("%4d: ", i);
            stack[i].Show();
        }
    }

    inline TypeCode *TypeMachine::GetType() const
    {
        if (orig_type) 
            return orig_type;

        return type;
    }

    inline void TypeMachine::SetCopyFunc(TypeFunc::CopyFunction new_copy_func, void *new_cookie)
    {
        uintptr_t fn = (uintptr_t) new_copy_func;

        TypeCode * t = type;
        if (orig_type != NULL)
            t = orig_type;

        if ((t->GetPrimitive() == TypeCode::PRIMITIVE_TYPE_COUNT) ||
            (fn >= TypeFunc::PRIMITIVE_FUNC_COUNT))
        {
          copy_func = new_copy_func;
          cookie    = new_cookie;
        }
        else
        {
          copy_func = TypeFunc::GetCopyFunction (t->GetPrimitive(), (TypeFunc::primitive_func_t) fn);
        }
    }

    inline size_t TypeMachine::GetCursor() const
    {
        return stack[top].offset;
    }

    inline size_t TypeMachine::GetCursorDisp() const
    {
        return stack[top].disp;
    }

    inline void TypeMachine::MoveCursor(size_t new_offset)
    {
        if (stack[top].offset == new_offset)
            return;

        top = 0;
        char * code = type->code;
        size_t pc = sizeof(TypeCode::Begin);
        size_t skip_reps = new_offset / type->GetDataSize();
        size_t disp = skip_reps * type->GetExtent();
        size_t offset = skip_reps * type->GetDataSize();

        for (;;) {
            TypeCode::Op *op = (TypeCode::Op *)(code + pc);
            switch (op->opcode) {
                case TypeCode::COPY:
                    {
                        TypeCode::Copy &copy = *(TypeCode::Copy *)op;
                        size_t copy_size = copy.bytes * copy.reps;
                        if (offset + copy_size > new_offset) {
                            size_t diff_size = new_offset - offset;
                            size_t rep_num   = diff_size / copy.bytes;
                            size_t rep_bytes = diff_size - copy.bytes * rep_num;
                            disp += copy.stride * rep_num;
                            stack[top] = Cursor(pc, disp, new_offset, rep_num,
                                    rep_bytes);
                            return;
                        } else {
                            pc += sizeof(TypeCode::Copy);
                            disp += copy.stride * copy.reps;
                            offset += copy_size;
                        }
                        break;
                    }
                case TypeCode::CALL:
                    {
                        TypeCode::Call &call = *(TypeCode::Call *)op;
                        TypeCode::Begin &sub =
                            *(TypeCode::Begin *)(code + pc + call.sub_type);

                        size_t sub_bytes = sub.data_size;
                        size_t copy_size = sub_bytes * call.reps;
                        if (offset + copy_size > new_offset) {
                            size_t diff_size = new_offset - offset;
                            size_t rep_num   = diff_size / sub_bytes;
                            size_t rep_bytes = diff_size - sub_bytes * rep_num;
                            disp += call.stride * rep_num;
                            offset = new_offset - rep_bytes;
                            stack[top] = Cursor(pc, disp, offset, rep_num, 0);

                            // process the sub type
                            top++;
                            pc += call.sub_type + sizeof(TypeCode::Begin);
                        } else {
                            pc += sizeof(TypeCode::Call);
                            disp += call.stride * call.reps;
                            offset += copy_size;
                        }
                        break;
                    }
                case TypeCode::SHIFT:
                    {
                        TypeCode::Shift &shift = *(TypeCode::Shift *)op;
                        disp += shift.shift;
                        pc += sizeof(TypeCode::Shift);
                        break;
                    }
                case TypeCode::END:
                    {
                        // go back to the beginning
                        pc = sizeof(TypeCode::Begin);
                        break;
                    }
                case TypeCode::BEGIN: assert(!"Not executable");
                default:          assert(!"Bogus opcode");
            }
        }
    }

    inline void TypeMachine::EnumerateBlocks(size_t disp[], size_t bytes[], size_t count)
    {
        int index = 0;
        while (count > 0) {
            Cursor &cursor = stack[top];
            assert(cursor.rep_bytes == 0);

            TypeCode::Op *op = (TypeCode::Op *)(type->code + cursor.pc);
            switch (op->opcode) {
                case TypeCode::COPY:
                    {
                        TypeCode::Copy &copy = *(TypeCode::Copy *)op;
                        size_t reps_left = copy.reps - cursor.rep_num;
                        assert(reps_left > 0);

                        size_t reps_to_copy = std::min(reps_left, count);
                        for (unsigned i = 0; i < reps_to_copy; i++, index++) {
                            disp[index]  = cursor.disp;
                            bytes[index] = copy.bytes;
                            cursor.disp += copy.stride;
                        }
                        cursor.offset  += copy.bytes * reps_to_copy;
                        cursor.rep_num += reps_to_copy;
                        if (cursor.rep_num == copy.reps) {
                            // move to next op
                            cursor.pc += sizeof(TypeCode::Copy);
                            cursor.rep_num = 0;
                        }

                        count -= reps_to_copy;
                        if (count == 0)
                            return;
                        break;
                    }
                case TypeCode::CALL:  ExecuteCall(op); break;
                case TypeCode::SHIFT: ExecuteShift(op); break;
                case TypeCode::END:   ExecuteEnd(); continue;
                case TypeCode::BEGIN: assert(!"Not executable");
                default:          assert(!"Bogus opcode");
            }
        }
    }

    inline void TypeMachine::InternalMemCopy(void *to, void *from, size_t size)
    {
        switch (size) {
            case 16:
                ((double *)to)[0] = ((double *)from)[0];
                ((double *)to)[1] = ((double *)from)[1];
                break;
            case 8: *(double *)to   = *(double *)from; break;
            case 4: *(uint32_t *)to = *(uint32_t *)from; break;
            case 2: *(uint16_t *)to = *(uint16_t *)from; break;
            case 1: *(uint8_t *)to  = *(uint8_t *)from; break;
            default: memcpy(to, from, size); break;
        }
    }

    template <bool PACK, bool INTERNAL>
        inline void TypeMachine::MemCopy(char *to, char *from, size_t size)
        {
            if (INTERNAL) {
                if (PACK)
                    InternalMemCopy(to, from, size);
                else
                    InternalMemCopy(from, to, size);
            } else {
                if (PACK)
                    copy_func(to, from, size, cookie);
                else
                    copy_func(from, to, size, cookie);
            }
        }

    template <bool PACK, bool INTERNAL>
        inline void TypeMachine::Copy(char *target, char *origin, size_t bytes,
                TypeCode::Copy &copy, Cursor &cursor)
        {
            size_t &pc        = cursor.pc;
            size_t &disp      = cursor.disp;
            size_t &offset    = cursor.offset;
            size_t &rep_num   = cursor.rep_num;
            size_t &rep_bytes = cursor.rep_bytes;

            // this copy op must provide enough data
            assert(bytes <= copy.bytes * (copy.reps - rep_num) - rep_bytes);

            size_t rep_bytes_left = copy.bytes - rep_bytes;
            if (bytes > rep_bytes_left) {
                // copy leftover of current rep
                MemCopy<PACK, INTERNAL>(target, origin + disp + rep_bytes, rep_bytes_left);
                target += rep_bytes_left;
                bytes  -= rep_bytes_left;

                // move to next rep
                disp   += copy.stride;
                offset += rep_bytes_left;
                rep_num++;
                rep_bytes = 0;

                // copy integral number of reps
                for (; bytes > copy.bytes; rep_num++) {
                    MemCopy<PACK, INTERNAL>(target, origin + disp, copy.bytes);
                    target += copy.bytes;
                    bytes  -= copy.bytes;
                    disp   += copy.stride;
                    offset += copy.bytes;
                }
                assert(rep_num < copy.reps);
            }

            // copy no more than one rep's data
            assert(bytes <= copy.bytes - rep_bytes);
            MemCopy<PACK, INTERNAL>(target, origin + disp + rep_bytes, bytes);
            target    += bytes;
            offset    += bytes;
            rep_bytes += bytes;

            if (rep_bytes == copy.bytes) {
                // move to next rep
                disp += copy.stride;
                rep_num++;
                rep_bytes = 0;
                if (rep_num == copy.reps) {
                    // move to next op
                    pc += sizeof(TypeCode::Copy);
                    rep_num = 0;
                }
            }
            return;
        }

    template <bool PACK, bool INTERNAL>
        inline void TypeMachine::Run(char *target, char *origin, size_t bytes)
        {
            while (bytes > 0) {
                Cursor &cursor = stack[top];
                TypeCode::Op *op = (TypeCode::Op *)(type->code + cursor.pc);
                switch (op->opcode) {
                    case TypeCode::COPY:
                        {
                            TypeCode::Copy &copy = *(TypeCode::Copy *)op;
                            size_t bytes_left = copy.bytes *
                                (copy.reps - cursor.rep_num) - cursor.rep_bytes;
                            assert(bytes_left > 0);

                            size_t bytes_to_copy = std::min(bytes_left, bytes);
                            Copy<PACK, INTERNAL>(target, origin, bytes_to_copy,
                                    copy, cursor);
                            target += bytes_to_copy;
                            bytes -= bytes_to_copy;
                            if (bytes == 0)
                                return;
                            break;
                        }
                    case TypeCode::CALL:  ExecuteCall(op); continue;
                    case TypeCode::SHIFT: ExecuteShift(op); break;
                    case TypeCode::END:   ExecuteEnd(); continue;
                    case TypeCode::BEGIN: assert(!"Not executable");
                    default:          assert(!"Bogus opcode");
                }
            }
        }

	// same as Run() but return as soon as we local some contiguous bytes.
        inline size_t TypeMachine::GetContigBytes()
        {
            for (;;) {
                Cursor &cursor = stack[top];
                TypeCode::Op *op = (TypeCode::Op *)(type->code + cursor.pc);
                switch (op->opcode) {
                    case TypeCode::COPY:
                        {
                            TypeCode::Copy &copy = *(TypeCode::Copy *)op;
                            size_t bytes_left = copy.bytes *
                                (copy.reps - cursor.rep_num) - cursor.rep_bytes;
                            assert(bytes_left > 0);

                            return bytes_left;
                            break;
                        }
                    case TypeCode::CALL:  ExecuteCall(op); continue;
                    case TypeCode::SHIFT: ExecuteShift(op); break;
                    case TypeCode::END:   ExecuteEnd(); continue;
                    case TypeCode::BEGIN: assert(!"Not executable");
                    default:          assert(!"Bogus opcode");
                }
            }
        }

    inline void TypeMachine::ExecuteCall(TypeCode::Op *op)
    {
        TypeCode::Call &call = *(TypeCode::Call *)op;
        Cursor &cursor = stack[top];
        top++;
        Cursor &new_cursor = stack[top];
        new_cursor.pc = cursor.pc + call.sub_type + sizeof(TypeCode::Begin);
        new_cursor.disp = cursor.disp;
        new_cursor.offset = cursor.offset;
        new_cursor.rep_num = 0;
        new_cursor.rep_bytes = 0;
    }

    inline void TypeMachine::ExecuteShift(TypeCode::Op *op)
    {
        TypeCode::Shift &shift = *(TypeCode::Shift *)op;
        Cursor &cursor = stack[top];
        cursor.pc   += sizeof(TypeCode::Shift);
        cursor.disp += shift.shift;
    }

    inline void TypeMachine::ExecuteEnd()
    {
        Cursor &cursor = stack[top];
        if (top == 0) {
            // repeat the type, keeping current displacement and offset
            cursor.pc = sizeof(TypeCode::Begin);
            cursor.rep_num = 0;
            cursor.rep_bytes = 0;
        } else {
            // return from the sub type
            top--;
            Cursor &old_cursor = stack[top];
            TypeCode::Call &call = *(TypeCode::Call *) (type->code + old_cursor.pc);
            old_cursor.disp += call.stride;
            old_cursor.offset = cursor.offset;
            old_cursor.rep_num++;
            old_cursor.rep_bytes = 0;
            if (old_cursor.rep_num == call.reps) {
                old_cursor.pc += sizeof(TypeCode::Call);
                old_cursor.rep_num = 0;
            }
        }
    }

    inline void TypeMachine::Pack(void *target, void *origin, size_t bytes)
    {
        if ((copy_func == NULL) || (copy_func == PAMI_DATA_COPY))
            Run<true, true>((char *)target, (char *)origin, bytes);
        else
            Run<true, false>((char *)target, (char *)origin, bytes);
    }

    inline void TypeMachine::Unpack(void *target, void *origin, size_t bytes)
    {
        if ((copy_func == NULL) || (copy_func == PAMI_DATA_COPY))
            Run<false, true>((char *)origin, (char *)target, bytes);
        else
            Run<false, false>((char *)origin, (char *)target, bytes);
    }

  }
}

#endif // _PAMI_TYPE_MACHINE_H
