// mlsl
// SPDX-FileCopyrightText: 2026 Korrikada
// SPDX-License-Identifier: Apache-2.0

#include <mlsl/Containers/FixedMap.hpp>

#include <catch2/catch_test_macros.hpp>

#include <functional>
#include <utility>

TEST_CASE("FixedMap stores and retrieves values by key")
{
	mlsl::FixedMap<int, int, 4> map;

	REQUIRE(map.Empty());
	REQUIRE(map.Capacity() == 4);

	REQUIRE(map.Add(2, 20));
	REQUIRE(map.Add(1, 10));
	REQUIRE(map.Add(3, 30));

	REQUIRE(map.Size() == 3);
	REQUIRE(map.Contains(1));
	REQUIRE_FALSE(map.Contains(9));

	auto value = map.Get(2);

	REQUIRE(value);
	REQUIRE(**value == 20);

	auto missing = map.Get(9);

	REQUIRE_FALSE(missing);
	REQUIRE(missing.error().type == mlsl::ErrorType::NotFound);
}

TEST_CASE("FixedMap keeps keys sorted regardless of insertion order")
{
	mlsl::FixedMap<int, int, 5> map;

	REQUIRE(map.Add(5, 0));
	REQUIRE(map.Add(1, 0));
	REQUIRE(map.Add(4, 0));
	REQUIRE(map.Add(2, 0));
	REQUIRE(map.Add(3, 0));

	int previous = 0;

	for (int key = 1; key <= 5; ++key)
	{
		REQUIRE(map.Contains(key));
		REQUIRE(key > previous);
		previous = key;
	}
}

TEST_CASE("FixedMap rejects duplicate keys")
{
	mlsl::FixedMap<int, int, 4> map;

	REQUIRE(map.Add(1, 10));

	auto duplicate = map.Add(1, 99);

	REQUIRE_FALSE(duplicate);
	REQUIRE(duplicate.error().type == mlsl::ErrorType::AlreadyExists);

	auto value = map.Get(1);

	REQUIRE(value);
	REQUIRE(**value == 10);
}

TEST_CASE("FixedMap reports out of memory when capacity is exceeded")
{
	mlsl::FixedMap<int, int, 2> map;

	REQUIRE(map.Add(1, 10));
	REQUIRE(map.Add(2, 20));
	REQUIRE(map.Full());

	auto overflow = map.Add(3, 30);

	REQUIRE_FALSE(overflow);
	REQUIRE(overflow.error().type == mlsl::ErrorType::OutOfMemory);
	REQUIRE(map.Size() == 2);
}

TEST_CASE("FixedMap removes entries and keeps remaining order")
{
	mlsl::FixedMap<int, int, 4> map;

	REQUIRE(map.Add(1, 10));
	REQUIRE(map.Add(2, 20));
	REQUIRE(map.Add(3, 30));

	REQUIRE(map.Remove(2));
	REQUIRE_FALSE(map.Remove(2));
	REQUIRE(map.Size() == 2);
	REQUIRE_FALSE(map.Contains(2));
	REQUIRE(map.Contains(1));
	REQUIRE(map.Contains(3));

	REQUIRE(map.Add(2, 22));

	auto value = map.Get(2);

	REQUIRE(value);
	REQUIRE(**value == 22);
}

TEST_CASE("FixedMap copy assignment preserves entries independently")
{
	mlsl::FixedMap<int, int, 4> left;
	mlsl::FixedMap<int, int, 4> right;

	REQUIRE(right.Add(1, 10));
	REQUIRE(right.Add(2, 20));

	left = right;

	REQUIRE(left.Size() == 2);
	REQUIRE(right.Remove(1));

	REQUIRE(left.Contains(1));
	REQUIRE_FALSE(right.Contains(1));
}

TEST_CASE("FixedMap move construction transfers entries")
{
	mlsl::FixedMap<int, int, 4> source;

	REQUIRE(source.Add(1, 10));
	REQUIRE(source.Add(2, 20));

	mlsl::FixedMap<int, int, 4> moved(std::move(source));

	REQUIRE(moved.Size() == 2);
	REQUIRE(moved.Contains(1));
	REQUIRE(moved.Contains(2));
	REQUIRE(source.Empty());
}

TEST_CASE("FixedMap honours a custom comparator")
{
	mlsl::FixedMap<int, int, 4, std::greater<int>> map;

	REQUIRE(map.Add(1, 10));
	REQUIRE(map.Add(3, 30));
	REQUIRE(map.Add(2, 20));

	REQUIRE(map.Contains(1));
	REQUIRE(map.Contains(2));
	REQUIRE(map.Contains(3));

	auto value = map.Get(3);

	REQUIRE(value);
	REQUIRE(**value == 30);
}
