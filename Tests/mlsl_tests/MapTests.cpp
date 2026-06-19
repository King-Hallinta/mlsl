// mlsl
// SPDX-FileCopyrightText: 2026 Korrikada
// SPDX-License-Identifier: Apache-2.0

#include <mlsl/Containers/Map.hpp>

#include <catch2/catch_test_macros.hpp>

#include <functional>
#include <utility>

TEST_CASE("Map stores and retrieves values by key")
{
	mlsl::Map<int, int> map;

	REQUIRE(map.Empty());

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

TEST_CASE("Map grows beyond its initial capacity while keeping keys sorted")
{
	mlsl::Map<int, int> map;

	for (int key = 20; key >= 1; --key)
	{
		REQUIRE(map.Add(key, key * 10));
	}

	REQUIRE(map.Size() == 20);
	REQUIRE(map.Capacity() >= 20);

	for (int key = 1; key <= 20; ++key)
	{
		auto value = map.Get(key);

		REQUIRE(value);
		REQUIRE(**value == key * 10);
	}
}

TEST_CASE("Map rejects duplicate keys")
{
	mlsl::Map<int, int> map;

	REQUIRE(map.Add(1, 10));

	auto duplicate = map.Add(1, 99);

	REQUIRE_FALSE(duplicate);
	REQUIRE(duplicate.error().type == mlsl::ErrorType::AlreadyExists);

	auto value = map.Get(1);

	REQUIRE(value);
	REQUIRE(**value == 10);
}

TEST_CASE("Map removes entries and keeps remaining order")
{
	mlsl::Map<int, int> map;

	REQUIRE(map.Add(1, 10));
	REQUIRE(map.Add(2, 20));
	REQUIRE(map.Add(3, 30));

	REQUIRE(map.Remove(2));
	REQUIRE_FALSE(map.Remove(2));
	REQUIRE(map.Size() == 2);
	REQUIRE_FALSE(map.Contains(2));

	REQUIRE(map.Add(2, 22));

	auto value = map.Get(2);

	REQUIRE(value);
	REQUIRE(**value == 22);
}

TEST_CASE("Map copy assignment preserves entries independently")
{
	mlsl::Map<int, int> left;
	mlsl::Map<int, int> right;

	REQUIRE(right.Add(1, 10));
	REQUIRE(right.Add(2, 20));

	left = right;

	REQUIRE(left.Size() == 2);
	REQUIRE(right.Remove(1));

	REQUIRE(left.Contains(1));
	REQUIRE_FALSE(right.Contains(1));
}

TEST_CASE("Map move construction transfers ownership of storage")
{
	mlsl::Map<int, int> source;

	REQUIRE(source.Add(1, 10));
	REQUIRE(source.Add(2, 20));

	mlsl::Map<int, int> moved(std::move(source));

	REQUIRE(moved.Size() == 2);
	REQUIRE(moved.Contains(1));
	REQUIRE(moved.Contains(2));
	REQUIRE(source.Empty());
}

TEST_CASE("Map honours a custom comparator")
{
	mlsl::Map<int, int, std::greater<int>> map;

	REQUIRE(map.Add(1, 10));
	REQUIRE(map.Add(3, 30));
	REQUIRE(map.Add(2, 20));

	auto value = map.Get(3);

	REQUIRE(value);
	REQUIRE(**value == 30);
}
