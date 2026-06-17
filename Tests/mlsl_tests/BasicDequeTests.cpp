// mlsl
// SPDX-FileCopyrightText: 2026 Korrikada
// SPDX-License-Identifier: Apache-2.0

#include <mlsl/Containers/BasicDeque.hpp>

#include <catch2/catch_test_macros.hpp>

namespace
{
	template <typename T, std::size_t N>
	class DequeHarness : public mlsl::BasicDeque<T, DequeHarness<T, N>>
	{
	public:
		using Base = mlsl::BasicDeque<T, DequeHarness<T, N>>;
		using typename Base::SizeType;

	public:
		T *GetStorage()
		{
			return reinterpret_cast<T *>(m_Storage);
		}

		const T *GetStorage() const
		{
			return reinterpret_cast<const T *>(m_Storage);
		}

		SizeType GetCapacity() const
		{
			return N;
		}

		std::expected<void, mlsl::Error> EnsureCapacity(SizeType required)
		{
			if (required > N)
			{
				return std::unexpected(mlsl::Error {mlsl::ErrorType::OutOfMemory});
			}

			return {};
		}

	private:
		alignas(T) std::byte m_Storage[sizeof(T) * N];
	};
} // namespace

TEST_CASE("BasicDeque provides double ended access semantics")
{
	DequeHarness<int, 3> deque;

	REQUIRE(deque.Append(2));
	REQUIRE(deque.Insert(1));
	REQUIRE(deque.Append(3));

	auto middle = deque.Get(1);

	REQUIRE(deque.Size() == 3);
	REQUIRE(deque.Front() == 1);
	REQUIRE(deque.Back() == 3);
	REQUIRE(deque[1] == 2);
	REQUIRE(middle);
	REQUIRE(**middle == 2);
}

TEST_CASE("BasicDeque reports out of memory through derived capacity")
{
	DequeHarness<int, 2> deque;

	REQUIRE(deque.Append(4));
	REQUIRE(deque.Append(5));

	auto overflow = deque.Insert(3);

	REQUIRE(not overflow);
	REQUIRE(overflow.error().type == mlsl::ErrorType::OutOfMemory);
}
