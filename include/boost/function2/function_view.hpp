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
struct function_view : callable<Signatures...>
{
  template<typename Func, typename = typename enable_for_signatures<Func, Signatures...>::type>
  constexpr function_view(Func& func) : callable<Signatures...>(&func) {}

  constexpr function_view() noexcept = default;
  constexpr function_view(std::nullptr_t) noexcept : callable<Signatures...>(nullptr) {}

  constexpr function_view(function_view && rhs) noexcept = default;
  constexpr function_view(const function_view & rhs) noexcept = default;

  constexpr function_view& operator=(function_view && rhs) noexcept = default;
  constexpr function_view& operator=(const function_view & rhs) noexcept = default;

  constexpr function_view& operator=(std::nullptr_t) noexcept
  {
    callable<Signatures...>::operator=(nullptr);
    return *this;
  }

};


}
}

#endif //BOOST_FUNCTION2_FUNCTION_REF_HPP
