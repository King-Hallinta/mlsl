// mlsl
// SPDX-FileCopyrightText: 2026 Korrikada
// SPDX-License-Identifier: Apache-2.0

#include <mlsl/Containers/List.hpp>

#include <catch2/catch_test_macros.hpp>

#include <utility>

TEST_CASE("List supports add insert erase and indexed access")
{
	mlsl::List<int> list;

	REQUIRE(list.AddBack(2));
	REQUIRE(list.AddFront(1));
	REQUIRE(list.AddBack(4));

	auto inserted = list.Insert(list.End(), 5);

	REQUIRE(inserted);
	REQUIRE(list.Size() == 4);
	REQUIRE(list.Front() == 1);
	REQUIRE(list.Back() == 5);

	auto item = list.Get(2);

	REQUIRE(item);
	REQUIRE(**item == 4);

	auto iterator = list.Begin();
	++iterator;

	auto next = list.Erase(iterator);

	REQUIRE(*next == 4);
	REQUIRE(list.Size() == 3);
	REQUIRE(list.Front() == 1);
	REQUIRE(list.Back() == 5);
}

TEST_CASE("List copy and move preserve element order")
{
	mlsl::List<int> list;

	REQUIRE(list.AddBack(7));
	REQUIRE(list.AddBack(8));
	REQUIRE(list.AddBack(9));

	mlsl::List<int> copy(list);

	REQUIRE(copy.Size() == 3);
	REQUIRE(copy.Front() == 7);
	REQUIRE(copy.Back() == 9);

	mlsl::List<int> moved(std::move(copy));

	REQUIRE(moved.Size() == 3);
	REQUIRE(moved.Front() == 7);
	REQUIRE(moved.Back() == 9);
	REQUIRE(copy.Empty());
}

TEST_CASE("List clear resets state for subsequent insertions")
{
	mlsl::List<int> list;

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

TEST_CASE("List insert at begin and erase tail preserve links")
{
	mlsl::List<int> list;

	REQUIRE(list.AddBack(2));
	REQUIRE(list.AddBack(3));

	auto inserted = list.Insert(list.Begin(), 1);

	REQUIRE(inserted);
	REQUIRE(list.Front() == 1);
	REQUIRE(list.Back() == 3);

	auto tail = list.Begin();
	++tail;
	++tail;

	auto next = list.Erase(tail);

	REQUIRE(next == list.End());
	REQUIRE(list.Size() == 2);
	REQUIRE(list.Front() == 1);
	REQUIRE(list.Back() == 2);
}

TEST_CASE("List erase of first element advances to new head")
{
	mlsl::List<int> list;

	REQUIRE(list.AddBack(1));
	REQUIRE(list.AddBack(2));
	REQUIRE(list.AddBack(3));

	auto next = list.Erase(list.Begin());

	REQUIRE(*next == 2);
	REQUIRE(list.Size() == 2);
	REQUIRE(list.Front() == 2);
	REQUIRE(list.Back() == 3);
}

TEST_CASE("List erase end iterator leaves container unchanged")
{
	mlsl::List<int> list;

	REQUIRE(list.AddBack(4));
	REQUIRE(list.AddBack(5));

	auto next = list.Erase(list.End());

	REQUIRE(next == list.End());
	REQUIRE(list.Size() == 2);
	REQUIRE(list.Front() == 4);
	REQUIRE(list.Back() == 5);
}
