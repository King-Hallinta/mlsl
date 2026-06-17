// mlsl
// SPDX-FileCopyrightText: 2026 Korrikada
// SPDX-License-Identifier: Apache-2.0

#include <mlsl/Containers/BasicQueue.hpp>

#include <catch2/catch_test_macros.hpp>

namespace
{
	template <typename T, std::size_t N>
	class QueueHarness : public mlsl::BasicQueue<T, QueueHarness<T, N>>
	{
	public:
		using Base = mlsl::BasicQueue<T, QueueHarness<T, N>>;
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

TEST_CASE("BasicQueue provides FIFO behavior and wraparound")
{
	QueueHarness<int, 3> queue;

	REQUIRE(queue.Add(1));
	REQUIRE(queue.Add(2));
	REQUIRE(queue.Add(3));
	REQUIRE(queue.Front() == 1);
	REQUIRE(queue.Back() == 3);

	queue.Remove();
	REQUIRE(queue.Add(4));

	REQUIRE(queue.Size() == 3);
	REQUIRE(queue.Front() == 2);
	REQUIRE(queue.Back() == 4);
}

TEST_CASE("BasicQueue reports out of memory through derived capacity")
{
	QueueHarness<int, 1> queue;

	REQUIRE(queue.Add(7));

	auto overflow = queue.Add(8);

	REQUIRE(not overflow);
	REQUIRE(overflow.error().type == mlsl::ErrorType::OutOfMemory);
}
