// mlsl
// SPDX-FileCopyrightText: 2026 Korrikada
// SPDX-License-Identifier: Apache-2.0

#include <mlsl/Memory/UniquePtr.hpp>

#include <catch2/catch_test_macros.hpp>

#include <utility>

namespace
{
	struct UniqueBase
	{
		virtual ~UniqueBase() = default;
		virtual int ReadValue() const = 0;
	};

	struct UniqueDerived : UniqueBase
	{
		static inline int destroyed = 0;

		int value = 0;

		explicit UniqueDerived(int newValue)
			: value(newValue)
		{
		}

		~UniqueDerived() override
		{
			++destroyed;
		}

		int ReadValue() const override
		{
			return value;
		}
	};
} // namespace

TEST_CASE("UniquePtr owns created values and supports release")
{
	UniqueDerived::destroyed = 0;

	auto pointer = mlsl::UniquePtr<UniqueDerived>::Create(42);

	REQUIRE(pointer);
	REQUIRE(pointer.Get() != nullptr);
	REQUIRE(pointer->ReadValue() == 42);
	REQUIRE((*pointer).ReadValue() == 42);

	auto raw = pointer.Release();

	REQUIRE(not pointer);
	REQUIRE(raw != nullptr);
	REQUIRE(raw->ReadValue() == 42);
	REQUIRE(UniqueDerived::destroyed == 0);

	delete raw;

	REQUIRE(UniqueDerived::destroyed == 1);
}

TEST_CASE("UniquePtr reset and move transfer ownership")
{
	UniqueDerived::destroyed = 0;

	mlsl::UniquePtr<UniqueDerived> pointer(new UniqueDerived(1));

	pointer.Reset(new UniqueDerived(2));

	REQUIRE(pointer);
	REQUIRE(pointer->ReadValue() == 2);
	REQUIRE(UniqueDerived::destroyed == 1);

	mlsl::UniquePtr<UniqueDerived> moved(std::move(pointer));

	REQUIRE(not pointer);
	REQUIRE(moved);
	REQUIRE(moved->ReadValue() == 2);

	moved = nullptr;

	REQUIRE(not moved);
	REQUIRE(UniqueDerived::destroyed == 2);
}

TEST_CASE("UniquePtr supports converting move construction and swap")
{
	UniqueDerived::destroyed = 0;

	mlsl::UniquePtr<UniqueDerived> derived(new UniqueDerived(5));
	mlsl::UniquePtr<UniqueBase> base(std::move(derived));
	mlsl::UniquePtr<UniqueBase> other(new UniqueDerived(9));

	REQUIRE(not derived);
	REQUIRE(base);
	REQUIRE(other);
	REQUIRE(base->ReadValue() == 5);
	REQUIRE(other->ReadValue() == 9);

	base.Swap(other);

	REQUIRE(base->ReadValue() == 9);
	REQUIRE(other->ReadValue() == 5);
}

TEST_CASE("UniquePtr default null and swap behavior")
{
	mlsl::UniquePtr<int> empty;
	mlsl::UniquePtr<int> value(new int(7));

	REQUIRE(not empty);
	REQUIRE(value);

	empty.Swap(value);

	REQUIRE(empty);
	REQUIRE(not value);
	REQUIRE(*empty == 7);

	empty.Reset();

	REQUIRE(not empty);
}
