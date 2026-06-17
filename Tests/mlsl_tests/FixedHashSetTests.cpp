// mlsl
// SPDX-FileCopyrightText: 2026 Korrikada
// SPDX-License-Identifier: Apache-2.0

#include <mlsl/Containers/FixedHashSet.hpp>

#include <catch2/catch_test_macros.hpp>

#include <cstddef>
#include <utility>

namespace
{
	struct FixedSetConstantHash
	{
		std::size_t operator()(int) const
		{
			return 0;
		}
	};
} // namespace

TEST_CASE("FixedHashSet stores up to fixed capacity")
{
	mlsl::FixedHashSet<int, 2> set;

	REQUIRE(set.Capacity() == 2);
	REQUIRE(set.Add(1));
	REQUIRE(set.Add(2));

	auto overflow = set.Add(3);
	auto reserveOverflow = set.Reserve(3);

	REQUIRE(not overflow);
	REQUIRE(overflow.error().type == mlsl::ErrorType::BufferOverflow);
	REQUIRE(not reserveOverflow);
	REQUIRE(reserveOverflow.error().type == mlsl::ErrorType::BufferOverflow);
	REQUIRE(set.Size() == 2);
	REQUIRE(set.Contains(1));
	REQUIRE(set.Contains(2));
}

TEST_CASE("FixedHashSet duplicate missing and remove behavior")
{
	mlsl::FixedHashSet<int, 3> set;

	REQUIRE(set.Add(5));

	auto duplicate = set.Add(5);
	auto missing = set.Get(6);

	REQUIRE(not duplicate);
	REQUIRE(duplicate.error().type == mlsl::ErrorType::AlreadyExists);
	REQUIRE(not missing);
	REQUIRE(missing.error().type == mlsl::ErrorType::NotFound);
	REQUIRE(set.Remove(5));
	REQUIRE(not set.Remove(5));
}

TEST_CASE("FixedHashSet handles collisions tombstones copy and move")
{
	mlsl::FixedHashSet<int, 4, FixedSetConstantHash> set;

	REQUIRE(set.Add(1));
	REQUIRE(set.Add(2));
	REQUIRE(set.Add(3));
	REQUIRE(set.Remove(2));
	REQUIRE(set.Add(4));

	mlsl::FixedHashSet<int, 4, FixedSetConstantHash> copy(set);
	mlsl::FixedHashSet<int, 4, FixedSetConstantHash> moved(std::move(copy));

	REQUIRE(moved.Contains(1));
	REQUIRE(moved.Contains(3));
	REQUIRE(moved.Contains(4));
	REQUIRE(copy.Empty());

	moved.Clear();

	REQUIRE(moved.Empty());
	REQUIRE(moved.Add(7));
	REQUIRE(moved.Contains(7));
}
