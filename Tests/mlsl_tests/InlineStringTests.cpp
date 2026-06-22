// mlsl
// SPDX-FileCopyrightText: 2026 Korrikada
// SPDX-License-Identifier: Apache-2.0

#include <mlsl/Strings/InlineString.hpp>
#include <mlsl/Strings/StringView.hpp>

#include <catch2/catch_test_macros.hpp>

#include <utility>

TEST_CASE("InlineString uses inline capacity before growth")
{
	mlsl::InlineString<8> string("abc");

	REQUIRE(string.Size() == 3);
	REQUIRE(string.Capacity() == string.InlineCapacity());
	REQUIRE(string.InlineCapacity() == 8);
	REQUIRE(string.CString()[3] == '\0');
	REQUIRE(string.Append("de"));
	REQUIRE(string.Equals(mlsl::StringView("abcde")));
}

TEST_CASE("InlineString treats null input as empty")
{
	mlsl::InlineString<4> string(nullptr);

	REQUIRE(string.Empty());
	REQUIRE(string.CString()[0] == '\0');
	REQUIRE(string.Assign(nullptr));
	REQUIRE(string.Empty());
}

TEST_CASE("InlineString grows and preserves contents")
{
	mlsl::InlineString<3> string("abc");

	REQUIRE(string.Append("def"));
	REQUIRE(string.Size() == 6);
	REQUIRE(string.Capacity() > string.InlineCapacity());
	REQUIRE(string.Equals(mlsl::StringView("abcdef")));
	REQUIRE(string.CString()[6] == '\0');
}

TEST_CASE("InlineString can append its own view across growth")
{
	mlsl::InlineString<2> string("ab");
	auto view = string.View();

	REQUIRE(string.Append(view));
	REQUIRE(string.Equals(mlsl::StringView("abab")));
	REQUIRE(string.CString()[4] == '\0');
}

TEST_CASE("InlineString supports copy move assign resize and substr")
{
	mlsl::InlineString<2> string("ab");

	REQUIRE(string.Append('c'));
	REQUIRE(string.Resize(5, 'x'));

	mlsl::InlineString<2> copy(string);
	mlsl::InlineString<2> moved(std::move(copy));
	auto subview = moved.Substr(2, 3);

	REQUIRE(moved.Equals(mlsl::StringView("abcxx")));
	REQUIRE(copy.Empty());
	REQUIRE(subview);
	REQUIRE(subview->Equals(mlsl::StringView("cxx")));

	mlsl::InlineString<2> assigned;
	assigned = moved;

	REQUIRE(assigned == mlsl::StringView("abcxx"));

	assigned.Clear();

	REQUIRE(assigned.Empty());
	REQUIRE(assigned.CString()[0] == '\0');
}

TEST_CASE("InlineString supports slice insert erase and replace")
{
	mlsl::InlineString<12> string("abcdef");

	auto slice = string.Slice(1, 4);

	REQUIRE(slice);
	REQUIRE(slice->Equals(mlsl::StringView("bcde")));
	REQUIRE(string.Insert(3, "XY"));
	REQUIRE(string.Equals(mlsl::StringView("abcXYdef")));
	REQUIRE(string.Erase(1, 3));
	REQUIRE(string.Equals(mlsl::StringView("aYdef")));
	REQUIRE(string.Replace(1, 1, "BCD"));
	REQUIRE(string.Equals(mlsl::StringView("aBCDdef")));
	REQUIRE(string.Replace(7, 0, '!'));
	REQUIRE(string.Equals(mlsl::StringView("aBCDdef!")));
	REQUIRE(string.CString()[8] == '\0');
}

TEST_CASE("InlineString can insert and replace with its own slice across growth")
{
	mlsl::InlineString<4> string("abcd");
	auto middle = string.Slice(1, 2);

	REQUIRE(middle);
	REQUIRE(string.Insert(0, *middle));
	REQUIRE(string.Capacity() > string.InlineCapacity());
	REQUIRE(string.Equals(mlsl::StringView("bcabcd")));

	auto tail = string.Slice(2, 4);

	REQUIRE(tail);
	REQUIRE(string.Replace(1, 2, *tail));
	REQUIRE(string.Equals(mlsl::StringView("babcdbcd")));
	REQUIRE(string.CString()[8] == '\0');
}

TEST_CASE("InlineString slice mutation reports invalid offsets")
{
	mlsl::InlineString<4> string("abc");

	auto insert = string.Insert(5, "x");
	auto erase = string.Erase(5, 1);

	REQUIRE(not insert);
	REQUIRE(insert.error().type == mlsl::ErrorType::OutOfBounds);
	REQUIRE(not erase);
	REQUIRE(erase.error().type == mlsl::ErrorType::OutOfBounds);
	REQUIRE(string.Equals(mlsl::StringView("abc")));
}

TEST_CASE("InlineWString supports wide growth")
{
	mlsl::InlineWString<2> string(L"ab");

	REQUIRE(string.Append(L"cd"));
	REQUIRE(string.Size() == 4);
	REQUIRE(string.Capacity() > string.InlineCapacity());
	REQUIRE(string.Equals(mlsl::WStringView(L"abcd")));
}
