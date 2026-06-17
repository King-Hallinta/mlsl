// mlsl
// SPDX-FileCopyrightText: 2026 Korrikada
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <mlsl/Allocators/Allocator.hpp>

namespace mlsl
{
	class PoolAllocator : public Allocator
	{
	public:
		PoolAllocator() = default;
		PoolAllocator(void *buffer, std::size_t bufferSize, std::size_t blockSize);
		PoolAllocator(const PoolAllocator &) = delete;
		PoolAllocator(PoolAllocator &&other) noexcept;
		~PoolAllocator() override = default;

	public:
		PoolAllocator &operator=(const PoolAllocator &) = delete;
		PoolAllocator &operator=(PoolAllocator &&other) noexcept;

	public:
		[[nodiscard]] std::expected<void *MLSL_RESTRICT, Error> Allocate(std::size_t size, std::size_t alignment = alignof(std::max_align_t)) override;
		void Deallocate(void *ptr, std::size_t size) override;
		[[nodiscard]] bool Owns(void *ptr) const override;

		void Reset();
		[[nodiscard]] std::size_t BlockSize() const;
		[[nodiscard]] std::size_t BlockCount() const;
		[[nodiscard]] std::size_t Available() const;

	private:
		void *m_Buffer = nullptr;
		std::size_t m_BufferSize = 0;
		std::size_t m_BlockSize = 0;
		std::size_t m_BlockCount = 0;
		std::size_t m_Available = 0;
		void *m_FreeList = nullptr;
	};
} // namespace mlsl
