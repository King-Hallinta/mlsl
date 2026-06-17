// mlsl
// SPDX-FileCopyrightText: 2026 Korrikada
// SPDX-License-Identifier: Apache-2.0

#include <mlsl/Memory/SharedPtr.hpp>
#include <mlsl/Memory/WeakPtr.hpp>

#include <catch2/catch_test_macros.hpp>

#include <utility>

namespace
{
	struct SharedBase
	{
		virtual ~SharedBase() = default;
		virtual int ReadValue() const = 0;
	};

	struct SharedDerived : SharedBase
	{
		static inline int destroyed = 0;

		int value = 0;

		explicit SharedDerived(int newValue)
			: value(newValue)
		{
		}

		~SharedDerived() override
		{
			++destroyed;
		}

		int ReadValue() const override
		{
			return value;
		}
	};

	struct PlainValue
	{
		static inline int destroyed = 0;

		int value = 0;

		explicit PlainValue(int newValue)
			: value(newValue)
		{
		}

		~PlainValue()
		{
			++destroyed;
		}
	};
} // namespace

TEST_CASE("SharedPtr tracks shared ownership and ref counts")
{
	SharedDerived::destroyed = 0;

	auto first = mlsl::SharedPtr<SharedDerived>::Create(12);

	REQUIRE(first);
	REQUIRE(first->ReadValue() == 12);
	REQUIRE(first.RefCount() == 1);

	mlsl::SharedPtr<SharedDerived> second(first);

	REQUIRE(first.RefCount() == 2);
	REQUIRE(second.RefCount() == 2);

	second.Reset();

	REQUIRE(first.RefCount() == 1);
	REQUIRE(SharedDerived::destroyed == 0);

	first.Reset();

	REQUIRE(SharedDerived::destroyed == 1);
}

TEST_CASE("SharedPtr supports converting copy and swap")
{
	SharedDerived::destroyed = 0;

	mlsl::SharedPtr<SharedDerived> derived(new SharedDerived(3));
	mlsl::SharedPtr<SharedBase> base(derived);
	mlsl::SharedPtr<SharedBase> other(new SharedDerived(8));

	REQUIRE(derived.RefCount() == 2);
	REQUIRE(base.RefCount() == 2);
	REQUIRE(base->ReadValue() == 3);
	REQUIRE(other->ReadValue() == 8);

	base.Swap(other);

	REQUIRE(base->ReadValue() == 8);
	REQUIRE(other->ReadValue() == 3);
	REQUIRE(derived.RefCount() == 2);

	other = nullptr;
	derived = nullptr;
	base = nullptr;

	REQUIRE(SharedDerived::destroyed == 2);
}

TEST_CASE("WeakPtr locks while shared owners exist and expires after release")
{
	SharedDerived::destroyed = 0;

	auto shared = mlsl::SharedPtr<SharedDerived>::Create(21);
	mlsl::WeakPtr<SharedDerived> weak(shared);

	REQUIRE(not weak.Expired());
	REQUIRE(weak.RefCount() == 1);

	auto locked = weak.Lock();

	REQUIRE(locked.has_value());
	REQUIRE((*locked)->ReadValue() == 21);
	REQUIRE((*locked).RefCount() == 2);

	locked->Reset();
	shared.Reset();

	REQUIRE(weak.Expired());
	REQUIRE(weak.RefCount() == 0);
	REQUIRE(SharedDerived::destroyed == 1);

	weak.Reset();

	REQUIRE(SharedDerived::destroyed == 1);
}

TEST_CASE("WeakPtr copy move and assignment preserve weak tracking")
{
	SharedDerived::destroyed = 0;

	auto shared = mlsl::SharedPtr<SharedDerived>::Create(4);
	mlsl::WeakPtr<SharedDerived> first(shared);
	mlsl::WeakPtr<SharedDerived> second(first);
	mlsl::WeakPtr<SharedDerived> third(std::move(second));

	REQUIRE(not second.Lock().has_value());
	REQUIRE(third.Lock().has_value());

	first = nullptr;

	shared.Reset();

	REQUIRE(third.Expired());
	REQUIRE(SharedDerived::destroyed == 1);

	third.Reset();

	REQUIRE(SharedDerived::destroyed == 1);
}

TEST_CASE("SharedPtr default reset and swap behavior")
{
	SharedDerived::destroyed = 0;

	mlsl::SharedPtr<SharedDerived> empty;
	mlsl::SharedPtr<SharedDerived> value(new SharedDerived(4));

	REQUIRE(not empty);
	REQUIRE(value.RefCount() == 1);

	empty.Swap(value);

	REQUIRE(empty);
	REQUIRE(not value);
	REQUIRE(empty->value == 4);
	REQUIRE(empty.RefCount() == 1);

	empty.Reset();

	REQUIRE(not empty);
	REQUIRE(SharedDerived::destroyed == 1);
}

TEST_CASE("WeakPtr default swap and expired lock behavior")
{
	SharedDerived::destroyed = 0;

	mlsl::WeakPtr<SharedDerived> empty;
	auto shared = mlsl::SharedPtr<SharedDerived>::Create(9);
	mlsl::WeakPtr<SharedDerived> weak(shared);

	REQUIRE(empty.Expired());
	REQUIRE(not empty.Lock().has_value());
	REQUIRE(not weak.Expired());

	empty.Swap(weak);

	REQUIRE(not empty.Expired());
	REQUIRE(weak.Expired());

	shared.Reset();

	REQUIRE(empty.Expired());
	REQUIRE(not empty.Lock().has_value());
	REQUIRE(SharedDerived::destroyed == 1);

	empty.Reset();

	REQUIRE(SharedDerived::destroyed == 1);
}

TEST_CASE("SharedPtr does not require intrusive base classes")
{
	PlainValue::destroyed = 0;

	auto shared = mlsl::SharedPtr<PlainValue>::Create(7);
	mlsl::WeakPtr<PlainValue> weak(shared);

	REQUIRE(shared->value == 7);
	REQUIRE(shared.RefCount() == 1);

	shared.Reset();

	REQUIRE(weak.Expired());
	REQUIRE(PlainValue::destroyed == 1);
}
