//
// Copyright (c) 2024 Klemens Morgenstern (klemens.morgenstern@gmx.net)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef BOOST_FUNCTION2_CALLABLE_HPP
#define BOOST_FUNCTION2_CALLABLE_HPP

#include <boost/function2/vtable.hpp>
#include <boost/mp11/function.hpp>
#include <boost/type_traits/make_void.hpp>

namespace boost
{
namespace function2
{

template<typename ... Signatures>
struct callable;

namespace detail
{

template<typename Signature, typename ... Signatures>
struct callable_base;

#define BOOST_FUNCTION2_SPECIALIZE_CALLABLE_BASE(Qualifier)                                                 \
template<typename Return, typename ... Args, typename ... Signatures>                                       \
struct callable_base<Return(Args...) Qualifier, Signatures ...>                                             \
{                                                                                                           \
    Return operator()(Args... args) Qualifier                                                               \
    {                                                                                                       \
      auto this_ = static_cast<callable<Signatures...>*>(this);                                             \
      return this_->vtable_->template get<Return(Args...) Qualifier>()(this_->target_, std::move(args)...); \
    }                                                                                                       \
};                                                                                                          \

BOOST_FUNCTION2_SPECIALIZE_CALLABLE_BASE();
BOOST_FUNCTION2_SPECIALIZE_CALLABLE_BASE(&);
BOOST_FUNCTION2_SPECIALIZE_CALLABLE_BASE(&&);
BOOST_FUNCTION2_SPECIALIZE_CALLABLE_BASE(const);
BOOST_FUNCTION2_SPECIALIZE_CALLABLE_BASE(const &);
BOOST_FUNCTION2_SPECIALIZE_CALLABLE_BASE(const &&);
BOOST_FUNCTION2_SPECIALIZE_CALLABLE_BASE(volatile);
BOOST_FUNCTION2_SPECIALIZE_CALLABLE_BASE(volatile &);
BOOST_FUNCTION2_SPECIALIZE_CALLABLE_BASE(volatile &&);
BOOST_FUNCTION2_SPECIALIZE_CALLABLE_BASE(const volatile);
BOOST_FUNCTION2_SPECIALIZE_CALLABLE_BASE(const volatile &);
BOOST_FUNCTION2_SPECIALIZE_CALLABLE_BASE(const volatile &&);


#if defined(__cpp_noexcept_function_type)
BOOST_FUNCTION2_SPECIALIZE_CALLABLE_BASE(noexcept);
BOOST_FUNCTION2_SPECIALIZE_CALLABLE_BASE(& noexcept);
BOOST_FUNCTION2_SPECIALIZE_CALLABLE_BASE(&& noexcept);
BOOST_FUNCTION2_SPECIALIZE_CALLABLE_BASE(const noexcept);
BOOST_FUNCTION2_SPECIALIZE_CALLABLE_BASE(const & noexcept);
BOOST_FUNCTION2_SPECIALIZE_CALLABLE_BASE(const && noexcept);
BOOST_FUNCTION2_SPECIALIZE_CALLABLE_BASE(volatile noexcept);
BOOST_FUNCTION2_SPECIALIZE_CALLABLE_BASE(volatile & noexcept);
BOOST_FUNCTION2_SPECIALIZE_CALLABLE_BASE(volatile && noexcept);
BOOST_FUNCTION2_SPECIALIZE_CALLABLE_BASE(const volatile noexcept);
BOOST_FUNCTION2_SPECIALIZE_CALLABLE_BASE(const volatile & noexcept);
BOOST_FUNCTION2_SPECIALIZE_CALLABLE_BASE(const volatile && noexcept);

#endif

#undef BOOST_FUNCTION2_SPECIALIZE_CALLABLE_BASE

}


template<typename ... Signatures>
struct callable : detail::callable_base<Signatures, Signatures...>...
{
  using detail::callable_base<Signatures, Signatures...>::operator()...;

  constexpr operator bool() const {return target_ != nullptr;}

#if !defined(BOOST_NO_RTTI)
  const std::type_info & target_type() const noexcept
  {
    if (vtable_ == nullptr)
      return typeid(nullptr);
    else
      return (*vtable_->get_type_info)();
  }
#endif

  template< class T >
  T* target() noexcept
  {
    if (&vtable<Signatures...>::template get<T>() == vtable_)
      return static_cast<T*>(target_);
    else
      return nullptr;
  }

  template< class T >
  const T* target() const noexcept
  {
    if (&vtable<Signatures...>::template get<T>() == vtable_)
      return static_cast<const T*>(target_);
    else
      return nullptr;
  }


 protected:

  template<typename , typename ... >
  friend struct detail::callable_base;

  template<typename Func>
  constexpr callable(Func * func, vtable<Signatures...> &vtable_ = vtable<Signatures...>::template get_vtable<Func>())
      : target_(func), vtable_(&vtable_) {}
  constexpr callable() noexcept = default;
  constexpr callable(std::nullptr_t) noexcept {}

  constexpr callable(const callable & rhs) noexcept = default;
  constexpr callable(callable && rhs) noexcept = default;

  constexpr callable& operator=(const callable & rhs) noexcept = default;
  constexpr callable& operator=(callable && rhs) noexcept = default;

  constexpr callable& operator=(std::nullptr_t ) noexcept
  {
    target_ = nullptr;
    vtable_ = nullptr;
    return *this;
  }


  void * target_ = nullptr;
  vtable<Signatures...> * vtable_ = nullptr;
};


template<typename Func,
         typename Signature,
         typename = void>
struct is_callable_with  : std::false_type {};

template<typename Func,
    typename Signature>
struct is_callable_with<
    Func, Signature,
    void_t<decltype(vtable_entry<Signature>::make_func(static_cast<Func*>(nullptr)))>> : std::true_type
{
};

template<typename Func, typename ... Signatures>
using enable_for_signatures = std::enable_if<mp11::mp_and<is_callable_with<Func, Signatures>...>::value >;


};
}

#endif //BOOST_FUNCTION2_CALLABLE_HPP
