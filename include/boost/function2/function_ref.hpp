//
// Copyright (c) 2024 Klemens Morgenstern (klemens.morgenstern@gmx.net)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef BOOST_FUNCTION2_FUNCTION_REF_HPP
#define BOOST_FUNCTION2_FUNCTION_REF_HPP

#include <boost/function2/callable.hpp>

namespace boost
{
namespace function2
{

template<typename ... Signatures>
struct function_ref : callable<Signatures...>
{
  template<typename Func>
  constexpr function_ref(Func& func /* TODO: Add SFINAE here */) : callable<Signatures...>(&func) {}

  constexpr function_ref(function_ref && rhs) noexcept = default;
  constexpr function_ref(const function_ref & rhs) noexcept = default;

  constexpr function_ref& operator=(function_ref && rhs) noexcept = default;
  constexpr function_ref& operator=(const function_ref & rhs) noexcept = default;
};


}
}

#endif //BOOST_FUNCTION2_FUNCTION_REF_HPP
