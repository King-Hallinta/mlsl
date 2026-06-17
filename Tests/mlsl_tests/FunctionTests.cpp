// mlsl
// SPDX-FileCopyrightText: 2026 Korrikada
// SPDX-License-Identifier: Apache-2.0

#include <mlsl/Core/Function.hpp>

#include <catch2/catch_test_macros.hpp>

#include <utility>

namespace
{
	int TripleValue(int value)
	{
		return value * 3;
	}

	struct DestructionCounter
	{
		int *destructions = nullptr;

		void operator()() const
		{
		}

		~DestructionCounter()
		{
			if (destructions != nullptr)
			{
				++(*destructions);
			}
		}
	};
} // namespace

TEST_CASE("Function defaults to empty")
{
	mlsl::Function<int()> function;

	REQUIRE(not function);
	REQUIRE(function.Empty());
}

TEST_CASE("Function owns directly assigned lambdas")
{
	int offset = 4;

	mlsl::Function<int(int)> function = [offset](int value)
	{
		return value + offset;
	};

	REQUIRE(function);
	REQUIRE(not function.Empty());
	REQUIRE(function(6) == 10);
}

TEST_CASE("Function owns free function bindings")
{
	mlsl::Function<int(int)> function = TripleValue;

	REQUIRE(function(7) == 21);
}

TEST_CASE("Function assignment replaces callable")
{
	mlsl::Function<int(int)> function = [](int value)
	{
		return value + 1;
	};

	REQUIRE(function(5) == 6);

	function = [](int value)
	{
		return value * 3;
	};

	REQUIRE(function(5) == 15);
}

TEST_CASE("Function supports void returns and reference arguments")
{
	mlsl::Function<void(int &)> function = [](int &value)
	{
		value += 7;
	};

	int value = 3;
	function(value);

	REQUIRE(value == 10);
}

TEST_CASE("Function supports mutable callables")
{
	mlsl::Function<int()> function = [count = 0]() mutable
	{
		return ++count;
	};

	REQUIRE(function() == 1);
	REQUIRE(function() == 2);
}

TEST_CASE("Function copy keeps callable state independent")
{
	mlsl::Function<int()> original = [count = 2]() mutable
	{
		return ++count;
	};

	REQUIRE(original() == 3);

	mlsl::Function<int()> copy = original;

	REQUIRE(original() == 4);
	REQUIRE(copy() == 4);
	REQUIRE(original() == 5);
	REQUIRE(copy() == 5);
}

TEST_CASE("Function move transfers callable")
{
	mlsl::Function<int(int)> source = [](int value)
	{
		return value * 2;
	};

	mlsl::Function<int(int)> destination = std::move(source);

	REQUIRE(source.Empty());
	REQUIRE(not source);
	REQUIRE(destination(8) == 16);
}

TEST_CASE("Function reset and null assignment clear callable")
{
	mlsl::Function<int(int)> function = [](int value)
	{
		return value - 1;
	};

	function.Reset();

	REQUIRE(function.Empty());
	REQUIRE(not function);

	function = [](int value)
	{
		return value + 5;
	};

	REQUIRE(function(2) == 7);

	function = nullptr;

	REQUIRE(function.Empty());
	REQUIRE(not function);
}

TEST_CASE("Function swap exchanges callables")
{
	mlsl::Function<int(int)> left = [](int value)
	{
		return value + 2;
	};

	mlsl::Function<int(int)> right = [](int value)
	{
		return value * 4;
	};

	left.Swap(right);

	REQUIRE(left(3) == 12);
	REQUIRE(right(3) == 5);
}

TEST_CASE("Function reset destroys owned callable")
{
	int destructions = 0;
	DestructionCounter callable {&destructions};

	{
		mlsl::Function<void()> function(callable);

		REQUIRE(destructions == 0);

		function.Reset();

		REQUIRE(destructions == 1);
	}

	REQUIRE(destructions == 1);
}
