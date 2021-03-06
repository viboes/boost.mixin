//
// Copyright (c) 2013 Borislav Stanimirov, Zahary Karadjov
//
// Distributed under the Boost Software License, Version 1.0.
// See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt
//
#include "basic.hpp"
#include "object_manager.hpp"
#include "system_messages.hpp"
#include "has_id_mixin.hpp"
#include "transform_mixin.hpp"
#include "d3d_renderer_mixin.hpp"
#include "gl_renderer_mixin.hpp"

using namespace boost::mixin;


// notice how the manager creates objects without knowing anything but the forward declaration
// of the mixin classes
void object_manager::create_objects()
{
    mutually_exclusive_mixins* rendering_mixins = new mutually_exclusive_mixins;
    rendering_mixins->add<d3d_renderer>();
    rendering_mixins->add<gl_renderer>();

    add_new_mutation_rule(rendering_mixins);

    object_type_template type;
    type
        .add<has_id>()
        .add<has_transform>()
        .add<d3d_renderer>()
        .create();

    for(int i=0; i<10; ++i)
    {
        _objects.push_back(new object(type));
        set_id(_objects.back(), i);
    }
}

void object_manager::change_rendering_sytem()
{
    same_type_mutator mutator;

    mutator
        //.remove<d3d_renderer>() <- implied by the mutation rule
        .add<gl_renderer>();

    for(size_t i=0; i<_objects.size(); ++i)
    {
        object& o = _objects[i];

        mutator.apply_to(o);
    }
}
