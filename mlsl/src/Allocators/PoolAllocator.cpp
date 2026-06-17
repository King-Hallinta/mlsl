// mlsl
// SPDX-FileCopyrightText: 2026 Korrikada
// SPDX-License-Identifier: Apache-2.0

#include <mlsl/Allocators/PoolAllocator.hpp>

#include <cstdint>

namespace mlsl
{
	PoolAllocator::PoolAllocator(void *buffer, std::size_t bufferSize, std::size_t blockSize)
		: m_Buffer(buffer), m_BufferSize(bufferSize), m_BlockSize(blockSize < sizeof(void *) ? sizeof(void *) : blockSize), m_BlockCount(bufferSize / m_BlockSize), m_Available(m_BlockCount), m_FreeList(nullptr)
	{
		auto ptr = static_cast<std::byte *>(m_Buffer);

		for (std::size_t i = 0; i < m_BlockCount - 1; ++i)
		{
			*reinterpret_cast<void **>(ptr + i * m_BlockSize) = ptr + (i + 1) * m_BlockSize;
		}

		if (m_BlockCount > 0)
		{
			*reinterpret_cast<void **>(ptr + (m_BlockCount - 1) * m_BlockSize) = nullptr;
			m_FreeList = ptr;
		}
	}

	PoolAllocator::PoolAllocator(PoolAllocator &&other) noexcept
		: m_Buffer(other.m_Buffer), m_BufferSize(other.m_BufferSize), m_BlockSize(other.m_BlockSize), m_BlockCount(other.m_BlockCount), m_Available(other.m_Available), m_FreeList(other.m_FreeList)
	{
		other.m_Buffer = nullptr;
		other.m_BufferSize = 0;
		other.m_BlockSize = 0;
		other.m_BlockCount = 0;
		other.m_Available = 0;
		other.m_FreeList = nullptr;
	}

	PoolAllocator &PoolAllocator::operator=(PoolAllocator &&other) noexcept
	{
		if (this != &other)
		{
			m_Buffer = other.m_Buffer;
			m_BufferSize = other.m_BufferSize;
			m_BlockSize = other.m_BlockSize;
			m_BlockCount = other.m_BlockCount;
			m_Available = other.m_Available;
			m_FreeList = other.m_FreeList;

			other.m_Buffer = nullptr;
			other.m_BufferSize = 0;
			other.m_BlockSize = 0;
			other.m_BlockCount = 0;
			other.m_Available = 0;
			other.m_FreeList = nullptr;
		}

		return *this;
	}

	std::expected<void *MLSL_RESTRICT, Error> PoolAllocator::Allocate(std::size_t size, std::size_t alignment)
	{
		(void)alignment;

		if (size > m_BlockSize || m_FreeList == nullptr)
		{
			return std::unexpected(Error {ErrorType::OutOfMemory});
		}

		void *ptr = m_FreeList;
		m_FreeList = *reinterpret_cast<void **>(m_FreeList);
		--m_Available;

		return ptr;
	}

	void PoolAllocator::Deallocate(void *ptr, std::size_t size)
	{
		(void)size;

		if (ptr == nullptr)
		{
			return;
		}

		*reinterpret_cast<void **>(ptr) = m_FreeList;
		m_FreeList = ptr;
		++m_Available;
	}

	bool PoolAllocator::Owns(void *ptr) const
	{
		auto addr = reinterpret_cast<std::uintptr_t>(ptr);
		auto start = reinterpret_cast<std::uintptr_t>(m_Buffer);

		return addr >= start && addr < start + m_BufferSize;
	}

	void PoolAllocator::Reset()
	{
		auto ptr = static_cast<std::byte *>(m_Buffer);

		for (std::size_t i = 0; i < m_BlockCount - 1; ++i)
		{
			*reinterpret_cast<void **>(ptr + i * m_BlockSize) = ptr + (i + 1) * m_BlockSize;
		}

		if (m_BlockCount > 0)
		{
			*reinterpret_cast<void **>(ptr + (m_BlockCount - 1) * m_BlockSize) = nullptr;
			m_FreeList = ptr;
		}

		m_Available = m_BlockCount;
	}

	std::size_t PoolAllocator::BlockSize() const
	{
		return m_BlockSize;
	}

	std::size_t PoolAllocator::BlockCount() const
	{
		return m_BlockCount;
	}

	std::size_t PoolAllocator::Available() const
	{
		return m_Available;
	}
} // namespace mlsl
