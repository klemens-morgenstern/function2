//
// Copyright (c) 2024 Klemens Morgenstern (klemens.morgenstern@gmx.net)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef BOOST_FUNCTION2_FUNCTION_HPP
#define BOOST_FUNCTION2_FUNCTION_HPP

namespace boost
{
namespace function2
{

template<typename ... Signatures>
struct function : callable<Signatures...>
{
  template<typename Func, typename = typename enable_for_signatures<std::decay_t<Func>, Signatures...>::type>
  constexpr function(Func&& func)
      : callable<Signatures...>(new std::decay_t<Func>(std::forward<Func>(func)),
                                copyable_vtable<Signatures...>::template get_vtable<std::decay_t<Func>>())
  {
  }

  constexpr function(const function & rhs) noexcept
      : callable<Signatures...>(static_cast<deletable_vtable<Signatures...>*>(this->vtable_)->copy_construct(rhs.target_), rhs.vtable_)
  {

  }
  constexpr function(function && rhs) noexcept : callable<Signatures...>(std::move(rhs))
  {
    rhs.target_ = nullptr;
    rhs.vtable_ = nullptr;
  }

  constexpr function& operator=(const function & rhs) noexcept
  {
    if (this->vtable_ == rhs.vtable_)
      static_cast<deletable_vtable<Signatures...>*>(this->vtable_)->copy_assign(this->target_, rhs.target_);
    else
      *this = function(rhs);

    return *this;
  }
  constexpr function& operator=(function && rhs) noexcept
  {
    std::swap(this->target_, rhs.target_);
    std::swap(this->vtable_, rhs.vtable_);
    return *this;
  }

  ~function()
  {
    if (*this)
      static_cast<deletable_vtable<Signatures...>*>(this->vtable_)->do_delete(this->target_);
  }

 private:
};



}
}

#endif //BOOST_FUNCTION2_FUNCTION_HPP
