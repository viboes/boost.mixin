#
# Copyright (c) 2013 Borislav Stanimirov, Zahary Karadjov
#
# Distributed under the Boost Software License, Version 1.0.
# See accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt
#

# script generating message declaration macros

MAX_ARITY = File.open('arity').read.strip.to_i + 1
OUT_FILE = '../include/boost/mixin/gen/message_macros.ipp'

HEADER = <<DATA
//
// Copyright (c) 2013 Borislav Stanimirov, Zahary Karadjov
//
// Distributed under the Boost Software License, Version 1.0.
// See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt
//
// this file is automatically generated by a script


DATA

DECL = File.open('message_macros_template', 'r').read


def params_for_arity(arity)

  return {
    :arity => 0, :args => '', :arg_types => '', :args_coma => '', :args_signature => '',
    :coma_args => '', :coma_arg_types => '', :fwd_args => '', :coma_fwd_args => ''
  } if arity == 0

  args = []
  arg_types = []
  caller_args = []
  caller_arg_types = []
  arity.times do |i|
    args << "a#{i}"
    arg_types << "arg#{i}_type"
  end

  args_coma = ', ' + arg_types.zip(args).flatten.join(', ')
  args_signature = ', ' + arg_types.zip(args).map { |tuple| tuple.join(' ') }. join(', ')
  fwd_args = arg_types.zip(args).map { |type, arg| "BOOST_MIXIN_FWD(#{type}, #{arg})" }. join(', ')
  args = args.join(', ')
  arg_types = arg_types.join(', ')

  {
    :arity => arity, :args => args, :arg_types => arg_types, :args_coma => args_coma, :args_signature => args_signature,
    :coma_args => ', ' + args, :coma_arg_types => ', ' + arg_types,
    :fwd_args => fwd_args, :coma_fwd_args => ', ' + fwd_args
  }

end

File.open(OUT_FILE, 'w') do |f|
  f.write(HEADER)
  MAX_ARITY.times do |i|
    params = params_for_arity(i)
    f.write(DECL % params)
  end
end


