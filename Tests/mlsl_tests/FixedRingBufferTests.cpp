// mlsl
// SPDX-FileCopyrightText: 2026 Korrikada
// SPDX-License-Identifier: Apache-2.0

#include <mlsl/Containers/FixedRingBuffer.hpp>

#include <catch2/catch_test_macros.hpp>

#include <utility>

TEST_CASE("FixedRingBuffer appends and removes in FIFO order")
{
	mlsl::FixedRingBuffer<int, 4> buffer;

	REQUIRE(buffer.Empty());
	REQUIRE(buffer.Capacity() == 4);

	REQUIRE(buffer.Add(1));
	REQUIRE(buffer.Add(2));
	REQUIRE(buffer.Add(3));

	REQUIRE(buffer.Size() == 3);
	REQUIRE(buffer.Front() == 1);
	REQUIRE(buffer.Back() == 3);
	REQUIRE(buffer[1] == 2);
}

TEST_CASE("FixedRingBuffer reports buffer overflow when full")
{
	mlsl::FixedRingBuffer<int, 2> buffer;

	REQUIRE(buffer.Add(1));
	REQUIRE(buffer.Add(2));

	auto overflow = buffer.Add(3);

	REQUIRE(not overflow);
	REQUIRE(overflow.error().type == mlsl::ErrorType::BufferOverflow);
	REQUIRE(buffer.Size() == 2);
	REQUIRE(buffer.Front() == 1);
	REQUIRE(buffer.Back() == 2);
}

TEST_CASE("FixedRingBuffer wraps around freed slots in FIFO order")
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

	REQUIRE(buffer.Size() == 4);
	REQUIRE(buffer.Front() == 3);
	REQUIRE(buffer.Back() == 6);
	REQUIRE(buffer[0] == 3);
	REQUIRE(buffer[1] == 4);
	REQUIRE(buffer[2] == 5);
	REQUIRE(buffer[3] == 6);

	auto overflow = buffer.Add(7);

	REQUIRE(not overflow);
	REQUIRE(overflow.error().type == mlsl::ErrorType::BufferOverflow);
}

TEST_CASE("FixedRingBuffer copy assignment preserves wrapped ordering")
{
	mlsl::FixedRingBuffer<int, 4> left;
	mlsl::FixedRingBuffer<int, 4> right;

	REQUIRE(right.Add(1));
	REQUIRE(right.Add(2));
	REQUIRE(right.Add(3));
	right.Remove();
	REQUIRE(right.Add(4));

	left = right;

	REQUIRE(left.Size() == 3);
	REQUIRE(left.Front() == 2);
	REQUIRE(left.Back() == 4);
	REQUIRE(left[1] == 3);
}

TEST_CASE("FixedRingBuffer move construction preserves order")
{
	mlsl::FixedRingBuffer<int, 3> source;

	REQUIRE(source.Add(4));
	REQUIRE(source.Add(5));
	REQUIRE(source.Add(6));

	mlsl::FixedRingBuffer<int, 3> moved(std::move(source));

	REQUIRE(moved.Size() == 3);
	REQUIRE(moved.Front() == 4);
	REQUIRE(moved.Back() == 6);
	REQUIRE(source.Empty());
}
