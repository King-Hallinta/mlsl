// mlsl
// SPDX-FileCopyrightText: 2026 Korrikada
// SPDX-License-Identifier: Apache-2.0

#include <mlsl/Strings/FixedString.hpp>
#include <mlsl/Strings/StringView.hpp>

#include <catch2/catch_test_macros.hpp>

TEST_CASE("FixedString stores text without allocation")
{
	mlsl::FixedString<8> string("abc");

	REQUIRE(string.Size() == 3);
	REQUIRE(string.Capacity() == 8);
	REQUIRE(string.CString()[3] == '\0');
	REQUIRE(string.Front() == 'a');
	REQUIRE(string.Back() == 'c');
	REQUIRE(string.Append("de"));
	REQUIRE(string.Equals(mlsl::StringView("abcde")));
}

TEST_CASE("FixedString treats null input as empty")
{
	mlsl::FixedString<4> string(nullptr);

	REQUIRE(string.Empty());
	REQUIRE(string.CString()[0] == '\0');
	REQUIRE(string.Assign(nullptr));
	REQUIRE(string.Empty());
}

TEST_CASE("FixedString reports overflow without changing contents")
{
	mlsl::FixedString<4> string("ab");

	auto result = string.Append("cde");

	REQUIRE(not result);
	REQUIRE(result.error().type == mlsl::ErrorType::BufferOverflow);
	REQUIRE(string.Equals(mlsl::StringView("ab")));

	result = string.Resize(5, 'x');

	REQUIRE(not result);
	REQUIRE(result.error().type == mlsl::ErrorType::BufferOverflow);
	REQUIRE(string.Equals(mlsl::StringView("ab")));
}

TEST_CASE("FixedString supports access substr and mutation")
{
	mlsl::FixedString<8> string;

	REQUIRE(string.Assign("abc"));
	REQUIRE(string.Append('d'));

	auto item = string.Get(2);
	auto subview = string.Substr(1, 2);

	REQUIRE(item);
	REQUIRE(**item == 'c');
	REQUIRE(subview);
	REQUIRE(subview->Equals(mlsl::StringView("bc")));

	string.Remove();

	REQUIRE(string.Equals(mlsl::StringView("abc")));
	REQUIRE(string.Resize(5, 'z'));
	REQUIRE(string.Equals(mlsl::StringView("abczz")));
}

TEST_CASE("FixedString supports slice insert erase and replace")
{
	mlsl::FixedString<12> string("abcdef");

	auto slice = string.Slice(2, 2);

	REQUIRE(slice);
	REQUIRE(slice->Equals(mlsl::StringView("cd")));
	REQUIRE(string.Insert(3, "XY"));
	REQUIRE(string.Equals(mlsl::StringView("abcXYdef")));
	REQUIRE(string.Erase(2, 3));
	REQUIRE(string.Equals(mlsl::StringView("abdef")));
	REQUIRE(string.Replace(2, 2, "CD"));
	REQUIRE(string.Equals(mlsl::StringView("abCDf")));
	REQUIRE(string.Replace(5, 0, '!'));
	REQUIRE(string.Equals(mlsl::StringView("abCDf!")));
	REQUIRE(string.CString()[6] == '\0');
}

TEST_CASE("FixedString slice overflow preserves contents")
{
	mlsl::FixedString<6> string("abcd");

	auto insert = string.Insert(2, "xyz");

	REQUIRE(not insert);
	REQUIRE(insert.error().type == mlsl::ErrorType::BufferOverflow);
	REQUIRE(string.Equals(mlsl::StringView("abcd")));

	auto replace = string.Replace(1, 1, "wxyz");

	REQUIRE(not replace);
	REQUIRE(replace.error().type == mlsl::ErrorType::BufferOverflow);
	REQUIRE(string.Equals(mlsl::StringView("abcd")));
	REQUIRE(string.CString()[4] == '\0');
}

TEST_CASE("FixedString slice mutation reports invalid offsets")
{
	mlsl::FixedString<8> string("abc");

	auto erase = string.Erase(4, 1);
	auto replace = string.Replace(4, 1, "x");

	REQUIRE(not erase);
	REQUIRE(erase.error().type == mlsl::ErrorType::OutOfBounds);
	REQUIRE(not replace);
	REQUIRE(replace.error().type == mlsl::ErrorType::OutOfBounds);
	REQUIRE(string.Equals(mlsl::StringView("abc")));
}

TEST_CASE("FixedWString supports wide aliases")
{
	mlsl::FixedWString<8> string(L"ab");

	REQUIRE(string.Append(L"cd"));
	REQUIRE(string.Size() == 4);
	REQUIRE(string.Equals(mlsl::WStringView(L"abcd")));
	REQUIRE(string.CString()[4] == L'\0');
}
