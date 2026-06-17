// mlsl
// SPDX-FileCopyrightText: 2026 Korrikada
// SPDX-License-Identifier: Apache-2.0

#include <mlsl/Containers/InlineDeque.hpp>

#include <catch2/catch_test_macros.hpp>

#include <utility>

TEST_CASE("InlineDeque grows beyond inline capacity and preserves order")
{
	mlsl::InlineDeque<int, 2> deque;

	REQUIRE(deque.Capacity() == 2);
	REQUIRE(deque.InlineCapacity() == 2);

	REQUIRE(deque.Append(1));
	REQUIRE(deque.Append(2));
	REQUIRE(deque.Append(3));

	REQUIRE(deque.Size() == 3);
	REQUIRE(deque.Capacity() >= 3);
	REQUIRE(deque.Front() == 1);
	REQUIRE(deque.Back() == 3);
	REQUIRE(deque[0] == 1);
	REQUIRE(deque[1] == 2);
	REQUIRE(deque[2] == 3);
}

TEST_CASE("InlineDeque supports wraparound after front removal")
{
	mlsl::InlineDeque<int, 2> deque;

	REQUIRE(deque.Append(5));
	REQUIRE(deque.Append(6));

	deque.RemoveFront();

	REQUIRE(deque.Append(7));
	REQUIRE(deque.Size() == 2);
	REQUIRE(deque.Front() == 6);
	REQUIRE(deque.Back() == 7);

	auto first = deque.Get(0);
	auto second = deque.Get(1);

	REQUIRE(first);
	REQUIRE(second);
	REQUIRE(**first == 6);
	REQUIRE(**second == 7);
}

TEST_CASE("InlineDeque move assignment transfers grown contents")
{
	mlsl::InlineDeque<int, 1> left;
	mlsl::InlineDeque<int, 1> right;

	REQUIRE(right.Append(1));
	REQUIRE(right.Append(2));
	REQUIRE(right.Append(3));

	left = std::move(right);

	REQUIRE(left.Size() == 3);
	REQUIRE(left.Front() == 1);
	REQUIRE(left.Back() == 3);
	REQUIRE(right.Empty());
}

TEST_CASE("InlineDeque assignment replaces existing wrapped contents")
{
	mlsl::InlineDeque<int, 2> left;
	mlsl::InlineDeque<int, 2> right;

	REQUIRE(left.Append(9));
	REQUIRE(left.Append(8));
	REQUIRE(right.Append(2));
	REQUIRE(right.Append(3));
	right.RemoveFront();
	REQUIRE(right.Append(4));

	left = right;

	REQUIRE(left.Size() == 2);
	REQUIRE(left.Front() == 3);
	REQUIRE(left.Back() == 4);
}

TEST_CASE("InlineDeque copy assignment preserves grown contents independently")
{
	mlsl::InlineDeque<int, 1> left;
	mlsl::InlineDeque<int, 1> right;

	REQUIRE(right.Append(1));
	REQUIRE(right.Append(2));
	REQUIRE(right.Append(3));

	left = right;

	REQUIRE(left.Size() == 3);
	REQUIRE(left.Front() == 1);
	REQUIRE(left.Back() == 3);

	right.RemoveFront();

	REQUIRE(left.Front() == 1);
	REQUIRE(right.Front() == 2);
}

TEST_CASE("InlineDeque move construction transfers wrapped contents")
{
	mlsl::InlineDeque<int, 2> source;

	REQUIRE(source.Append(4));
	REQUIRE(source.Append(5));
	source.RemoveFront();
	REQUIRE(source.Append(6));

	mlsl::InlineDeque<int, 2> moved(std::move(source));

	REQUIRE(moved.Size() == 2);
	REQUIRE(moved.Front() == 5);
	REQUIRE(moved.Back() == 6);
	REQUIRE(source.Empty());
}

TEST_CASE("InlineDeque growth preserves wrapped values")
{
	mlsl::InlineDeque<int, 2> deque;

	REQUIRE(deque.Append(1));
	REQUIRE(deque.Append(2));
	deque.RemoveFront();
	REQUIRE(deque.Append(3));
	REQUIRE(deque.Append(4));

	REQUIRE(deque.Capacity() > deque.InlineCapacity());
	REQUIRE(deque.Size() == 3);
	REQUIRE(deque.Front() == 2);
	REQUIRE(deque.Back() == 4);
	REQUIRE(deque[0] == 2);
	REQUIRE(deque[1] == 3);
	REQUIRE(deque[2] == 4);
}

TEST_CASE("InlineDeque clear resets head for subsequent front insertions")
{
	mlsl::InlineDeque<int, 3> deque;

	REQUIRE(deque.Append(4));
	REQUIRE(deque.Append(5));
	deque.RemoveFront();
	deque.Clear();

	REQUIRE(deque.Empty());
	REQUIRE(deque.Insert(9));
	REQUIRE(deque.Append(10));
	REQUIRE(deque.Size() == 2);
	REQUIRE(deque.Front() == 9);
	REQUIRE(deque.Back() == 10);
}
