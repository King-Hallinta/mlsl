// mlsl
// SPDX-FileCopyrightText: 2026 Korrikada
// SPDX-License-Identifier: Apache-2.0

#include <mlsl/Memory/IntrusivePtr.hpp>

#include <catch2/catch_test_macros.hpp>

#include <utility>

namespace
{
	struct Counted
	{
		static inline int live = 0;

		int refs = 0;
		int value = 0;

		explicit Counted(int newValue)
			: value(newValue)
		{
			++live;
		}

		virtual ~Counted()
		{
			--live;
		}

		void AddReference()
		{
			++refs;
		}

		void ReleaseReference()
		{
			if (--refs == 0)
			{
				delete this;
			}
		}
	};

	struct CountedDerived : Counted
	{
		explicit CountedDerived(int newValue)
			: Counted(newValue)
		{
		}
	};
} // namespace

TEST_CASE("IntrusivePtr adopts a pointer and tracks the intrusive count")
{
	Counted::live = 0;

	mlsl::IntrusivePtr<Counted> empty;

	REQUIRE(not empty);
	REQUIRE(empty.Get() == nullptr);

	{
		mlsl::IntrusivePtr<Counted> pointer(new Counted(5));

		REQUIRE(pointer);
		REQUIRE(pointer->value == 5);
		REQUIRE((*pointer).value == 5);
		REQUIRE(pointer.Get()->refs == 1);
		REQUIRE(Counted::live == 1);

		{
			mlsl::IntrusivePtr<Counted> shared(pointer);

			REQUIRE(pointer.Get()->refs == 2);
		}

		REQUIRE(pointer.Get()->refs == 1);
	}

	REQUIRE(Counted::live == 0);
}

TEST_CASE("IntrusivePtr move transfers ownership without changing the count")
{
	Counted::live = 0;

	mlsl::IntrusivePtr<Counted> pointer(new Counted(1));

	REQUIRE(pointer.Get()->refs == 1);

	mlsl::IntrusivePtr<Counted> moved(std::move(pointer));

	REQUIRE(not pointer);
	REQUIRE(moved);
	REQUIRE(moved.Get()->refs == 1);

	moved = nullptr;

	REQUIRE(Counted::live == 0);
}

TEST_CASE("IntrusivePtr copy assignment releases the previous object")
{
	Counted::live = 0;

	mlsl::IntrusivePtr<Counted> first(new Counted(1));
	mlsl::IntrusivePtr<Counted> second(new Counted(2));

	REQUIRE(Counted::live == 2);

	first = second;

	REQUIRE(Counted::live == 1);
	REQUIRE(first->value == 2);
	REQUIRE(second.Get()->refs == 2);

	first = nullptr;
	second = nullptr;

	REQUIRE(Counted::live == 0);
}

TEST_CASE("IntrusivePtr Reset replaces and Release detaches the managed object")
{
	Counted::live = 0;

	mlsl::IntrusivePtr<Counted> pointer(new Counted(7));

	pointer.Reset(new Counted(8));

	REQUIRE(pointer->value == 8);
	REQUIRE(Counted::live == 1);

	Counted *raw = pointer.Release();

	REQUIRE(not pointer);
	REQUIRE(raw->refs == 1);
	REQUIRE(Counted::live == 1);

	raw->ReleaseReference();

	REQUIRE(Counted::live == 0);
}

TEST_CASE("IntrusivePtr swap exchanges managed objects")
{
	Counted::live = 0;

	mlsl::IntrusivePtr<Counted> first(new Counted(1));
	mlsl::IntrusivePtr<Counted> second(new Counted(2));

	first.Swap(second);

	REQUIRE(first->value == 2);
	REQUIRE(second->value == 1);

	first = nullptr;
	second = nullptr;

	REQUIRE(Counted::live == 0);
}

TEST_CASE("IntrusivePtr supports converting construction from a derived pointer")
{
	Counted::live = 0;

	mlsl::IntrusivePtr<CountedDerived> derived(new CountedDerived(3));

	REQUIRE(derived.Get()->refs == 1);

	mlsl::IntrusivePtr<Counted> base(derived);

	REQUIRE(base.Get()->refs == 2);
	REQUIRE(base->value == 3);

	derived = nullptr;

	REQUIRE(Counted::live == 1);

	base = nullptr;

	REQUIRE(Counted::live == 0);
}
