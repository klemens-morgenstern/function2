#include <iostream>

#include <boost/function2.hpp>
#include "boost/function2/unique_function.hpp"


int main()
{
  namespace fu2 = boost::function2;

  auto x = [i = 1](auto value) mutable { printf("X %d %s\n", i++, typeid(value).name()); return value;};

  fu2::function_ref<int(int), double(double)> rf{x};
  fu2::unique_function<int(int), double(double)> uf{x};

  printf("XYZ %s\n", rf.target_type().name());

  printf("Int %d\n", rf(42));
  printf("Int %d\n", rf(43));
  printf("Dbl %f\n", rf(4.2));

  printf("XYZ %s\n", uf.target_type().name());

  printf("Int %d\n", uf(42));
  printf("Dbl %f\n", uf(4.2));

  //std::get<0>(f.entries)((void*)pm);
  //fu2::function_ref<int(), int(int), int(int, int)> ff;

  return 0;
}
