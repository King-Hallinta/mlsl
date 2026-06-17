// mlsl
// SPDX-FileCopyrightText: 2026 Korrikada
// SPDX-License-Identifier: Apache-2.0

#include <mlsl/Memory/ObjectPool.hpp>

#include <catch2/catch_test_macros.hpp>

namespace
{
	struct PoolValue
	{
		static inline int destroyed = 0;

		int value = 0;

		PoolValue() = default;

		explicit PoolValue(int newValue)
			: value(newValue)
		{
		}

		~PoolValue()
		{
			++destroyed;
		}
	};
} // namespace

TEST_CASE("ObjectPool acquires objects until capacity is exhausted")
{
	PoolValue::destroyed = 0;

	mlsl::ObjectPool<PoolValue, 2> pool;
	auto first = pool.Acquire(10);
	auto second = pool.Acquire(20);
	auto third = pool.Acquire(30);

	REQUIRE(pool.Capacity() == 2);
	REQUIRE(first);
	REQUIRE(second);
	REQUIRE(not third);
	REQUIRE(third.error().type == mlsl::ErrorType::OutOfMemory);
	REQUIRE(pool.Available() == 0);
	REQUIRE(first.value()->value == 10);
	REQUIRE(second.value()->value == 20);
	REQUIRE(pool.Owns(*first));
	REQUIRE(pool.Owns(*second));
}

TEST_CASE("ObjectPool release destroys objects and makes slots reusable")
{
	PoolValue::destroyed = 0;

	mlsl::ObjectPool<PoolValue, 2> pool;
	auto first = pool.Acquire(7);

	REQUIRE(first);

	auto releasedAddress = *first;

	pool.Release(releasedAddress);

	REQUIRE(PoolValue::destroyed == 1);
	REQUIRE(pool.Available() == pool.Capacity());

	auto next = pool.Acquire(11);

	REQUIRE(next);
	REQUIRE(*next == releasedAddress);
	REQUIRE(next.value()->value == 11);

	pool.Release(*next);

	REQUIRE(PoolValue::destroyed == 2);
}

TEST_CASE("ObjectPool release ignores null and distinguishes external memory")
{
	PoolValue::destroyed = 0;

	mlsl::ObjectPool<PoolValue, 1> pool;
	PoolValue external {5};

	pool.Release(nullptr);

	REQUIRE(pool.Available() == pool.Capacity());
	REQUIRE(not pool.Owns(&external));
	REQUIRE(PoolValue::destroyed == 0);
}

namespace
{
	struct DefaultPoolValue
	{
		static inline int destroyed = 0;

		int value = 5;

		~DefaultPoolValue()
		{
			++destroyed;
		}
	};
}

TEST_CASE("ObjectPool supports default construction arguments")
{
	DefaultPoolValue::destroyed = 0;

	mlsl::ObjectPool<DefaultPoolValue, 1> pool;
	auto item = pool.Acquire();

	REQUIRE(item);
	REQUIRE((*item)->value == 5);
	REQUIRE(pool.Available() == 0);

	pool.Release(*item);

	REQUIRE(pool.Available() == 1);
	REQUIRE(DefaultPoolValue::destroyed == 1);
}

TEST_CASE("ObjectPool owns only addresses inside its storage")
{
	mlsl::ObjectPool<DefaultPoolValue, 1> pool;
	DefaultPoolValue external;
	auto item = pool.Acquire();

	REQUIRE(item);
	REQUIRE(pool.Owns(*item));
	REQUIRE(not pool.Owns(&external));

	pool.Release(*item);
}
