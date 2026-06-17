// mlsl
// SPDX-FileCopyrightText: 2026 Korrikada
// SPDX-License-Identifier: Apache-2.0

#include <mlsl/Containers/FixedStack.hpp>

#include <catch2/catch_test_macros.hpp>

#include <utility>

TEST_CASE("FixedStack stores values in LIFO order")
{
	mlsl::FixedStack<int, 3> stack;

	REQUIRE(stack.Empty());
	REQUIRE(stack.Capacity() == 3);

	auto first = stack.Add(1);
	auto second = stack.Add(2);
	auto third = stack.Add(3);

	REQUIRE(first);
	REQUIRE(second);
	REQUIRE(third);
	REQUIRE(stack.Size() == 3);
	REQUIRE(stack.Top() == 3);

	stack.Remove();

	REQUIRE(stack.Size() == 2);
	REQUIRE(stack.Top() == 2);

	stack.Clear();

	REQUIRE(stack.Empty());
}

TEST_CASE("FixedStack reports out of memory when capacity is exceeded")
{
	mlsl::FixedStack<int, 2> stack;

	REQUIRE(stack.Add(4));
	REQUIRE(stack.Add(5));

	auto overflow = stack.Add(6);

	REQUIRE(not overflow);
	REQUIRE(overflow.error().type == mlsl::ErrorType::OutOfMemory);
	REQUIRE(stack.Size() == 2);
	REQUIRE(stack.Top() == 5);
}

TEST_CASE("FixedStack copy assignment preserves values independently")
{
	mlsl::FixedStack<int, 4> left;
	mlsl::FixedStack<int, 4> right;

	REQUIRE(left.Add(9));
	REQUIRE(right.Add(1));
	REQUIRE(right.Add(2));
	REQUIRE(right.Add(3));

	left = right;

	REQUIRE(left.Size() == 3);
	REQUIRE(left.Top() == 3);

	right.Remove();

	REQUIRE(left.Top() == 3);
	REQUIRE(right.Top() == 2);
}

TEST_CASE("FixedStack move construction transfers stored values")
{
	mlsl::FixedStack<int, 3> source;

	REQUIRE(source.Add(4));
	REQUIRE(source.Add(5));

	mlsl::FixedStack<int, 3> moved(std::move(source));

	REQUIRE(moved.Size() == 2);
	REQUIRE(moved.Top() == 5);
	REQUIRE(source.Empty());
}
