// mlsl
// SPDX-FileCopyrightText: 2026 Korrikada
// SPDX-License-Identifier: Apache-2.0

#include <mlsl/Allocators/LinearArena.hpp>
#include <mlsl/Memory/Alignment.hpp>

#include <cstdint>

namespace mlsl
{
	LinearArena::LinearArena(void *buffer, std::size_t size)
		: m_Buffer(buffer), m_Size(size), m_Offset(0)
	{
	}

	LinearArena::LinearArena(LinearArena &&other) noexcept
		: m_Buffer(other.m_Buffer), m_Size(other.m_Size), m_Offset(other.m_Offset)
	{
		other.m_Buffer = nullptr;
		other.m_Size = 0;
		other.m_Offset = 0;
	}

	LinearArena &LinearArena::operator=(LinearArena &&other) noexcept
	{
		if (this != &other)
		{
			m_Buffer = other.m_Buffer;
			m_Size = other.m_Size;
			m_Offset = other.m_Offset;

			other.m_Buffer = nullptr;
			other.m_Size = 0;
			other.m_Offset = 0;
		}

		return *this;
	}

	std::expected<void *MLSL_RESTRICT, Error> LinearArena::Allocate(std::size_t size, std::size_t alignment)
	{
		if (alignment == 0)
		{
			return std::unexpected(Error {ErrorType::InvalidArgument});
		}

		auto current = reinterpret_cast<std::uintptr_t>(m_Buffer) + m_Offset;
		auto aligned = AlignAddress(current, alignment);
		auto offset = aligned - reinterpret_cast<std::uintptr_t>(m_Buffer);

		if (offset > m_Size or size > m_Size - offset)
		{
			return std::unexpected(Error {ErrorType::OutOfMemory});
		}

		m_Offset = offset + size;

		return reinterpret_cast<void *>(aligned);
	}

	bool LinearArena::Owns(void *ptr) const
	{
		auto addr = reinterpret_cast<std::uintptr_t>(ptr);
		auto start = reinterpret_cast<std::uintptr_t>(m_Buffer);

		return addr >= start && addr < start + m_Offset;
	}

	void LinearArena::Reset()
	{
		m_Offset = 0;
	}

	std::size_t LinearArena::Used() const
	{
		return m_Offset;
	}

	std::size_t LinearArena::Remaining() const
	{
		return m_Size - m_Offset;
	}

	std::size_t LinearArena::Capacity() const
	{
		return m_Size;
	}
} // namespace mlsl
