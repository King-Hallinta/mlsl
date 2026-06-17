// mlsl
// SPDX-FileCopyrightText: 2026 Korrikada
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <utility>

namespace mlsl
{
	template <typename T, std::size_t N>
	FixedRingBuffer<T, N>::FixedRingBuffer(const FixedRingBuffer &other)
	{
		for (SizeType i = 0; i < other.m_Size; ++i)
		{
			static_cast<void>(this->Add(other[i]));
		}
	}

	template <typename T, std::size_t N>
	FixedRingBuffer<T, N>::FixedRingBuffer(FixedRingBuffer &&other) noexcept
	{
		for (SizeType i = 0; i < other.m_Size; ++i)
		{
			static_cast<void>(this->Add(std::move(other[i])));
		}

		other.Clear();
	}

	template <typename T, std::size_t N>
	FixedRingBuffer<T, N>::~FixedRingBuffer()
	{
		this->Clear();
	}

	template <typename T, std::size_t N>
	FixedRingBuffer<T, N> &FixedRingBuffer<T, N>::operator=(const FixedRingBuffer &other)
	{
		if (this != &other)
		{
			this->Clear();

			for (SizeType i = 0; i < other.m_Size; ++i)
			{
				static_cast<void>(this->Add(other[i]));
			}
		}

		return *this;
	}

	template <typename T, std::size_t N>
	FixedRingBuffer<T, N> &FixedRingBuffer<T, N>::operator=(FixedRingBuffer &&other) noexcept
	{
		if (this != &other)
		{
			this->Clear();

			for (SizeType i = 0; i < other.m_Size; ++i)
			{
				static_cast<void>(this->Add(std::move(other[i])));
			}

			other.Clear();
		}

		return *this;
	}

	template <typename T, std::size_t N>
	T *FixedRingBuffer<T, N>::GetStorage()
	{
		return reinterpret_cast<T *>(m_Storage);
	}

	template <typename T, std::size_t N>
	const T *FixedRingBuffer<T, N>::GetStorage() const
	{
		return reinterpret_cast<const T *>(m_Storage);
	}

	template <typename T, std::size_t N>
	typename FixedRingBuffer<T, N>::SizeType FixedRingBuffer<T, N>::GetCapacity() const
	{
		return N;
	}

	template <typename T, std::size_t N>
	std::expected<void, Error> FixedRingBuffer<T, N>::EnsureCapacity(SizeType required)
	{
		if (required > N)
		{
			return std::unexpected(Error {ErrorType::BufferOverflow});
		}

		return {};
	}

	template <typename T, std::size_t N>
	typename FixedRingBuffer<T, N>::SizeType FixedRingBuffer<T, N>::Capacity() const
	{
		return N;
	}
} // namespace mlsl
