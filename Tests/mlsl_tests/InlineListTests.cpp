// mlsl
// SPDX-FileCopyrightText: 2026 Korrikada
// SPDX-License-Identifier: Apache-2.0

#include <mlsl/Containers/InlineList.hpp>

#include <catch2/catch_test_macros.hpp>

#include <utility>

TEST_CASE("InlineList supports inline storage operations")
{
	mlsl::InlineList<int, 2> list;

	REQUIRE(list.InlineCapacity() == 2);
	REQUIRE(list.Append(1));
	REQUIRE(list.Append(3));

	auto inserted = list.Insert(list.Begin(), 0);

	REQUIRE(inserted);
	REQUIRE(list.Size() == 3);
	REQUIRE(list.Front() == 0);
	REQUIRE(list.Back() == 3);

	auto item = list.Get(1);

	REQUIRE(item);
	REQUIRE(**item == 1);
}

TEST_CASE("InlineList supports erase clear and growth beyond inline capacity")
{
	mlsl::InlineList<int, 1> list;

	REQUIRE(list.Append(4));
	REQUIRE(list.Append(5));
	REQUIRE(list.Insert(3));

	REQUIRE(list.Size() == 3);
	REQUIRE(list.Front() == 3);
	REQUIRE(list.Back() == 5);

	auto iterator = list.Begin();
	++iterator;

	auto next = list.Erase(iterator);

	REQUIRE(list.Size() == 2);
	REQUIRE(*next == 5);

	list.Clear();

	REQUIRE(list.Empty());
}

TEST_CASE("InlineList move assignment transfers overflow contents")
{
	mlsl::InlineList<int, 1> left;
	mlsl::InlineList<int, 1> right;

	REQUIRE(right.Append(1));
	REQUIRE(right.Append(2));
	REQUIRE(right.Append(3));

	left = std::move(right);

	REQUIRE(left.Size() == 3);
	REQUIRE(left.Front() == 1);
	REQUIRE(left.Back() == 3);
	REQUIRE(right.Empty());
}

TEST_CASE("InlineList assignment replaces existing contents after clear")
{
	mlsl::InlineList<int, 2> left;
	mlsl::InlineList<int, 2> right;

	REQUIRE(left.Append(9));
	REQUIRE(left.Append(8));
	left.Clear();

	REQUIRE(right.Append(4));
	REQUIRE(right.Append(5));
	REQUIRE(right.Append(6));

	left = right;

	REQUIRE(left.Size() == 3);
	REQUIRE(left.Front() == 4);
	REQUIRE(left.Back() == 6);
}

TEST_CASE("InlineList copy assignment preserves order beyond inline capacity")
{
	mlsl::InlineList<int, 1> left;
	mlsl::InlineList<int, 1> right;

	REQUIRE(left.Append(9));
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

TEST_CASE("InlineList remove operations preserve remaining elements")
{
	mlsl::InlineList<int, 2> list;

	REQUIRE(list.Append(4));
	REQUIRE(list.Append(5));
	REQUIRE(list.Append(6));

	list.RemoveFront();

	REQUIRE(list.Front() == 5);
	REQUIRE(list.Back() == 6);

	list.RemoveBack();

	REQUIRE(list.Size() == 1);
	REQUIRE(list.Front() == 5);
	REQUIRE(list.Back() == 5);
}
