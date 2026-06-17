// mlsl
// SPDX-FileCopyrightText: 2026 Korrikada
// SPDX-License-Identifier: Apache-2.0

#include <mlsl/Containers/FixedDeque.hpp>

#include <catch2/catch_test_macros.hpp>

#include <utility>

TEST_CASE("FixedDeque supports front back and indexed access")
{
	mlsl::FixedDeque<int, 4> deque;

	REQUIRE(deque.Append(2));
	REQUIRE(deque.Insert(1));
	REQUIRE(deque.Append(3));

	auto middle = deque.Get(1);
	auto missing = deque.Get(3);

	REQUIRE(deque.Size() == 3);
	REQUIRE(not deque.Empty());
	REQUIRE(deque.Front() == 1);
	REQUIRE(deque.Back() == 3);
	REQUIRE(deque[1] == 2);
	REQUIRE(middle);
	REQUIRE(**middle == 2);
	REQUIRE(not missing);
	REQUIRE(missing.error().type == mlsl::ErrorType::OutOfBounds);
}

TEST_CASE("FixedDeque reports out of memory when full")
{
	mlsl::FixedDeque<int, 2> deque;

	REQUIRE(deque.Append(4));
	REQUIRE(deque.Insert(3));

	auto overflow = deque.Append(5);

	REQUIRE(not overflow);
	REQUIRE(overflow.error().type == mlsl::ErrorType::OutOfMemory);
	REQUIRE(deque.Size() == 2);
	REQUIRE(deque.Front() == 3);
	REQUIRE(deque.Back() == 4);

	deque.RemoveFront();
	deque.RemoveBack();

	REQUIRE(deque.Empty());
}

TEST_CASE("FixedDeque clear resets state for subsequent front and back insertion")
{
	mlsl::FixedDeque<int, 3> deque;

	REQUIRE(deque.Append(1));
	REQUIRE(deque.Append(2));
	REQUIRE(deque.Insert(0));

	deque.RemoveFront();
	deque.Clear();

	REQUIRE(deque.Empty());

	REQUIRE(deque.Insert(9));
	REQUIRE(deque.Append(10));
	REQUIRE(deque.Size() == 2);
	REQUIRE(deque.Front() == 9);
	REQUIRE(deque.Back() == 10);
}

TEST_CASE("FixedDeque copy assignment preserves wrapped ordering")
{
	mlsl::FixedDeque<int, 4> left;
	mlsl::FixedDeque<int, 4> right;

	REQUIRE(right.Append(2));
	REQUIRE(right.Append(3));
	REQUIRE(right.Insert(1));
	right.RemoveFront();
	REQUIRE(right.Append(4));

	left = right;

	REQUIRE(left.Size() == 3);
	REQUIRE(left.Front() == 2);
	REQUIRE(left.Back() == 4);
	REQUIRE(left[1] == 3);
}

TEST_CASE("FixedDeque move assignment transfers wrapped contents")
{
	mlsl::FixedDeque<int, 4> left;
	mlsl::FixedDeque<int, 4> right;

	REQUIRE(right.Append(2));
	REQUIRE(right.Append(3));
	REQUIRE(right.Insert(1));
	right.RemoveFront();
	REQUIRE(right.Append(4));

	left = std::move(right);

	REQUIRE(left.Size() == 3);
	REQUIRE(left.Front() == 2);
	REQUIRE(left.Back() == 4);
	REQUIRE(left[1] == 3);
	REQUIRE(right.Empty());
}

TEST_CASE("FixedDeque move construction preserves order")
{
	mlsl::FixedDeque<int, 3> source;

	REQUIRE(source.Append(5));
	REQUIRE(source.Append(6));
	REQUIRE(source.Insert(4));

	mlsl::FixedDeque<int, 3> moved(std::move(source));

	REQUIRE(moved.Size() == 3);
	REQUIRE(moved.Front() == 4);
	REQUIRE(moved.Back() == 6);
	REQUIRE(source.Empty());
}
