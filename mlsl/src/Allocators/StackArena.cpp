// mlsl
// SPDX-FileCopyrightText: 2026 Korrikada
// SPDX-License-Identifier: Apache-2.0

#include <mlsl/Allocators/StackArena.hpp>
#include <mlsl/Memory/Alignment.hpp>

#include <cstdint>

namespace mlsl
{
	StackArena::StackArena(void *buffer, std::size_t size)
		: m_Buffer(buffer), m_Size(size), m_Offset(0)
	{
	}

	StackArena::StackArena(StackArena &&other) noexcept
		: m_Buffer(other.m_Buffer), m_Size(other.m_Size), m_Offset(other.m_Offset)
	{
		other.m_Buffer = nullptr;
		other.m_Size = 0;
		other.m_Offset = 0;
	}

	StackArena &StackArena::operator=(StackArena &&other) noexcept
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

	std::expected<void *MLSL_RESTRICT, Error> StackArena::Allocate(std::size_t size, std::size_t alignment)
	{
		if (alignment == 0)
		{
			return std::unexpected(Error {ErrorType::InvalidArgument});
		}

		auto current = reinterpret_cast<std::uintptr_t>(m_Buffer) + m_Offset;
		auto aligned = AlignAddress(current, alignment);
		auto adjustment = aligned - current;
		auto offset = m_Offset + adjustment;

		if (offset > m_Size or size > m_Size - offset)
		{
			return std::unexpected(Error {ErrorType::OutOfMemory});
		}

		m_Offset = offset + size;

		return reinterpret_cast<void *>(aligned);
	}

	bool StackArena::Owns(void *ptr) const
	{
		auto addr = reinterpret_cast<std::uintptr_t>(ptr);
		auto start = reinterpret_cast<std::uintptr_t>(m_Buffer);

		return addr >= start && addr < start + m_Offset;
	}

	StackArena::Marker StackArena::GetMarker() const
	{
		return Marker {m_Offset};
	}

	void StackArena::FreeToMarker(Marker marker)
	{
		m_Offset = marker.offset;
	}

	void StackArena::Reset()
	{
		m_Offset = 0;
	}

	std::size_t StackArena::Used() const
	{
		return m_Offset;
	}

	std::size_t StackArena::Capacity() const
	{
		return m_Size;
	}
} // namespace mlsl
