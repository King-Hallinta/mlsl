// mlsl
// SPDX-FileCopyrightText: 2026 Korrikada
// SPDX-License-Identifier: Apache-2.0

#include <mlsl/Containers/Set.hpp>

#include <catch2/catch_test_macros.hpp>

#include <functional>
#include <utility>

TEST_CASE("Set stores unique keys")
{
	mlsl::Set<int> set;

	REQUIRE(set.Empty());

	REQUIRE(set.Add(2));
	REQUIRE(set.Add(1));
	REQUIRE(set.Add(3));

	REQUIRE(set.Size() == 3);
	REQUIRE(set.Contains(1));
	REQUIRE_FALSE(set.Contains(9));
}

TEST_CASE("Set grows beyond its initial capacity while keeping keys sorted")
{
	mlsl::Set<int> set;

	for (int key = 20; key >= 1; --key)
	{
		REQUIRE(set.Add(key));
	}

	REQUIRE(set.Size() == 20);
	REQUIRE(set.Capacity() >= 20);

	for (int key = 1; key <= 20; ++key)
	{
		REQUIRE(set.Contains(key));
	}
}

TEST_CASE("Set rejects duplicate keys")
{
	mlsl::Set<int> set;

	REQUIRE(set.Add(1));

	auto duplicate = set.Add(1);

	REQUIRE_FALSE(duplicate);
	REQUIRE(duplicate.error().type == mlsl::ErrorType::AlreadyExists);
	REQUIRE(set.Size() == 1);
}

TEST_CASE("Set removes keys and keeps remaining order")
{
	mlsl::Set<int> set;

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

TEST_CASE("Set copy assignment preserves keys independently")
{
	mlsl::Set<int> left;
	mlsl::Set<int> right;

	REQUIRE(right.Add(1));
	REQUIRE(right.Add(2));

	left = right;

	REQUIRE(left.Size() == 2);
	REQUIRE(right.Remove(1));

	REQUIRE(left.Contains(1));
	REQUIRE_FALSE(right.Contains(1));
}

TEST_CASE("Set move construction transfers ownership of storage")
{
	mlsl::Set<int> source;

	REQUIRE(source.Add(1));
	REQUIRE(source.Add(2));

	mlsl::Set<int> moved(std::move(source));

	REQUIRE(moved.Size() == 2);
	REQUIRE(moved.Contains(1));
	REQUIRE(moved.Contains(2));
	REQUIRE(source.Empty());
}

TEST_CASE("Set honours a custom comparator")
{
	mlsl::Set<int, std::greater<int>> set;

	REQUIRE(set.Add(1));
	REQUIRE(set.Add(3));
	REQUIRE(set.Add(2));

	REQUIRE(set.Contains(1));
	REQUIRE(set.Contains(2));
	REQUIRE(set.Contains(3));
}
