// mlsl
// SPDX-FileCopyrightText: 2026 Korrikada
// SPDX-License-Identifier: Apache-2.0

#include <mlsl/Containers/Deque.hpp>

#include <catch2/catch_test_macros.hpp>

#include <utility>

TEST_CASE("Deque grows and preserves front back and index order")
{
	mlsl::Deque<int> deque;

	REQUIRE(deque.Capacity() == 0);
	REQUIRE(deque.Reserve(4));
	REQUIRE(deque.Capacity() >= 4);

	REQUIRE(deque.AddBack(2));
	REQUIRE(deque.AddFront(1));
	REQUIRE(deque.AddBack(3));
	REQUIRE(deque.AddBack(4));

	auto item = deque.Get(2);

	REQUIRE(deque.Size() == 4);
	REQUIRE(deque.Front() == 1);
	REQUIRE(deque.Back() == 4);
	REQUIRE(deque[1] == 2);
	REQUIRE(item);
	REQUIRE(**item == 3);
}

TEST_CASE("Deque supports wraparound copy and move")
{
	mlsl::Deque<int> deque;

	REQUIRE(deque.AddBack(5));
	REQUIRE(deque.AddBack(6));
	REQUIRE(deque.AddBack(7));

	deque.RemoveFront();
	REQUIRE(deque.AddBack(8));

	mlsl::Deque<int> copy(deque);

	REQUIRE(copy.Size() == 3);
	REQUIRE(copy.Front() == 6);
	REQUIRE(copy.Back() == 8);

	mlsl::Deque<int> moved(std::move(copy));

	REQUIRE(moved.Size() == 3);
	REQUIRE(moved.Front() == 6);
	REQUIRE(moved.Back() == 8);
	REQUIRE(copy.Empty());
}

TEST_CASE("Deque clear resets state for subsequent front and back insertion")
{
	mlsl::Deque<int> deque;

	REQUIRE(deque.AddBack(1));
	REQUIRE(deque.AddBack(2));
	REQUIRE(deque.AddFront(0));

	deque.RemoveFront();
	deque.Clear();

	REQUIRE(deque.Empty());

	REQUIRE(deque.AddFront(9));
	REQUIRE(deque.AddBack(10));
	REQUIRE(deque.Size() == 2);
	REQUIRE(deque.Front() == 9);
	REQUIRE(deque.Back() == 10);
}

TEST_CASE("Deque move assignment transfers contents")
{
	mlsl::Deque<int> left;
	mlsl::Deque<int> right;

	REQUIRE(right.AddBack(3));
	REQUIRE(right.AddBack(4));
	REQUIRE(right.AddBack(5));

	left = std::move(right);

	REQUIRE(left.Size() == 3);
	REQUIRE(left.Front() == 3);
	REQUIRE(left.Back() == 5);
	REQUIRE(right.Empty());
}

TEST_CASE("Deque reserve does not shrink or disturb existing contents")
{
	mlsl::Deque<int> deque;

	REQUIRE(deque.Reserve(6));
	auto initialCapacity = deque.Capacity();

	REQUIRE(initialCapacity >= 6);
	REQUIRE(deque.AddBack(1));
	REQUIRE(deque.AddBack(2));
	REQUIRE(deque.AddFront(0));
	REQUIRE(deque.Reserve(3));

	REQUIRE(deque.Capacity() == initialCapacity);
	REQUIRE(deque.Size() == 3);
	REQUIRE(deque.Front() == 0);
	REQUIRE(deque.Back() == 2);
	REQUIRE(deque[1] == 1);
}

TEST_CASE("Deque assignment replaces prior wrapped contents")
{
	mlsl::Deque<int> left;
	mlsl::Deque<int> right;

	REQUIRE(left.AddBack(10));
	REQUIRE(left.AddBack(11));
	REQUIRE(right.AddBack(1));
	REQUIRE(right.AddBack(2));
	REQUIRE(right.AddBack(3));

	right.RemoveFront();
	REQUIRE(right.AddBack(4));

	left = right;

	REQUIRE(left.Size() == 3);
	REQUIRE(left.Front() == 2);
	REQUIRE(left.Back() == 4);
	REQUIRE(left[1] == 3);
}

TEST_CASE("Deque wraparound preserves indexed order after mixed operations")
{
	mlsl::Deque<int> deque;

	REQUIRE(deque.AddBack(1));
	REQUIRE(deque.AddBack(2));
	REQUIRE(deque.AddBack(3));
	deque.RemoveFront();
	REQUIRE(deque.AddFront(0));
	REQUIRE(deque.AddBack(4));

	REQUIRE(deque.Size() == 4);
	REQUIRE(deque.Front() == 0);
	REQUIRE(deque.Back() == 4);
	REQUIRE(deque[0] == 0);
	REQUIRE(deque[1] == 2);
	REQUIRE(deque[2] == 3);
	REQUIRE(deque[3] == 4);
}

TEST_CASE("Deque remove back leaves remaining prefix intact")
{
	mlsl::Deque<int> deque;

	REQUIRE(deque.AddBack(7));
	REQUIRE(deque.AddBack(8));
	REQUIRE(deque.AddBack(9));

	deque.RemoveBack();

	REQUIRE(deque.Size() == 2);
	REQUIRE(deque.Front() == 7);
	REQUIRE(deque.Back() == 8);
}
