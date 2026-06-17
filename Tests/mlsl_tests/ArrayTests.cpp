// mlsl
// SPDX-FileCopyrightText: 2026 Korrikada
// SPDX-License-Identifier: Apache-2.0

#include <mlsl/Containers/Array.hpp>

#include <catch2/catch_test_macros.hpp>

TEST_CASE("Array stores initializer list values")
{
	mlsl::Array<int, 3> array {1, 2, 3};

	REQUIRE(array.Size() == 3);
	REQUIRE(not array.Empty());
	REQUIRE(array[0] == 1);
	REQUIRE(array[1] == 2);
	REQUIRE(array[2] == 3);
}

TEST_CASE("Array truncates initializer lists that exceed capacity")
{
	mlsl::Array<int, 3> array {1, 2, 3, 4};

	REQUIRE(array[0] == 1);
	REQUIRE(array[1] == 2);
	REQUIRE(array[2] == 3);
}

TEST_CASE("Array Get reports out of bounds access")
{
	mlsl::Array<int, 3> array {1, 2, 3};

	auto value = array.Get(1);
	auto outOfBounds = array.Get(3);

	REQUIRE(value);
	REQUIRE(**value == 2);
	REQUIRE(not outOfBounds);
	REQUIRE(outOfBounds.error().type == mlsl::ErrorType::OutOfBounds);
}

TEST_CASE("Array exposes front back and contiguous data")
{
	mlsl::Array<int, 3> array {1, 2, 3};

	REQUIRE(array.Front() == 1);
	REQUIRE(array.Back() == 3);
	REQUIRE(array.Data() == array.Begin());
	REQUIRE(array.End() == array.Begin() + array.Size());
}

TEST_CASE("Array Fill assigns all elements")
{
	mlsl::Array<int, 4> array {1, 2, 3, 4};

	array.Fill(7);

	for (auto it = array.Begin(); it != array.End(); ++it)
	{
		REQUIRE(*it == 7);
	}
}

TEST_CASE("Const Array exposes read only access")
{
	const mlsl::Array<int, 3> array {1, 2, 3};

	auto value = array.Get(2);

	REQUIRE(value);
	REQUIRE(**value == 3);
	REQUIRE(array.Front() == 1);
	REQUIRE(array.Back() == 3);
	REQUIRE(array.Data() == array.Begin());
	REQUIRE(array.End() == array.Begin() + array.Size());
}

TEST_CASE("Array default initialization and pointer access stay stable")
{
	mlsl::Array<int, 4> array;

	REQUIRE(array.Size() == 4);
	REQUIRE(not array.Empty());
	REQUIRE(array.Data() == array.Begin());
	REQUIRE(array.End() == array.Begin() + array.Size());

	for (std::size_t i = 0; i < array.Size(); ++i)
	{
		array[i] = static_cast<int>(i + 1);
	}

	for (std::size_t i = 0; i < array.Size(); ++i)
	{
		auto item = array.Get(i);

		REQUIRE(item);
		REQUIRE(**item == static_cast<int>(i + 1));
	}
}

TEST_CASE("Array fill overwrites truncated initializer state")
{
	mlsl::Array<int, 5> array {9, 8};

	array.Fill(3);

	for (std::size_t i = 0; i < array.Size(); ++i)
	{
		REQUIRE(array[i] == 3);
	}
}
