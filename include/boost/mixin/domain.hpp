//
// Copyright (c) 2013-2014 Borislav Stanimirov, Zahary Karadjov
//
// Distributed under the Boost Software License, Version 1.0.
// See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt
//
#if !defined(_BOOST_MIXIN_DOMAIN_HPP_INCLUDED)
#define _BOOST_MIXIN_DOMAIN_HPP_INCLUDED

#include <boost/mixin/global.hpp>
#include <boost/mixin/mixin_type_info.hpp>
#include <boost/mixin/object_type_info.hpp>
#include <boost/mixin/feature.hpp>
#include <boost/mixin/feature_parser.hpp>
#include <boost/mixin/message.hpp>

#include <boost/ptr_container/ptr_set.hpp>
#include <boost/type_traits/alignment_of.hpp>

#if BOOST_MIXIN_USING_CXX11
#   include <unordered_map>
#else
#   include <boost/unordered_map.hpp>
#endif

/**
 * \file
 * Domain related classes and functions.
 */

// The domain collection of mixins and messages
// It serves as a library instance of sorts

namespace boost
{
namespace mixin
{

class mutation_rule;
class object_type_mutation;
class global_allocator;

namespace internal
{

struct message_t;

#if !BOOST_MIXIN_USING_CXX11
// we need to define a hash function for bitsets
// curiously boost doesn't seem to have one
struct hash_avaliable_mixins_bitset : std::unary_function<available_mixins_bitset, size_t>
{
    size_t operator()(const available_mixins_bitset& bs) const
    {
        const int bits = sizeof(size_t)*8;
        size_t result = 0;
        for(size_t i=0; i<BOOST_MIXIN_MAX_MIXINS; ++i)
        {
            // xor groups of bits
            result ^= size_t(bs[i]) << (i % bits);
        }
        return result;
    }
};
#endif

class BOOST_MIXIN_API domain : public noncopyable
{
public:
    static domain& instance();

    void add_new_mutation_rule(mutation_rule* rule);
    void apply_mutation_rules(object_type_mutation& mutation);

    size_t num_registered_mixins() const { return _num_registered_mixins; }

    template <typename Mixin>
    void register_mixin_type(mixin_type_info& info)
    {
        BOOST_ASSERT(info.id == INVALID_MIXIN_ID);
        BOOST_ASSERT_MSG(_num_registered_mixins < BOOST_MIXIN_MAX_MIXINS,
                         "you have to increase the maximum number of mixins");

        info.name = BOOST_MIXIN_TYPE_NAME(Mixin);
        info.size = sizeof(Mixin);
        info.alignment = alignment_of<Mixin>::value;
        info.constructor = &call_mixin_constructor<Mixin>;
        info.destructor = &call_mixin_destructor<Mixin>;
        info.allocator = _allocator;

        internal_register_mixin_type(info);

        // see comments in feature_instance on why this manual registration is needed
        feature_registrator reg;
        _boost_parse_mixin_features((Mixin*)nullptr, reg);

        feature_parser<Mixin> parser;
        _boost_parse_mixin_features((Mixin*)nullptr, parser);
    }

    template <typename Feature>
    void register_feature(Feature& feature)
    {
        // see comments in feature_instance on why features may be registered multiple times
        if(feature.id != INVALID_FEATURE_ID)
        {
            return;
        }

        internal_register_feature(feature);
    }

    // creates a new type info if needed
    const object_type_info* get_object_type_info(const mixin_type_info_vector& mixins);

    const mixin_type_info& mixin_info(mixin_id id) const
    {
        BOOST_ASSERT(id != INVALID_MIXIN_ID);
        BOOST_ASSERT(id <= _num_registered_mixins);
        BOOST_ASSERT(_mixin_type_infos[id]);

        return *_mixin_type_infos[id];
    }

    // sets the current domain allocator
    void set_allocator(global_allocator* allocator);
    global_allocator* allocator() const { return _allocator; }

    // get mixin id by name string
    mixin_id get_mixin_id_by_name(const char* mixin_name) const;

boost_mixin_internal:

    domain();
    ~domain();

    mixin_type_info* _mixin_type_infos[BOOST_MIXIN_MAX_MIXINS];
    size_t _num_registered_mixins;

    void internal_register_mixin_type(mixin_type_info& info);

    const message_t* _messages[BOOST_MIXIN_MAX_MESSAGES];
    size_t _num_registered_messages;

#if BOOST_MIXIN_USING_CXX11
    typedef std::unordered_map<available_mixins_bitset, object_type_info*> object_type_info_map;
#else
    typedef boost::unordered_map<available_mixins_bitset, object_type_info*, hash_avaliable_mixins_bitset > object_type_info_map;
#endif

    object_type_info_map _object_type_infos;

    std::vector<mutation_rule*> _mutation_rules;

    // feature registration functions for the supported kinds of features
    void internal_register_feature(message_t& m);

    // allocators
    global_allocator* _allocator;
};

} // namespace internal

// allocator functions

/// Sets an global allocator for all mixins.
void BOOST_MIXIN_API set_global_allocator(global_allocator* allocator);

} // namespace mixin
} // namespace boost

#endif // _BOOST_MIXIN_DOMAIN_HPP_INCLUDED
