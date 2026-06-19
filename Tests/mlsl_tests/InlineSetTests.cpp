// mlsl
// SPDX-FileCopyrightText: 2026 Korrikada
// SPDX-License-Identifier: Apache-2.0

#include <mlsl/Containers/InlineSet.hpp>

#include <catch2/catch_test_macros.hpp>

#include <functional>
#include <utility>

TEST_CASE("InlineSet stays inline below its inline capacity")
{
	mlsl::InlineSet<int, 4> set;

	REQUIRE(set.Empty());
	REQUIRE(set.InlineCapacity() == 4);

	REQUIRE(set.Add(2));
	REQUIRE(set.Add(1));
	REQUIRE(set.Add(3));

	REQUIRE(set.Size() == 3);
	REQUIRE(set.Capacity() == 4);
	REQUIRE(set.Contains(1));
}

TEST_CASE("InlineSet spills to the heap past its inline capacity while keeping keys sorted")
{
	mlsl::InlineSet<int, 2> set;

	for (int key = 10; key >= 1; --key)
	{
		REQUIRE(set.Add(key));
	}

	REQUIRE(set.Size() == 10);
	REQUIRE(set.Capacity() > set.InlineCapacity());

	for (int key = 1; key <= 10; ++key)
	{
		REQUIRE(set.Contains(key));
	}
}

TEST_CASE("InlineSet rejects duplicate keys")
{
	mlsl::InlineSet<int, 4> set;

	REQUIRE(set.Add(1));

	auto duplicate = set.Add(1);

	REQUIRE_FALSE(duplicate);
	REQUIRE(duplicate.error().type == mlsl::ErrorType::AlreadyExists);
}

TEST_CASE("InlineSet removes keys and keeps remaining order")
{
	mlsl::InlineSet<int, 4> set;

	REQUIRE(set.Add(3));
	REQUIRE(set.Add(1));
	REQUIRE(set.Add(2));

	REQUIRE(set.Remove(2));
	REQUIRE_FALSE(set.Remove(2));
	REQUIRE(set.Size() == 2);
	REQUIRE(set.Contains(1));
	REQUIRE(set.Contains(3));
	REQUIRE_FALSE(set.Contains(2));
}

TEST_CASE("InlineSet copy assignment preserves keys independently")
{
	mlsl::InlineSet<int, 2> left;
	mlsl::InlineSet<int, 2> right;

	for (int key = 1; key <= 5; ++key)
	{
		REQUIRE(right.Add(key));
	}

	left = right;

	REQUIRE(left.Size() == 5);
	REQUIRE(right.Remove(1));

	REQUIRE(left.Contains(1));
	REQUIRE_FALSE(right.Contains(1));
}

TEST_CASE("InlineSet move construction transfers keys")
{
	mlsl::InlineSet<int, 2> source;

	for (int key = 1; key <= 5; ++key)
	{
		REQUIRE(source.Add(key));
	}

	mlsl::InlineSet<int, 2> moved(std::move(source));

	REQUIRE(moved.Size() == 5);

	for (int key = 1; key <= 5; ++key)
	{
		REQUIRE(moved.Contains(key));
	}

	REQUIRE(source.Empty());
}

TEST_CASE("InlineSet honours a custom comparator")
{
	mlsl::InlineSet<int, 4, std::greater<int>> set;

	REQUIRE(set.Add(1));
	REQUIRE(set.Add(3));
	REQUIRE(set.Add(2));

	REQUIRE(set.Contains(1));
	REQUIRE(set.Contains(2));
	REQUIRE(set.Contains(3));
}
