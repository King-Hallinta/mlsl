// mlsl
// SPDX-FileCopyrightText: 2026 Korrikada
// SPDX-License-Identifier: Apache-2.0

#include <mlsl/Containers/RingBuffer.hpp>

#include <catch2/catch_test_macros.hpp>

#include <utility>

TEST_CASE("RingBuffer appends and removes in FIFO order")
{
	mlsl::RingBuffer<int> buffer;

	REQUIRE(buffer.Empty());

	REQUIRE(buffer.Add(1));
	REQUIRE(buffer.Add(2));
	REQUIRE(buffer.Add(3));

	REQUIRE(buffer.Size() == 3);
	REQUIRE(buffer.Front() == 1);
	REQUIRE(buffer.Back() == 3);
	REQUIRE(buffer[1] == 2);

	auto middle = buffer.Get(1);
	auto missing = buffer.Get(3);

	REQUIRE(middle);
	REQUIRE(**middle == 2);
	REQUIRE(not missing);
	REQUIRE(missing.error().type == mlsl::ErrorType::OutOfBounds);

	buffer.Remove();

	REQUIRE(buffer.Size() == 2);
	REQUIRE(buffer.Front() == 2);
}

TEST_CASE("RingBuffer grows automatically as elements are appended")
{
	mlsl::RingBuffer<int> buffer;

	REQUIRE(buffer.Capacity() == 0);

	for (int value = 1; value <= 20; ++value)
	{
		REQUIRE(buffer.Add(value));
	}

	REQUIRE(buffer.Size() == 20);
	REQUIRE(buffer.Capacity() >= 20);

	for (int index = 0; index < 20; ++index)
	{
		REQUIRE(buffer[static_cast<mlsl::RingBuffer<int>::SizeType>(index)] == index + 1);
	}
}

TEST_CASE("RingBuffer preserves FIFO order when a wrapped buffer grows")
{
	mlsl::RingBuffer<int> buffer;

	for (int value = 1; value <= 8; ++value)
	{
		REQUIRE(buffer.Add(value));
	}

	buffer.Remove();
	buffer.Remove();
	buffer.Remove();

	for (int value = 9; value <= 12; ++value)
	{
		REQUIRE(buffer.Add(value));
	}

	REQUIRE(buffer.Size() == 9);
	REQUIRE(buffer.Front() == 4);
	REQUIRE(buffer.Back() == 12);

	for (int index = 0; index < 9; ++index)
	{
		REQUIRE(buffer[static_cast<mlsl::RingBuffer<int>::SizeType>(index)] == index + 4);
	}
}

TEST_CASE("RingBuffer copy construction preserves contents independently")
{
	mlsl::RingBuffer<int> source;

	REQUIRE(source.Add(1));
	REQUIRE(source.Add(2));
	REQUIRE(source.Add(3));

	mlsl::RingBuffer<int> copy(source);

	REQUIRE(copy.Size() == 3);
	REQUIRE(copy.Front() == 1);
	REQUIRE(copy.Back() == 3);

	source.Remove();

	REQUIRE(copy.Front() == 1);
}

TEST_CASE("RingBuffer move construction transfers contents")
{
	mlsl::RingBuffer<int> source;

	REQUIRE(source.Add(1));
	REQUIRE(source.Add(2));

	mlsl::RingBuffer<int> moved(std::move(source));

	REQUIRE(moved.Size() == 2);
	REQUIRE(moved.Front() == 1);
	REQUIRE(moved.Back() == 2);
	REQUIRE(source.Empty());
}
