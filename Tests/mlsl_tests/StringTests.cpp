// mlsl
// SPDX-FileCopyrightText: 2026 Korrikada
// SPDX-License-Identifier: Apache-2.0

#include <mlsl/Strings/String.hpp>
#include <mlsl/Strings/StringView.hpp>

#include <catch2/catch_test_macros.hpp>

#include <utility>

TEST_CASE("String owns null terminated dynamic storage")
{
	mlsl::String string("hello");

	REQUIRE(string.Size() == 5);
	REQUIRE(string.Capacity() >= 5);
	REQUIRE(not string.Empty());
	REQUIRE(string.CString()[5] == '\0');
	REQUIRE(string.Data() == string.Begin());
	REQUIRE(string.End() == string.Begin() + string.Size());
	REQUIRE(string.Front() == 'h');
	REQUIRE(string.Back() == 'o');

	string[1] = 'a';

	REQUIRE(string.CString()[1] == 'a');
}

TEST_CASE("String treats null input as empty")
{
	mlsl::String string(nullptr);

	REQUIRE(string.Empty());
	REQUIRE(string.CString()[0] == '\0');
	REQUIRE(string.Assign(nullptr));
	REQUIRE(string.Empty());
}

TEST_CASE("String mutates through assign append add remove resize and clear")
{
	mlsl::String string;

	REQUIRE(string.Assign("ab"));
	REQUIRE(string.Append("cd"));
	REQUIRE(string.Append('e'));
	REQUIRE(string.Size() == 5);
	REQUIRE(string.Equals(mlsl::StringView("abcde")));

	string.Remove();

	REQUIRE(string.Equals(mlsl::StringView("abcd")));
	REQUIRE(string.Resize(6, 'x'));
	REQUIRE(string.Size() == 6);
	REQUIRE(string[4] == 'x');
	REQUIRE(string[5] == 'x');
	REQUIRE(string.CString()[6] == '\0');

	string.Clear();

	REQUIRE(string.Empty());
	REQUIRE(string.CString()[0] == '\0');
}

TEST_CASE("String can append its own view across growth")
{
	mlsl::String string("abcd");
	auto view = string.View();

	REQUIRE(string.Append(view));
	REQUIRE(string.Equals(mlsl::StringView("abcdabcd")));
	REQUIRE(string.CString()[8] == '\0');
}

TEST_CASE("String preserves content across reserve copy move and substr")
{
	mlsl::String string("alpha");

	REQUIRE(string.Reserve(32));
	REQUIRE(string.Capacity() >= 32);

	mlsl::String copy(string);
	mlsl::String moved(std::move(copy));
	auto subview = moved.Substr(1, 3);
	auto missing = moved.Get(10);

	REQUIRE(moved.Equals(mlsl::StringView("alpha")));
	REQUIRE(copy.Empty());
	REQUIRE(subview);
	REQUIRE(subview->Equals(mlsl::StringView("lph")));
	REQUIRE(not missing);
	REQUIRE(missing.error().type == mlsl::ErrorType::OutOfBounds);

	mlsl::String assigned;
	assigned = moved;

	REQUIRE(assigned == mlsl::StringView("alpha"));
}

TEST_CASE("WString supports wide storage and comparison")
{
	mlsl::WString string(L"wide");

	REQUIRE(string.Size() == 4);
	REQUIRE(string.CString()[4] == L'\0');
	REQUIRE(string.Append(L" text"));
	REQUIRE(string.Equals(mlsl::WStringView(L"wide text")));
	REQUIRE(string.Compare(mlsl::WStringView(L"wide z")) < 0);
}
