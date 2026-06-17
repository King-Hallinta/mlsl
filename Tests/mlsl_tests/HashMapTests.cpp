// mlsl
// SPDX-FileCopyrightText: 2026 Korrikada
// SPDX-License-Identifier: Apache-2.0

#include <mlsl/Containers/HashMap.hpp>

#include <catch2/catch_test_macros.hpp>

#include <cstddef>
#include <utility>

namespace
{
	struct ConstantHash
	{
		std::size_t operator()(int) const
		{
			return 1;
		}
	};

	struct LastDigitHash
	{
		std::size_t operator()(int value) const
		{
			return static_cast<std::size_t>(value % 10);
		}
	};

	struct LastDigitEqual
	{
		bool operator()(int left, int right) const
		{
			return left % 10 == right % 10;
		}
	};
} // namespace

TEST_CASE("HashMap adds finds removes and reports duplicates")
{
	mlsl::HashMap<int, int> map;

	REQUIRE(map.Empty());
	REQUIRE(map.Capacity() == 0);
	REQUIRE(map.Add(1, 10));
	REQUIRE(map.Add(2, 20));

	auto value = map.Get(1);
	auto duplicate = map.Add(1, 99);
	auto missing = map.Get(3);

	REQUIRE(map.Size() == 2);
	REQUIRE(map.Capacity() >= 2);
	REQUIRE(map.Contains(2));
	REQUIRE(value);
	REQUIRE(**value == 10);
	REQUIRE(not duplicate);
	REQUIRE(duplicate.error().type == mlsl::ErrorType::AlreadyExists);
	REQUIRE(not missing);
	REQUIRE(missing.error().type == mlsl::ErrorType::NotFound);
	REQUIRE(map.Remove(1));
	REQUIRE(not map.Contains(1));
	REQUIRE(not map.Remove(1));
}

TEST_CASE("HashMap reserve growth copy and move preserve values")
{
	mlsl::HashMap<int, int> map;

	REQUIRE(map.Reserve(16));
	REQUIRE(map.Capacity() >= 16);

	for (int i = 0; i < 12; ++i)
	{
		REQUIRE(map.Add(i, i * 10));
	}

	mlsl::HashMap<int, int> copy(map);
	mlsl::HashMap<int, int> moved(std::move(copy));

	REQUIRE(moved.Size() == 12);
	REQUIRE(copy.Empty());

	for (int i = 0; i < 12; ++i)
	{
		auto value = moved.Get(i);

		REQUIRE(value);
		REQUIRE(**value == i * 10);
	}

	mlsl::HashMap<int, int> assigned;
	assigned = moved;

	REQUIRE(assigned.Size() == 12);

	auto assignedValue = assigned.Get(7);

	REQUIRE(assignedValue);
	REQUIRE(**assignedValue == 70);

	mlsl::HashMap<int, int> moveAssigned;
	moveAssigned = std::move(assigned);

	auto movedValue = moveAssigned.Get(9);

	REQUIRE(moveAssigned.Size() == 12);
	REQUIRE(assigned.Empty());
	REQUIRE(movedValue);
	REQUIRE(**movedValue == 90);
}

TEST_CASE("HashMap handles collisions and reuses deleted slots")
{
	mlsl::HashMap<int, int, ConstantHash> map;

	REQUIRE(map.Add(1, 10));
	REQUIRE(map.Add(2, 20));
	REQUIRE(map.Add(3, 30));
	REQUIRE(map.Remove(2));
	REQUIRE(map.Add(4, 40));

	REQUIRE(map.Size() == 3);
	REQUIRE(**map.Get(1) == 10);
	REQUIRE(**map.Get(3) == 30);
	REQUIRE(**map.Get(4) == 40);
	REQUIRE(not map.Contains(2));
}

TEST_CASE("HashMap supports custom equality")
{
	mlsl::HashMap<int, int, LastDigitHash, LastDigitEqual> map(LastDigitHash {}, LastDigitEqual {});

	REQUIRE(map.Add(11, 110));
	REQUIRE(map.Contains(21));

	auto duplicate = map.Add(21, 210);
	auto value = map.Get(31);

	REQUIRE(not duplicate);
	REQUIRE(duplicate.error().type == mlsl::ErrorType::AlreadyExists);
	REQUIRE(value);
	REQUIRE(**value == 110);
}

TEST_CASE("HashMap clear resets contents")
{
	mlsl::HashMap<int, int> map;

	REQUIRE(map.Add(1, 10));
	REQUIRE(map.Add(2, 20));

	map.Clear();

	REQUIRE(map.Empty());
	REQUIRE(not map.Contains(1));
	REQUIRE(map.Add(3, 30));
	REQUIRE(**map.Get(3) == 30);
}

TEST_CASE("HashMap const get returns const value pointer")
{
	mlsl::HashMap<int, int> map;

	REQUIRE(map.Add(1, 10));

	const auto &constMap = map;
	auto value = constMap.Get(1);

	REQUIRE(value);
	REQUIRE(**value == 10);
}
