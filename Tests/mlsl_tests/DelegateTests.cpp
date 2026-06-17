// mlsl
// SPDX-FileCopyrightText: 2026 Korrikada
// SPDX-License-Identifier: Apache-2.0

#include <mlsl/Core/Delegate.hpp>

#include <catch2/catch_test_macros.hpp>

namespace
{
	int DoubleValue(int value)
	{
		return value * 2;
	}

	void StoreValue(int *target, int value)
	{
		*target = value;
	}

	struct Multiplier
	{
		int factor = 1;

		int Multiply(int value)
		{
			return value * factor;
		}

		int MultiplyConst(int value) const
		{
			return value * factor;
		}
	};
} // namespace

TEST_CASE("Delegate invokes free functions")
{
	mlsl::Delegate<int(int)> delegate(DoubleValue);

	REQUIRE(delegate);
	REQUIRE(not delegate.Empty());
	REQUIRE(delegate(4) == 8);
}

TEST_CASE("Delegate binds compile time free functions")
{
	auto delegate = mlsl::Delegate<int(int)>::From<DoubleValue>();

	REQUIRE(delegate(5) == 10);
}

TEST_CASE("Delegate invokes referenced callables")
{
	int offset = 3;
	auto callable = [offset](int value)
	{
		return value + offset;
	};

	mlsl::Delegate<int(int)> delegate;
	delegate.Bind(callable);

	REQUIRE(delegate(7) == 10);
}

TEST_CASE("Delegate invokes referenced const callables")
{
	const auto callable = [](int value)
	{
		return value - 2;
	};

	mlsl::Delegate<int(int)> delegate;
	delegate.Bind(callable);

	REQUIRE(delegate(9) == 7);
}

TEST_CASE("Delegate invokes member functions")
{
	Multiplier multiplier;
	multiplier.factor = 4;

	auto delegate = mlsl::Delegate<int(int)>::From<Multiplier, &Multiplier::Multiply>(&multiplier);

	REQUIRE(delegate(6) == 24);
}

TEST_CASE("Delegate invokes const member functions")
{
	const Multiplier multiplier {5};

	auto delegate = mlsl::Delegate<int(int)>::From<Multiplier, &Multiplier::MultiplyConst>(&multiplier);

	REQUIRE(delegate(3) == 15);
}

TEST_CASE("Delegate supports void return values")
{
	int value = 0;
	mlsl::Delegate<void(int *, int)> delegate(StoreValue);

	delegate(&value, 42);

	REQUIRE(value == 42);
}

TEST_CASE("Delegate reset and null assignment clear binding")
{
	mlsl::Delegate<int(int)> delegate(DoubleValue);

	delegate.Reset();

	REQUIRE(not delegate);
	REQUIRE(delegate.Empty());

	delegate = DoubleValue;

	REQUIRE(delegate(2) == 4);

	delegate = nullptr;

	REQUIRE(not delegate);
	REQUIRE(delegate.Empty());
}
