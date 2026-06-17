// mlsl
// SPDX-FileCopyrightText: 2026 Korrikada
// SPDX-License-Identifier: Apache-2.0

#include <mlsl/Containers/InlineHashMap.hpp>

#include <catch2/catch_test_macros.hpp>

#include <cstddef>
#include <utility>

namespace
{
	struct InlineMapConstantHash
	{
		std::size_t operator()(int) const
		{
			return 2;
		}
	};
} // namespace

TEST_CASE("InlineHashMap uses inline capacity before growth")
{
	mlsl::InlineHashMap<int, int, 2> map;

	REQUIRE(map.InlineCapacity() == 2);
	REQUIRE(map.Capacity() == 2);
	REQUIRE(map.Add(1, 10));
	REQUIRE(map.Add(2, 20));
	REQUIRE(map.Capacity() == map.InlineCapacity());
}

TEST_CASE("InlineHashMap grows and preserves values")
{
	mlsl::InlineHashMap<int, int, 2> map;

	REQUIRE(map.Add(1, 10));
	REQUIRE(map.Add(2, 20));
	REQUIRE(map.Add(3, 30));

	REQUIRE(map.Capacity() > map.InlineCapacity());
	REQUIRE(map.Size() == 3);
	REQUIRE(**map.Get(1) == 10);
	REQUIRE(**map.Get(2) == 20);
	REQUIRE(**map.Get(3) == 30);
}

TEST_CASE("InlineHashMap handles collisions and tombstone reuse")
{
	mlsl::InlineHashMap<int, int, 2, InlineMapConstantHash> map;

	REQUIRE(map.Add(1, 10));
	REQUIRE(map.Add(2, 20));
	REQUIRE(map.Remove(1));
	REQUIRE(map.Add(3, 30));

	REQUIRE(not map.Contains(1));
	REQUIRE(**map.Get(2) == 20);
	REQUIRE(**map.Get(3) == 30);
}

TEST_CASE("InlineHashMap copy move assignment and clear work after growth")
{
	mlsl::InlineHashMap<int, int, 1> source;

	REQUIRE(source.Add(1, 10));
	REQUIRE(source.Add(2, 20));
	REQUIRE(source.Add(3, 30));

	mlsl::InlineHashMap<int, int, 1> copy(source);
	mlsl::InlineHashMap<int, int, 1> moved(std::move(copy));

	REQUIRE(moved.Size() == 3);
	REQUIRE(copy.Empty());

	mlsl::InlineHashMap<int, int, 1> assigned;
	assigned = moved;

	REQUIRE(assigned.Size() == 3);
	REQUIRE(**assigned.Get(3) == 30);

	assigned.Clear();

	REQUIRE(assigned.Empty());
	REQUIRE(assigned.Add(4, 40));
	REQUIRE(**assigned.Get(4) == 40);
}
