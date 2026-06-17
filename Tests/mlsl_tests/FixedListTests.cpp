// mlsl
// SPDX-FileCopyrightText: 2026 Korrikada
// SPDX-License-Identifier: Apache-2.0

#include <mlsl/Containers/FixedList.hpp>

#include <catch2/catch_test_macros.hpp>

TEST_CASE("FixedList supports front back get and iteration")
{
	mlsl::FixedList<int, 4> list;

	REQUIRE(list.AddBack(2));
	REQUIRE(list.AddFront(1));
	REQUIRE(list.AddBack(3));

	auto item = list.Get(1);
	auto missing = list.Get(3);
	int sum = 0;

	for (auto it = list.Begin(); it != list.End(); ++it)
	{
		sum += *it;
	}

	REQUIRE(list.Size() == 3);
	REQUIRE(not list.Empty());
	REQUIRE(list.Front() == 1);
	REQUIRE(list.Back() == 3);
	REQUIRE(item);
	REQUIRE(**item == 2);
	REQUIRE(not missing);
	REQUIRE(missing.error().type == mlsl::ErrorType::OutOfBounds);
	REQUIRE(sum == 6);
}

TEST_CASE("FixedList supports insert erase and capacity limits")
{
	mlsl::FixedList<int, 3> list;

	REQUIRE(list.AddBack(1));
	REQUIRE(list.AddBack(3));

	auto inserted = list.Insert(list.End(), 4);

	REQUIRE(inserted);
	REQUIRE(*(*inserted) == 4);
	REQUIRE(list.Back() == 4);

	auto overflow = list.Insert(list.Begin(), 0);

	REQUIRE(not overflow);
	REQUIRE(overflow.error().type == mlsl::ErrorType::OutOfMemory);

	auto eraseIt = list.Begin();
	++eraseIt;

	auto next = list.Erase(eraseIt);

	REQUIRE(list.Size() == 2);
	REQUIRE(*next == 4);
	REQUIRE(list.Front() == 1);
	REQUIRE(list.Back() == 4);

	list.RemoveFront();
	list.RemoveBack();

	REQUIRE(list.Empty());
}

TEST_CASE("FixedList clear resets storage for later reuse")
{
	mlsl::FixedList<int, 3> list;

	REQUIRE(list.AddBack(1));
	REQUIRE(list.AddBack(2));
	REQUIRE(list.AddBack(3));

	list.Clear();

	REQUIRE(list.Empty());

	REQUIRE(list.AddFront(7));
	REQUIRE(list.AddBack(8));
	REQUIRE(list.Size() == 2);
	REQUIRE(list.Front() == 7);
	REQUIRE(list.Back() == 8);
}

TEST_CASE("FixedList copy assignment preserves values independently")
{
	mlsl::FixedList<int, 4> left;
	mlsl::FixedList<int, 4> right;

	REQUIRE(right.AddBack(2));
	REQUIRE(right.AddBack(3));
	REQUIRE(right.AddBack(4));

	left = right;

	REQUIRE(left.Size() == 3);
	REQUIRE(left.Front() == 2);
	REQUIRE(left.Back() == 4);

	right.RemoveFront();

	REQUIRE(left.Front() == 2);
	REQUIRE(right.Front() == 3);
}

TEST_CASE("FixedList erase of middle element preserves neighbors")
{
	mlsl::FixedList<int, 4> list;

	REQUIRE(list.AddBack(1));
	REQUIRE(list.AddBack(2));
	REQUIRE(list.AddBack(3));
	REQUIRE(list.AddBack(4));

	auto iterator = list.Begin();
	++iterator;
	++iterator;

	auto next = list.Erase(iterator);

	REQUIRE(*next == 4);
	REQUIRE(list.Size() == 3);
	REQUIRE(list.Front() == 1);
	REQUIRE(list.Back() == 4);

	auto item = list.Get(1);

	REQUIRE(item);
	REQUIRE(**item == 2);
}

TEST_CASE("FixedList insert at begin reuses freed nodes")
{
	mlsl::FixedList<int, 3> list;

	REQUIRE(list.AddBack(2));
	REQUIRE(list.AddBack(3));
	list.RemoveFront();

	auto inserted = list.Insert(list.Begin(), 1);

	REQUIRE(inserted);
	REQUIRE(list.Size() == 2);
	REQUIRE(list.Front() == 1);
	REQUIRE(list.Back() == 3);
}
