// mlsl
// SPDX-FileCopyrightText: 2026 Korrikada
// SPDX-License-Identifier: Apache-2.0

#include <mlsl/Containers/FixedSet.hpp>

#include <catch2/catch_test_macros.hpp>

#include <functional>
#include <utility>

TEST_CASE("FixedSet stores unique keys")
{
	mlsl::FixedSet<int, 4> set;

	REQUIRE(set.Empty());
	REQUIRE(set.Capacity() == 4);

	REQUIRE(set.Add(2));
	REQUIRE(set.Add(1));
	REQUIRE(set.Add(3));

	REQUIRE(set.Size() == 3);
	REQUIRE(set.Contains(1));
	REQUIRE_FALSE(set.Contains(9));
}

TEST_CASE("FixedSet rejects duplicate keys")
{
	mlsl::FixedSet<int, 4> set;

	REQUIRE(set.Add(1));

	auto duplicate = set.Add(1);

	REQUIRE_FALSE(duplicate);
	REQUIRE(duplicate.error().type == mlsl::ErrorType::AlreadyExists);
	REQUIRE(set.Size() == 1);
}

TEST_CASE("FixedSet reports out of memory when capacity is exceeded")
{
	mlsl::FixedSet<int, 2> set;

	REQUIRE(set.Add(1));
	REQUIRE(set.Add(2));
	REQUIRE(set.Full());

	auto overflow = set.Add(3);

	REQUIRE_FALSE(overflow);
	REQUIRE(overflow.error().type == mlsl::ErrorType::OutOfMemory);
	REQUIRE(set.Size() == 2);
}

TEST_CASE("FixedSet removes keys and keeps remaining order")
{
	mlsl::FixedSet<int, 4> set;

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

TEST_CASE("FixedSet copy assignment preserves keys independently")
{
	mlsl::FixedSet<int, 4> left;
	mlsl::FixedSet<int, 4> right;

	REQUIRE(right.Add(1));
	REQUIRE(right.Add(2));

	left = right;

	REQUIRE(left.Size() == 2);
	REQUIRE(right.Remove(1));

	REQUIRE(left.Contains(1));
	REQUIRE_FALSE(right.Contains(1));
}

TEST_CASE("FixedSet move construction transfers keys")
{
	mlsl::FixedSet<int, 4> source;

	REQUIRE(source.Add(1));
	REQUIRE(source.Add(2));

	mlsl::FixedSet<int, 4> moved(std::move(source));

	REQUIRE(moved.Size() == 2);
	REQUIRE(moved.Contains(1));
	REQUIRE(moved.Contains(2));
	REQUIRE(source.Empty());
}

TEST_CASE("FixedSet honours a custom comparator")
{
	mlsl::FixedSet<int, 4, std::greater<int>> set;

	REQUIRE(set.Add(1));
	REQUIRE(set.Add(3));
	REQUIRE(set.Add(2));

	REQUIRE(set.Contains(1));
	REQUIRE(set.Contains(2));
	REQUIRE(set.Contains(3));
}
