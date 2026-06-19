// mlsl
// SPDX-FileCopyrightText: 2026 Korrikada
// SPDX-License-Identifier: Apache-2.0

#include <mlsl/Containers/Deque.hpp>
#include <mlsl/Containers/FixedDeque.hpp>
#include <mlsl/Containers/FixedQueue.hpp>
#include <mlsl/Containers/FixedRingBuffer.hpp>
#include <mlsl/Containers/FixedStack.hpp>
#include <mlsl/Containers/Stack.hpp>

#include <catch2/catch_test_macros.hpp>

TEST_CASE("Stack iteration visits elements from bottom to top")
{
	mlsl::FixedStack<int, 4> stack;

	REQUIRE(stack.Begin() == stack.End());

	REQUIRE(stack.Add(1));
	REQUIRE(stack.Add(2));
	REQUIRE(stack.Add(3));

	int expected = 1;

	for (auto it = stack.Begin(); it != stack.End(); ++it)
	{
		REQUIRE(*it == expected);
		++expected;
	}

	REQUIRE(expected == 4);
}

TEST_CASE("Stack iterators allow mutation through the iterator")
{
	mlsl::Stack<int> stack;

	REQUIRE(stack.Add(1));
	REQUIRE(stack.Add(2));
	REQUIRE(stack.Add(3));

	for (auto it = stack.Begin(); it != stack.End(); ++it)
	{
		*it *= 10;
	}

	const mlsl::Stack<int> &constStack = stack;
	int sum = 0;

	for (auto it = constStack.Begin(); it != constStack.End(); ++it)
	{
		sum += *it;
	}

	REQUIRE(sum == 60);
}

TEST_CASE("Deque iteration visits elements from front to back")
{
	mlsl::FixedDeque<int, 4> deque;

	REQUIRE(deque.Begin() == deque.End());

	REQUIRE(deque.Append(1));
	REQUIRE(deque.Append(2));
	REQUIRE(deque.Insert(0));

	int expected = 0;

	for (auto it = deque.Begin(); it != deque.End(); ++it)
	{
		REQUIRE(*it == expected);
		++expected;
	}

	REQUIRE(expected == 3);
}

TEST_CASE("Deque iteration follows logical order across a wrapped buffer")
{
	mlsl::FixedDeque<int, 4> deque;

	REQUIRE(deque.Append(1));
	REQUIRE(deque.Append(2));
	REQUIRE(deque.Append(3));
	REQUIRE(deque.Append(4));

	deque.RemoveFront();
	deque.RemoveFront();

	REQUIRE(deque.Append(5));
	REQUIRE(deque.Append(6));

	int values[4] = {0, 0, 0, 0};
	int index = 0;

	for (auto it = deque.Begin(); it != deque.End(); ++it)
	{
		values[index] = *it;
		++index;
	}

	REQUIRE(index == 4);
	REQUIRE(values[0] == 3);
	REQUIRE(values[1] == 4);
	REQUIRE(values[2] == 5);
	REQUIRE(values[3] == 6);
}

TEST_CASE("Deque iterator supports decrement and post-increment")
{
	mlsl::FixedDeque<int, 4> deque;

	REQUIRE(deque.Append(10));
	REQUIRE(deque.Append(20));
	REQUIRE(deque.Append(30));

	auto it = deque.Begin();
	auto copy = it++;

	REQUIRE(*copy == 10);
	REQUIRE(*it == 20);

	--it;

	REQUIRE(*it == 10);
}

TEST_CASE("Queue iteration visits elements from front to back")
{
	mlsl::FixedQueue<int, 4> queue;

	REQUIRE(queue.Add(1));
	REQUIRE(queue.Add(2));
	REQUIRE(queue.Add(3));

	int expected = 1;

	for (auto it = queue.Begin(); it != queue.End(); ++it)
	{
		REQUIRE(*it == expected);
		++expected;
	}

	REQUIRE(expected == 4);
}

TEST_CASE("RingBuffer iteration follows FIFO order across freed slots")
{
	mlsl::FixedRingBuffer<int, 4> buffer;

	REQUIRE(buffer.Add(1));
	REQUIRE(buffer.Add(2));
	REQUIRE(buffer.Add(3));
	REQUIRE(buffer.Add(4));

	buffer.Remove();
	buffer.Remove();

	REQUIRE(buffer.Add(5));
	REQUIRE(buffer.Add(6));

	int expected = 3;

	for (auto it = buffer.Begin(); it != buffer.End(); ++it)
	{
		REQUIRE(*it == expected);
		++expected;
	}

	REQUIRE(expected == 7);
}
