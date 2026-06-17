// mlsl
// SPDX-FileCopyrightText: 2026 Korrikada
// SPDX-License-Identifier: Apache-2.0

#include <mlsl/Memory/Span.hpp>

#include <catch2/catch_test_macros.hpp>

TEST_CASE("Span exposes pointer based access over explicit storage")
{
	int values[] {2, 4, 6, 8};
	mlsl::Span<int> span(values, 4);

	auto item = span.Get(2);
	auto missing = span.Get(4);

	REQUIRE(span.Size() == 4);
	REQUIRE(span.SizeBytes() == sizeof(values));
	REQUIRE(not span.Empty());
	REQUIRE(item);
	REQUIRE(**item == 6);
	REQUIRE(not missing);
	REQUIRE(missing.error().type == mlsl::ErrorType::OutOfBounds);
	REQUIRE(span.Front() == 2);
	REQUIRE(span.Back() == 8);
	REQUIRE(span.Data() == values);
	REQUIRE(span.Begin() == values);
	REQUIRE(span.End() == values + 4);
}

TEST_CASE("Span constructed from array exposes subviews into the same storage")
{
	int values[] {1, 2, 3, 4, 5};
	mlsl::Span<int> span(values);

	auto first = span.First(2);
	auto middle = span.Subspan(1, 3);
	auto last = span.Last(2);

	REQUIRE(first.Size() == 2);
	REQUIRE(middle.Size() == 3);
	REQUIRE(last.Size() == 2);

	first[1] = 20;
	middle[1] = 30;
	last[1] = 50;

	REQUIRE(values[1] == 20);
	REQUIRE(values[2] == 30);
	REQUIRE(values[4] == 50);
}

TEST_CASE("Const Span preserves read only element access")
{
	const int values[] {3, 6, 9};
	const mlsl::Span<const int> span(values, 3);

	auto item = span.Get(1);

	REQUIRE(item);
	REQUIRE(**item == 6);
	REQUIRE(span.Front() == 3);
	REQUIRE(span.Back() == 9);
	REQUIRE(span.Data() == values);
}

TEST_CASE("Default Span is empty and reports out of bounds")
{
	mlsl::Span<int> span;

	auto item = span.Get(0);

	REQUIRE(span.Empty());
	REQUIRE(span.Size() == 0);
	REQUIRE(span.SizeBytes() == 0);
	REQUIRE(span.Data() == nullptr);
	REQUIRE(not item);
	REQUIRE(item.error().type == mlsl::ErrorType::OutOfBounds);
}
