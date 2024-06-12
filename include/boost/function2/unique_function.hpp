//
// Copyright (c) 2024 Klemens Morgenstern (klemens.morgenstern@gmx.net)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef BOOST_FUNCTION2_UNIQUE_FUNCTION_HPP
#define BOOST_FUNCTION2_UNIQUE_FUNCTION_HPP

#include <boost/function2/callable.hpp>

namespace boost
{
namespace function2
{

template<typename ... Signatures>
struct unique_function : callable<Signatures...>
{
  template<typename Func>
  constexpr unique_function(Func&& func /* TODO: Add SFINAE here */)
    : callable<Signatures...>(new std::decay_t<Func>(std::forward<Func>(func)))
    , do_delete_{[](void * p) {delete static_cast<std::decay_t<Func>*>(p);}}
  {

  }

  constexpr unique_function(const unique_function & rhs) noexcept = delete;
  constexpr unique_function(unique_function && rhs) noexcept : callable<Signatures...>(std::move(rhs))
  {
      rhs.target_ = nullptr;
      rhs.vtable_ = nullptr;
  }

  constexpr unique_function& operator=(const unique_function & rhs) noexcept = delete;
  constexpr unique_function& operator=(unique_function && rhs) noexcept
  {
      std::swap(this->target_, rhs.target_);
      std::swap(this->vtable_, rhs.vtable_);
      return *this;
  }

  ~unique_function()
  {
    if (*this)
      // TODO: move do_delete_ into the VTABLE
      (*do_delete_)(this->target_);
  }

 private:
  void (*do_delete_)(void*);
};

}
}

#endif //BOOST_FUNCTION2_UNIQUE_FUNCTION_HPP
