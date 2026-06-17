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

	REQUIRE(string.Add('c'));
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

TEST_CASE("InlineWString supports wide growth")
{
	mlsl::InlineWString<2> string(L"ab");

	REQUIRE(string.Append(L"cd"));
	REQUIRE(string.Size() == 4);
	REQUIRE(string.Capacity() > string.InlineCapacity());
	REQUIRE(string.Equals(mlsl::WStringView(L"abcd")));
}
