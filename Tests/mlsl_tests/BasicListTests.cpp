// mlsl
// SPDX-FileCopyrightText: 2026 Korrikada
// SPDX-License-Identifier: Apache-2.0

#include <mlsl/Containers/BasicList.hpp>

#include <catch2/catch_test_macros.hpp>

namespace
{
	template <typename T, std::size_t N>
	class ListHarness : public mlsl::BasicList<T, ListHarness<T, N>>
	{
	public:
		using Base = mlsl::BasicList<T, ListHarness<T, N>>;
		using typename Base::NodeType;

	public:
		std::expected<NodeType *, mlsl::Error> AllocateNode()
		{
			if (m_FreeList == nullptr)
			{
				return std::unexpected(mlsl::Error {mlsl::ErrorType::OutOfMemory});
			}

			Slot *slot = m_FreeList;
			m_FreeList = slot->next;

			return reinterpret_cast<NodeType *>(slot->storage);
		}

		void DeallocateNode(NodeType *node)
		{
			Slot *slot = reinterpret_cast<Slot *>(node);
			slot->next = m_FreeList;
			m_FreeList = slot;
		}

	public:
		ListHarness()
		{
			for (std::size_t i = 0; i < N - 1; ++i)
			{
				m_Storage[i].next = &m_Storage[i + 1];
			}

			m_Storage[N - 1].next = nullptr;
			m_FreeList = &m_Storage[0];
		}

	private:
		union Slot {
			alignas(NodeType) std::byte storage[sizeof(NodeType)];
			Slot *next;
		};

	private:
		Slot m_Storage[N];
		Slot *m_FreeList = nullptr;
	};
} // namespace

TEST_CASE("BasicList provides insert erase and iteration semantics")
{
	ListHarness<int, 4> list;

	REQUIRE(list.AddBack(2));
	REQUIRE(list.AddFront(1));
	REQUIRE(list.AddBack(4));

	auto inserted = list.Insert(list.End(), 5);

	REQUIRE(inserted);
	REQUIRE(list.Size() == 4);
	REQUIRE(list.Front() == 1);
	REQUIRE(list.Back() == 5);

	auto item = list.Get(2);

	REQUIRE(item);
	REQUIRE(**item == 4);

	auto iterator = list.Begin();
	++iterator;

	auto next = list.Erase(iterator);

	REQUIRE(*next == 4);
	REQUIRE(list.Size() == 3);
}

TEST_CASE("BasicList reports out of memory through derived node allocator")
{
	ListHarness<int, 2> list;

	REQUIRE(list.AddBack(7));
	REQUIRE(list.AddBack(8));

	auto overflow = list.AddFront(6);

	REQUIRE(not overflow);
	REQUIRE(overflow.error().type == mlsl::ErrorType::OutOfMemory);
	REQUIRE(list.Size() == 2);
	REQUIRE(list.Front() == 7);
	REQUIRE(list.Back() == 8);
}
