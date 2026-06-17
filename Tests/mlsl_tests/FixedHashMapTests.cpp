// mlsl
// SPDX-FileCopyrightText: 2026 Korrikada
// SPDX-License-Identifier: Apache-2.0

#include <mlsl/Containers/FixedHashMap.hpp>

#include <catch2/catch_test_macros.hpp>

#include <cstddef>
#include <utility>

namespace
{
	struct FixedMapConstantHash
	{
		std::size_t operator()(int) const
		{
			return 0;
		}
	};
} // namespace

TEST_CASE("FixedHashMap stores up to fixed capacity")
{
	mlsl::FixedHashMap<int, int, 3> map;

	REQUIRE(map.Capacity() == 3);
	REQUIRE(map.Add(1, 10));
	REQUIRE(map.Add(2, 20));
	REQUIRE(map.Add(3, 30));

	auto overflow = map.Add(4, 40);
	auto reserveOverflow = map.Reserve(4);

	REQUIRE(not overflow);
	REQUIRE(overflow.error().type == mlsl::ErrorType::BufferOverflow);
	REQUIRE(not reserveOverflow);
	REQUIRE(reserveOverflow.error().type == mlsl::ErrorType::BufferOverflow);
	REQUIRE(map.Size() == 3);
	REQUIRE(**map.Get(1) == 10);
	REQUIRE(not map.Contains(4));
}

TEST_CASE("FixedHashMap duplicate and missing lookup are explicit")
{
	mlsl::FixedHashMap<int, int, 2> map;

	REQUIRE(map.Add(5, 50));

	auto duplicate = map.Add(5, 60);
	auto missing = map.Get(6);

	REQUIRE(not duplicate);
	REQUIRE(duplicate.error().type == mlsl::ErrorType::AlreadyExists);
	REQUIRE(not missing);
	REQUIRE(missing.error().type == mlsl::ErrorType::NotFound);
	REQUIRE(**map.Get(5) == 50);
}

TEST_CASE("FixedHashMap handles collisions tombstones and clear")
{
	mlsl::FixedHashMap<int, int, 4, FixedMapConstantHash> map;

	REQUIRE(map.Add(1, 10));
	REQUIRE(map.Add(2, 20));
	REQUIRE(map.Add(3, 30));
	REQUIRE(map.Remove(2));
	REQUIRE(map.Add(4, 40));

	REQUIRE(map.Size() == 3);
	REQUIRE(**map.Get(1) == 10);
	REQUIRE(**map.Get(3) == 30);
	REQUIRE(**map.Get(4) == 40);

	map.Clear();

	REQUIRE(map.Empty());
	REQUIRE(map.Add(7, 70));
	REQUIRE(**map.Get(7) == 70);
}

TEST_CASE("FixedHashMap copy and move preserve contents")
{
	mlsl::FixedHashMap<int, int, 4> source;

	REQUIRE(source.Add(1, 10));
	REQUIRE(source.Add(2, 20));

	mlsl::FixedHashMap<int, int, 4> copy(source);
	mlsl::FixedHashMap<int, int, 4> moved(std::move(copy));

	REQUIRE(moved.Size() == 2);
	REQUIRE(copy.Empty());
	REQUIRE(**moved.Get(1) == 10);

	mlsl::FixedHashMap<int, int, 4> assigned;
	assigned = moved;

	REQUIRE(assigned.Size() == 2);
	REQUIRE(**assigned.Get(2) == 20);
}

TEST_CASE("FixedHashMap const get returns const value pointer")
{
	mlsl::FixedHashMap<int, int, 2> map;

	REQUIRE(map.Add(1, 10));

	const auto &constMap = map;
	auto value = constMap.Get(1);

	REQUIRE(value);
	REQUIRE(**value == 10);
}
