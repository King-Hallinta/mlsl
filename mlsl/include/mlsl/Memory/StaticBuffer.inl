// mlsl
// SPDX-FileCopyrightText: 2026 Korrikada
// SPDX-License-Identifier: Apache-2.0

#pragma once

namespace mlsl
{
	template <std::size_t Capacity, std::size_t BufferAlignment>
	constexpr typename StaticBuffer<Capacity, BufferAlignment>::Pointer StaticBuffer<Capacity, BufferAlignment>::Data()
	{
		return m_Data;
	}

	template <std::size_t Capacity, std::size_t BufferAlignment>
	constexpr typename StaticBuffer<Capacity, BufferAlignment>::ConstPointer StaticBuffer<Capacity, BufferAlignment>::Data() const
	{
		return m_Data;
	}

	template <std::size_t Capacity, std::size_t BufferAlignment>
	constexpr typename StaticBuffer<Capacity, BufferAlignment>::SizeType StaticBuffer<Capacity, BufferAlignment>::Size() const
	{
		return Capacity;
	}

	template <std::size_t Capacity, std::size_t BufferAlignment>
	constexpr typename StaticBuffer<Capacity, BufferAlignment>::SizeType StaticBuffer<Capacity, BufferAlignment>::Alignment() const
	{
		return BufferAlignment;
	}
} // namespace mlsl
