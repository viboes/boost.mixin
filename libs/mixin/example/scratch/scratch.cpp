//
// Copyright (c) 2013-2014 Borislav Stanimirov, Zahary Karadjov
//
// Distributed under the Boost Software License, Version 1.0.
// See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt
//

#include <boost/mixin.hpp>
#include <iostream>

using namespace boost::mixin;
using namespace std;

BOOST_DECLARE_MIXIN(has_u1m1);
BOOST_DECLARE_MIXIN(has_m1);
BOOST_DECLARE_MIXIN(has_u2m2);
BOOST_DECLARE_MIXIN(has_m2);

BOOST_MIXIN_MULTICAST_MESSAGE_0(int, m1);
BOOST_MIXIN_MULTICAST_MESSAGE_0(void, m2);
BOOST_MIXIN_MESSAGE_0(void, u1);
BOOST_MIXIN_MESSAGE_0(void, u2);

typedef boost::error_info<struct my_tag,std::string> my_tag_error_info;

int main()
{
    {
        object o;

        mutate(o)
            .add<has_u1m1>()
            .add<has_m1>();

        m1<combinators::sum>(o);
        m1(o, combinators::sum<int>());
    }

    return 0;
}

class has_u1m1
{
public:
    void u1() {}
    int m1() { return 1; }
};

class has_m1
{
public:
    int m1() { return 2; }
};

class has_u2m2
{
public:
    void u2() {}
    void m2() {}
};

class has_m2
{
public:
    void m2() {}
};

// this order should be important if the messages aren't sorted by mixin name
BOOST_DEFINE_MIXIN(has_u1m1, u1_msg & m1_msg);
BOOST_DEFINE_MIXIN(has_m1, m1_msg);
BOOST_DEFINE_MIXIN(has_u2m2, u2_msg & m2_msg);
BOOST_DEFINE_MIXIN(has_m2, m2_msg);

BOOST_MIXIN_DEFINE_MESSAGE(m1);
BOOST_MIXIN_DEFINE_MESSAGE(m2);
BOOST_MIXIN_DEFINE_MESSAGE(u1);
BOOST_MIXIN_DEFINE_MESSAGE(u2);
