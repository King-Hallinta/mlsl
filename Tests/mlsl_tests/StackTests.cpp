// mlsl
// SPDX-FileCopyrightText: 2026 Korrikada
// SPDX-License-Identifier: Apache-2.0

#include <mlsl/Containers/Stack.hpp>

#include <catch2/catch_test_macros.hpp>

#include <utility>

TEST_CASE("Stack grows and preserves LIFO order")
{
	mlsl::Stack<int> stack;

	REQUIRE(stack.Capacity() == 0);
	REQUIRE(stack.Add(1));
	REQUIRE(stack.Add(2));
	REQUIRE(stack.Add(3));

	REQUIRE(stack.Size() == 3);
	REQUIRE(stack.Capacity() >= 3);
	REQUIRE(stack.Top() == 3);

	stack.Remove();

	REQUIRE(stack.Top() == 2);
	REQUIRE(stack.Size() == 2);
}

TEST_CASE("Stack reserve copy and move preserve values")
{
	mlsl::Stack<int> stack;

	REQUIRE(stack.Reserve(6));
	REQUIRE(stack.Capacity() >= 6);
	REQUIRE(stack.Add(4));
	REQUIRE(stack.Add(5));
	REQUIRE(stack.Add(6));

	mlsl::Stack<int> copy(stack);

	REQUIRE(copy.Size() == 3);
	REQUIRE(copy.Top() == 6);

	mlsl::Stack<int> moved(std::move(copy));

	REQUIRE(moved.Size() == 3);
	REQUIRE(moved.Top() == 6);
	REQUIRE(copy.Empty());
}

TEST_CASE("Stack move assignment transfers allocated contents")
{
	mlsl::Stack<int> left;
	mlsl::Stack<int> right;

	REQUIRE(right.Add(1));
	REQUIRE(right.Add(2));
	REQUIRE(right.Add(3));

	left = std::move(right);

	REQUIRE(left.Size() == 3);
	REQUIRE(left.Top() == 3);
	REQUIRE(right.Empty());
}

TEST_CASE("Stack clear leaves instance reusable")
{
	mlsl::Stack<int> stack;

	REQUIRE(stack.Add(7));
	REQUIRE(stack.Add(8));
	stack.Clear();

	REQUIRE(stack.Empty());
	REQUIRE(stack.Add(9));
	REQUIRE(stack.Size() == 1);
	REQUIRE(stack.Top() == 9);
}

TEST_CASE("Stack reserve does not shrink and preserves values")
{
	mlsl::Stack<int> stack;

	REQUIRE(stack.Reserve(5));
	auto initialCapacity = stack.Capacity();

	REQUIRE(initialCapacity >= 5);
	REQUIRE(stack.Add(1));
	REQUIRE(stack.Add(2));
	REQUIRE(stack.Add(3));
	REQUIRE(stack.Reserve(2));

	REQUIRE(stack.Capacity() == initialCapacity);
	REQUIRE(stack.Size() == 3);
	REQUIRE(stack.Top() == 3);

	stack.Remove();

	REQUIRE(stack.Top() == 2);
}

TEST_CASE("Stack assignment replaces existing contents")
{
	mlsl::Stack<int> left;
	mlsl::Stack<int> right;

	REQUIRE(left.Add(9));
	REQUIRE(left.Add(8));
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
