// mlsl
// SPDX-FileCopyrightText: 2026 Korrikada
// SPDX-License-Identifier: Apache-2.0

#include <mlsl/Allocators/FreeListAllocator.hpp>
#include <mlsl/Memory/Alignment.hpp>

#include <cstdint>

namespace mlsl
{
	FreeListAllocator::FreeListAllocator(void *buffer, std::size_t size, SearchPolicy policy)
		: m_Buffer(buffer), m_Size(size), m_Used(0), m_Policy(policy), m_FreeList(nullptr)
	{
		if (buffer != nullptr and size >= sizeof(Block))
		{
			m_FreeList = static_cast<Block *>(buffer);
			m_FreeList->size = size;
			m_FreeList->next = nullptr;
		}
	}

	FreeListAllocator::FreeListAllocator(FreeListAllocator &&other) noexcept
		: m_Buffer(other.m_Buffer), m_Size(other.m_Size), m_Used(other.m_Used), m_Policy(other.m_Policy), m_FreeList(other.m_FreeList)
	{
		other.m_Buffer = nullptr;
		other.m_Size = 0;
		other.m_Used = 0;
		other.m_FreeList = nullptr;
	}

	FreeListAllocator &FreeListAllocator::operator=(FreeListAllocator &&other) noexcept
	{
		if (this != &other)
		{
			m_Buffer = other.m_Buffer;
			m_Size = other.m_Size;
			m_Used = other.m_Used;
			m_Policy = other.m_Policy;
			m_FreeList = other.m_FreeList;

			other.m_Buffer = nullptr;
			other.m_Size = 0;
			other.m_Used = 0;
			other.m_FreeList = nullptr;
		}

		return *this;
	}

	std::expected<void *MLSL_RESTRICT, Error> FreeListAllocator::Allocate(std::size_t size, std::size_t alignment)
	{
		if (alignment == 0)
		{
			return std::unexpected(Error {ErrorType::InvalidArgument});
		}

		if (size > m_Size)
		{
			return std::unexpected(Error {ErrorType::OutOfMemory});
		}

		Block *prev = nullptr;
		Block *curr = m_FreeList;
		Block *bestPrev = nullptr;
		Block *bestBlock = nullptr;
		std::size_t bestAdjustment = 0;

		while (curr != nullptr)
		{
			auto currAddr = reinterpret_cast<std::uintptr_t>(curr);
			auto dataAddr = currAddr + sizeof(Header);
			auto aligned = AlignAddress(dataAddr, alignment);
			auto adjustment = aligned - currAddr;
			auto requiredSize = size + adjustment;

			if (curr->size >= requiredSize)
			{
				if (m_Policy == SearchPolicy::FirstFit)
				{
					bestPrev = prev;
					bestBlock = curr;
					bestAdjustment = adjustment;

					break;
				}
				else if (bestBlock == nullptr || curr->size < bestBlock->size)
				{
					bestPrev = prev;
					bestBlock = curr;
					bestAdjustment = adjustment;
				}
			}

			prev = curr;
			curr = curr->next;
		}

		if (bestBlock == nullptr)
		{
			return std::unexpected(Error {ErrorType::OutOfMemory});
		}

		auto blockAddr = reinterpret_cast<std::uintptr_t>(bestBlock);
		auto alignedAddr = blockAddr + bestAdjustment;
		auto remainingSize = bestBlock->size - size - bestAdjustment;

		if (remainingSize >= sizeof(Block) + sizeof(Header))
		{
			auto newBlock = reinterpret_cast<Block *>(alignedAddr + size);
			newBlock->size = remainingSize;
			newBlock->next = bestBlock->next;

			if (bestPrev != nullptr)
			{
				bestPrev->next = newBlock;
			}
			else
			{
				m_FreeList = newBlock;
			}
		}
		else
		{
			if (bestPrev != nullptr)
			{
				bestPrev->next = bestBlock->next;
			}
			else
			{
				m_FreeList = bestBlock->next;
			}
		}

		auto header = reinterpret_cast<Header *>(alignedAddr - sizeof(Header));
		header->size = size + bestAdjustment;
		header->adjustment = bestAdjustment;

		m_Used += header->size;

		return reinterpret_cast<void *>(alignedAddr);
	}

	void FreeListAllocator::Deallocate(void *ptr, std::size_t size)
	{
		(void)size;

		if (ptr == nullptr)
		{
			return;
		}

		auto addr = reinterpret_cast<std::uintptr_t>(ptr);
		auto header = reinterpret_cast<Header *>(addr - sizeof(Header));
		auto blockAddr = addr - header->adjustment;
		auto block = reinterpret_cast<Block *>(blockAddr);

		block->size = header->size;
		m_Used -= header->size;

		Block *prev = nullptr;
		Block *curr = m_FreeList;

		while (curr != nullptr && reinterpret_cast<std::uintptr_t>(curr) < blockAddr)
		{
			prev = curr;
			curr = curr->next;
		}

		block->next = curr;

		if (prev != nullptr)
		{
			prev->next = block;
			Coalesce(prev, block);
		}
		else
		{
			m_FreeList = block;
		}

		if (curr != nullptr)
		{
			Coalesce(block, curr);
		}
	}

	bool FreeListAllocator::Owns(void *ptr) const
	{
		auto addr = reinterpret_cast<std::uintptr_t>(ptr);
		auto start = reinterpret_cast<std::uintptr_t>(m_Buffer);

		return addr >= start && addr < start + m_Size;
	}

	void FreeListAllocator::Reset()
	{
		m_Used = 0;

		if (m_Buffer != nullptr and m_Size >= sizeof(Block))
		{
			m_FreeList = static_cast<Block *>(m_Buffer);
			m_FreeList->size = m_Size;
			m_FreeList->next = nullptr;
		}
		else
		{
			m_FreeList = nullptr;
		}
	}

	std::size_t FreeListAllocator::Used() const
	{
		return m_Used;
	}

	std::size_t FreeListAllocator::Capacity() const
	{
		return m_Size;
	}

	void FreeListAllocator::Coalesce(Block *prev, Block *block)
	{
		auto prevAddr = reinterpret_cast<std::uintptr_t>(prev);
		auto blockAddr = reinterpret_cast<std::uintptr_t>(block);

		if (prevAddr + prev->size == blockAddr)
		{
			prev->size += block->size;
			prev->next = block->next;
		}
	}
} // namespace mlsl
