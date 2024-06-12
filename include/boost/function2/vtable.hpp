//
// Copyright (c) 2024 Klemens Morgenstern (klemens.morgenstern@gmx.net)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef BOOST_FUNCTION2_VTABLE_HPP
#define BOOST_FUNCTION2_VTABLE_HPP

#include <boost/callable_traits/add_noexcept.hpp>
#include <boost/callable_traits/add_transaction_safe.hpp>
#include <boost/callable_traits/add_varargs.hpp>
#include <boost/callable_traits/args.hpp>
#include <boost/callable_traits/function_type.hpp>
#include <boost/callable_traits/is_const_member.hpp>
#include <boost/callable_traits/is_lvalue_reference_member.hpp>
#include <boost/callable_traits/is_rvalue_reference_member.hpp>
#include <boost/callable_traits/is_volatile_member.hpp>
#include <boost/callable_traits/is_noexcept.hpp>
#include <boost/callable_traits/is_invocable.hpp>
#include <boost/callable_traits/is_transaction_safe.hpp>
#include <boost/callable_traits/return_type.hpp>

namespace boost
{
namespace function2
{

template<typename Signature>
struct vtable_entry
{
  using this_c = typename std::conditional<callable_traits::is_const_member_v<Signature>, typename std::add_const<void>::type, void>::type;
  using this_cv = typename std::conditional<callable_traits::is_volatile_member_v<Signature>, typename std::add_volatile<this_c>::type, this_c>::type;

  using args = callable_traits::args_t<Signature>;
  using return_type = callable_traits::return_type_t<Signature>;

  template<typename Return, typename ... Args>
  static auto deduce_internal_signature(Return *, std::tuple<Args...> *) -> Return(*)(this_cv *, Args...);

  using base_type = typename std::remove_pointer<
      decltype(deduce_internal_signature(
          std::declval<return_type *>(),
          std::declval<args *>()))>::type;

  constexpr static bool is_noexcept = callable_traits::is_noexcept_v<Signature>;
  using type = typename std::conditional<is_noexcept, callable_traits::add_noexcept_t<base_type>, base_type>::type;

  template<typename Func, typename ... Args>
  constexpr static auto make_func_impl(Func *, std::tuple<Args...> *)
  -> typename std::enable_if<
      callable_traits::is_invocable<
            typename std::conditional<callable_traits::is_rvalue_reference_member_v<Func>, Func &&, Func &>::type, Args...>::value &&
      std::is_convertible<
        decltype(std::declval<
            typename std::conditional<callable_traits::is_rvalue_reference_member_v<Func>, Func &&, Func &>::type
            >()(std::declval<Args>()...)),
        return_type
      >::value, type>::type *
  {
    return +[](this_cv *this_, Args ... args) noexcept(is_noexcept)
    {
      using func_type =
          typename std::conditional<callable_traits::is_rvalue_reference_member_v<Func>, Func &&, Func &>
              ::type;

      return static_cast<func_type>(*reinterpret_cast<Func *>(this_))(static_cast<Args>(args)...);
    };
  }

  template<typename Func>
  constexpr static auto make_func(Func *f) -> decltype(make_func_impl(f, static_cast<args *>(nullptr)))
  {
    return make_func_impl(f, static_cast<args *>(nullptr));
  }
};


template<typename ... Signatures>
struct vtable
{
  template<typename Func>
  constexpr vtable(Func *f)
      : entries{vtable_entry<Signatures>::make_func(f)...}
#if !defined(BOOST_NO_RTTI)
      , get_type_info(+[]() -> const std::type_info & { return typeid(Func); })
#endif
  {}

  std::tuple<typename vtable_entry<Signatures>::type *...> entries;
#if !defined(BOOST_NO_RTTI)

  const std::type_info &(*get_type_info)();

#endif

  template<typename Signature>
  constexpr auto get() const -> typename vtable_entry<Signature>::type *
  {
    return std::get<typename vtable_entry<Signature>::type *>(entries);
  }

  template<typename Func>
  static vtable &get_vtable()
  {
    static vtable vt{static_cast<Func *>(nullptr)};
    return vt;
  }
};

template<typename ... Signature>
struct deletable_vtable : vtable<Signature...>
{
  template<typename Func>
  constexpr deletable_vtable(Func *f)
      : vtable<Signature...>(f),
        do_delete([](void * me) { delete static_cast<Func*>(me); })
  {
  }

  template<typename Func>
  static deletable_vtable &get_vtable()
  {
    static deletable_vtable vt{static_cast<Func *>(nullptr)};
    return vt;
  }

  void(*do_delete)(void*);
};

template<typename ... Signature>
struct copyable_vtable : deletable_vtable<Signature...>
{
  template<typename Func>
  constexpr copyable_vtable(Func *f)
      : deletable_vtable<Signature...>(f),
        copy_construct([](void * me) -> void * { return new Func(*static_cast<Func*>(me)); }),
        copy_assign([](void * lhs, void * rhs) { *static_cast<Func*>(lhs) = *static_cast<Func*>(rhs); })
  {
  }

  template<typename Func>
  static copyable_vtable &get_vtable()
  {
    static copyable_vtable vt{static_cast<Func *>(nullptr)};
    return vt;
  }

  void* (*copy_construct)(void*);
  void  (*copy_assign)   (void*, void *);
};

}
}

#endif //BOOST_FUNCTION2_VTABLE_HPP
