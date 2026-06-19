// mlsl
// SPDX-FileCopyrightText: 2026 Korrikada
// SPDX-License-Identifier: Apache-2.0

#include <mlsl/Containers/InlineMap.hpp>

#include <catch2/catch_test_macros.hpp>

#include <functional>
#include <utility>

TEST_CASE("InlineMap stays inline below its inline capacity")
{
	mlsl::InlineMap<int, int, 4> map;

	REQUIRE(map.Empty());
	REQUIRE(map.InlineCapacity() == 4);

	REQUIRE(map.Add(2, 20));
	REQUIRE(map.Add(1, 10));
	REQUIRE(map.Add(3, 30));

	REQUIRE(map.Size() == 3);
	REQUIRE(map.Capacity() == 4);

	auto value = map.Get(2);

	REQUIRE(value);
	REQUIRE(**value == 20);
}

TEST_CASE("InlineMap spills to the heap past its inline capacity while keeping keys sorted")
{
	mlsl::InlineMap<int, int, 2> map;

	for (int key = 10; key >= 1; --key)
	{
		REQUIRE(map.Add(key, key * 100));
	}

	REQUIRE(map.Size() == 10);
	REQUIRE(map.Capacity() > map.InlineCapacity());

	for (int key = 1; key <= 10; ++key)
	{
		auto value = map.Get(key);

		REQUIRE(value);
		REQUIRE(**value == key * 100);
	}
}

TEST_CASE("InlineMap rejects duplicate keys")
{
	mlsl::InlineMap<int, int, 4> map;

	REQUIRE(map.Add(1, 10));

	auto duplicate = map.Add(1, 99);

	REQUIRE_FALSE(duplicate);
	REQUIRE(duplicate.error().type == mlsl::ErrorType::AlreadyExists);
}

TEST_CASE("InlineMap removes entries and keeps remaining order")
{
	mlsl::InlineMap<int, int, 4> map;

	REQUIRE(map.Add(1, 10));
	REQUIRE(map.Add(2, 20));
	REQUIRE(map.Add(3, 30));

	REQUIRE(map.Remove(2));
	REQUIRE_FALSE(map.Remove(2));
	REQUIRE(map.Size() == 2);
	REQUIRE(map.Contains(1));
	REQUIRE(map.Contains(3));
	REQUIRE_FALSE(map.Contains(2));
}

TEST_CASE("InlineMap copy assignment preserves entries independently")
{
	mlsl::InlineMap<int, int, 2> left;
	mlsl::InlineMap<int, int, 2> right;

	for (int key = 1; key <= 5; ++key)
	{
		REQUIRE(right.Add(key, key * 10));
	}

	left = right;

	REQUIRE(left.Size() == 5);
	REQUIRE(right.Remove(1));

	REQUIRE(left.Contains(1));
	REQUIRE_FALSE(right.Contains(1));
}

TEST_CASE("InlineMap move construction transfers entries")
{
	mlsl::InlineMap<int, int, 2> source;

	for (int key = 1; key <= 5; ++key)
	{
		REQUIRE(source.Add(key, key * 10));
	}

	mlsl::InlineMap<int, int, 2> moved(std::move(source));

	REQUIRE(moved.Size() == 5);

	for (int key = 1; key <= 5; ++key)
	{
		REQUIRE(moved.Contains(key));
	}

	REQUIRE(source.Empty());
}

TEST_CASE("InlineMap honours a custom comparator")
{
	mlsl::InlineMap<int, int, 4, std::greater<int>> map;

	REQUIRE(map.Add(1, 10));
	REQUIRE(map.Add(3, 30));
	REQUIRE(map.Add(2, 20));

	auto value = map.Get(3);

	REQUIRE(value);
	REQUIRE(**value == 30);
}
