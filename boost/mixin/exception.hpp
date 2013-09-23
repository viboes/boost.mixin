//
// Copyright (c) 2013 Borislav Stanimirov, Zahary Karadjov
//
// Distributed under the Boost Software License, Version 1.0.
// See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt
//
#if !defined(_BOOST_MIXIN_EXCEPTION_HPP_INCLUDED)
#define _BOOST_MIXIN_EXCEPTION_HPP_INCLUDED

#include "global.hpp"
#include <boost/preprocessor/stringize.hpp>

#if BOOST_MIXIN_USE_EXCEPTIONS

#include <boost/exception/all.hpp>

namespace boost
{
namespace mixin
{

class exception : public ::boost::exception, public ::std::exception {};

class invalid_domain : public exception {};

class invalid_transform : public exception {};

}
}

#   define BOOST_MIXIN_THROW_UNLESS(test, exception) do { if(!(test)) { BOOST_THROW_EXCEPTION(exception()); } } while(false)

#else // BOOST_MIXIN_USE_EXCEPTIONS

#   define BOOST_MIXIN_THROW_UNLESS(test, exception) BOOST_ASSERT_MSG(test, BOOST_PP_STRINGIZE(exception))

#endif // BOOST_MIXIN_USE_EXCEPTIONS

#endif // _BOOST_MIXIN_EXCEPTION_HPP_INCLUDED
