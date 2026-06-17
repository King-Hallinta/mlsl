// mlsl
// SPDX-FileCopyrightText: 2026 Korrikada
// SPDX-License-Identifier: Apache-2.0

#include <mlsl/Containers/BasicStack.hpp>

#include <catch2/catch_test_macros.hpp>

namespace
{
	template <typename T, std::size_t N>
	class StackHarness : public mlsl::BasicStack<T, StackHarness<T, N>>
	{
	public:
		using Base = mlsl::BasicStack<T, StackHarness<T, N>>;
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

TEST_CASE("BasicStack provides LIFO behavior through derived storage")
{
	StackHarness<int, 3> stack;

	REQUIRE(stack.Empty());
	REQUIRE(stack.Add(1));
	REQUIRE(stack.Add(2));
	REQUIRE(stack.Add(3));
	REQUIRE(stack.Size() == 3);
	REQUIRE(stack.Top() == 3);

	stack.Remove();

	REQUIRE(stack.Size() == 2);
	REQUIRE(stack.Top() == 2);

	stack.Clear();

	REQUIRE(stack.Empty());
}

TEST_CASE("BasicStack reports out of memory through derived capacity")
{
	StackHarness<int, 2> stack;

	REQUIRE(stack.Add(4));
	REQUIRE(stack.Add(5));

	auto overflow = stack.Add(6);

	REQUIRE(not overflow);
	REQUIRE(overflow.error().type == mlsl::ErrorType::OutOfMemory);
	REQUIRE(stack.Size() == 2);
	REQUIRE(stack.Top() == 5);
}
