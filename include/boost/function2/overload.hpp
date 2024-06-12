//
// Copyright (c) 2024 Klemens Morgenstern (klemens.morgenstern@gmx.net)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef BOOST_FUNCTION2_OVERLOADED_HPP
#define BOOST_FUNCTION2_OVERLOADED_HPP

#include <utility>

namespace boost
{
namespace function2
{

template<typename ... Fs>
struct overload_t : Fs...
{
    overload_t(Fs ... fs)  : Fs(std::forward<Fs>(fs))... {}
    using Fs::operator()...;
};

template<typename ... Fs>
auto overload(Fs && ... fs) -> overload_t<std::decay_t<Fs>...>
{
  return overload_t<std::decay_t<Fs>...>{std::forward<Fs>(fs)...};
}


}
}

#endif //BOOST_FUNCTION2_OVERLOADED_HPP
