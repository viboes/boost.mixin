//
// Copyright (c) 2013 Borislav Stanimirov, Zahary Karadjov
//
// Distributed under the Boost Software License, Version 1.0.
// See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt
//
#include "internal.hpp"

#include <boost/mixin/allocators.hpp>
#include <boost/mixin/object_type_info.hpp>

namespace boost
{
namespace mixin
{

const size_t domain_allocator::mixin_data_size = sizeof(internal::mixin_data_in_object);

size_t domain_allocator::calculate_mem_size_for_mixin(size_t mixin_size, size_t mixin_alignment)
{
    // normally alignof(x) + sizeof(x) is enough for an aligned allocation
    // but in this case we want to have an object* before that and the alignment
    // could be smaller than sizeof(object*) - especially on 64 bit platforms
    size_t mem_size = (sizeof(object*) + mixin_alignment - 1) / mixin_alignment; // integer division rounding up
    mem_size *= mixin_alignment;
    mem_size += mixin_size;

    return mem_size;
}

size_t domain_allocator::calculate_mixin_offset(const char* buffer, size_t mixin_alignment)
{
    // now malloc (or new) should make sure to give us memory that's word aligned
    // that means that buffer should be aligned to sizeof(ptr)

    // WARNING: if you don't have a custom allocator and this assert fails
    // this means that memory not-aligned to the pointer size was allocated
    // Thie platform is strange and creepy and is not supported by the default allocator
    // you should write your own, that does allocate properly aligned memory
    BOOST_ASSERT_MSG(uintptr_t(buffer) % sizeof(object*) == 0,
        "allocators should always return memory aligned to sizeof(void*)");

    uintptr_t mixin_pos = (uintptr_t(buffer + sizeof(object*)) + mixin_alignment - 1) / mixin_alignment;
    mixin_pos *= mixin_alignment;

    return mixin_pos - uintptr_t(buffer);
}


static inline char* allocate_mixin_data(size_t count)
{
    BOOST_ASSERT(domain_allocator::mixin_data_size == sizeof(internal::mixin_data_in_object));
    return new char[sizeof(internal::mixin_data_in_object) * count];
}

static inline void deallocate_mixin_data(char* ptr)
{
    delete[] ptr;
}

char* mixin_allocator::alloc_mixin_data(size_t count)
{
    BOOST_ASSERT(false); // a mixin allocator should never have to allocate mixin data
    return allocate_mixin_data(count);
}

void mixin_allocator::dealloc_mixin_data(char* ptr)
{
    BOOST_ASSERT(false); // a mixin allocator should never heve do deallocate mixin data
    deallocate_mixin_data(ptr);
}

namespace internal
{

char* default_domain_allocator::alloc_mixin_data(size_t count)
{
#if defined(BOOST_MIXIN_DEBUG)
    _has_allocated = true;
#endif
    return allocate_mixin_data(count);
}

void default_domain_allocator::dealloc_mixin_data(char* ptr)
{
#if defined(BOOST_MIXIN_DEBUG)
    BOOST_ASSERT(_has_allocated); // what? deallocate without ever allocating?
#endif
    deallocate_mixin_data(ptr);
}

void default_domain_allocator::alloc_mixin(size_t mixin_size, size_t mixin_alignment, char*& out_buffer, size_t& out_mixin_offset)
{
#if defined(BOOST_MIXIN_DEBUG)
    _has_allocated = true;
#endif

    size_t mem_size = calculate_mem_size_for_mixin(mixin_size, mixin_alignment);

    out_buffer = new char[mem_size];

    out_mixin_offset = calculate_mixin_offset(out_buffer, mixin_alignment);

    BOOST_ASSERT(out_mixin_offset + mixin_size <= mem_size); // we should have room for the mixin
}

void default_domain_allocator::dealloc_mixin(char* ptr)
{
#if defined(BOOST_MIXIN_DEBUG)
    BOOST_ASSERT(_has_allocated); // what? deallocate without ever allocating?
#endif
    delete[] ptr;
}

} // namespace internal

} // namespace mixin
} // namespace boost