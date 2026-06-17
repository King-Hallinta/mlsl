// mlsl
// SPDX-FileCopyrightText: 2026 Korrikada
// SPDX-License-Identifier: Apache-2.0

#include <mlsl/Containers/InlineHashSet.hpp>

#include <catch2/catch_test_macros.hpp>

#include <cstddef>
#include <utility>

namespace
{
	struct InlineSetConstantHash
	{
		std::size_t operator()(int) const
		{
			return 2;
		}
	};
} // namespace

TEST_CASE("InlineHashSet uses inline capacity before growth")
{
	mlsl::InlineHashSet<int, 2> set;

	REQUIRE(set.InlineCapacity() == 2);
	REQUIRE(set.Capacity() == 2);
	REQUIRE(set.Add(1));
	REQUIRE(set.Add(2));
	REQUIRE(set.Capacity() == set.InlineCapacity());
}

TEST_CASE("InlineHashSet grows and preserves keys")
{
	mlsl::InlineHashSet<int, 2> set;

	REQUIRE(set.Add(1));
	REQUIRE(set.Add(2));
	REQUIRE(set.Add(3));

	REQUIRE(set.Capacity() > set.InlineCapacity());
	REQUIRE(set.Contains(1));
	REQUIRE(set.Contains(2));
	REQUIRE(set.Contains(3));
}

TEST_CASE("InlineHashSet handles collisions and tombstone reuse")
{
	mlsl::InlineHashSet<int, 2, InlineSetConstantHash> set;

	REQUIRE(set.Add(1));
	REQUIRE(set.Add(2));
	REQUIRE(set.Remove(1));
	REQUIRE(set.Add(3));

	REQUIRE(not set.Contains(1));
	REQUIRE(set.Contains(2));
	REQUIRE(set.Contains(3));
}

TEST_CASE("InlineHashSet copy move assignment and clear work after growth")
{
	mlsl::InlineHashSet<int, 1> source;

	REQUIRE(source.Add(1));
	REQUIRE(source.Add(2));
	REQUIRE(source.Add(3));

	mlsl::InlineHashSet<int, 1> copy(source);
	mlsl::InlineHashSet<int, 1> moved(std::move(copy));

	REQUIRE(copy.Empty());
	REQUIRE(moved.Size() == 3);

	mlsl::InlineHashSet<int, 1> assigned;
	assigned = moved;

	REQUIRE(assigned.Contains(3));

	assigned.Clear();

	REQUIRE(assigned.Empty());
	REQUIRE(assigned.Add(4));
	REQUIRE(assigned.Contains(4));
}
