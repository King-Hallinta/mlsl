// mlsl
// SPDX-FileCopyrightText: 2026 Korrikada
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <mlsl/Allocators/Allocator.hpp>

namespace mlsl
{
	class FreeListAllocator : public Allocator
	{
	public:
		enum class SearchPolicy
		{
			FirstFit,
			BestFit
		};

	public:
		FreeListAllocator() = default;
		FreeListAllocator(void *buffer, std::size_t size, SearchPolicy policy = SearchPolicy::FirstFit);
		FreeListAllocator(const FreeListAllocator &) = delete;
		FreeListAllocator(FreeListAllocator &&other) noexcept;
		~FreeListAllocator() override = default;

	public:
		FreeListAllocator &operator=(const FreeListAllocator &) = delete;
		FreeListAllocator &operator=(FreeListAllocator &&other) noexcept;

	public:
		[[nodiscard]] std::expected<void *MLSL_RESTRICT, Error> Allocate(std::size_t size, std::size_t alignment = alignof(std::max_align_t)) override;
		void Deallocate(void *ptr, std::size_t size) override;
		[[nodiscard]] bool Owns(void *ptr) const override;

		void Reset();
		[[nodiscard]] std::size_t Used() const;
		[[nodiscard]] std::size_t Capacity() const;

	private:
		struct Block
		{
			std::size_t size;
			Block *next;
		};

		struct Header
		{
			std::size_t size;
			std::size_t adjustment;
		};

	private:
		void Coalesce(Block *prev, Block *block);

	private:
		void *m_Buffer = nullptr;
		std::size_t m_Size = 0;
		std::size_t m_Used = 0;
		SearchPolicy m_Policy = SearchPolicy::FirstFit;
		Block *m_FreeList = nullptr;
	};
} // namespace mlsl
