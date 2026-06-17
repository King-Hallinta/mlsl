// mlsl
// SPDX-FileCopyrightText: 2026 Korrikada
// SPDX-License-Identifier: Apache-2.0

#include <mlsl/Containers/Queue.hpp>

#include <catch2/catch_test_macros.hpp>

#include <utility>

TEST_CASE("Queue grows and preserves FIFO order")
{
	mlsl::Queue<int> queue;

	REQUIRE(queue.Capacity() == 0);
	REQUIRE(queue.Add(1));
	REQUIRE(queue.Add(2));
	REQUIRE(queue.Add(3));

	REQUIRE(queue.Size() == 3);
	REQUIRE(queue.Capacity() >= 3);
	REQUIRE(queue.Front() == 1);
	REQUIRE(queue.Back() == 3);

	queue.Remove();

	REQUIRE(queue.Front() == 2);
	REQUIRE(queue.Add(4));
	REQUIRE(queue.Back() == 4);
}

TEST_CASE("Queue reserve copy and move preserve queued values")
{
	mlsl::Queue<int> queue;

	REQUIRE(queue.Reserve(5));
	REQUIRE(queue.Capacity() >= 5);
	REQUIRE(queue.Add(7));
	REQUIRE(queue.Add(8));
	REQUIRE(queue.Add(9));

	mlsl::Queue<int> copy(queue);

	REQUIRE(copy.Size() == 3);
	REQUIRE(copy.Front() == 7);
	REQUIRE(copy.Back() == 9);

	mlsl::Queue<int> moved(std::move(copy));

	REQUIRE(moved.Size() == 3);
	REQUIRE(moved.Front() == 7);
	REQUIRE(moved.Back() == 9);
	REQUIRE(copy.Empty());
}

TEST_CASE("Queue move assignment transfers contents")
{
	mlsl::Queue<int> left;
	mlsl::Queue<int> right;

	REQUIRE(right.Add(3));
	REQUIRE(right.Add(4));
	REQUIRE(right.Add(5));

	left = std::move(right);

	REQUIRE(left.Size() == 3);
	REQUIRE(left.Front() == 3);
	REQUIRE(left.Back() == 5);
	REQUIRE(right.Empty());
}

TEST_CASE("Queue clear after wraparound allows fresh reuse")
{
	mlsl::Queue<int> queue;

	REQUIRE(queue.Add(1));
	REQUIRE(queue.Add(2));
	REQUIRE(queue.Add(3));

	queue.Remove();
	REQUIRE(queue.Add(4));
	queue.Clear();

	REQUIRE(queue.Empty());
	REQUIRE(queue.Add(7));
	REQUIRE(queue.Add(8));
	REQUIRE(queue.Front() == 7);
	REQUIRE(queue.Back() == 8);
}

TEST_CASE("Queue clear resets head and allows reuse from empty state")
{
	mlsl::Queue<int> queue;

	REQUIRE(queue.Add(1));
	REQUIRE(queue.Add(2));
	REQUIRE(queue.Add(3));

	queue.Remove();
	queue.Remove();
	queue.Clear();

	REQUIRE(queue.Empty());

	REQUIRE(queue.Add(4));
	REQUIRE(queue.Add(5));
	REQUIRE(queue.Size() == 2);
	REQUIRE(queue.Front() == 4);
	REQUIRE(queue.Back() == 5);
}

TEST_CASE("Queue assignment replaces existing contents")
{
	mlsl::Queue<int> left;
	mlsl::Queue<int> right;

	REQUIRE(left.Add(1));
	REQUIRE(left.Add(2));
	REQUIRE(right.Add(7));
	REQUIRE(right.Add(8));
	REQUIRE(right.Add(9));

	left = right;

	REQUIRE(left.Size() == 3);
	REQUIRE(left.Front() == 7);
	REQUIRE(left.Back() == 9);

	right.Remove();

	REQUIRE(left.Front() == 7);
	REQUIRE(right.Front() == 8);
}
