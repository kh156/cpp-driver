/*
  Copyright (c) 2014-2015 DataStax

  Licensed under the Apache License, Version 2.0 (the "License");
  you may not use this file except in compliance with the License.
  You may obtain a copy of the License at

  http://www.apache.org/licenses/LICENSE-2.0

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.
*/

#ifdef STAND_ALONE
#   define BOOST_TEST_MODULE cassandra
#endif

#include "atomic.hpp"

#include <boost/test/unit_test.hpp>

#include <limits>
#include <stdint.h>

enum Enum {
  ONE, TWO, THREE
};

struct Object {};

template <class T>
void test_atomic_integer() {
  const T max_value = std::numeric_limits<T>::max();
  const T min_value = std::numeric_limits<T>::min();

  cass::Atomic<T> i(0);

  BOOST_CHECK(i.load() == 0);

  BOOST_CHECK(i.exchange(1) == 0);
  BOOST_CHECK(i.load() == 1);

  i.store(0);
  T expected = 0;
  BOOST_CHECK(i.compare_exchange_strong(expected, 1));
  BOOST_CHECK(expected == 0);
  BOOST_CHECK(i.load() == 1);

  BOOST_CHECK(!i.compare_exchange_strong(expected, 1));
  BOOST_CHECK(expected == 1);
  BOOST_CHECK(i.load() == 1);

  i.store(0);
  expected = 0;
  BOOST_CHECK(i.compare_exchange_weak(expected, 1));
  BOOST_CHECK(expected == 0);
  BOOST_CHECK(i.load() == 1);

  BOOST_CHECK(!i.compare_exchange_weak(expected, 1));
  BOOST_CHECK(expected == 1);
  BOOST_CHECK(i.load() == 1);

  i.store(0);
  BOOST_CHECK(i.fetch_add(1) == 0);
  BOOST_CHECK(i.load() == 1);
  BOOST_CHECK(i.fetch_sub(1) == 1);
  BOOST_CHECK(i.load() == 0);

  i.store(max_value);
  BOOST_CHECK(i.load() == max_value);
  BOOST_CHECK(i.fetch_add(1) == max_value);
  BOOST_CHECK(i.load() == min_value);

  i.store(min_value);
  BOOST_CHECK(i.fetch_sub(1) == min_value);
  BOOST_CHECK(i.load() == max_value);
}

BOOST_AUTO_TEST_SUITE(atomic)

BOOST_AUTO_TEST_CASE(integers)
{
  test_atomic_integer<int32_t>();
  test_atomic_integer<int64_t>();
  test_atomic_integer<uint32_t>();
  test_atomic_integer<uint64_t>();
}

BOOST_AUTO_TEST_CASE(other_types)
{
  cass::Atomic<Enum> e;
  cass::Atomic<Object*> p;
  cass::Atomic<void*> v;
  cass::Atomic<bool> b;
}

BOOST_AUTO_TEST_SUITE_END()
