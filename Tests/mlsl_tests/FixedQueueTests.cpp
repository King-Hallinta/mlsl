// mlsl
// SPDX-FileCopyrightText: 2026 Korrikada
// SPDX-License-Identifier: Apache-2.0

#include <mlsl/Containers/FixedQueue.hpp>

#include <catch2/catch_test_macros.hpp>

#include <utility>

TEST_CASE("FixedQueue preserves FIFO order across wraparound")
{
	mlsl::FixedQueue<int, 3> queue;

	REQUIRE(queue.Add(1));
	REQUIRE(queue.Add(2));
	REQUIRE(queue.Add(3));
	REQUIRE(queue.Size() == 3);
	REQUIRE(queue.Front() == 1);
	REQUIRE(queue.Back() == 3);

	queue.Remove();

	REQUIRE(queue.Size() == 2);
	REQUIRE(queue.Front() == 2);

	auto wrapped = queue.Add(4);

	REQUIRE(wrapped);
	REQUIRE(queue.Size() == 3);
	REQUIRE(queue.Front() == 2);
	REQUIRE(queue.Back() == 4);

	queue.Remove();
	queue.Remove();

	REQUIRE(queue.Size() == 1);
	REQUIRE(queue.Front() == 4);
	REQUIRE(queue.Back() == 4);
}

TEST_CASE("FixedQueue reports out of memory when full")
{
	mlsl::FixedQueue<int, 2> queue;

	REQUIRE(queue.Add(7));
	REQUIRE(queue.Add(8));

	auto overflow = queue.Add(9);

	REQUIRE(not overflow);
	REQUIRE(overflow.error().type == mlsl::ErrorType::OutOfMemory);
	REQUIRE(queue.Size() == 2);
	REQUIRE(queue.Front() == 7);
	REQUIRE(queue.Back() == 8);
}

TEST_CASE("FixedQueue clear resets head for later reuse")
{
	mlsl::FixedQueue<int, 3> queue;

	REQUIRE(queue.Add(1));
	REQUIRE(queue.Add(2));
	REQUIRE(queue.Add(3));

	queue.Remove();
	queue.Clear();

	REQUIRE(queue.Empty());

	REQUIRE(queue.Add(7));
	REQUIRE(queue.Add(8));
	REQUIRE(queue.Size() == 2);
	REQUIRE(queue.Front() == 7);
	REQUIRE(queue.Back() == 8);
}

TEST_CASE("FixedQueue copy assignment preserves wrapped ordering")
{
	mlsl::FixedQueue<int, 4> left;
	mlsl::FixedQueue<int, 4> right;

	REQUIRE(right.Add(1));
	REQUIRE(right.Add(2));
	REQUIRE(right.Add(3));
	right.Remove();
	REQUIRE(right.Add(4));

	left = right;

	REQUIRE(left.Size() == 3);
	REQUIRE(left.Front() == 2);
	REQUIRE(left.Back() == 4);

	right.Remove();

	REQUIRE(left.Front() == 2);
	REQUIRE(right.Front() == 3);
}

TEST_CASE("FixedQueue move assignment transfers wrapped contents")
{
	mlsl::FixedQueue<int, 4> left;
	mlsl::FixedQueue<int, 4> right;

	REQUIRE(right.Add(1));
	REQUIRE(right.Add(2));
	REQUIRE(right.Add(3));
	right.Remove();
	REQUIRE(right.Add(4));

	left = std::move(right);

	REQUIRE(left.Size() == 3);
	REQUIRE(left.Front() == 2);
	REQUIRE(left.Back() == 4);
	REQUIRE(right.Empty());
}

TEST_CASE("FixedQueue move construction preserves FIFO order")
{
	mlsl::FixedQueue<int, 3> source;

	REQUIRE(source.Add(5));
	REQUIRE(source.Add(6));

	mlsl::FixedQueue<int, 3> moved(std::move(source));

	REQUIRE(moved.Size() == 2);
	REQUIRE(moved.Front() == 5);
	REQUIRE(moved.Back() == 6);
	REQUIRE(source.Empty());
}
