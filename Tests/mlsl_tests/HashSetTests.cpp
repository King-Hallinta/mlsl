// mlsl
// SPDX-FileCopyrightText: 2026 Korrikada
// SPDX-License-Identifier: Apache-2.0

#include <mlsl/Containers/HashSet.hpp>

#include <catch2/catch_test_macros.hpp>

#include <cstddef>
#include <utility>

namespace
{
	struct SetConstantHash
	{
		std::size_t operator()(int) const
		{
			return 1;
		}
	};

	struct SetLastDigitHash
	{
		std::size_t operator()(int value) const
		{
			return static_cast<std::size_t>(value % 10);
		}
	};

	struct SetLastDigitEqual
	{
		bool operator()(int left, int right) const
		{
			return left % 10 == right % 10;
		}
	};
} // namespace

TEST_CASE("HashSet adds finds removes and reports duplicates")
{
	mlsl::HashSet<int> set;

	REQUIRE(set.Empty());
	REQUIRE(set.Add(1));
	REQUIRE(set.Add(2));

	auto value = set.Get(1);
	const auto &constSet = set;
	auto constValue = constSet.Get(2);
	auto duplicate = set.Add(1);
	auto missing = set.Get(3);

	REQUIRE(set.Size() == 2);
	REQUIRE(set.Contains(2));
	REQUIRE(value);
	REQUIRE(**value == 1);
	REQUIRE(constValue);
	REQUIRE(**constValue == 2);
	REQUIRE(not duplicate);
	REQUIRE(duplicate.error().type == mlsl::ErrorType::AlreadyExists);
	REQUIRE(not missing);
	REQUIRE(missing.error().type == mlsl::ErrorType::NotFound);
	REQUIRE(set.Remove(1));
	REQUIRE(not set.Remove(1));
}

TEST_CASE("HashSet reserve growth copy and move preserve keys")
{
	mlsl::HashSet<int> set;

	REQUIRE(set.Reserve(12));

	for (int i = 0; i < 10; ++i)
	{
		REQUIRE(set.Add(i));
	}

	mlsl::HashSet<int> copy(set);
	mlsl::HashSet<int> moved(std::move(copy));

	REQUIRE(copy.Empty());
	REQUIRE(moved.Size() == 10);
	REQUIRE(moved.Contains(7));

	mlsl::HashSet<int> assigned;
	assigned = std::move(moved);

	REQUIRE(assigned.Size() == 10);
	REQUIRE(assigned.Contains(9));
	REQUIRE(moved.Empty());
}

TEST_CASE("HashSet handles collisions and tombstone reuse")
{
	mlsl::HashSet<int, SetConstantHash> set;

	REQUIRE(set.Add(1));
	REQUIRE(set.Add(2));
	REQUIRE(set.Add(3));
	REQUIRE(set.Remove(2));
	REQUIRE(set.Add(4));

	REQUIRE(set.Contains(1));
	REQUIRE(set.Contains(3));
	REQUIRE(set.Contains(4));
	REQUIRE(not set.Contains(2));
}

TEST_CASE("HashSet supports custom equality")
{
	mlsl::HashSet<int, SetLastDigitHash, SetLastDigitEqual> set(SetLastDigitHash {}, SetLastDigitEqual {});

	REQUIRE(set.Add(11));
	REQUIRE(set.Contains(21));

	auto duplicate = set.Add(31);

	REQUIRE(not duplicate);
	REQUIRE(duplicate.error().type == mlsl::ErrorType::AlreadyExists);
}
