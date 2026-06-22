// mlsl
// SPDX-FileCopyrightText: 2026 Korrikada
// SPDX-License-Identifier: Apache-2.0

#include <mlsl/Strings/StringView.hpp>

#include <catch2/catch_test_macros.hpp>

TEST_CASE("StringView exposes literal data and bounds checked access")
{
	mlsl::StringView view("hello");

	auto item = view.Get(1);
	auto missing = view.Get(5);

	REQUIRE(view.Size() == 5);
	REQUIRE(not view.Empty());
	REQUIRE(view.Front() == 'h');
	REQUIRE(view.Back() == 'o');
	REQUIRE(view.Data()[0] == 'h');
	REQUIRE(view.Begin() == view.Data());
	REQUIRE(view.End() == view.Data() + view.Size());
	REQUIRE(item);
	REQUIRE(**item == 'e');
	REQUIRE(not missing);
	REQUIRE(missing.error().type == mlsl::ErrorType::OutOfBounds);
}

TEST_CASE("StringView treats null data as empty")
{
	mlsl::StringView view(nullptr);
	auto subview = view.Substr(0);

	REQUIRE(view.Empty());
	REQUIRE(view.Size() == 0);
	REQUIRE(view.Data() == nullptr);
	REQUIRE(view.End() == nullptr);
	REQUIRE(subview);
	REQUIRE(subview->Empty());
	REQUIRE(subview->Data() == nullptr);
}

TEST_CASE("StringView supports explicit size substr and compare")
{
	const char data[] {'a', 'b', '\0', 'c'};
	mlsl::StringView view(data, 4);
	auto subview = view.Substr(1, 2);
	auto missing = view.Substr(5, 1);

	REQUIRE(view.Size() == 4);
	REQUIRE(view[2] == '\0');
	REQUIRE(subview);
	REQUIRE(subview->Size() == 2);
	REQUIRE((*subview)[0] == 'b');
	REQUIRE((*subview)[1] == '\0');
	REQUIRE(not missing);
	REQUIRE(missing.error().type == mlsl::ErrorType::OutOfBounds);
	REQUIRE(mlsl::StringView("abc").Equals(mlsl::StringView("abc")));
	REQUIRE(mlsl::StringView("abc").Compare(mlsl::StringView("abd")) < 0);
	REQUIRE(mlsl::StringView("abc") != mlsl::StringView("ab"));
}

TEST_CASE("StringView supports slice alias")
{
	mlsl::StringView view("abcdef");
	auto slice = view.Slice(2, 3);
	auto tail = view.Slice(4);
	auto missing = view.Slice(7, 1);

	REQUIRE(slice);
	REQUIRE(slice->Equals(mlsl::StringView("cde")));
	REQUIRE(tail);
	REQUIRE(tail->Equals(mlsl::StringView("ef")));
	REQUIRE(not missing);
	REQUIRE(missing.error().type == mlsl::ErrorType::OutOfBounds);
}

TEST_CASE("WStringView exposes wide literals")
{
	mlsl::WStringView view(L"wide");
	auto tail = view.Substr(1);

	REQUIRE(view.Size() == 4);
	REQUIRE(view.Front() == L'w');
	REQUIRE(view.Back() == L'e');
	REQUIRE(tail);
	REQUIRE(tail->Size() == 3);
	REQUIRE((*tail)[0] == L'i');
}
